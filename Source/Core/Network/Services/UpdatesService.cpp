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
#include "UpdatesService.h"
#include "App.h"
#include "Config.h"
#include "ResourceManager.h"

// Try to update resources and versions info after:
#define UPDATE_INFO_TIMEOUT_MS (1000 * 10)

UpdatesService::UpdatesService()
{
    this->startTimer(UPDATE_INFO_TIMEOUT_MS);
}

void UpdatesService::timerCallback()
{
    this->stopTimer();
    this->getThreadFor<UpdatesCheckThread>()->checkForUpdates(this);
}

//===----------------------------------------------------------------------===//
// UpdatesCheckThread::Listener
//===----------------------------------------------------------------------===//

static Identifier getPlatformId()
{
    using namespace Serialization::Api;

#if JUCE_WINDOWS
  #if JUCE_32BIT
    return PlatformIds::windows32;
  #elif JUCE_64BIT
    return PlatformIds::windows64;
  #endif
#elif JUCE_LINUX
  #if JUCE_32BIT
    return PlatformIds::linux32;
  #elif JUCE_64BIT
    return PlatformIds::linux64;
  #endif
#elif JUCE_MAC
    return PlatformIds::mac;
#elif JUCE_IOS
    return PlatformIds::ios;
#elif JUCE_ANDROID
    return PlatformIds::android;
#else
    jassertfalse;
#endif
}

void UpdatesService::updatesCheckOk(const UpdatesInfo info)
{
    // TODO:
    
    // 1
    // check if current version has newer build available
    // tell dialog launcher to schedule an update dialog to be shown

    const auto platformId(getPlatformId());
    for (const auto &version : info.getVersions())
    {
        if (version.getPlatformId() == platformId)
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
            const auto requestThread = this->getThreadFor<RequestResourceThread>();
            requestThread->requestResource(this, newResource.getName(), delay);
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

void UpdatesService::updatesCheckFailed(const Array<String> &errors)
{
    Logger::writeToLog("updatesCheckFailed: " + errors.getFirst());
}

//===----------------------------------------------------------------------===//
// RequestResourceThread::Listener
//===----------------------------------------------------------------------===//

void UpdatesService::requestResourceOk(const Identifier &resourceId, const ValueTree &resource)
{
    App::Helio().getResourceManagerFor(resourceId).updateBaseResource(resource);
}

void UpdatesService::requestResourceFailed(const Identifier &resourceId, const Array<String> &errors)
{

}
