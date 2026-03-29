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

Optional<script::Value> ScriptEngine::makeLanguageExtension(const String &name) const
{
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
    return this->playgroundMode.get() ? 1337 : 42069;
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
