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
#include "AuthManager.h"

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

#define REMOTE_UPDATE_TIMEOUT_MS (1000 * 60 * 30)

AuthManager::AuthManager() :
    authState(Unknown),
    lastRequestState(RequestSucceed)
{
    this->loginThread = new LoginThread();
    this->logoutThread = new LogoutThread();
    this->projectListThread = new RequestProjectsListThread();

    this->requestSessionData();
    this->startTimer(REMOTE_UPDATE_TIMEOUT_MS);
}

AuthManager::AuthState AuthManager::getAuthorizationState() const
{
    return (this->authState);
}

AuthManager::RequestState AuthManager::getLastRequestState() const
{
    return (this->lastRequestState);
}

String AuthManager::getUserLoginOfCurrentSession() const
{
    return this->currentLogin;
}


void AuthManager::login(const String &login, const String &passwordHash)
{
    if (this->isBusy())
    {
        return;
    }
    
    this->loginThread->login(this, login, passwordHash);
}

void AuthManager::logout()
{
    if (this->isBusy())
    {
        return;
    }
    
    this->logoutThread->logout(this);
}

Array<RemoteProjectDescription> AuthManager::getListOfRemoteProjects()
{
    return this->lastReceivedProjects;
};

bool AuthManager::isBusy() const
{
    return (this->loginThread->isThreadRunning() ||
            this->logoutThread->isThreadRunning() ||
            this->projectListThread->isThreadRunning());
}


//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void AuthManager::timerCallback()
{
    this->requestSessionData();
}

void AuthManager::requestSessionData()
{
    this->projectListThread->requestListAndEmail(this);
}


//===----------------------------------------------------------------------===//
// LoginThread::Listener
//===----------------------------------------------------------------------===//

void AuthManager::loginOk(const String &userEmail)
{
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->currentLogin = userEmail;
    this->requestSessionData();
    
    // a dirty hack
    ArpeggiatorsManager::getInstance().pull();

    this->sendChangeMessage();
}

void AuthManager::loginAuthorizationFailed()
{
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
    this->sendChangeMessage();
}

void AuthManager::loginConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// LogoutThread::Listener
//===----------------------------------------------------------------------===//

void AuthManager::logoutOk()
{
    this->lastRequestState = RequestSucceed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
    this->sendChangeMessage();
}

void AuthManager::logoutFailed()
{
    this->lastRequestState = RequestFailed;
    this->sendChangeMessage();
}

void AuthManager::logoutConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// RequestProjectsListThread::Listener
//===----------------------------------------------------------------------===//

void AuthManager::listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list)
{
    //Logger::writeToLog("listRequestOk: " + userEmail);
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->lastReceivedProjects = list;
    this->currentLogin = userEmail;
    this->sendChangeMessage();
}

void AuthManager::listRequestAuthorizationFailed()
{
    //Logger::writeToLog("listRequestAuthorizationFailed: ");
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void AuthManager::listRequestConnectionFailed()
{
    //Logger::writeToLog("listRequestConnectionFailed: ");
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}

