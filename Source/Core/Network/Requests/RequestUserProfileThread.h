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

#include "HelioApiRoutes.h"
#include "HelioApiRequest.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "UserProfileDto.h"

class RequestUserProfileThread final : public Thread
{
public:

    RequestUserProfileThread() : Thread("RequestUserProfile"), listener(nullptr), profile({}) {}
    ~RequestUserProfileThread() override
    {
        this->stopThread(1000);
    }

    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void requestProfileOk(const UserProfileDto profile) = 0;
        virtual void requestProfileFailed(const Array<String> &errors) = 0;
        friend class RequestUserProfileThread;
    };
    
    void requestUserProfile(RequestUserProfileThread::Listener *authListener, UserProfileDto existingProfile)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->profile = existingProfile;
        this->listener = authListener;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api;

        const HelioApiRequest request(ApiRoutes::requestUserProfile);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is200())
        {
            callRequestListener(RequestUserProfileThread, requestProfileFailed, self->response.getErrors());
        }

        const String newProfileAvatarUrl = UserProfileDto(this->response.getBody(), {}).getAvatarUrl();
        Image profileAvatar = this->profile.getAvatar();

        if (newProfileAvatarUrl != this->profile.getAvatarUrl())
        {
            const int s = 16; // local avatar thumbnail size
            profileAvatar = { Image::RGB, s, s, true };
            MemoryBlock downloadedData;
            URL(newProfileAvatarUrl).readEntireBinaryStream(downloadedData, false);
            MemoryInputStream inputStream(downloadedData, false);
            Image remoteAvatar = ImageFileFormat::loadFrom(inputStream).rescaled(s, s, Graphics::highResamplingQuality);
            Graphics g(profileAvatar);
            g.setTiledImageFill(remoteAvatar, 0, 0, 1.f);
            g.fillAll();
        }

        this->profile = { this->response.getBody(), profileAvatar };

        callRequestListener(RequestUserProfileThread, requestProfileOk, self->profile);
    }
    
    UserProfileDto profile;
    HelioApiRequest::Response response;
    RequestUserProfileThread::Listener *listener;
    
    friend class BackendService;
};
