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
#include "PushThread.h"
#include "VersionControl.h"
#include "HelioApiRoutes.h"
#include "SerializationKeys.h"
#include "Revision.h"
#include "RevisionDto.h"
#include "RevisionsListDto.h"
#include "ProjectInfo.h"

PushThread::PushThread() : Thread("Push") {}

PushThread::~PushThread()
{
    this->stopThread(1000);
}

void PushThread::doPush(WeakReference<VersionControl> vcs, WeakReference<ProjectTreeItem> project)
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

void PushThread::run()
{
    namespace ApiKeys = Serialization::Api::V1;
    namespace ApiRoutes = Routes::HelioFM::Api;

    //VCS::Revision::getUuid(this->vcs->getHead().getHeadingRevision())
    //this->vcs->getRoot()

    HashMap<String, ValueTree> revisionsIndex;
    // TODO build index recursive from root

    const String projectRoute(ApiRoutes::projects.replace(":projectId", this->project->getId()));
    const BackendRequest headsRequest(projectRoute);
    this->response = headsRequest.get();

    RevisionsListDto remoteHeads(this->response.getBody());

    if (this->response.is(404))
    {
        // Put the project:
        const BackendRequest createProjectRequest(projectRoute);
        ValueTree initSession(ApiKeys::session);
        // head reference will be put later when all revisions are pushed
        initSession.setProperty(ApiKeys::Projects::title, this->project->getProjectInfo()->getFullName(), nullptr);
        this->response = createProjectRequest.get();
        if (!this->response.is2xx())
        {
            Logger::writeToLog("Failed to create the project on remote: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(PushThread, onPushFailed, self->response.getErrors());
            return;
        }
    }
    else if (!this->response.is200())
    {
        Logger::writeToLog("Failed to fetch project heads from remote: " + this->response.getErrors().getFirst());
        callbackOnMessageThread(PushThread, onPushFailed, self->response.getErrors());
        return;
    }

    Array<RevisionDto> newLocalRevisions;
    Array<RevisionDto> newRemoteRevisions;

    // TODO find them all

    if (newLocalRevisions.isEmpty() &&
        newRemoteRevisions.isEmpty())
    {
        // TODO up to date
    }

    // handle new remote leafs: 
    // - ask for full leaf data
    // - if parent is not present locally, ask for parent leaf
    // - if parent is present locally, finish

    // handle outdated remote leafs
    // - recursively push all children

    // find all new local leafs
    // - fucking push them

    Array<ValueTree> localMismatchedLeafs; // not found in remote leafs. could be either new or outdated (TODO calc that on the server?)



    // isn't it easier to fetch ALL revision ids??
    // and fucking match missing?
    // will end up with 2 lists:
    // - present locally, missing remotely
    // - present remotely, missing locally
    // pushing is simple as fuck
    // then, build trees from that new remote revisions (only mark new revisions as `shallow`)
    // get all data for new revisions


    // 2.

    // For a simple case, consider 2 devices and server (in the centre):
    // 1    1   1
    // 1<<  1   1    GET /vcs/projects/:id, returns leafs, and we see that these leafs are *exactly* our leafs ("up to date")
    // 1a   1   1b
    // 1a<< 1   1b   GET /vcs/projects/:id, returns leafs, and we see that these leafs are behind of what we have
    // 1a>> 1   1b   PUT /vcs/projects/:id, sending all new revisions (what about NEW branches?)
    //              if the server was modified in the meanwhile - returns 4xx? (and WHAT about NEW branches?)
    //                (it should check if sent parents are server's leafs with no children)
    // 2    2   1b   if the server was not modified in the meanwhile, it will accept all new data
    // 2    2 <<1b   GET /vcs/projects/:id, returns leafs, and we see that these leafs are something new, so we cannot push
    // 2    2 >>1b   GET /vcs/revisions/:leaf/branch, for each new leaf, tries to fetch history up to his parent (i.e. a node with more than one child)
    //               WHAT about multiple parents?
    // 2    2   2b
    // 2    2 <<2b   PUT /vcs/projects/:id, sending all new revisions
    // 2    3   3
    // 2c   3   3    makes some changes
    // 2c>> 3   3    GET /vcs/projects/:id, returns leafs, and we see that some leafs are behind of what we have, *but* some are new
    // 2ñ<< 3   3    GET /vcs/revisions/:leaf/branch, for each new leaf, tries to fetch history until we see the merge point
    // 3c   3   3    merges the fetched branch with his own
    // 3ñ>> 3   3

    // 3.
    
}
