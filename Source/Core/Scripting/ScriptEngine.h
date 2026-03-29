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
class KeySignatureEvent;

#include "Interpreter.h"
#include "Arpeggiator.h"

class ScriptEngine final :
    public script::EvaluationContext,
    public Thread,
    public Timer
{
public:

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
        virtual void addNotes(MidiTrack *track, Array<Note> &notes) = 0;
        virtual void joinAdjacent(MidiTrack *track) = 0;
        virtual void arpeggiate(MidiTrack *track, Arpeggiator::Ptr arp) = 0;
        virtual void alignToScale(MidiTrack *track) = 0;

        struct HostContext final
        {
            // todo key and time signatures here
            Array<Scale::Ptr> allScales;
            int projectPeriodSize;
        };
        virtual HostContext fillHostContext() const = 0;

        // todo all refactorings
        //virtual void refactorJoinAdjacent(MidiTrack *track) = 0;
    };

    struct Breakpoint final
    {
        String symbolName;
        Range<int> parentListRange;
    };

    SideEffects &sideEffects;

    SideEffects::HostContext hostContext;
    ReadWriteLock hostContextLock;

    explicit ScriptEngine(SideEffects &sideEffects);

    bool evaluate(const String &code,
        bool shouldEvaluateInPlayground,
        Optional<Breakpoint> breakpoint = {});

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isPlayground() const noexcept;
    StringArray getTopLevelFunctionNames() const;
    const Array<Range<int>> &getBlockRanges() const;
    const script::Value &getParsingResult() const;
    const script::Value &getEvaluationResult() const;
    const Optional<script::ParsingError> &getParsingError() const;
    const Optional<script::EvaluationError> &getEvaluationError() const;

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

private:

    // when on, disables all side effects and only validates things,
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

    //===------------------------------------------------------------------===//
    // Timer and thread for evaluating in background
    //===------------------------------------------------------------------===//

    void timerCallback() override
    {
        if (!this->isThreadRunning())
        {
            this->stopTimer();
            this->startThread();
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

            DBG("Evaluated in " +
                String(Time::getMillisecondCounter() - this->startTime.get()) + " ms");

            if (this->threadShouldExit())
            {
                return;
            }

            const MessageManagerLock lock(Thread::getCurrentThread());
            jassert(lock.lockWasGained());
            if (lock.lockWasGained())
            {
                this->sideEffects.onProgramTerminated(true);
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

            const MessageManagerLock lock(Thread::getCurrentThread());
            jassert(lock.lockWasGained());
            if (lock.lockWasGained())
            {
                this->sideEffects.onProgramTerminated(false);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptEngine)
};
