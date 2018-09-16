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
#include "ResourceSyncService.h"
#include "App.h"
#include "Config.h"
#include "ResourceManager.h"

// Try to update resources and versions info after:
#define UPDATE_INFO_TIMEOUT_MS (1000 * 10)

ResourceSyncService::ResourceSyncService()
{
    this->prepareUpdatesCheckThread()->checkForUpdates(UPDATE_INFO_TIMEOUT_MS);
}

//===----------------------------------------------------------------------===//
// UpdatesCheckThread::Listener
//===----------------------------------------------------------------------===//

static Identifier getPlatformType()
{
    using namespace Serialization::Api;

#if JUCE_WINDOWS
  #if JUCE_32BIT
    return PlatformTypes::windows32;
  #elif JUCE_64BIT
    return PlatformTypes::windows64;
  #endif
#elif JUCE_LINUX
  #if JUCE_32BIT
    return PlatformTypes::linux32;
  #elif JUCE_64BIT
    return PlatformTypes::linux64;
  #endif
#elif JUCE_MAC
    return PlatformTypes::mac;
#elif JUCE_IOS
    return PlatformTypes::ios;
#elif JUCE_ANDROID
    return PlatformTypes::android;
#else
    jassertfalse;
#endif
}

RequestResourceThread *ResourceSyncService::prepareResourceRequestThread()
{
    auto *thread = this->getNewThreadFor<RequestResourceThread>();

    thread->onRequestResourceOk = [](const Identifier &resourceId, const ValueTree &resource)
    {
        App::Helio().getResourceManagerFor(resourceId).updateBaseResource(resource);
    };
    
    return thread;
}

UpdatesCheckThread *ResourceSyncService::prepareUpdatesCheckThread()
{
    auto *thread = this->getNewThreadFor<UpdatesCheckThread>();

    thread->onUpdatesCheckOk = [this](const AppInfoDto info)
    {
        const auto platformType(getPlatformType());
        for (const auto &version : info.getVersions())
        {
            if (version.getPlatformType() == platformType)
            {
                // TODO: make update info available on the dashboard page
            }
        }

        // Check if any available resource has a hash different from stored one
        // then start threads to fetch those resources (with somewhat random delays)

        AppInfoDto lastUpdatesInfo;
        Config::load(lastUpdatesInfo, Serialization::Config::lastUpdatesInfo);
        bool everythingIsUpToDate = true;

        Random r;
        for (const auto &newResource : info.getResources())
        {
            if (lastUpdatesInfo.resourceSeemsOutdated(newResource))
            {
                // I just don't want to fire all requests at once:
                const auto delay = r.nextInt(5) * 1000;
                this->prepareResourceRequestThread()->requestResource(newResource.getType(), delay);
                everythingIsUpToDate = false;
            }
        }

        if (!everythingIsUpToDate)
        {
            Config::save(info, Serialization::Config::lastUpdatesInfo);
        }
        else
        {
            Logger::writeToLog("All resources are up to date");
        }
    };

    thread->onUpdatesCheckFailed = [](const Array<String> &errors)
    {
        Logger::writeToLog("updatesCheckFailed: " + errors.getFirst());
    };

    return thread;
}
