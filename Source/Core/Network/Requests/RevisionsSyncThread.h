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

#pragma once

#if !NO_NETWORK

#include "BackendRequest.h"
#include "VersionControl.h"
#include "Revision.h"

class RevisionsSyncThread final : public Thread
{
public:
    
    RevisionsSyncThread();
    ~RevisionsSyncThread() override;
    
    Function<void()> onFetchDone;
    Function<void(bool nothingToSync)> onSyncDone;
    Function<void(const Array<String> &errors)> onSyncFailed;

    void doFetch(WeakReference<VersionControl> vcs,
        const String &projectId, const String &projectName);

    void doSync(WeakReference<VersionControl> vcs,
        const String &projectId, const String &projectName,
        const Array<String> &revisionIdsToPull = {},
        const Array<String> &revisionIdsToPush = {});

private:
    
    void run() override;
    void pushSubtreeRecursively(VCS::Revision::Ptr subtree);
    
    bool fetchOnly;
    String projectId;
    String projectName;
    
    WeakReference<VersionControl> vcs;

    // If empty, synchronizes all revisions
    Array<String> idsToPull;
    Array<String> idsToPush;

    BackendRequest::Response response;

    friend class BackendService;
};

#endif
