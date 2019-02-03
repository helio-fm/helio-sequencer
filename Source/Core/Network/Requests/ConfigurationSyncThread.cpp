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
#include "ConfigurationSyncThread.h"
#include "SerializationKeys.h"
#include "Network.h"

namespace ApiKeys = Serialization::Api::V1;
namespace ApiRoutes = Routes::Api;

ConfigurationSyncThread::ConfigurationSyncThread() :
    Thread("Sync") {}

ConfigurationSyncThread::~ConfigurationSyncThread()
{
    this->stopThread(1000);
}

void ConfigurationSyncThread::run()
{
    WaitableEvent::wait();

    while (!this->threadShouldExit())
    {
        // while queue is not empty:
        while (!resourcesToUpload.isEmpty() && !resourcesToDelete.isEmpty())
        {
            //
        }

        callbackOnMessageThread(ConfigurationSyncThread, onQueueEmptied);
        WaitableEvent::wait();
    }
}
