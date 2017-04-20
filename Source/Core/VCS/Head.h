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

#include "Serializable.h"
#include "Revision.h"
#include "Pack.h"

namespace VCS
{
    class HeadState;
    class TrackedItem;
    class TrackedItemsSource;
    class DeltaDataSource;

    class Head :
        private Thread,
        public ChangeListener, // слушает, изменился ли проект, чтоб запустить билд при показе редактора
        public ChangeBroadcaster, // оповещает о том, что дифф начал или закончил обновляться
        public Serializable
    {
    public:

        Head(const Head &other);

        explicit Head(Pack::Ptr packPtr,
             WeakReference<TrackedItemsSource> targetProject = nullptr);

        ~Head() override;


        Revision getHeadingRevision() const;
        
        Revision getDiff() const;
        bool isDiffOutdated() const;
        void setDiffOutdated(bool isOutdated);

        bool isRebuildingDiff() const; // для change-listener'ов
        void setRebuildingDiffMode(bool isBuildingNow);
        
        bool hasAnythingOnTheStage() const;
        bool hasTrackedItemsOnTheStage() const;

        void mergeStateWith(Revision changes);

        bool moveTo(const Revision &revision); // перестраивает индекс-состояние
        
        void pointTo(const Revision &revision); // не перестраивает индекс
        
        bool resetChangedItemToState(const VCS::RevisionItem::Ptr diffItem);

        void checkout();

        void cherryPick(const Array<Uuid> uuids);
        
        void cherryPickAll();


        void rebuildDiffIfNeeded(); // это вызывается в редакторе, когда он становится видимым

        void rebuildDiffNow(); // это вызывается в редакторе, когда он видим и слышит изменения vcs

        void rebuildDiffSynchronously(); // грязный хак для quick-stash
        
        
        //===------------------------------------------------------------------===//
        // Serializable
        //
        
        XmlElement *serialize() const override;
        
        void deserialize(const XmlElement &xml) override;
        
        void reset() override;
        
        
        //===------------------------------------------------------------------===//
        // ChangeListener
        //

        void changeListenerCallback(ChangeBroadcaster *source) override;

    private:

        //===------------------------------------------------------------------===//
        // Thread
        //

        void run() override;

        void checkoutItem(VCS::RevisionItem::Ptr stateItem);

        ReadWriteLock outdatedMarkerLock;
        bool diffOutdated;

        ReadWriteLock diffLock;
        Revision diff;
        
        ReadWriteLock rebuildingDiffLock;
        bool rebuildingDiffMode;

    private:

        Revision headingAt;

        ReadWriteLock stateLock;

        ScopedPointer<HeadState> state;

    private:

        WeakReference<TrackedItemsSource> targetVcsItemsSource; // ProjectTreeItem

        Pack::Ptr pack;

        JUCE_LEAK_DETECTOR(Head)

    };
} // namespace VCS
