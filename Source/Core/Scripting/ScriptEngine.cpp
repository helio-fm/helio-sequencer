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
#include "Config.h"

ScriptEngine::ScriptEngine(SideEffects &sideEffects) :
    Thread("ScriptingPlayground"),
    sideEffects(sideEffects) {}

bool ScriptEngine::evaluate(const String &code,
    bool shouldEvaluateInPlayground, Optional<Breakpoint> breakpoint)
{
    {
        const ScopedWriteLock lock(this->breakpointLock);
        this->breakpoint = breakpoint;
    }

    try
    {
        #if DEBUG
        auto parseStartMs = Time::getMillisecondCounter();
        #endif

        // parse immediately, it's usually fast and we want code blocks asap
        {
            const ScopedWriteLock lock(this->parsingResultLock);
            this->parsingError = {};
            this->codeBlockRanges.clearQuick();
            this->parsingResult = script::parse(code, this->codeBlockRanges);
        }

        DBG("Parsed in " + String(Time::getMillisecondCounter() - parseStartMs) + " ms");

        {
            const ScopedWriteLock lock(this->hostContextLock);
            this->hostContext = this->sideEffects.fillHostContext();
        }

        if (shouldEvaluateInPlayground)
        {
            if (this->isThreadRunning())
            {
                this->signalThreadShouldExit();
            }

            this->playgroundMode = true;
            constexpr auto debounceMs = 69;
            this->startTimer(debounceMs);
        }
        else
        {
            this->stopTimer();
            this->stopThread(1000);
            this->playgroundMode = false;

            {
                const ScopedWriteLock lock(this->evaluationResultLock);

                this->rootScope = {};
                this->evaluationError = {};
                this->resetEvaluationContext();

                this->evaluationResult =
                    this->parsingResult.evaluate(this->rootScope, *this);
            }

            DBG("Evaluated in " + String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");
        }
    }
    catch (script::ParsingError error)
    {
        this->parsingError = error;
        DBG("Parsing failed in " + String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");
        return false;
    }
    catch (script::EvaluationError error)
    {
        this->evaluationError = error;
        DBG("Evaluation failed in " + String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");
        return false;
    }

    return true;
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

// todo seed function
script::Value random(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    static Random random;

    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);

    if (args.size() == 1 && args.getReference(0).isSymbol())
    {
        const auto &arg = args.getReference(0).asSymbol();
        if (arg.startsWithChar('d'))
        {
            const auto x = arg.substring(1).getIntValue();
            if (x > 0)
            {
                const auto result = (random.nextInt() % x) == 0;
                return script::Value(result);
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

        return list.getUnchecked(random.nextInt(list.size()));
    }
    else if (args.size() == 2)
    {
        const auto &low = args.getReference(0);
        const auto &high = args.getReference(1);
        if (low.isInteger() && high.isInteger())
        {
            return script::Value(random.nextInt(Range<int>(low.castToInt(), high.castToInt())));
        }
        else if (low.isFloat() || high.isFloat())
        {
            return script::Value(low.castToFloat() +
                float(random.nextDouble() * (high.castToFloat() - low.castToFloat())));
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
    castToSelf(context)->sideEffects.resetProject();
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
    castToSelf(context)->sideEffects.resetTimeline();
    return {};
}

script::Value addKeySignature(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 3);

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostContextLock);
    const auto keySignature =
        script::interop::makeKeySignature(script::Value(move(args)),
            self->hostContext.allScales, self->hostContext.temperament->getPeriodSize());

    if (isPlaygroundMode(context))
    {
        return {};
    }

    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    self->sideEffects.addKeySignature(keySignature);
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
    const auto trackId = castToSelf(context)->sideEffects.makePianoTrack(args.getReference(0).asString());

    if (!notes.isEmpty())
    {
        auto *track = castToSelf(context)->sideEffects.findPianoTrackById(trackId);
        if (track == nullptr)
        {
            throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
                trackId, "track not found");
        }

        castToSelf(context)->sideEffects.addNotes(track, notes, true);
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
    auto *track = castToSelf(context)->sideEffects.findPianoTrackById(trackId);
    if (track == nullptr)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            trackId, "track not found");
    }

    castToSelf(context)->sideEffects.addNotes(track, notes, true);
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
    const ScopedReadLock lock(self->hostContextLock);
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

    if (args.getReference(1).castToInt() <= 0)
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(scale:render-key scale degree)",
            "expected an ordinal degree, i.e starting from 1 (sorry programmers)");
    }

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostContextLock);
    const auto scale =
        script::interop::makeScale(args.getReference(0),
            {}, self->hostContext.temperament->getPeriodSize());
    const auto row = scale->getChromaticKey(args.getReference(1).castToInt() - 1, 0, true);
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

    script::Value::List result;
    for (int i = 0; i < tempTrack.sequence->size(); ++i)
    {
        const Note &note = tempTrack.sequence->getNoteUnchecked(i);
        result.add(script::Value(script::interop::makeNoteValue(note)));
    }

    return script::Value(move(result));
}

script::Value arpeggiate(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 2);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:arpeggiate (notes ...) (arpeggiator-notes ...))");

    if (!args.getReference(1).isList())
    {
        throw script::EvaluationError(script::EvaluationError::Type::InvalidArgument,
            "(refactor:arpeggiate (notes ...) (arpeggiator-notes ...))",
            "expected a list of in-scale notes, got " + args.getReference(1).debug());
    }

    const auto arpeggiator =
        script::interop::makeArpeggiator(args.getReference(1));

    TemporaryPianoTrack tempTrack(notes);

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostContextLock);

    const Clip noTransform;
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

    script::Value::List result;
    for (int i = 0; i < tempTrack.sequence->size(); ++i)
    {
        const Note &note = tempTrack.sequence->getNoteUnchecked(i);
        result.add(script::Value(script::interop::makeNoteValue(note)));
    }

    return script::Value(move(result));
}

script::Value alignToScale(const script::Value::List &unevaluatedArgs,
    script::Scope &scope, script::EvaluationContext &context)
{
    const auto args = script::evaluateArgs(unevaluatedArgs, scope, context);
    script::checkNumArgs(args, 1);

    const auto notes = parseNotesList(args.getReference(0),
        "(refactor:align-to-scale ((note1) (note2) ...))");

    TemporaryPianoTrack tempTrack(notes);

    auto *self = castToSelf(context);
    const ScopedReadLock lock(self->hostContextLock);

    const Clip noTransform;
    SequencerOperations::shiftInScaleKeyRelative(*tempTrack.sequence,
        noTransform,
        self->hostContext.keySignatures,
        self->hostContext.temperament->getHighlighting(),
        0,      // just align, don't shift
        false,  // undoable
        false); // shouldCheckpoint

    script::Value::List result;
    for (int i = 0; i < tempTrack.sequence->size(); ++i)
    {
        const Note &note = tempTrack.sequence->getNoteUnchecked(i);
        result.add(script::Value(script::interop::makeNoteValue(note)));
    }

    return script::Value(move(result));
}

} // namespace refactor

} // namespace extensions

Optional<script::Value> ScriptEngine::makeLanguageExtension(const String &name) const
{
    using Value = script::Value;

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

    if (name == "chord:triad") return Value({ Value(1), Value(3), Value(5) });
    if (name == "chord:seventh") return Value({ Value(1), Value(3), Value(5), Value(7) });
    // plus basic triads for all degrees:
    if (name == "chord:supertonic") return Value({ Value(2), Value(4), Value(6) });
    if (name == "chord:mediant") return Value({ Value(3), Value(5), Value(7) });
    if (name == "chord:subdominant") return Value({ Value(4), Value(6), Value(8) });
    if (name == "chord:dominant") return Value({ Value(5), Value(7), Value(9) });
    if (name == "chord:submediant") return Value({ Value(6), Value(8), Value(10) });
    if (name == "chord:subtonic") return Value({ Value(7), Value(9), Value(11) });

    if (name == "track:make") return Value::makeBuiltInFunction(name, extensions::track::make);
    if (name == "track:add-notes") return Value::makeBuiltInFunction(name, extensions::track::addNotes);

    if (name == "scale:find") return Value::makeBuiltInFunction(name, extensions::scale::find);
    if (name == "scale:render-key") return Value::makeBuiltInFunction(name, extensions::scale::renderKey);

    if (name == "timeline:add-key") return Value::makeBuiltInFunction(name, extensions::timeline::addKeySignature);
    if (name == "timeline:reset") return Value::makeBuiltInFunction(name, extensions::timeline::reset);

    if (name == "project:reset") return Value::makeBuiltInFunction(name, extensions::project::reset);
    if (name == "project:period-size")
    {
        const ScopedReadLock lock(this->hostContextLock);
        return Value(this->hostContext.temperament->getPeriodSize());
    }

    // todo more refactorings here
    if (name == "refactor:join-adjacent") return Value::makeBuiltInFunction(name, extensions::refactor::joinAdjacent);
    if (name == "refactor:arpeggiate") return Value::makeBuiltInFunction(name, extensions::refactor::arpeggiate);
    if (name == "refactor:align-to-scale") return Value::makeBuiltInFunction(name, extensions::refactor::alignToScale);

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
