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
#include "ResourceManager.h"
#include "Config.h"

#include "App.h"
#include "MainLayout.h"
#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"

// Try to update resources and versions info after:
#define UPDATE_INFO_TIMEOUT_MS (1000 * 10)

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

ResourceSyncService::ResourceSyncService(const ResourceManagerPool &rm) :
    resourceManagers(rm)
{
    this->prepareUpdatesCheckThread()->checkForUpdates(UPDATE_INFO_TIMEOUT_MS);
}

void ResourceSyncService::syncRevisions(WeakReference<VersionControl> vcs,
    const String &projectId, const String &projectName,
    const Array<String> &revisionIdsToSync)
{
    if (auto *thread = this->getRunningThreadFor<ProjectSyncThread>())
    {
        //Logger::writeToLog("Warning: attempt to start a revision sync thread while another one is running");
        return;
    }

    this->prepareProjectSyncThread()->doSync(vcs, projectId, projectName, revisionIdsToSync);
}

void ResourceSyncService::cancelSyncRevisions()
{
    if (auto *thread = this->getRunningThreadFor<ProjectSyncThread>())
    {
        thread->signalThreadShouldExit();
    }
}

void ResourceSyncService::cloneProject(WeakReference<VersionControl> vcs, const String &projectId)
{
    if (auto *thread = this->getRunningThreadFor<ProjectCloneThread>())
    {
        //Logger::writeToLog("Warning: attempt to start a project clone thread while another one is running");
        return;
    }

    ScopedPointer<ProgressTooltip> tooltip(new ProgressTooltip(false));
    App::Layout().showModalComponentUnowned(tooltip.release());

    this->prepareProjectCloneThread()->clone(vcs, projectId);
}

void ResourceSyncService::cancelCloneProject()
{
    if (auto *thread = this->getRunningThreadFor<ProjectCloneThread>())
    {
        thread->signalThreadShouldExit();
    }
}

RequestResourceThread *ResourceSyncService::prepareResourceRequestThread()
{
    auto *thread = this->getNewThreadFor<RequestResourceThread>();

    thread->onRequestResourceOk = [this](const Identifier &resourceId, const ValueTree &resource)
    {
        if (this->resourceManagers.contains(resourceId))
        {
            this->resourceManagers.at(resourceId)->updateBaseResource(resource);
        }
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
            if (version.getPlatformType().equalsIgnoreCase(platformType))
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

        if (everythingIsUpToDate)
        {
            Logger::writeToLog("All resources are up to date");
        }

        // Versions info might have changed:
        Config::save(info, Serialization::Config::lastUpdatesInfo);
    };

    thread->onUpdatesCheckFailed = [](const Array<String> &errors)
    {
        Logger::writeToLog("updatesCheckFailed: " + errors.getFirst());
    };

    return thread;
}

ProjectSyncThread *ResourceSyncService::prepareProjectSyncThread()
{
    auto *thread = this->getNewThreadFor<ProjectSyncThread>();
    
    thread->onFetchDone = []()
    {
        // do nothing? VCS will sendChangeMessage
        // and views will update themselves on the message thread
    };

    thread->onSyncDone = [this](bool nothingToSync)
    {
        auto &layout = App::Layout();
        layout.hideModalComponentIfAny();
        layout.showTooltip(nothingToSync ? TRANS("vcs::sync::uptodate") : TRANS("vcs::sync::done"));
        layout.showModalComponentUnowned(new SuccessTooltip());
    };

    thread->onSyncFailed = [](const Array<String> &errors)
    {
        auto &layout = App::Layout();
        layout.hideModalComponentIfAny();
        layout.showTooltip(errors.getFirst());
        layout.showModalComponentUnowned(new FailTooltip());
    };

    return thread;
}

ProjectCloneThread *ResourceSyncService::prepareProjectCloneThread()
{
    auto *thread = this->getNewThreadFor<ProjectCloneThread>();
    
    thread->onCloneDone = []()
    {
        auto &layout = App::Layout();
        layout.hideModalComponentIfAny();
        layout.showModalComponentUnowned(new SuccessTooltip());
        // do nothing? VCS will sendChangeMessage
        // and views will update themselves on the message thread
    };

    thread->onCloneFailed = [](const Array<String> &errors)
    {
        auto &layout = App::Layout();
        layout.hideModalComponentIfAny();
        layout.showTooltip(errors.getFirst());
        layout.showModalComponentUnowned(new FailTooltip());

        // TODO find project stub by id and delete
        //App::Workspace().deleteProject(id)
    };

    return thread;
}
