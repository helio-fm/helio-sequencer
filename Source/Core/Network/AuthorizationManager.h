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
#include "RequestProjectsListThread.h"

// todo rework as ProfileManager?

class AuthorizationManager :
    public ChangeBroadcaster,
    private Timer,
    private LoginThread::Listener,
    private LogoutThread::Listener,
    private RequestProjectsListThread::Listener
{
public:
    
    AuthorizationManager();
    
    ~AuthorizationManager() override;

    enum AuthState
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
    

    AuthState getAuthorizationState() const;

    RequestState getLastRequestState() const;

    String getUserLoginOfCurrentSession() const;
    
    Array<RemoteProjectDescription> getListOfRemoteProjects();


    void login(const String &login, const String &passwordHash);

    void logout();

    void requestSessionData();

private:

    bool isBusy() const;

    void timerCallback() override;

    Array<RemoteProjectDescription> lastReceivedProjects;
    String currentLogin;
    
    AuthState authState;
    RequestState lastRequestState;
    
    ScopedPointer<LoginThread> loginThread;
    ScopedPointer<LogoutThread> logoutThread;
    ScopedPointer<RequestProjectsListThread> projectListThread;

private:
    
    // will be called on the main thread:
    
    void loginOk(const String &userEmail) override;
    void loginAuthorizationFailed() override;
    void loginConnectionFailed() override;
    
    void logoutOk() override;
    void logoutFailed() override;
    void logoutConnectionFailed() override;
    
    void listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list) override;
    void listRequestAuthorizationFailed() override;
    void listRequestConnectionFailed() override;

};
