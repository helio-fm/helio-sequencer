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

#include "DataEncoder.h"
#include "HelioApiRoutes.h"
#include "HelioApiRequest.h"

#include "Config.h"
#include "SerializationKeys.h"

struct UserProfile final : public ReferenceCountedObject
{
    // TODO

    typedef ReferenceCountedObjectPtr<UserProfile> Ptr;

    static UserProfile::Ptr empty()
    {
        return UserProfile::Ptr(new UserProfile());
    }

};

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
        virtual void requestProfileOk(const UserProfile::Ptr profile) = 0;
        virtual void requestProfileFailed(const Array<String> &errors) = 0;
        virtual void requestProfileConnectionFailed() = 0;
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
        const HelioApiRequest request(HelioFM::Api::V1::requestUserProfile);
        this->response = request.get();

        if (this->response.result.failed())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<RequestUserProfileThread *>(ptr);
                self->listener->requestProfileConnectionFailed();
                return nullptr;
            }, this);
            return;
        }

        if (this->response.statusCode != 200)
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<RequestUserProfileThread *>(ptr);
                self->listener->requestProfileFailed(self->response.errors);
                return nullptr;
            }, this);
            return;
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
        {
            const auto self = static_cast<RequestUserProfileThread *>(ptr);
            self->listener->requestProfileOk(self->userProfile);
            return nullptr;
        }, this);
    }
    
    UserProfile::Ptr userProfile;
    HelioApiRequest::Response response;
    RequestUserProfileThread::Listener *listener;
    
    friend class SessionManager;
};
