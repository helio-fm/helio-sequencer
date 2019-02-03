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
#include "ProjectDeleteThread.h"
#include "Network.h"

namespace ApiKeys = Serialization::Api::V1;
namespace ApiRoutes = Routes::Api;

ProjectDeleteThread::ProjectDeleteThread() :
    Thread("DeleteProject") {}

ProjectDeleteThread::~ProjectDeleteThread()
{
    this->stopThread(1000);
}

void ProjectDeleteThread::doDelete(const String &projectId)
{
    if (this->isThreadRunning())
    {
        DBG("Warning: failed to start project delete thread, already running");
        return;
    }

    this->projectId = projectId;
    this->startThread(9);
}

void ProjectDeleteThread::run()
{
    const String projectRoute(ApiRoutes::project.replace(":projectId", this->projectId));
    const BackendRequest revisionsRequest(projectRoute);
    this->response = revisionsRequest.del();

    // expect "no content" for successful delete or "not found",
    // when not found, just pretend we have deleted it successfully:
    if (this->response.is(404) || this->response.is(204))
    {
        callbackOnMessageThread(ProjectDeleteThread, onDeleteDone, self->projectId);
        return;
    }

    DBG("Failed to delete project from remote: " + this->response.getErrors().getFirst());
    callbackOnMessageThread(ProjectDeleteThread, onDeleteFailed, self->response.getErrors(), self->projectId);
}
