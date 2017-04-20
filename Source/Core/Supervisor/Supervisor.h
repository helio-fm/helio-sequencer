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

#include "Serializable.h"

class Session;
class SessionManager;
class SaveLoadManager;

class Supervisor : private Serializable
{
public:

    static void track(const String &key);

public:

    Supervisor();
    ~Supervisor() override;

    void trackActivity(const String &key);
    void trackException(const std::exception *e, const String &sourceFilename, int lineNumber);
    void trackCrash();

private:

    void loadSessions();
    void saveSessions();

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    ReadWriteLock sessionLock;
    ScopedPointer<Session> currentSession;
    ScopedPointer<SessionManager> sessionManager;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Supervisor);

};
