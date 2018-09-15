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
#include "AuthThread.h"
#include "TokenUpdateThread.h"
#include "RequestResourceThread.h"
#include "RequestUserProfileThread.h"
#include "UpdatesCheckThread.h"

class SessionService final :
    public ChangeBroadcaster,
    private BackendService,
    private AuthThread::Listener,
    private TokenUpdateThread::Listener,
    private RequestUserProfileThread::Listener
{
public:
    
    SessionService();

    using AuthCallback = Function<void(bool, const Array<String> &)>;

    static String getApiToken();
    static bool isLoggedIn();

    const UserProfileDto &getUserProfile() const noexcept;
    
    void signIn(const String &provider, AuthCallback callback = nullptr);
    void cancelSignInProcess();
    void signOut();

private:

    void timerCallback() override;
    static void setApiToken(const String &token);

    UserProfileDto userProfile;
    void resetUserProfile();

    AuthCallback authCallback;

private:
    
    // will be called on the message thread:
    
    void authSessionInitiated(const AuthSessionDto session, const String &redirect) override;
    void authSessionFinished(const AuthSessionDto session) override;
    void authSessionFailed(const Array<String> &errors) override;

    void requestProfileOk(const UserProfileDto profile) override;
    void requestProfileFailed(const Array<String> &errors) override;

    void tokenUpdateOk(const String &newToken) override;
    void tokenUpdateFailed(const Array<String> &errors) override;
    void tokenUpdateNoResponse() override;

};
