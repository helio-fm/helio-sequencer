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

#pragma once

class Note;
class MidiTrack;

#include "Serializable.h"
#include "Interpreter.h"
#include "Scale.h"
#include "Arpeggiator.h"
#include "Temperament.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesAggregator.h"

class ScriptEngine final :
    public script::EvaluationContext,
    public Serializable,
    public Thread,
    public Timer
{
public:

    ScriptEngine();

    //===------------------------------------------------------------------===//
    // Interop with project
    //===------------------------------------------------------------------===//

    struct SideEffects
    {
        virtual ~SideEffects() = default;

        // called with the message thread locked:
        virtual void onProgramTerminated(bool success) = 0;

        // may be called from a separate thread, do locks when needed:
        virtual void resetProject() = 0;
        virtual void resetTimeline() = 0;
        virtual void addKeySignature(const KeySignatureEvent &event) = 0;
        virtual String makePianoTrack(const String &trackName) = 0;
        virtual MidiTrack *findPianoTrackById(const String &trackId) = 0;
        virtual void addNotes(MidiTrack *track, Array<Note> &notes, bool undoable) = 0;

        struct ReadOnlyContext final
        {
            Temperament::Ptr temperament;
            WeakReference<KeySignaturesSequence> keySignatures;
            WeakReference<TimeSignaturesAggregator> timeSignatures;
            Array<Scale::Ptr> allScales;
        };

        virtual ReadOnlyContext fillHostContext() const = 0;

        JUCE_DECLARE_WEAK_REFERENCEABLE(SideEffects)
    };

    ReadWriteLock hostLock;
    WeakReference<SideEffects> hostSideEffects;
    SideEffects::ReadOnlyContext hostContext;

    struct Breakpoint final
    {
        String symbolName;
        Range<int> parentListRange;
    };

    bool evaluate(const String &code,
        WeakReference<SideEffects> sideEffects,
        bool shouldEvaluateInPlayground,
        Optional<Breakpoint> breakpoint = {});

    //===------------------------------------------------------------------===//
    // Rng
    //===------------------------------------------------------------------===//

    // a wrapper around JUCE's Random that remembers the original seed
    struct Random final
    {
        void randomize()
        {
            this->generator.setSeedRandomly();
            // when generating a random seed, let's make it look nicer: shorter
            // and always positive, 2 billion numbers should be enough for anybody
            this->randomize(std::abs(script::Value::Integer(this->generator.getSeed())));
        }

        void randomize(script::Value::Integer seed)
        {
            this->originalSeed = seed;
            this->generator.setSeed(seed);
        }

        inline script::Value rollDie(int d) noexcept
        {
            const bool result = (this->generator.nextInt() % d) == 0;
            return script::Value(result);
        }

        inline script::Value getNextInteger(Range<script::Value::Integer> range) noexcept
        {
            return script::Value(this->generator.nextInt(range));
        }

        inline script::Value getNextFloat(Range<script::Value::Float> range) noexcept
        {
            return script::Value(range.getStart() +
                script::Value::Float(this->generator.nextDouble() * range.getLength()));
        }

        inline script::Value pickListItem(const script::Value::List &list) noexcept
        {
            return list.getUnchecked(this->generator.nextInt(list.size()));
        }

        juce::Random generator;

        script::Value::Integer originalSeed = 0;
    };

    Random random;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isPlayground() const noexcept;
    script::Scope &getRootScope() noexcept;
    StringArray getTopLevelFunctionNames() const;
    const Array<Range<int>> &getBlockRanges() const;
    const script::Value &getParsingResult() const;
    const script::Value &getEvaluationResult() const;
    const Optional<script::ParsingError> &getParsingError() const;
    const Optional<script::EvaluationError> &getEvaluationError() const;

    int getEditorDefaultCaretPosition() const noexcept;
    int getEditorDefaultStartLine() const noexcept;
    void updateEditorDefaults(int caretPosition, int startLine) noexcept;

    //===------------------------------------------------------------------===//
    // EvaluationContext
    //===------------------------------------------------------------------===//

    Optional<script::Value>
        makeLanguageExtension(const String &valueName) const override;

    bool shouldAbort() const override;

    bool hasBreakpoints() const override;
    bool shouldBreakAt(const script::Value &value,
        const Range<int> &sourceCodeRange) const override;

    int getMaxCallStackSize() const override;
    int getMaxEvaluationTimeMs() const override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const noexcept override;
    void deserialize(const SerializedData &data) noexcept override;
    void reset() noexcept override;

private:

    // when on, disables all mutating side effects,
    // plus has lower limits on execution time and max call stack depth:
    Atomic<bool> playgroundMode = false;

    script::Scope rootScope;

    script::Value parsingResult;
    Optional<script::ParsingError> parsingError;
    // inclusive ranges of parentheses for highlighters:
    Array<Range<int>> codeBlockRanges;
    ReadWriteLock parsingResultLock;

    script::Value evaluationResult;
    Optional<script::EvaluationError> evaluationError;
    ReadWriteLock evaluationResultLock;

    Optional<Breakpoint> breakpoint;
    ReadWriteLock breakpointLock;

    // these fields are here only for serialization:
    int editorDefaultCaretPosition = 0;
    int editorDefaultStartLine = 0;

    //===------------------------------------------------------------------===//
    // Timer and thread for evaluating in background
    //===------------------------------------------------------------------===//

    void timerCallback() override
    {
        if (!this->isThreadRunning())
        {
            this->stopTimer();
            this->startThread(7);
        }
        else
        {
            this->signalThreadShouldExit();
        }
    }

    void run() override
    {
        // keep a copy, parsingResult will surely change:
        script::Value localProgram;
        {
            const ScopedReadLock lock(this->parsingResultLock);
            if (!this->parsingResult.isProgram())
            {
                return; // seems like it wasn't parsed correctly
            }

            localProgram = this->parsingResult;
        }

        try
        {
            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->evaluationError = {};
                this->resetEvaluationContext();
            }

            script::Scope scope;
            auto result = localProgram.evaluate(scope, *this);

            if (this->threadShouldExit())
            {
                return;
            }

            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->evaluationResult = move(result);
                this->rootScope = move(scope);
            }

            // DBG("Evaluated in " +
            //     String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");

            if (this->threadShouldExit())
            {
                return;
            }

            const MessageManagerLock mmLock(Thread::getCurrentThread());
            jassert(mmLock.lockWasGained());
            if (mmLock.lockWasGained())
            {
                const ScopedWriteLock lock(this->hostLock);
                if (this->hostSideEffects != nullptr)
                {
                    this->hostSideEffects->onProgramTerminated(true);
                }
            }
        }
        catch (script::EvaluationError error)
        {
            {
                const ScopedWriteLock lock(this->evaluationResultLock);
                this->evaluationError = error;
            }

            DBG("Evaluation failed in " +
                String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");

            if (this->threadShouldExit())
            {
                return;
            }

            const MessageManagerLock mmLock(Thread::getCurrentThread());
            jassert(mmLock.lockWasGained());
            if (mmLock.lockWasGained())
            {
                const ScopedWriteLock lock(this->hostLock);
                if (this->hostSideEffects != nullptr)
                {
                    this->hostSideEffects->onProgramTerminated(false);
                }
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptEngine)
};
