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
#include "LoginThread.h"
#include "LogoutThread.h"

#include "AuthorizationDialog.h"
#include "HelioTheme.h"
#include "App.h"

#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"

#include "ArpeggiatorsManager.h"

// Try to update our sliding session and reload user profile after 15 seconds
#define UPDATE_SESSION_TIMEOUT_MS (1000 * 15)

SessionManager::SessionManager() :
    authState(Unknown),
    lastRequestState(RequestSucceed)
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

SessionManager::RequestState SessionManager::getLastRequestState() const
{
    return this->lastRequestState;
}

String SessionManager::getUserLoginOfCurrentSession() const
{
    return this->currentLogin;
}


void SessionManager::login(const String &login, const String &passwordHash)
{
    const auto signInThread = this->getRequestThread<SignInThread>();
    signInThread->login(this, login, passwordHash);
}

void SessionManager::logout()
{
    // TODO just remove the token and cleanup projects list?
    this->logoutThread->logout(this);
}

//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void SessionManager::timerCallback()
{
    const Time nowMinusHalfDay = Time::getCurrentTime() - RelativeTime::hours(12);
    const Time lastSessionUpdateTime = Time::fromISO8601(Config::get(Serialization::Api::sessionLastUpdateTime));
    const String lastSessionToken = SessionManager::getApiToken();
    const String deviceId = Config::getMachineId();
    if (lastSessionToken.isNotEmpty() &&
        lastSessionUpdateTime < nowMinusHalfDay)
    {
        this->reloginThread->relogin(this, lastSessionToken, deviceId);
    }

    // TODO reloadUserProfile
}

//===----------------------------------------------------------------------===//
// ReloginThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::reloginOk(const String &newToken)
{
    Config::set(Serialization::Api::sessionLastUpdateTime, Time::getCurrentTime().toISO8601(true));
    Config::set(Serialization::Api::sessionLastToken, newToken);
    this->sendChangeMessage();
}

void SessionManager::reloginAuthorizationFailed()
{
    Config::set(Serialization::Api::sessionLastToken, {});
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}

void SessionManager::reloginConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// LoginThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::signInOk(const String &userEmail, const String &newToken)
{
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->currentLogin = userEmail;
    
    // a dirty hack
    ArpeggiatorsManager::getInstance().pull();

    this->sendChangeMessage();
}

void SessionManager::signInAuthorizationFailed()
{
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void SessionManager::signInConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// LogoutThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::logoutOk()
{
    this->lastRequestState = RequestSucceed;
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void SessionManager::logoutFailed()
{
    this->lastRequestState = RequestFailed;
    this->sendChangeMessage();
}

void SessionManager::logoutConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->sendChangeMessage();
}
