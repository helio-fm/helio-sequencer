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
#include "JsonSerializer.h"
#include "Config.h"

// Try to update our sliding session after 5 seconds
#define UPDATE_SESSION_TIMEOUT_MS (1000 * 5)

class JsonWebToken final
{
public:

    JsonWebToken(const String &token)
    {
        if (token.isNotEmpty())
        {
            StringArray blocks;
            blocks.addTokens(token, ".", "");
            if (blocks.size() == 3)
            {
                MemoryBlock block;
                {
                    MemoryOutputStream outStream(block, false);
                    Base64::convertFromBase64(outStream, blocks[1]);
                }

                const JsonSerializer decoder;
                decoder.loadFromString(block.toString(), this->jwt);
            }
        }
    }

    const bool isValid() const noexcept
    {
        return this->jwt.isValid();
    }

    const Time getExpiry() const
    {
        if (this->jwt.isValid())
        {
            return Time(int64(this->jwt.getProperty("exp")) * 1000);
        }

        return {};
    }

    const String getIssuer() const
    {
        if (this->jwt.isValid())
        {
            return this->jwt.getProperty("iss");
        }

        return {};
    }

private:

    ValueTree jwt;
};

SessionService::SessionService(UserProfile &userProfile) : userProfile(userProfile)
{
    const auto token = this->userProfile.getApiToken();
    if (token.isNotEmpty())
    {
        // Assuming we're using JWT, try to get token expiry:
        JsonWebToken jwt(token);
        if (jwt.isValid())
        {
            const Time now = Time::getCurrentTime();
            const Time expiry = jwt.getExpiry();
            Logger::writeToLog("Found token expiring " + expiry.toString(true, true));

            if (expiry < now)
            {
                Logger::writeToLog("Token seems to be expired, removing");
                this->userProfile.clearProfileAndSession();
            }
            else if ((expiry - now).inDays() <= 5)
            {
                Logger::writeToLog("Attempting to re-issue auth token");
                this->prepareTokenUpdateThread()->updateToken(token, UPDATE_SESSION_TIMEOUT_MS);
            }
            else
            {
                Logger::writeToLog("Token seems to be ok, skipping session update step");
                this->prepareProfileRequestThread()->doRequest(this->userProfile.needsAvatarImage());
            }

            return;
        }

        Logger::writeToLog("Warning: auth token seems to be invalid, removing");
        this->userProfile.clearProfileAndSession();
    }
}

//===----------------------------------------------------------------------===//
// Sign in / sign out
//===----------------------------------------------------------------------===//

void SessionService::signIn(const String &provider, AuthCallback callback)
{
    if (this->authCallback != nullptr)
    {
        jassertfalse;
        callback(false, { "Auth is already in progress" });
        return;
    }

    this->authCallback = callback;
    this->prepareAuthThread()->requestWebAuth(provider);
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
    // TODO: need to erase token on server, and then:
    this->userProfile.clearProfileAndSession();
}

//===----------------------------------------------------------------------===//
// Updating session
//===----------------------------------------------------------------------===//

AuthThread *SessionService::prepareAuthThread()
{
    auto *thread = this->getNewThreadFor<AuthThread>();

    thread->onAuthSessionInitiated = [](const AuthSessionDto session, const String &redirect)
    {
        jassert(redirect.isNotEmpty());
        URL(Routes::HelioFM::Web::baseURL + redirect).launchInDefaultBrowser();
    };

    thread->onAuthSessionFinished = [this](const AuthSessionDto session)
    {
        this->userProfile.setApiToken(session.getToken());
        // Don't call authCallback right now, instead request a user profile and callback when ready
        this->prepareProfileRequestThread()->doRequest(this->userProfile.needsAvatarImage());
    };

    thread->onAuthSessionFailed = [this](const Array<String> &errors)
    {
        Logger::writeToLog("Login failed: " + errors.getFirst());
        if (this->authCallback != nullptr)
        {
            this->authCallback(false, errors);
            this->authCallback = nullptr;
        }
    };

    return thread;
}

TokenUpdateThread *SessionService::prepareTokenUpdateThread()
{
    auto *thread = this->getNewThreadFor<TokenUpdateThread>();

    thread->onTokenUpdateOk = [this](const String &newToken)
    {
        this->userProfile.setApiToken(newToken);
        this->prepareProfileRequestThread()->doRequest(this->userProfile.needsAvatarImage());
    };

    thread->onTokenUpdateFailed = [this](const Array<String> &errors)
    {
        // This might be the case of connection error,
        // so we should not reset the token and profile
        if (!errors.isEmpty())
        {
            this->userProfile.clearProfileAndSession();
        }
    };

    return thread;
}

RequestUserProfileThread *SessionService::prepareProfileRequestThread()
{
    auto *thread = this->getNewThreadFor<RequestUserProfileThread>();

    thread->onRequestProfileOk = [this](const UserProfileDto profile)
    {
        this->userProfile.updateProfile(profile);
        if (this->authCallback != nullptr)
        {
            this->authCallback(true, {});
            this->authCallback = nullptr;
        }
    };

    thread->onRequestProfileFailed = [this](const Array<String> &errors)
    {
        if (this->authCallback != nullptr)
        {
            this->authCallback(false, errors);
            this->authCallback = nullptr;
        }
    };

    return thread;
}
