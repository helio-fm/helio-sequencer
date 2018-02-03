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
#include "RequestProjectsListThread.h"

#include "AuthorizationDialog.h"
#include "HelioTheme.h"
#include "App.h"

#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"

#include "ArpeggiatorsManager.h"

// Try to update our sliding session and reload projects list after 15 seconds
#define UPDATE_SESSION_TIMEOUT_MS (1000 * 15)

SessionManager::SessionManager() :
    authState(Unknown),
    lastRequestState(RequestSucceed)
{
    this->loginThread = new LoginThread();
    this->logoutThread = new LogoutThread();
    this->projectListThread = new RequestProjectsListThread();

    this->startTimer(UPDATE_SESSION_TIMEOUT_MS);
}

SessionManager::SessionState SessionManager::getAuthorizationState() const
{
    return (this->authState);
}

SessionManager::RequestState SessionManager::getLastRequestState() const
{
    return (this->lastRequestState);
}

String SessionManager::getUserLoginOfCurrentSession() const
{
    return this->currentLogin;
}


void SessionManager::login(const String &login, const String &passwordHash)
{
    this->loginThread->login(this, login, passwordHash);
}

void SessionManager::logout()
{
    // TODO just remove the token and cleanup projects list?
    this->logoutThread->logout(this);
}

Array<RemoteProjectDescription> SessionManager::getListOfRemoteProjects()
{
    return this->lastReceivedProjects;
};

//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void SessionManager::timerCallback()
{
    const Time nowMinusHalfDay = Time::getCurrentTime() - RelativeTime::hours(12);
    const Time lastSessionUpdateTime = Time::fromISO8601(Config::get(Serialization::Core::sessionLastUpdateTime));
    const String lastSessionToken = Config::get(Serialization::Core::sessionLastToken);
    const String deviceId = Config::getMachineId();
    if (lastSessionToken.isNotEmpty() &&
        lastSessionUpdateTime < nowMinusHalfDay)
    {
        this->reloginThread->relogin(this, lastSessionToken, deviceId);
    }

    this->reloadRemoteProjectsList();
}

void SessionManager::reloadRemoteProjectsList()
{
    this->projectListThread->requestListAndEmail(this);
}

//===----------------------------------------------------------------------===//
// ReloginThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::reloginOk(const String &newToken)
{
    Config::set(Serialization::Core::sessionLastUpdateTime, Time::getCurrentTime().toISO8601(true));
    Config::set(Serialization::Core::sessionLastToken, newToken);
    this->sendChangeMessage();
}

void SessionManager::reloginAuthorizationFailed()
{
    Config::set(Serialization::Core::sessionLastToken, {});
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

void SessionManager::loginOk(const String &userEmail, const String &newToken)
{
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->currentLogin = userEmail;
    this->reloadRemoteProjectsList();
    
    // a dirty hack
    ArpeggiatorsManager::getInstance().pull();

    this->sendChangeMessage();
}

void SessionManager::loginAuthorizationFailed()
{
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
    this->sendChangeMessage();
}

void SessionManager::loginConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// LogoutThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::logoutOk()
{
    this->lastRequestState = RequestSucceed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
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


//===----------------------------------------------------------------------===//
// RequestProjectsListThread::Listener
//===----------------------------------------------------------------------===//

void SessionManager::listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list)
{
    //Logger::writeToLog("listRequestOk: " + userEmail);
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->lastReceivedProjects = list;
    this->currentLogin = userEmail;
    this->sendChangeMessage();
}

void SessionManager::listRequestAuthorizationFailed()
{
    //Logger::writeToLog("listRequestAuthorizationFailed: ");
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void SessionManager::listRequestConnectionFailed()
{
    //Logger::writeToLog("listRequestConnectionFailed: ");
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}

