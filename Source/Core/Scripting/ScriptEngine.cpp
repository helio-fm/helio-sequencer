/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ScriptEngine.h"
#include "SequencerOperations.h"
#include "DocumentHelpers.h"
#include "Config.h"

ScriptEngine::ScriptEngine() :
    Thread("ScriptingPlayground") {}

bool ScriptEngine::evaluate(const String &code,
    WeakReference<SideEffects> hostSideEffects,
    bool shouldEvaluateInPlayground,
    Optional<Breakpoint> breakpoint)
{
    // parse immediately, it's usually fast and we want code blocks asap
    // #if DEBUG
    // const auto parseStartMs = Time::getMillisecondCounter();
    // #endif
    try
    {
        const ScopedWriteLock lock(this->parsingResultLock);
        this->parsingError = {};
        this->codeBlockRanges.clearQuick();
        this->parsingResult = script::parse(code, this->codeBlockRanges);
        // DBG("Parsed in " + String(Time::getMillisecondCounter() - parseStartMs) + " ms");
    }
    catch (script::ParsingError error)
    {
        this->parsingError = error;
        // DBG("Parsing failed in " + String(Time::getMillisecondCounter() - parseStartMs) + " ms");
        return false;
    }

    {
        const ScopedWriteLock lock(this->hostLock);
        this->hostSideEffects = hostSideEffects;
        jassert(this->hostSideEffects != nullptr);
        this->hostContext = this->hostSideEffects->fillHostContext();
    }

    {
        const ScopedWriteLock lock(this->breakpointLock);
        this->breakpoint = breakpoint;
        jassert(shouldEvaluateInPlayground || !breakpoint.hasValue());
    }

    if (shouldEvaluateInPlayground)
    {
        if (this->isThreadRunning())
        {
            this->signalThreadShouldExit();
        }

        // the playground uses fixed seed by default
        // so that evaluation pop-ups in the editor are less confusing
        // (but the user can still set a different seed in the code):
        this->random.randomize(this->random.originalSeed);
        this->playgroundMode = true;

        constexpr auto debounceMs = 69;
        this->startTimer(debounceMs);
    }
    else
    {
        this->stopTimer();
        this->stopThread(1000);
        this->playgroundMode = false;

        try
        {
            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->rootScope = {};
                this->evaluationError = {};
                this->resetEvaluationContext();
            }

            this->random.randomize();
            auto result = this->parsingResult.evaluate(this->rootScope, *this);

            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->evaluationResult = move(result);
            }

            DBG("Evaluated in " + String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");

            const ScopedWriteLock lock(this->hostLock);
            if (this->hostSideEffects != nullptr)
            {
                this->hostSideEffects->onProgramTerminated(true);
            }
        }
        catch (script::EvaluationError error)
        {
            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->evaluationError = error;
            }

            DBG("Evaluation failed in " + String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");

            const ScopedWriteLock lock(this->hostLock);
            if (this->hostSideEffects != nullptr)
            {
                this->hostSideEffects->onProgramTerminated(false);
            }

            return false;
        }
    }

    return true;
}

script::Scope &ScriptEngine::getRootScope() noexcept
{
    return this->rootScope;
}

bool ScriptEngine::isPlayground() const noexcept
{
    return this->playgroundMode.get();
}

const Array<Range<int>> &ScriptEngine::getBlockRanges() const
{
    const ScopedReadLock lock(this->parsingResultLock);
    return this->codeBlockRanges;
}

const script::Value &ScriptEngine::getParsingResult() const
{
    const ScopedReadLock lock(this->parsingResultLock);
    return this->parsingResult;
}

const Optional<script::ParsingError> &ScriptEngine::getParsingError() const
{
    const ScopedReadLock lock(this->parsingResultLock);
    return this->parsingError;
}

StringArray ScriptEngine::getTopLevelFunctionNames() const
{
    const ScopedReadLock lock(this->evaluationResultLock);
    return this->rootScope.findAllFunctionNames();
}

const script::Value &ScriptEngine::getEvaluationResult() const
{
    const ScopedReadLock lock(this->evaluationResultLock);
    return this->evaluationResult;
}

const Optional<script::EvaluationError> &ScriptEngine::getEvaluationError() const
{
    const ScopedReadLock lock(this->evaluationResultLock);
    return this->evaluationError;
}

void ScriptEngine::onEditorOpen(int &outCaretPosition, int &outStartLine) noexcept
{
    const ScopedWriteLock lock(this->evaluationResultLock);
    if (this->evaluationError.hasValue() &&
        this->evaluationError->type == script::EvaluationError::Type::BreakpointHit)
    {
        this->evaluationError = {};
    }

    outCaretPosition = this->editorDefaultCaretPosition;
    outStartLine = this->editorDefaultStartLine;
}

void ScriptEngine::onEditorClose(int caretPosition, int startLine) noexcept
{
    this->stopTimer();
    this->stopThread(1000);

    this->editorDefaultCaretPosition = caretPosition;
    this->editorDefaultStartLine = startLine;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData ScriptEngine::serialize() const noexcept
{
    SerializedData data(Serialization::Core::scriptEditor);

    data.setProperty(Serialization::Core::scriptEditorSeed, this->random.originalSeed);
    data.setProperty(Serialization::Core::scriptEditorCaret, this->editorDefaultCaretPosition);
    data.setProperty(Serialization::Core::scriptEditorLine, this->editorDefaultStartLine);

    return data;
}

void ScriptEngine::deserialize(const SerializedData &data) noexcept
{
    const auto root = data.hasType(Serialization::Core::scriptEditor) ?
        data : data.getChildWithName(Serialization::Core::scriptEditor);

    if (!root.isValid())
    {
        return;
    }

    this->editorDefaultCaretPosition =
        root.getProperty(Serialization::Core::scriptEditorCaret);
    this->editorDefaultStartLine =
        root.getProperty(Serialization::Core::scriptEditorLine);
    const auto lastSeed =
        root.getProperty(Serialization::Core::scriptEditorSeed,
            this->random.originalSeed);
    this->random.randomize(lastSeed);
}

void ScriptEngine::reset() noexcept {}

//===----------------------------------------------------------------------===//
// Domain-specific extensions
//===----------------------------------------------------------------------===//

#include "ScriptTypes.h"

namespace extensions
{

static ScriptEngine *castToSelf(script::EvaluationContext &context)
{
    auto *ptr = &context;
    if (auto *self = dynamic_cast<ScriptEngine *>(ptr))
    {
        return self;
    }

    jassertfalse;
    throw std::exception();
}

static bool isPlaygroundMode(script::EvaluationContext &context)
{
    return castToSelf(context)->isPlayground();
}

script::Value load(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);

    if (args.isEmpty())
    {
        throw script::EvaluationError(script::EvaluationError::Type::TooFewArguments);
    }

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);

    // no caching here, everything is hopefully fast enough
    script::Value result;
    for (const auto &value : args)
    {
        const auto file = DocumentHelpers::findFileInLocationOrDocuments(value.asString());
        if (!file.existsAsFile())
        {
            throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument);
        }

        Array<Range<int>> codeBlockRanges;
        const auto program = script::parse(file.loadFileAsString(), codeBlockRanges);
        result = program.evaluate(self->getRootScope(), *self);
    }

    return result;
}

script::Value seed(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);

    if (args.size() > 1)
    {
        throw script::EvaluationError(script::EvaluationError::Type::TooManyArguments);
    }

    auto &random = castToSelf(context)->random;

    if (!args.isEmpty())
    {
        random.randomize(args.getReference(0).castToInteger());
    }

    return script::Value(random.originalSeed);
}

script::Value random(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);

    auto &random = castToSelf(context)->random;

    if (args.size() == 1 && args.getReference(0).isSymbol())
    {
        const auto &arg = args.getReference(0).asSymbol();
        if (arg.startsWithChar('d'))
        {
            const auto d = arg.substring(1).getIntValue();
            if (d > 0)
            {
                return random.rollDie(d);
            }
        }
    }
    else if (args.size() == 1 && args.getReference(0).isList())
    {
        const auto &list = args.getReference(0).asList();
        if (list.isEmpty())
        {
            throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
                "(random (list ...))", "expected a non-empty list");
        }

        return random.pickListItem(list);
    }
    else if (args.size() == 2)
    {
        const auto &low = args.getReference(0);
        const auto &high = args.getReference(1);
        if (low.isInteger() && high.isInteger())
        {
            return random.getNextInteger({ low.castToInteger(), high.castToInteger() });
        }
        else if (low.isFloat() || high.isFloat())
        {
            return random.getNextFloat({ low.castToFloat(), high.castToFloat() });
        }
    }

    throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument);
}

namespace project
{

script::Value reset(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    script::checkNumArgs(unevaluatedArgs, 0);

    if (isPlaygroundMode(context))
    {
        return {};
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    auto *self = castToSelf(context);
    const ScopedWriteLock lock(self->hostLock);
    self->hostSideEffects->resetProject();
    return {};
}

} // namespace project

namespace timeline
{

script::Value reset(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    script::checkNumArgs(unevaluatedArgs, 0);

    if (isPlaygroundMode(context))
    {
        return {};
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    auto *self = castToSelf(context);
    const ScopedWriteLock lock(self->hostLock);
    self->hostSideEffects->resetTimeline();
    return {};
}

script::Value addKeySignature(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 3);

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);
    const auto keySignature =
        script::interop::makeKeySignature(script::Value(move(args)),
            self->hostContext.allScales, self->hostContext.temperament->getPeriodSize());

    if (isPlaygroundMode(context))
    {
        return {};
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    self->hostSideEffects->addKeySignature(keySignature);
    return {};
}

} // namespace timeline

namespace track
{

script::Value make(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    if (args.isEmpty() || !args.getReference(0).isString())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(track:make track-name)", "expected a track name");
    }

    if (args.size() > 2)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(track:make track-name ((note1) (note2) ...))");
    }

    Array<Note> notes;
    if (args.size() > 1)
    {
        if (!args.getReference(1).isList())
        {
            throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
                "(track:make track-name ((note1) (note2) ...))",
                "expected a list of notes, got " + args.getReference(1).debug());
        }

        for (const auto &noteValue : args.getReference(1).asList())
        {
            notes.add(script::interop::makeNote(noteValue));
        }
    }

    if (isPlaygroundMode(context))
    {
        return script::Value::makeString("example-track-id");
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    auto *self = castToSelf(context);
    const ScopedWriteLock lock(self->hostLock);
    const auto trackId = self->hostSideEffects->makePianoTrack(args.getReference(0).asString());

    if (!notes.isEmpty())
    {
        auto *track = self->hostSideEffects->findPianoTrackById(trackId);
        if (track == nullptr)
        {
            throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
                trackId, "track not found");
        }

        self->hostSideEffects->addNotes(track, notes, true);
    }

    return script::Value::makeString(trackId);
}

script::Value addNotes(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    if (!args.getReference(0).isString())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(track:add-notes track-id ((note1) (note2) ...))",
            "expected a track id, got " + args.getReference(0).debug());
    }

    if (!args.getReference(1).isList())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(track:add-notes track-id ((note1) (note2) ...))",
            "expected a list of notes, got " + args.getReference(1).debug());
    }

    Array<Note> notes;
    for (const auto &noteValue : args.getReference(1).asList())
    {
        notes.add(script::interop::makeNote(noteValue));
    }

    if (isPlaygroundMode(context))
    {
        return {};
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

    const auto trackId = args.getReference(0).asString();

    auto *self = castToSelf(context);
    const ScopedWriteLock lock(self->hostLock);
    auto *track = self->hostSideEffects->findPianoTrackById(trackId);
    if (track == nullptr)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            trackId, "track not found");
    }

    self->hostSideEffects->addNotes(track, notes, true);
    return {};
}

} // namespace track

namespace scale
{

script::Value find(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    if (!args.getReference(0).isString())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(scale:find scale-name)",
            "expected a name, got " + args.getReference(0).debug());
    }

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);
    const auto &targetName = args.getReference(0).asString();
    for (const auto &scale : self->hostContext.allScales)
    {
        if (self->hostContext.temperament->getPeriodSize() == scale->getBasePeriod() &&
            scale->getUnlocalizedName().startsWithIgnoreCase(targetName))
        {
            return script::interop::makeScaleValue(scale);
        }
    }

    for (const auto &scale : self->hostContext.allScales)
    {
        if (self->hostContext.temperament->getPeriodSize() == scale->getBasePeriod() &&
            scale->getUnlocalizedName().containsWholeWordIgnoreCase(targetName))
        {
            return script::interop::makeScaleValue(scale);
        }
    }

    throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
        "(scale:find scale-name)",
        "cannot find " + targetName);
}

script::Value renderKey(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    if (!args.getReference(0).isList())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(scale:render-key scale degree)",
            "expected a scale, got " + args.getReference(0).debug());
    }

    if (!args.getReference(1).isInteger())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(scale:render-key scale degree)",
            "expected a scale degree, got " + args.getReference(1).debug());
    }

    if (args.getReference(1).castToInteger() <= 0)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(scale:render-key scale degree)",
            "expected an ordinal degree, i.e starting from 1 (sorry programmers)");
    }

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);
    const auto scale =
        script::interop::makeScale(args.getReference(0),
            {}, self->hostContext.temperament->getPeriodSize());
    const auto row = scale->getChromaticKey(args.getReference(1).castToInteger() - 1, 0, true);
    return script::Value(row);
}

} // namespace scale

namespace refactor
{

// a glue to perform in-place refactorings without adding a track to the project:
struct TemporaryPianoTrack final : public VirtualMidiTrack
{
    explicit TemporaryPianoTrack(const Array<Note> &notes)
    {
        this->sequence = make<PianoSequence>(*this, this->dummyEventDispatcher);

        Array<Note> ownedNotes;
        for (const auto &noteParams : notes)
        {
            ownedNotes.add(Note(this->sequence.get(), noteParams).withNewId());
        }

        this->sequence->insertGroup(ownedNotes, false);
    }

    script::Value toValue() const noexcept
    {
        script::Value::List result;
        for (int i = 0; i < this->sequence->size(); ++i)
        {
            const Note &note = this->sequence->getNoteUnchecked(i);
            result.add(script::Value(script::interop::makeNoteValue(note)));
        }

        return script::Value(move(result));
    }

    String getTrackInstrumentId() const noexcept override { return {}; }
    MidiSequence *getSequence() const noexcept override { return this->sequence.get(); }

    DummyProjectEventDispatcher dummyEventDispatcher;
    UniquePointer<PianoSequence> sequence;
};

static Array<Note> parseNotesList(const script::Value &value, const String &symbolName)
{
    if (!value.isList())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            symbolName,
            "expected a list of notes, got " + value.debug());
    }

    Array<Note> notes;
    for (const auto &noteValue : value.asList())
    {
        notes.add(script::interop::makeNote(noteValue));
    }

    return notes;
}

script::Value joinAdjacent(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:join-adjacent ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::joinAdjacent(*tempTrack.sequence, false, false);
    return tempTrack.toValue();
}

script::Value arpeggiate(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    if (!args.getReference(0).isList() || !args.getReference(1).isList())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(refactor:arpeggiate (arpeggiator-notes ...) (notes ...))",
            "expected a list of in-scale notes, got " + args.getReference(0).debug());
    }

    const auto arpeggiator =
        script::interop::makeArpeggiator(args.getReference(0));

    const auto notes = parseNotesList(args.getReference(1),
        "(refactor:arpeggiate (arpeggiator-notes ...) (notes ...))");

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);

    const Clip noTransform;
    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::arpeggiate(*tempTrack.sequence,
        noTransform,
        arpeggiator,
        self->hostContext.temperament,
        self->hostContext.keySignatures,
        self->hostContext.timeSignatures,
        1.f,    // speed, todo custom
        0.f,    // randomness
        false,  // reversed
        true,   // chord-bound
        false,  // undoable
        false); // shouldCheckpoint

    return tempTrack.toValue();
}

script::Value shiftInScaleKeys(script::EvaluationContext &context,
    const Array<Note> &notes, int deltaKey)
{
    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);

    const Clip noTransform;
    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::shiftInScaleKeyRelative(*tempTrack.sequence,
        noTransform,
        self->hostContext.keySignatures,
        self->hostContext.temperament->getHighlighting(),
        deltaKey,
        false,
        false);

    return tempTrack.toValue();
}

script::Value alignToScale(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:align-to-scale ((note1) (note2) ...))");

    return shiftInScaleKeys(context, notes, 0);
}

script::Value transposeInScale(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    const auto deltaKey = args.getReference(0).castToInteger();
    const auto notes = parseNotesList(args.getReference(1),
        "(refactor:transpose-in-scale delta-key ((note1) (note2) ...))");

    return shiftInScaleKeys(context, notes, deltaKey);
}

script::Value legato(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:legato ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::makeLegato(*tempTrack.sequence, 0, false, false);
    return tempTrack.toValue();
}

script::Value staccato(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto noteLength = jmax(Globals::minNoteLength, args.getReference(0).castToFloat());
    const auto notes = parseNotesList(args.getReference(1),
        "(refactor:staccato note-length ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::makeStaccato(*tempTrack.sequence, noteLength, false, false);
    return tempTrack.toValue();
}

script::Value retrograde(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:retrograde ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::retrograde(*tempTrack.sequence, false, false);
    return tempTrack.toValue();
}

script::Value invertMelody(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:invert-melody ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::melodicInversion(*tempTrack.sequence, false, false);
    return tempTrack.toValue();
}

script::Value invertChord(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    const auto inversionOrder = args.getReference(0).castToInteger();
    if (inversionOrder == 0)
    {
        return args.getUnchecked(1);
    }

    if (abs(inversionOrder) > 7)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(refactor:invert-chord order ((note1) (note2) ...))",
            "order of inversion looks too large");
    }

    const auto notes = parseNotesList(args.getReference(1),
        "(refactor:invert-chord order ((note1) (note2) ...))");

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostLock);
    const auto periodSize = self->hostContext.temperament->getPeriodSize();

    TemporaryPianoTrack tempTrack(notes);
    for (int i = 0; i < abs(inversionOrder); ++i)
    {
        SequencerOperations::invertChord(*tempTrack.sequence,
            (inversionOrder > 0 ? 1 : -1) * periodSize, false, false);
    }

    return tempTrack.toValue();
}

script::Value quantize(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto resolution = args.getReference(0).castToFloat();
    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:quantize resolution ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::quantize(*tempTrack.sequence, resolution, false, false);
    return tempTrack.toValue();
}

script::Value cleanupOverlaps(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:cleanup-overlaps ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);
    SequencerOperations::cleanupOverlaps(*tempTrack.sequence, false, false);
    return tempTrack.toValue();
}

} // namespace refactor

} // namespace extensions

Optional<script::Value> ScriptEngine::makeLanguageExtension(const String &name) const
{
    using Value = script::Value;

    if (name == "load") return Value::makeBuiltInFunction(name, extensions::load);

    if (name == "seed") return Value::makeBuiltInFunction(name, extensions::seed);
    if (name == "random") return Value::makeBuiltInFunction(name, extensions::random);
    if (name.startsWithChar('d') && name.substring(1).getIntValue() > 0)
    {
        // RPG dice notation symbols are self-evaluating:
        return script::Value::makeSymbol(name);
    }

    if (name == "tonic") return Value(1);
    if (name == "supertonic") return Value(2);
    if (name == "mediant") return Value(3);
    if (name == "subdominant") return Value(4);
    if (name == "dominant") return Value(5);
    if (name == "submediant") return Value(6);
    if (name == "subtonic") return Value(7);

    #define makeTriad(x) Value({ Value(x), Value((x) + 2), Value((x) + 4) })
    if (name == "triad:tonic" || name == "triad") return makeTriad(1);
    if (name == "triad:supertonic") return makeTriad(2);
    if (name == "triad:mediant") return makeTriad(3);
    if (name == "triad:subdominant") return makeTriad(4);
    if (name == "triad:dominant") return makeTriad(5);
    if (name == "triad:submediant") return makeTriad(6);
    if (name == "triad:subtonic") return makeTriad(7);
    #undef makeTriad

    #define makeSeventh(x) Value({ Value(x), Value((x) + 2), Value((x) + 4), Value((x) + 6) })
    if (name == "seventh:tonic" || name == "seventh") return makeSeventh(1);
    if (name == "seventh:supertonic") return makeSeventh(2);
    if (name == "seventh:mediant") return makeSeventh(3);
    if (name == "seventh:subdominant") return makeSeventh(4);
    if (name == "seventh:dominant") return makeSeventh(5);
    if (name == "seventh:submediant") return makeSeventh(6);
    if (name == "seventh:subtonic") return makeSeventh(7);
    #undef makeSeventh

    #define makeSus2(x) Value({ Value(x), Value((x) + 1), Value((x) + 4) })
    if (name == "sus2:tonic" || name == "sus2") return makeSus2(1);
    if (name == "sus2:supertonic") return makeSus2(2);
    if (name == "sus2:mediant") return makeSus2(3);
    if (name == "sus2:subdominant") return makeSus2(4);
    if (name == "sus2:dominant") return makeSus2(5);
    if (name == "sus2:submediant") return makeSus2(6);
    if (name == "sus2:subtonic") return makeSus2(7);
    #undef makeSus2

    #define makeSus4(x) Value({ Value(x), Value((x) + 3), Value((x) + 4) })
    if (name == "sus4:tonic" || name == "sus4") return makeSus4(1);
    if (name == "sus4:supertonic") return makeSus4(2);
    if (name == "sus4:mediant") return makeSus4(3);
    if (name == "sus4:subdominant") return makeSus4(4);
    if (name == "sus4:dominant") return makeSus4(5);
    if (name == "sus4:submediant") return makeSus4(6);
    if (name == "sus4:subtonic") return makeSus4(7);
    #undef makeSus4

    if (name == "track:make") return Value::makeBuiltInFunction(name, extensions::track::make);
    if (name == "track:add-notes") return Value::makeBuiltInFunction(name, extensions::track::addNotes);

    if (name == "scale:find") return Value::makeBuiltInFunction(name, extensions::scale::find);
    if (name == "scale:render-key") return Value::makeBuiltInFunction(name, extensions::scale::renderKey);

    if (name == "timeline:add-key") return Value::makeBuiltInFunction(name, extensions::timeline::addKeySignature);
    if (name == "timeline:reset") return Value::makeBuiltInFunction(name, extensions::timeline::reset);

    if (name == "project:reset") return Value::makeBuiltInFunction(name, extensions::project::reset);
    if (name == "project:period-size")
    {
        const ScopedReadLock lock(this->hostLock);
        return Value(this->hostContext.temperament->getPeriodSize());
    }

    if (name == "refactor:join-adjacent") return Value::makeBuiltInFunction(name, extensions::refactor::joinAdjacent);
    if (name == "refactor:arpeggiate") return Value::makeBuiltInFunction(name, extensions::refactor::arpeggiate);
    if (name == "refactor:align-to-scale") return Value::makeBuiltInFunction(name, extensions::refactor::alignToScale);
    if (name == "refactor:legato") return Value::makeBuiltInFunction(name, extensions::refactor::legato);
    if (name == "refactor:staccato") return Value::makeBuiltInFunction(name, extensions::refactor::staccato);
    if (name == "refactor:retrograde") return Value::makeBuiltInFunction(name, extensions::refactor::retrograde);
    if (name == "refactor:invert-melody") return Value::makeBuiltInFunction(name, extensions::refactor::invertMelody);
    if (name == "refactor:invert-chord") return Value::makeBuiltInFunction(name, extensions::refactor::invertChord);
    if (name == "refactor:quantize") return Value::makeBuiltInFunction(name, extensions::refactor::quantize);
    if (name == "refactor:transpose-in-scale") return Value::makeBuiltInFunction(name, extensions::refactor::transposeInScale);
    if (name == "refactor:cleanup-overlaps") return Value::makeBuiltInFunction(name, extensions::refactor::cleanupOverlaps);

    return {};
}

bool ScriptEngine::shouldAbort() const
{
    return this->playgroundMode.get() && this->threadShouldExit();
}

int ScriptEngine::getMaxCallStackSize() const
{
    return this->playgroundMode.get() ? 69 : 420;
}

int ScriptEngine::getMaxEvaluationTimeMs() const
{
    return this->playgroundMode.get() ? 4200 : 42069;
}

bool ScriptEngine::hasBreakpoints() const
{
    const ScopedReadLock lock(this->breakpointLock);
    return this->breakpoint.hasValue();
}

bool ScriptEngine::shouldBreakAt(const script::Value &value,
    const Range<int> &parentListRange) const
{
    const ScopedReadLock lock(this->breakpointLock);
    if (!this->breakpoint.hasValue())
    {
        return false;
    }

    return value.isSymbol() &&
        this->breakpoint->symbolName == value.asSymbol() &&
        this->breakpoint->parentListRange == parentListRange;
}
