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

#include "BackendRequest.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "UserProfileDto.h"
#include "UserProfile.h"

class RequestUserProfileThread final : public Thread
{
public:

    RequestUserProfileThread() : Thread("RequestUserProfile"), profile({}) {}
    ~RequestUserProfileThread() override
    {
        this->stopThread(1000);
    }

    Function<void(const UserProfileDto profile)> onRequestProfileOk;
    Function<void(const Array<String> &errors)> onRequestProfileFailed;

    void doRequest(bool fetchAvatarData)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->shouldFetchAvatar = fetchAvatarData;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::Api;

        const BackendRequest request(ApiRoutes::userProfile);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is200())
        {
            callbackOnMessageThread(RequestUserProfileThread, onRequestProfileFailed, self->response.getErrors());
        }

        this->profile = { this->response.getBody(), {} };

        if (this->shouldFetchAvatar)
        {
            const String newProfileAvatarUrl = UserProfileDto(this->response.getBody(), {}).getAvatarUrl();
            URL(newProfileAvatarUrl).readEntireBinaryStream(this->profile.getAvatarData(), false);
        }

        callbackOnMessageThread(RequestUserProfileThread, onRequestProfileOk, self->profile);
    }
    
    bool shouldFetchAvatar = false;

    UserProfileDto profile;
    BackendRequest::Response response;

    friend class BackendService;
};
