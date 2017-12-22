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

class HelioLogger : public Logger,
                    public ChangeBroadcaster
{
public:

    String getText()
    {
#if JUCE_DEBUG
        const ScopedReadLock lock(this->logLock);
        return this->log;
#else
        return String::empty;
#endif
    }

protected:

    void logMessage(const String &message) override
    {
#if JUCE_DEBUG
        const ScopedWriteLock lock(this->logLock);
        this->log += message;
        this->log += "\n";
        Logger::outputDebugString(message);
        this->sendChangeMessage();
#endif
    }

private:

    ReadWriteLock logLock;

    String log;

};
