/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "RevisionsSyncThread.h"
#include "ProjectDto.h"
#include "RevisionsSyncHelpers.h"
#include "Workspace.h"
#include "NetworkServices.h"

#if !NO_NETWORK

namespace ApiKeys = Serialization::Api::V1;
namespace ApiRoutes = Routes::Api;

RevisionsSyncThread::RevisionsSyncThread() :
    Thread("Sync"), fetchOnly(false) {}

RevisionsSyncThread::~RevisionsSyncThread()
{
    this->stopThread(1000);
}

void RevisionsSyncThread::doFetch(WeakReference<VersionControl> vcs,
    const String &projectId, const String &projectName)
{
    if (this->isThreadRunning())
    {
        DBG("Warning: failed to start revision fetch thread, already running");
        return;
    }

    this->fetchOnly = true;
    this->projectId = projectId;
    this->projectName = projectName;
    this->vcs = vcs;
    this->idsToPull = {};
    this->idsToPush = {};
    this->startThread(2); // bg fetching is a really low priority task
}

void RevisionsSyncThread::doSync(WeakReference<VersionControl> vcs,
    const String &projectId, const String &projectName,
    const Array<String> &revisionIdsToPull,
    const Array<String> &revisionIdsToPush)
{
    if (this->isThreadRunning())
    {
        DBG("Warning: failed to start revision sync thread, already running");
        return;
    }

    this->fetchOnly = false;
    this->projectId = projectId;
    this->projectName = projectName;

    this->vcs = vcs;
    this->idsToPull = revisionIdsToPull;
    this->idsToPush = revisionIdsToPush;
    this->startThread(7);
}

void RevisionsSyncThread::run()
{
    RevisionsMap localRevisions;
    RevisionsSyncHelpers::buildLocalRevisionsIndex(localRevisions, this->vcs->getRoot());

    const String projectRoute(ApiRoutes::project.replace(":projectId", this->projectId));
    const BackendRequest revisionsRequest(projectRoute);
    this->response = revisionsRequest.get();

    const ProjectDto remoteProject(this->response.getBody());

    if (this->response.is(404))
    {
        if (!this->fetchOnly)
        {
            // Put the project:
            const BackendRequest createProjectRequest(projectRoute);
            SerializedData payload(ApiKeys::Projects::project);
            // head reference will be put later when all revisions are pushed
            payload.setProperty(ApiKeys::Projects::title, this->projectName);
            this->response = createProjectRequest.put(payload);
            if (!this->response.is2xx())
            {
                DBG("Failed to create the project on remote: " + this->response.getErrors().getFirst());
                callbackOnMessageThread(RevisionsSyncThread, onSyncFailed, self->response.getErrors());
                return;
            }

            App::Workspace().getUserProfile().onProjectRemoteInfoUpdated({ this->response.getBody() });
        }
    }
    else if (!this->response.is200())
    {
        DBG("Failed to fetch project heads from remote: " + this->response.getErrors().getFirst());
        this->vcs->updateRemoteSyncCache({});
        callbackOnMessageThread(RevisionsSyncThread, onSyncFailed, self->response.getErrors());
        return;
    }

    // the info about what revisions are available remotely will be needed by revision tree:
    this->vcs->updateRemoteSyncCache(remoteProject.getRevisions());
    
    using RevisionDtosMap = FlatHashMap<String, RevisionDto, StringHash>;

    RevisionDtosMap remoteRevisions;
    for (const auto &child : remoteProject.getRevisions())
    {
        remoteRevisions[child.getId()] = child;
    }

    // find all new revisions on the remote
    Array<RevisionDto> newRemoteRevisions;
    for (const auto &remoteRevision : remoteRevisions)
    {
        if (!localRevisions.contains(remoteRevision.second.getId()))
        {
            newRemoteRevisions.add(remoteRevision.second);
        }
    }

    // find all new revisions locally
    ReferenceCountedArray<VCS::Revision> newLocalRevisions;
    for (const auto &localRevision : localRevisions)
    {
        if (!remoteRevisions.contains(localRevision.second->getUuid()))
        {
            newLocalRevisions.add(localRevision.second);
        }
    }

    // everything is up to date
    if (newLocalRevisions.isEmpty() && newRemoteRevisions.isEmpty())
    {
        callbackOnMessageThread(RevisionsSyncThread, onSyncDone, true);
        return;
    }

    // build tree(s) of shallow VCS::Revision from newRemoteRevisions list and append them to VCS
    const auto newRemoteSubtrees = RevisionsSyncHelpers::constructRemoteBranches(newRemoteRevisions);
    for (auto subtree : newRemoteSubtrees)
    {
        this->vcs->appendSubtree(subtree.second, subtree.first);
    }

    // callback that fetch is done and stop if that's all we need
    callbackOnMessageThread(RevisionsSyncThread, onFetchDone);

    if (this->fetchOnly)
    {
        return;
    }

    Array<String> remoteRevisionsToPull;
    if (!this->idsToPull.isEmpty())
    {
        // if told explicitly to sync some known revisions, only add them
        // (assuming they are all shallow copies, if you're getting exception
        // from updateShallowRevisionData() below, make sure to pass the correct ids):
        remoteRevisionsToPull.addArray(this->idsToPull);
    }
    else
    {
        // otherwise pull all new revisions, if any:
        for (const auto &dto : newRemoteRevisions)
        {
            remoteRevisionsToPull.addIfNotAlreadyThere(dto.getId());
        }
    }

    // if anything is needed to pull, fetch all data for each, then update and callback
    for (const auto &revisionId : remoteRevisionsToPull)
    {
        const String revisionRoute(ApiRoutes::projectRevision
            .replace(":projectId", this->projectId)
            .replace(":revisionId", revisionId));

        const BackendRequest revisionRequest(revisionRoute);
        this->response = revisionRequest.get();
        if (!this->response.is2xx())
        {
            DBG("Failed to fetch revision data: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(RevisionsSyncThread, onSyncFailed, self->response.getErrors());
            return;
        }

        const RevisionDto fullRevision(this->response.getBody());
        const auto revision = this->vcs->updateShallowRevisionData(fullRevision.getId(), fullRevision.getData());
    }

    // if anything is needed to push,
    // build tree(s) from newLocalRevisions list
    const auto newLocalTrees = RevisionsSyncHelpers::constructNewLocalTrees(newLocalRevisions);

    // push them recursively, starting from the root, so that
    // each pushed revision already has a valid remote parent
    for (auto *subtree : newLocalTrees)
    {
        this->pushSubtreeRecursively(subtree);
    }

    // finally, update project head ref
    const BackendRequest createProjectRequest(projectRoute);
    SerializedData payload(ApiKeys::Projects::project);
    // head reference will be put later when all revisions are pushed
    payload.setProperty(ApiKeys::Projects::title, this->projectName);
    payload.setProperty(ApiKeys::Projects::head, this->vcs->getHead().getHeadingRevision()->getUuid());
    this->response = createProjectRequest.put(payload);
    if (!this->response.is2xx())
    {
        DBG("Failed to update the project on remote: " + this->response.getErrors().getFirst());
        callbackOnMessageThread(RevisionsSyncThread, onSyncFailed, self->response.getErrors());
        return;
    }

    callbackOnMessageThread(RevisionsSyncThread, onSyncDone, false);
}

void RevisionsSyncThread::pushSubtreeRecursively(VCS::Revision::Ptr root)
{
    // todo debug and fix `push branch` for non-existing remotely project
    if (this->idsToPush.isEmpty() ||
        this->idsToPush.contains(root->getUuid()))
    {
        const String revisionRoute(ApiRoutes::projectRevision
            .replace(":projectId", this->projectId)
            .replace(":revisionId", root->getUuid()));

        SerializedData payload(ApiKeys::Revisions::revision);
        payload.setProperty(ApiKeys::Revisions::message, root->getMessage());
        payload.setProperty(ApiKeys::Revisions::timestamp, String(root->getTimeStamp()));
        payload.setProperty(ApiKeys::Revisions::parentId,
            (root->getParent() ? var(root->getParent()->getUuid()) : var()));

        SerializedData data(ApiKeys::Revisions::data);
        data.appendChild(root->serializeDeltas());
        payload.appendChild(data);

        const BackendRequest revisionRequest(revisionRoute);
        this->response = revisionRequest.put(payload);
        if (!this->response.is2xx())
        {
            DBG("Failed to put revision data: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(RevisionsSyncThread, onSyncFailed, self->response.getErrors());
            return;
        }

        // notify vcs that revision is available remotely
        this->vcs->updateLocalSyncCache(root);
    }

    for (auto *child : root->getChildren())
    {
        this->pushSubtreeRecursively(child);
    }
}

#endif
