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
#include "ProjectSyncThread.h"
#include "HelioApiRoutes.h"
#include "SerializationKeys.h"
#include "RevisionDto.h"
#include "ProjectDto.h"
#include "ProjectInfo.h"

ProjectSyncThread::ProjectSyncThread() : Thread("Push") {}

ProjectSyncThread::~ProjectSyncThread()
{
    this->stopThread(1000);
}

void ProjectSyncThread::doSync(WeakReference<VersionControl> vcs, WeakReference<ProjectTreeItem> project)
{
    if (this->isThreadRunning())
    {
        Logger::writeToLog("Warning: failed to start push thread, already running");
        return;
    }

    this->vcs = vcs;
    this->project = project;
    this->startThread(7);
}

using RevisionsMap = SparseHashMap<String, VCS::Revision::Ptr, StringHash>;
static void buildLocalRevisionsIndex(RevisionsMap &map, VCS::Revision::Ptr root)
{
    map[root->getUuid()] = root;
    for (const auto child : root->getChildren())
    {
        buildLocalRevisionsIndex(map, child);
    }
}

static ReferenceCountedArray<VCS::Revision> constructRevisionTrees(const Array<VCS::Revision::Ptr> &list)
{
    ReferenceCountedArray<VCS::Revision> trees;
    for (const auto dto : list)
    {
        if (VCS::Revision::Ptr parent = findParentIn(trees))
        {
            parent->addChild(dto);
        }
        else
        {
            trees.add(dto);
        }
    }
}

static VCS::Revision::Ptr createShallowRevision(const RevisionDto dto)
{
    return { new VCS::Revision(nullptr, dto.getMessage()) };
}

void ProjectSyncThread::run()
{
    namespace ApiKeys = Serialization::Api::V1;
    namespace ApiRoutes = Routes::HelioFM::Api;

    RevisionsMap localRevisions;
    buildLocalRevisionsIndex(localRevisions, this->vcs->getRoot());

    const String projectRoute(ApiRoutes::projects.replace(":projectId", this->project->getId()));
    const BackendRequest revisionsRequest(projectRoute);
    this->response = revisionsRequest.get();

    const ProjectDto remoteProject(this->response.getBody());

    if (this->response.is(404))
    {
        // Put the project:
        const BackendRequest createProjectRequest(projectRoute);
        ValueTree payload(ApiKeys::session);
        // head reference will be put later when all revisions are pushed
        payload.setProperty(ApiKeys::Projects::title, this->project->getProjectInfo()->getFullName(), nullptr);
        this->response = createProjectRequest.put(payload);
        if (!this->response.is2xx())
        {
            Logger::writeToLog("Failed to create the project on remote: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(ProjectSyncThread, onSyncFailed, self->response.getErrors());
            return;
        }
    }
    else if (!this->response.is200())
    {
        Logger::writeToLog("Failed to fetch project heads from remote: " + this->response.getErrors().getFirst());
        callbackOnMessageThread(ProjectSyncThread, onSyncFailed, self->response.getErrors());
        return;
    }

    RevisionsMap remoteRevisions;
    for (const auto child : remoteProject.getRevisions())
    {
        VCS::Revision::Ptr shallowRevision(createShallowRevision(child));
        remoteRevisions[shallowRevision->getUuid()] = shallowRevision;
    }

    // Find all new revisions on the remote
    Array<VCS::Revision::Ptr> newRemoteRevisions;
    for (const auto remoteRevision : remoteRevisions)
    {
        if (!localRevisions.contains(remoteRevision.second->getUuid()))
        {
            newRemoteRevisions.add(remoteRevision.second);
        }
    }

    // Find all new revisions locally
    Array<VCS::Revision::Ptr> newLocalRevisions;
    for (const auto &localRevision : localRevisions)
    {
        if (!remoteRevisions.contains(localRevision.second->getUuid()))
        {
            newLocalRevisions.add(localRevision.second);
        }
    }

    // Everything is up to date
    if (newLocalRevisions.isEmpty() && newRemoteRevisions.isEmpty())
    {
        if (this->onSyncDone != nullptr)
        {
            this->onSyncDone(true, 0, 0);
        }
        return;
    }

    // build tree(s) of shallow VCS::Revision from newRemoteRevisions list
    const auto newRemoteTrees = constructRevisionTrees(newLocalRevisions);
    // append them to VCS,
    // callback that fetch is done,

    // if anything is needed to pull,
    // fetch all data for each shallow revision, update and callback

    // if anything is needed to push,
    // build tree(s) from newLocalRevisions list
    const auto newLocalTrees = constructRevisionTrees(newLocalRevisions);
    // push them recursively, starting from the root, so that
    // each pushed revision already has a valid remote parent

}
