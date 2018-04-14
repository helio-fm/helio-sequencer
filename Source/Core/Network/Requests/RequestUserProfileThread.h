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
#include "UserProfile.h"

class RequestUserProfileThread final : private Thread
{
public:

    RequestUserProfileThread() : Thread("RequestUserProfile"), listener(nullptr) {}
    ~RequestUserProfileThread() override
    {
        this->stopThread(1000);
    }

    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void requestProfileOk(const UserProfile profile) = 0;
        virtual void requestProfileFailed(const Array<String> &errors) = 0;
        friend class RequestUserProfileThread;
    };
    
    void requestUserProfile(RequestUserProfileThread::Listener *authListener)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->listener = authListener;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api::V1;

        const HelioApiRequest request(ApiRoutes::requestUserProfile);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is200())
        {
            callRequestListener(RequestUserProfileThread, requestProfileFailed, self->response.getErrors());
        }

        callRequestListener(RequestUserProfileThread, requestProfileOk, { self->response.getBody() });
    }
    
    HelioApiRequest::Response response;
    RequestUserProfileThread::Listener *listener;
    
    friend class BackendService;
};
