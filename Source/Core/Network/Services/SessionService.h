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

#include "BackendService.h"
#include "SignInThread.h"
#include "SignUpThread.h"
#include "TokenUpdateThread.h"
#include "RequestResourceThread.h"
#include "RequestUserProfileThread.h"
#include "UpdatesCheckThread.h"

class SessionService final :
    private BackendService,
    private SignInThread::Listener,
    private SignUpThread::Listener,
    private TokenUpdateThread::Listener,
    private RequestUserProfileThread::Listener
{
public:
    
    SessionService();

    static String getApiToken();

    enum SessionState
    {
        LoggedIn,
        NotLoggedIn,
        Unknown
    };

    SessionState getAuthorizationState() const;
    String getUserLoginOfCurrentSession() const;
    
    void signIn(const String &login, const String &passwordHash);
    void signOut();

private:

    void timerCallback() override;

    SessionState authState;
    UserProfile userProfile;
    
private:
    
    // will be called on the main thread:
    
    void signInOk(const String &userEmail, const String &newToken) override;
    void signInFailed(const Array<String> &errors) override;

    void signUpOk(const String &userEmail, const String &newToken) override;
    void signUpFailed(const Array<String> &errors) override;

    void requestProfileOk(const UserProfile profile) override;
    void requestProfileFailed(const Array<String> &errors) override;

    void tokenUpdateOk(const String &newToken) override;
    void tokenUpdateFailed(const Array<String> &errors) override;
    void tokenUpdateNoResponse() override;

};
