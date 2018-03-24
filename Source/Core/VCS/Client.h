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

class VersionControl;

#include "SyncThread.h"
#include "SyncMessage.h"
#include "PushThread.h"
#include "PullThread.h"
#include "RemovalThread.h"

namespace VCS
{
    class Client :
        public ChangeBroadcaster, // на него подписываются PushComponent и PullComponent
        private ChangeListener // слушает PushThread и PullThread
    {
    public:

        explicit Client(VersionControl &versionControl);
        
        bool push();
        bool pull();
        bool remove();
        
        bool isPushing() const;
        bool isPulling() const;
        bool isRemoving() const;

        bool isPushDone() const;
        bool isPullDone() const;
        bool isRemovalDone() const;

        bool pullFinishedWithSuccess() const;
        bool pullFinishedWithFail() const;

        bool pushFinishedWithSuccess() const;
        bool pushFinishedWithFail() const;

        bool removalFinishedWithSuccess() const;
        bool removalFinishedWithFail() const;
        
    private:

        void changeListenerCallback(ChangeBroadcaster *source) override;

    private:

        VersionControl &vcs;

        ScopedPointer<Component> progressBar;
        
        ScopedPointer<PushThread> pushThread;
        ScopedPointer<PullThread> pullThread;
        ScopedPointer<RemovalThread> removalThread;

        void deletePushThread();
        void deletePullThread();
        void deleteRemovalThread();

    private:

        SyncThread::State lastPushState;
        SyncThread::State lastPullState;
        SyncThread::State lastRemovalState;

    };
}  // namespace VCS
