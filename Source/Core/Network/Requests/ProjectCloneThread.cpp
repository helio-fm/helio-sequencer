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
#include "ProjectCloneThread.h"
#include "RevisionsSyncHelpers.h"
#include "ProjectDto.h"
#include "Network.h"

namespace ApiKeys = Serialization::Api::V1;

ProjectCloneThread::ProjectCloneThread() : Thread("Clone") {}

ProjectCloneThread::~ProjectCloneThread()
{
    this->stopThread(1000);
}

void ProjectCloneThread::doClone(WeakReference<VersionControl> vcs, const String &projectId)
{
    if (this->isThreadRunning())
    {
        DBG("Warning: failed to start project clone thread, already running");
        return;
    }

    this->vcs = vcs;
    this->projectId = projectId;
    this->startThread(7);
}

void ProjectCloneThread::run()
{
    const String projectRoute(Routes::Api::project.replace(":projectId", this->projectId));
    const BackendRequest revisionsRequest(projectRoute);
    this->response = revisionsRequest.get();

    const ProjectDto remoteProject(this->response.getBody());
    if (this->response.is(404))
    {
        DBG("Attempted to clone non-existing project, removing");
        callbackOnMessageThread(ProjectCloneThread, onProjectMissing, self->projectId);
        return;
    }
    else if (!this->response.is200())
    {
        DBG("Failed to clone project from remote: " + this->response.getErrors().getFirst());
        callbackOnMessageThread(ProjectCloneThread, onCloneFailed, self->response.getErrors(), self->projectId);
        return;
    }

    // the info about what revisions are available remotely will be needed by revision tree:
    this->vcs->updateRemoteSyncCache(remoteProject.getRevisions());
        
    // build tree(s) of shallow VCS::Revision from newRemoteRevisions list and append them to VCS
    const auto remoteHistory = RevisionsSyncHelpers::constructRemoteTree(remoteProject.getRevisions());
    if (remoteHistory == nullptr)
    {
        DBG("Failed to construct remote history tree");
        callbackOnMessageThread(ProjectCloneThread, onCloneFailed, {}, self->projectId);
    }

    this->vcs->replaceHistory(remoteHistory);

    this->newHead = nullptr;

    // if anything is needed to pull, fetch all data for each, then update and callback
    for (const auto dto : remoteProject.getRevisions())
    {
        const String revisionRoute(Routes::Api::projectRevision
            .replace(":projectId", this->projectId)
            .replace(":revisionId", dto.getId()));

        const BackendRequest revisionRequest(revisionRoute);
        this->response = revisionRequest.get();
        if (!this->response.is2xx())
        {
            DBG("Failed to fetch revision data: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(ProjectCloneThread, onCloneFailed, self->response.getErrors(), self->projectId);
            return;
        }

        const RevisionDto fullData(this->response.getBody());
        auto revision = this->vcs->updateShallowRevisionData(fullData.getId(), fullData.getData());

        // if project's head is null, this will at least point the new head to one of leafs:
        if ((this->newHead == nullptr && revision->getChildren().isEmpty()) ||
            revision->getUuid() == remoteProject.getHead())
        {
            this->newHead = revision;
        }
    }

    jassert(this->newHead != nullptr);

    // checkout the head revision
    MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
    {
        auto *self = static_cast<ProjectCloneThread *>(ptr);
        self->vcs->checkout(self->newHead);
        return nullptr;
    }, this);

    callbackOnMessageThread(ProjectCloneThread, onCloneDone);
}
