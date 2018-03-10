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

#include "SignInThread.h"
#include "SignUpThread.h"
#include "TokenCheckThread.h"
#include "TokenUpdateThread.h"
#include "RequestResourceThread.h"
#include "RequestUserProfileThread.h"
#include "UpdatesCheckThread.h"

class SessionManager final :
    public ChangeBroadcaster,
    private Timer,
    private SignInThread::Listener,
    private SignUpThread::Listener,
    private TokenCheckThread::Listener,
    private TokenUpdateThread::Listener,
    private RequestUserProfileThread::Listener
{
public:
    
    SessionManager();

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

    template<typename T>
    T *getRequestThread()
    {
        for (const auto thread : this->requestThreads)
        {
            if (!thread->isThreadRunning())
            {
                if (T *target = dynamic_cast<T *>(thread))
                {
                    return target;
                }
            }
        }

        return static_cast<T *>(this->requestThreads.add(new T()));
    }

private:

    void timerCallback() override;

    String currentLogin;
    SessionState authState;
    UserProfile::Ptr userProfile;

    OwnedArray<Thread> requestThreads;

private:
    
    // will be called on the main thread:
    
    void signInOk(const String &userEmail, const String &newToken) override;
    void signInFailed(const Array<String> &errors) override;

    void signUpOk(const String &userEmail, const String &newToken) override;
    void signUpFailed(const Array<String> &errors) override;

    void tokenCheckOk() override;
    void tokenCheckFailed(const Array<String> &errors) override;

    void tokenUpdateOk(const String &newToken) override;
    void tokenUpdateFailed(const Array<String> &errors) override;

    void requestProfileOk(const UserProfile::Ptr profile) override;
    void requestProfileFailed(const Array<String> &errors) override;

};
