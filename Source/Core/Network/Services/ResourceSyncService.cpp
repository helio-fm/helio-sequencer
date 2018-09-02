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
    this->startTimer(UPDATE_INFO_TIMEOUT_MS);
}

void ResourceSyncService::timerCallback()
{
    this->stopTimer();
    this->getNewThreadFor<UpdatesCheckThread>()->checkForUpdates(this);
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

void ResourceSyncService::updatesCheckOk(const UpdatesInfo info)
{
    // TODO:
    
    // 1
    // check if current version has newer build available
    // tell dialog launcher to schedule an update dialog to be shown

    const auto platformType(getPlatformType());
    for (const auto &version : info.getVersions())
    {
        if (version.getPlatformType() == platformType)
        {

        }
    }

    // 2
    // check if any available resource has a hash different from stored one
    // then start threads to fetch those resources (with somewhat random delays)
    // store UpdateInfo in Config

    UpdatesInfo lastUpdatesInfo;
    Config::load(lastUpdatesInfo, Serialization::Config::lastUpdatesInfo);
    bool everythingIsUpToDate = true;

    Random r;
    for (const auto &newResource : info.getResources())
    {
        if (lastUpdatesInfo.seemsOutdatedFor(newResource))
        {
            // I just don't want to fire all requests at once:
            const auto delay = r.nextInt(5) * 1000;
            const auto requestThread = this->getNewThreadFor<RequestResourceThread>();
            requestThread->requestResource(this, newResource.getType(), delay);
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
}

void ResourceSyncService::updatesCheckFailed(const Array<String> &errors)
{
    Logger::writeToLog("updatesCheckFailed: " + errors.getFirst());
}

//===----------------------------------------------------------------------===//
// RequestResourceThread::Listener
//===----------------------------------------------------------------------===//

void ResourceSyncService::requestResourceOk(const Identifier &resourceId, const ValueTree &resource)
{
    App::Helio().getResourceManagerFor(resourceId).updateBaseResource(resource);
}

void ResourceSyncService::requestResourceFailed(const Identifier &resourceId, const Array<String> &errors)
{

}
