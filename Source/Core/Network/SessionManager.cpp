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

#include "Common.h"
#include "SessionManager.h"

#include "Config.h"

#include "AuthorizationDialog.h"
#include "HelioTheme.h"
#include "App.h"

#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"

// Try to update our sliding session and reload user profile after 15 seconds
#define UPDATE_SESSION_TIMEOUT_MS (1000 * 15)

SessionManager::SessionManager() :
    authState(Unknown)
{
    this->startTimer(UPDATE_SESSION_TIMEOUT_MS);
}

String SessionManager::getApiToken()
{
    return Config::get(Serialization::Api::sessionLastToken, {});
}

SessionManager::SessionState SessionManager::getAuthorizationState() const
{
    return this->authState;
}

String SessionManager::getUserLoginOfCurrentSession() const
{
    return this->currentLogin;
}

void SessionManager::signIn(const String &login, const String &passwordHash)
{
    const auto signInThread = this->getRequestThread<SignInThread>();
    signInThread->signIn(this, login, passwordHash);
}

void SessionManager::signOut()
{
    Config::set(Serialization::Api::sessionLastUpdateTime, Time::getCurrentTime().toISO8601(true));
    Config::set(Serialization::Api::sessionLastToken, {});
    this->currentLogin = {};
    this->userProfile = UserProfile::empty();
}

//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void SessionManager::timerCallback()
{
    const String lastSessionToken = SessionManager::getApiToken();
    const Time nowMinusHalfDay = Time::getCurrentTime() - RelativeTime::hours(12);
    const Time lastSessionUpdateTime =
        Time::fromISO8601(Config::get(Serialization::Api::sessionLastUpdateTime));

    if (lastSessionToken.isNotEmpty() &&
        lastSessionUpdateTime < nowMinusHalfDay)
    {
        const auto tokenUpdateThread = this->getRequestThread<TokenUpdateThread>();
        tokenUpdateThread->updateToken(this, lastSessionToken);
    }

    const auto userProfileThread = this->getRequestThread<RequestUserProfileThread>();
    userProfileThread->requestUserProfile(this);
}

//===----------------------------------------------------------------------===//
// SignInThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::signInOk(const String &userEmail, const String &newToken)
{
    this->authState = LoggedIn;
    this->currentLogin = userEmail;
    this->sendChangeMessage();
}

void SessionManager::signInFailed(const Array<String> &errors)
{
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void SessionManager::signInConnectionFailed()
{
    this->authState = Unknown;
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// SignUpThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::signUpOk(const String &userEmail, const String &newToken)
{
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void SessionManager::signUpFailed(const Array<String> &errors)
{
    this->sendChangeMessage();
}

void SessionManager::signUpConnectionFailed()
{
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// TokenCheckThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::tokenCheckOk()
{

}

void SessionManager::tokenCheckFailed(const Array<String> &errors)
{

}

void SessionManager::tokenCheckConnectionFailed()
{

}

//===----------------------------------------------------------------------===//
// TokenUpdateThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::tokenUpdateOk(const String &newToken)
{
    Config::set(Serialization::Api::sessionLastToken, newToken);
    Config::set(Serialization::Api::sessionLastUpdateTime,
        Time::getCurrentTime().toISO8601(true));
    //this->sendChangeMessage();
}

void SessionManager::tokenUpdateFailed(const Array<String> &errors)
{
    Config::set(Serialization::Api::sessionLastToken, {});
    this->userProfile = UserProfile::empty();
    this->sendChangeMessage();
}

void SessionManager::tokenUpdateConnectionFailed()
{
    this->authState = Unknown;
    this->userProfile = UserProfile::empty();
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// RequestUserProfileThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::requestProfileOk(const UserProfile::Ptr profile)
{
    this->userProfile = profile;
    this->sendChangeMessage();
}

void SessionManager::requestProfileFailed(const Array<String> &errors)
{

}

void SessionManager::requestProfileConnectionFailed()
{

}
