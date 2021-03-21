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
#include "ProjectSyncService.h"
#include "Config.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "ProgressTooltip.h"

#if !NO_NETWORK

void ProjectSyncService::fetchRevisionsInfo(WeakReference<VersionControl> vcs,
    const String &projectId, const String &projectName)
{
    if (auto *thread = this->getRunningThreadFor<RevisionsSyncThread>())
    {
        DBG("Warning: attempt to start a revision fetch thread while another one is running");
        return;
    }

    this->prepareFetchRevisionsThread()->doFetch(vcs, projectId, projectName);
}

void ProjectSyncService::syncRevisions(WeakReference<VersionControl> vcs,
    const String &projectId, const String &projectName,
    const Array<String> &revisionIdsToPull,
    const Array<String> &revisionIdsToPush)
{
    if (auto *thread = this->getRunningThreadFor<RevisionsSyncThread>())
    {
        DBG("Warning: attempt to start a revision sync thread while another one is running");
        return;
    }

    this->prepareSyncRevisionsThread()->doSync(vcs, projectId,
        projectName, revisionIdsToPull, revisionIdsToPush);
}

void ProjectSyncService::cancelSyncRevisions()
{
    if (auto *thread = this->getRunningThreadFor<RevisionsSyncThread>())
    {
        thread->signalThreadShouldExit();
    }
}

void ProjectSyncService::cloneProject(WeakReference<VersionControl> vcs, const String &projectId)
{
    if (auto *thread = this->getRunningThreadFor<ProjectCloneThread>())
    {
        DBG("Warning: attempt to start a project clone thread while another one is running");
        return;
    }

    App::showModalComponent(make<ProgressTooltip>(false));

    this->prepareProjectCloneThread()->doClone(vcs, projectId);
}

void ProjectSyncService::cancelCloneProject()
{
    if (auto *thread = this->getRunningThreadFor<ProjectCloneThread>())
    {
        thread->signalThreadShouldExit();
    }
}

void ProjectSyncService::deleteProject(const String &projectId)
{
    if (auto *thread = this->getRunningThreadFor<ProjectDeleteThread>())
    {
        DBG("Warning: attempt to start a project delete thread while another one is running");
        return;
    }

    App::showModalComponent(make<ProgressTooltip>(false));

    this->prepareProjectDeleteThread()->doDelete(projectId);
}

RevisionsSyncThread *ProjectSyncService::prepareSyncRevisionsThread()
{
    auto *thread = this->getNewThreadFor<RevisionsSyncThread>();
    
    thread->onFetchDone = []()
    {
        // do nothing? VCS will sendChangeMessage
        // and views will update themselves on the message thread
    };

    thread->onSyncDone = [this](bool nothingToSync)
    {
        const String message = nothingToSync ? TRANS(I18n::VCS::syncUptodate) : TRANS(I18n::VCS::syncDone);
        App::Layout().showTooltip(message, MainLayout::TooltipIcon::Success);
    };

    thread->onSyncFailed = [](const Array<String> &errors)
    {
        App::Layout().showTooltip(errors.getFirst(), MainLayout::TooltipIcon::Failure);
    };

    return thread;
}

RevisionsSyncThread *ProjectSyncService::prepareFetchRevisionsThread()
{
    auto *thread = this->getNewThreadFor<RevisionsSyncThread>();
    // No callbacks, since fetching is meant to be performed
    // transparently in a background thread.
    return thread;
}

ProjectCloneThread *ProjectSyncService::prepareProjectCloneThread()
{
    auto *thread = this->getNewThreadFor<ProjectCloneThread>();
    
    thread->onCloneDone = []()
    {
        App::Layout().showTooltip({}, MainLayout::TooltipIcon::Success);
        // do nothing? VCS will sendChangeMessage
        // and views will update themselves on the message thread
    };

    thread->onProjectMissing = [](const String &projectId)
    {
        // unload and delete the stub, remove remote info
        auto &workspace = App::Workspace();
        workspace.unloadProject(projectId, true, false);
        workspace.getUserProfile().onProjectRemoteInfoReset(projectId);
    };

    thread->onCloneFailed = [](const Array<String> &errors, const String &projectId)
    {
        App::Layout().showTooltip(errors.getFirst(), MainLayout::TooltipIcon::Failure);
        // now find project stub by id, unload and delete it locally
        App::Workspace().unloadProject(projectId, true, false);
    };

    return thread;
}

ProjectDeleteThread *ProjectSyncService::prepareProjectDeleteThread()
{
    auto *thread = this->getNewThreadFor<ProjectDeleteThread>();

    thread->onDeleteDone = [](const String &projectId)
    {
        App::Layout().showTooltip({}, MainLayout::TooltipIcon::Success);
        App::Workspace().getUserProfile().onProjectRemoteInfoReset(projectId);
    };

    thread->onDeleteFailed = [](const Array<String> &errors, const String &projectId)
    {
        App::Layout().showTooltip(errors.getFirst(), MainLayout::TooltipIcon::Failure);
    };

    return thread;
}

#endif
