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

#pragma once

#include "BackendRequest.h"
#include "VersionControl.h"
#include "Revision.h"

class ProjectCloneThread final : public Thread
{
public:
    
    ProjectCloneThread();
    ~ProjectCloneThread() override;
    
    Function<void()> onCloneDone;
    Function<void(const String &projectId)> onProjectMissing;
    Function<void(const Array<String> &errors, const String &projectId)> onCloneFailed;

    void doClone(WeakReference<VersionControl> vcs, const String &projectId);

private:
    
    void run() override;
    
    String projectId;
    WeakReference<VersionControl> vcs;
    VCS::Revision::Ptr newHead;

    BackendRequest::Response response;

    friend class BackendService;
};
