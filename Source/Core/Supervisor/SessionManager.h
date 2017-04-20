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
#include "Session.h"

class SessionManager :
    public Serializable,
    private Thread
{
public:

    SessionManager();

    ~SessionManager() override;


    void addOrUpdateSession(const Session *session);

    void submitSessions();

    void stopSubmit();


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;

private:

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;

private:

    ReadWriteLock sessionsLock;

    OwnedArray<Session> sessions;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SessionManager);

};
