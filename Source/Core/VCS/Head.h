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

#include "Snapshot.h"
#include "Revision.h"

namespace VCS
{
    class TrackedItem;
    class TrackedItemsSource;

    class Head :
        public ChangeListener, // listens to project changes to set diff outdated
        public Serializable
    {
    public:

        Head(const Head &other);
        explicit Head(TrackedItemsSource &targetProject);

        Revision::Ptr getHeadingRevision() const;
        
        Revision::Ptr getDiff() const;
        bool isDiffOutdated() const;
        void setDiffOutdated(bool isOutdated);
        
        bool hasAnythingOnTheStage() const;
        bool hasTrackedItemsOnTheStage() const;

        void mergeStateWith(Revision::Ptr changes);
        bool moveTo(const Revision::Ptr revision); // rebuilds state index
        void pointTo(const Revision::Ptr revision); // does not rebuild index

        void checkout();
        void cherryPick(const Array<Uuid> uuids);
        void cherryPickAll();
        bool resetChanges(const Array<RevisionItem::Ptr> &changes);

        void rebuildDiffIfNeeded();
        
        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        SerializedData serialize() const override;
        void deserialize(const SerializedData &data) override;
        void reset() override;
        
        //===--------------------------------------------------------------===//
        // ChangeListener
        //===--------------------------------------------------------------===//

        void changeListenerCallback(ChangeBroadcaster *source) override;

    private:

        void checkoutItem(RevisionItem::Ptr stateItem);
        bool resetChangedItemToState(const RevisionItem::Ptr diffItem);

        Atomic<bool> diffOutdated = false;

        ReadWriteLock diffLock;
        Revision::Ptr diff;

    private:

        Revision::Ptr headingAt;

        ReadWriteLock stateLock;
        UniquePointer<Snapshot> state;

    private:

        TrackedItemsSource &targetVcsItemsSource;

        JUCE_LEAK_DETECTOR(Head)

    };
} // namespace VCS
