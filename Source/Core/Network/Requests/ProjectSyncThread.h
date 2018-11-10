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

#include "ProjectTreeItem.h"
#include "BackendRequest.h"
#include "VersionControl.h"
#include "Revision.h"

class ProjectSyncThread final : public Thread
{
public:
    
    ProjectSyncThread();
    ~ProjectSyncThread() override;
    
    Function<void()> onFetchDone;
    Function<void(const VCS::Revision::Ptr revision)> onRevisionPushed;
    Function<void(const VCS::Revision::Ptr revision)> onRevisionPulled;
    Function<void(int numRevisionsPulled, int numRevisionsPushed)> onSyncDone;
    Function<void(const Array<String> &errors)> onSyncFailed;

    void doSync(WeakReference<VersionControl> vcs,
        const String &projectId, const String &projectName,
        const Array<String> &revisionIdsToSync = {});

private:
    
    void run() override;
    void pushSubtreeRecursively(VCS::Revision::Ptr subtree);
    
    String projectId;
    String projectName;
    WeakReference<VersionControl> vcs;

    // If empty, synchronizes all revisions
    Array<String> idsToSync;

    BackendRequest::Response response;

    friend class BackendService;
};
