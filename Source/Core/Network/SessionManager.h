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

#include "LoginThread.h"
#include "LogoutThread.h"
#include "TokenCheckThread.h"
#include "TokenUpdateThread.h"
#include "RequestResourceThread.h"
#include "RequestUserProfileThread.h"
#include "UpdatesCheckThread.h"

class SessionManager :
    public ChangeBroadcaster,
    private Timer,
    private LoginThread::Listener,
    private LogoutThread::Listener,
    private RequestProjectsListThread::Listener
{
public:
    
    SessionManager();

    class Token
    {
    public:
        enum State
        {
            Unknown,
            Valid,
            Expired
        };

    private:

        State state;
        Time timeIssued;
        String payload;
    };

    enum SessionState
    {
        LoggedIn,
        NotLoggedIn,
        Unknown
    };
    
    enum RequestState
    {
        RequestSucceed,
        RequestFailed,
        ConnectionFailed
    };

    SessionState getAuthorizationState() const;
    RequestState getLastRequestState() const;
    String getUserLoginOfCurrentSession() const;
    
    void reloadRemoteProjectsList();
    Array<RemoteProjectDescription> getListOfRemoteProjects();

    void login(const String &login, const String &passwordHash);
    void logout();

private:

    void timerCallback() override;

    Array<RemoteProjectDescription> lastReceivedProjects;
    String currentLogin;
    
    SessionState authState;
    RequestState lastRequestState;
    
    ScopedPointer<LoginThread> loginThread;
    ScopedPointer<LogoutThread> logoutThread;
    ScopedPointer<RequestProjectsListThread> projectListThread;

private:
    
    // will be called on the main thread:
    
    void loginOk(const String &userEmail, const String &newToken) override;
    void loginAuthorizationFailed() override;
    void loginConnectionFailed() override;
    
    void logoutOk() override;
    void logoutFailed() override;
    void logoutConnectionFailed() override;
    
    void listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list) override;
    void listRequestAuthorizationFailed() override;
    void listRequestConnectionFailed() override;

};
