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
#include "SerializationKeys.h"
#include "VersionControl.h"
#include "AuthSession.h"
#include "Config.h"
#include "App.h"

class PullThread final : public Thread
{
public:
    
    PullThread() : Thread("Pull") {}
    ~PullThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        // TODO
        friend class PullThread;
    };
    
    void pull(PullThread::Listener *listener, VersionControl *vcs)
    {
        if (this->isThreadRunning())
        {
            Logger::writeToLog("Warning: failed to start pull thread, already running");
            return;
        }

        // TOOD
        this->listener = listener;
        this->startThread(3);
    }

private:
    
    void run() override
    {
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api;
        
        // TODO
    }
    
    HelioApiRequest::Response response;
    PullThread::Listener *listener = nullptr;

    friend class BackendService;
};
