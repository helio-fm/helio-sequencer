/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class DebugLogger : public Logger,
                    public ChangeBroadcaster
{
public:

    DebugLogger() = default;

    String getText() const
    {
#if JUCE_DEBUG
        const ScopedReadLock lock(this->logLock);
        return this->log;
#else
        return {};
#endif
    }

protected:

#if JUCE_DEBUG
    void logMessage(const String &message) override
    {
        const ScopedWriteLock lock(this->logLock);
        this->log += message;
        this->log += newLine;
        Logger::outputDebugString(message);
        this->sendChangeMessage();
    }
#else
    void logMessage(const String &) override {}
#endif

private:

    ReadWriteLock logLock;
    String log;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugLogger)
};
