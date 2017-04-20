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
#include "AuthorizationManager.h"

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

AuthorizationManager::AuthorizationManager() :
    authState(Unknown),
    lastRequestState(RequestSucceed)
{
    this->loginThread = new LoginThread();
    this->logoutThread = new LogoutThread();
    this->projectListThread = new RequestProjectsListThread();

    this->requestSessionData();
    this->startTimer(REMOTE_UPDATE_TIMEOUT_MS);
}

AuthorizationManager::~AuthorizationManager()
{
    //===------------------------------------------------------------------===//
}


AuthorizationManager::AuthState AuthorizationManager::getAuthorizationState() const
{
    return (this->authState);
}

AuthorizationManager::RequestState AuthorizationManager::getLastRequestState() const
{
    return (this->lastRequestState);
}

String AuthorizationManager::getUserLoginOfCurrentSession() const
{
    return this->currentLogin;
}


void AuthorizationManager::login(const String &login, const String &passwordHash)
{
    if (this->isBusy())
    {
        return;
    }
    
    this->loginThread->login(this, login, passwordHash);
}

void AuthorizationManager::logout()
{
    if (this->isBusy())
    {
        return;
    }
    
    this->logoutThread->logout(this);
}

Array<RemoteProjectDescription> AuthorizationManager::getListOfRemoteProjects()
{
    return this->lastReceivedProjects;
};

bool AuthorizationManager::isBusy() const
{
    return (this->loginThread->isThreadRunning() ||
            this->logoutThread->isThreadRunning() ||
            this->projectListThread->isThreadRunning());
}


//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void AuthorizationManager::timerCallback()
{
    this->requestSessionData();
}

void AuthorizationManager::requestSessionData()
{
    this->projectListThread->requestListAndEmail(this);
}


//===----------------------------------------------------------------------===//
// LoginThread::Listener
//===----------------------------------------------------------------------===//

void AuthorizationManager::loginOk(const String &userEmail)
{
    this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->currentLogin = userEmail;
    this->requestSessionData();
    
    // a dirty hack
    ArpeggiatorsManager::getInstance().pull();

    this->sendChangeMessage();
}

void AuthorizationManager::loginAuthorizationFailed()
{
    this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
    this->sendChangeMessage();
}

void AuthorizationManager::loginConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// LogoutThread::Listener
//===----------------------------------------------------------------------===//

void AuthorizationManager::logoutOk()
{
    this->lastRequestState = RequestSucceed;
    this->authState = NotLoggedIn;
    this->lastReceivedProjects.clear();
    
    this->sendChangeMessage();
}

void AuthorizationManager::logoutFailed()
{
    this->lastRequestState = RequestFailed;
    this->sendChangeMessage();
}

void AuthorizationManager::logoutConnectionFailed()
{
    this->lastRequestState = ConnectionFailed;
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// RequestProjectsListThread::Listener
//===----------------------------------------------------------------------===//

void AuthorizationManager::listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list)
{
	//Logger::writeToLog("listRequestOk: " + userEmail);
	this->lastRequestState = RequestSucceed;
    this->authState = LoggedIn;
    this->lastReceivedProjects = list;
    this->currentLogin = userEmail;
    this->sendChangeMessage();
}

void AuthorizationManager::listRequestAuthorizationFailed()
{
	//Logger::writeToLog("listRequestAuthorizationFailed: ");
	this->lastRequestState = RequestFailed;
    this->authState = NotLoggedIn;
    this->sendChangeMessage();
}

void AuthorizationManager::listRequestConnectionFailed()
{
	//Logger::writeToLog("listRequestConnectionFailed: ");
	this->lastRequestState = ConnectionFailed;
    this->authState = Unknown;
    this->lastReceivedProjects.clear();
    this->sendChangeMessage();
}

