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
#include "SessionService.h"

#include "Config.h"
#include "JsonSerializer.h"

#include "HelioTheme.h"
#include "App.h"

#include "SuccessTooltip.h"
#include "FailTooltip.h"

// Try to update our sliding session after 5 seconds
#define UPDATE_SESSION_TIMEOUT_MS (1000 * 5)

SessionService::SessionService() : userProfile({})
{
    Config::load(this->userProfile, Serialization::Config::activeUserProfile);
    const String token = SessionService::getApiToken();
    if (token.isNotEmpty())
    {
        // Assuming we're using JWT, try to get token expiry:
        StringArray jwtBlocks;
        jwtBlocks.addTokens(token, ".", "");
        if (jwtBlocks.size() == 3)
        {
            MemoryBlock block;
            {
                MemoryOutputStream outStream(block, false);
                Base64::convertFromBase64(outStream, jwtBlocks[1]);
            }

            ValueTree jwt;
            static JsonSerializer decoder;
            if (decoder.loadFromString(block.toString(), jwt).wasOk())
            {
                const Time now = Time::getCurrentTime();
                const Time expiry(int64(jwt.getProperty("exp")) * 1000);
                Logger::writeToLog("Found token expiring " + expiry.toString(true, true));
                if (expiry < now)
                {
                    Logger::writeToLog("Token seems to be expired, removing");
                    SessionService::setApiToken({});
                    this->resetUserProfile();
                }
                else if ((expiry - now).inDays() <= 5)
                {
                    Logger::writeToLog("Attempting to re-issue auth token");
                    this->startTimer(UPDATE_SESSION_TIMEOUT_MS);
                }

                return;
            }
        }

        Logger::writeToLog("Warning: auth token seems to be invalid, removing");
        SessionService::setApiToken({});
        this->resetUserProfile();
    }
}

String SessionService::getApiToken()
{
    return Config::get(Serialization::Api::sessionToken, {});
}

void SessionService::setApiToken(const String &token)
{
    Config::set(Serialization::Api::sessionToken, token);
}

bool SessionService::isLoggedIn()
{
    return SessionService::getApiToken().isNotEmpty();
}

const UserProfile &SessionService::getUserProfile() const noexcept
{
    return this->userProfile;
}

void SessionService::resetUserProfile()
{
    this->userProfile.reset();
    Config::save(this->userProfile, Serialization::Config::activeUserProfile);
}

//===----------------------------------------------------------------------===//
// Sign in / sign out
//===----------------------------------------------------------------------===//

void SessionService::signIn(const String &provider, AuthCallback callback)
{
    if (this->authCallback != nullptr)
    {
        jassertfalse; // You should never hit this line
        callback(false, { "Auth is already in progress" });
        return;
    }

    this->authCallback = callback;
    this->getNewThreadFor<AuthThread>()->requestWebAuth(this, provider);
}

void SessionService::cancelSignInProcess()
{
    if (auto *thread = this->getRunningThreadFor<AuthThread>())
    {
        thread->signalThreadShouldExit();
        this->authCallback = nullptr;
    }
}

void SessionService::signOut()
{
    // TODO: need to erase token on server?
    this->resetUserProfile();
    SessionService::setApiToken({});
}

//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

void SessionService::timerCallback()
{
    this->stopTimer();
    const String token = SessionService::getApiToken();
    this->getNewThreadFor<TokenUpdateThread>()->updateToken(this, token);
}

//===----------------------------------------------------------------------===//
// SignInThread::Listener
//===----------------------------------------------------------------------===//

void SessionService::authSessionInitiated(const AuthSession session, const String &redirect)
{
    jassert(redirect.isNotEmpty());
    URL(Routes::HelioFM::Web::baseURL + redirect).launchInDefaultBrowser();
}

void SessionService::authSessionFinished(const AuthSession session)
{
    SessionService::setApiToken(session.getToken());
    // Don't call authCallback right now, instead request a user profile and callback when ready
    this->getNewThreadFor<RequestUserProfileThread>()->requestUserProfile(this, this->userProfile);
}

void SessionService::authSessionFailed(const Array<String> &errors)
{
    Logger::writeToLog("Login failed: " + errors.getFirst());
    if (this->authCallback != nullptr)
    {
        this->authCallback(false, errors);
        this->authCallback = nullptr;
    }
}

//===----------------------------------------------------------------------===//
// RequestUserProfileThread::Listener
//===----------------------------------------------------------------------===//

void SessionService::requestProfileOk(const UserProfile profile)
{
    this->userProfile = profile;
    Config::save(this->userProfile, Serialization::Config::activeUserProfile);
    if (this->authCallback != nullptr)
    {
        this->authCallback(true, {});
        this->authCallback = nullptr;
    }
}

void SessionService::requestProfileFailed(const Array<String> &errors)
{
    this->resetUserProfile();
    if (this->authCallback != nullptr)
    {
        this->authCallback(false, errors);
        this->authCallback = nullptr;
    }
}

//===----------------------------------------------------------------------===//
// TokenUpdateThread::Listener
//===----------------------------------------------------------------------===//

void SessionService::tokenUpdateOk(const String &newToken)
{
    SessionService::setApiToken(newToken);
}

void SessionService::tokenUpdateFailed(const Array<String> &errors)
{
    this->resetUserProfile();
    SessionService::setApiToken({});
}

void SessionService::tokenUpdateNoResponse()
{
    // This might be the case of connection error,
    // so we should not reset the token and profile
}
