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
#include "Head.h"
#include "TrackedItemsSource.h"
#include "ProjectNode.h"
#include "TrackedItem.h"
#include "Diff.h"
#include "DiffLogic.h"

using namespace VCS;

#define DIFF_BUILD_THREAD_STOP_TIMEOUT 5000

Head::Head(const Head &other) :
    Thread("Diff Thread"),
    targetVcsItemsSource(other.targetVcsItemsSource),
    diffOutdated(other.diffOutdated),
    rebuildingDiffMode(false),
    diff(other.diff),
    headingAt(other.headingAt),
    state(new Snapshot(other.state.get())) {}

Head::Head(TrackedItemsSource &targetProject) :
    Thread("Diff Thread"),
    targetVcsItemsSource(targetProject),
    diffOutdated(false),
    rebuildingDiffMode(false),
    diff(new Revision()),
    headingAt(new Revision()),
    state(new Snapshot()) {}

Revision::Ptr Head::getHeadingRevision() const
{
    return this->headingAt;
}

Revision::Ptr Head::getDiff() const
{
    const ScopedReadLock lock(this->diffLock);
    return this->diff;
}

bool Head::hasAnythingOnTheStage() const
{
    return !this->getDiff()->getItems().isEmpty();
}

bool Head::hasTrackedItemsOnTheStage() const
{
    for (const auto *revRecord : this->getDiff()->getItems())
    {
        if (revRecord->getType() != RevisionItem::Added)
        {
            return true;
        }
    }
    
    return false;
}

bool Head::isDiffOutdated() const
{
    const ScopedReadLock lock(this->outdatedMarkerLock);
    return this->diffOutdated;
}

void Head::setDiffOutdated(bool isOutdated)
{
    const ScopedWriteLock lock(this->outdatedMarkerLock);
    this->diffOutdated = isOutdated;
}

bool Head::isRebuildingDiff() const
{
    const ScopedReadLock lock(this->rebuildingDiffLock);
    return this->rebuildingDiffMode;
}

void Head::setRebuildingDiffMode(bool isBuildingNow)
{
    const ScopedWriteLock lock(this->rebuildingDiffLock);
    this->rebuildingDiffMode = isBuildingNow;
}

void Head::mergeStateWith(Revision::Ptr changes)
{
    DBG("Head::mergeStateWith " + changes->getUuid());

    Revision::Ptr headRevision(this->getHeadingRevision());
    for (auto *changesItem : changes->getItems())
    {
        if (changesItem->getType() == RevisionItem::Added)
        {
            if (this->state != nullptr)
            {
                this->state->addItem(changesItem);
            }
        }
        else if (changesItem->getType() == RevisionItem::Removed)
        {
            if (this->state != nullptr)
            {
                this->state->removeItem(changesItem);
            }
        }
        else if (changesItem->getType() == RevisionItem::Changed)
        {
            if (this->state != nullptr)
            {
                this->state->mergeItem(changesItem);
            }
        }
        else
        {
            jassertfalse;
        }
    }
}

bool Head::moveTo(const Revision::Ptr revision)
{
    if (this->isThreadRunning())
    {
        this->stopThread(DIFF_BUILD_THREAD_STOP_TIMEOUT);
    }

    // first, reset the snapshot state
    {
        const ScopedWriteLock lock(this->stateLock);
        this->state.reset(new Snapshot());
    }

    // a path from the root to current revision
    ReferenceCountedArray<Revision> treePath;
    Revision::Ptr currentRevision(revision);
    while (currentRevision != nullptr)
    {
        treePath.insert(0, currentRevision);
        currentRevision = currentRevision->getParent();
    }

    // then move from the root back to target revision
    for (const auto *rev : treePath)
    {
        DBG("VCS head moved to " + rev->getUuid());

        // picking all deltas and applying them to current state
        for (auto *item : rev->getItems())
        {
            if (item->getType() == RevisionItem::Added)
            {
                this->state->addItem(item);
            }
            else if (item->getType() == RevisionItem::Removed)
            {
                this->state->removeItem(item);
            }
            else if (item->getType() == RevisionItem::Changed)
            {
                this->state->mergeItem(item);
            }
            else
            {
                jassertfalse;
            }
        }
    }

    this->headingAt = revision;
    this->setDiffOutdated(true);
    return true;
}

void Head::pointTo(const Revision::Ptr revision)
{
    this->headingAt = revision;
    this->setDiffOutdated(true);
}


bool Head::resetChangedItemToState(const RevisionItem::Ptr diffItem)
{
    if (this->state == nullptr)
    { return false; }

    // на входе - один из айтемов диффа
    TrackedItem *sourceItem = nullptr;

    // ищем в собранном состоянии айтем с соответствующим уидом
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        TrackedItem *item = this->state->getTrackedItem(i);

        if (item->getUuid() == diffItem->getUuid())
        {
            sourceItem = item;
            break;
        }
    }

    // обработать тип - добавлено, удалено, изменено
    if (diffItem->getType() == RevisionItem::Changed)
    {
        TrackedItem *targetItem = nullptr;

        // ищем в проекте айтем с соответствующим уидом
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            TrackedItem *item = this->targetVcsItemsSource.getTrackedItem(i);

            if (item->getUuid() == diffItem->getUuid())
            {
                targetItem = item;
                break;
            }
        }

        if (targetItem)
        {
            targetItem->resetStateTo(*sourceItem);
            return true;
        }
    }
    else if (diffItem->getType() == RevisionItem::Added)
    {
        TrackedItem *targetItem = nullptr;

        // снова ищем исходный с тем же уидом и вызываем deleteTrackedItem
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            TrackedItem *item = this->targetVcsItemsSource.getTrackedItem(i);

            if (item->getUuid() == diffItem->getUuid())
            {
                targetItem = item;
                break;
            }
        }

        if (targetItem)
        {
            return this->targetVcsItemsSource.deleteTrackedItem(targetItem);
        }
    }
    else if (diffItem->getType() == RevisionItem::Removed)
    {
        const Identifier logicType(sourceItem->getDiffLogic()->getType());
        const Uuid id(sourceItem->getUuid());
        this->targetVcsItemsSource.initTrackedItem(logicType, id, *sourceItem);
        return true;
    }

    return false;
}

void Head::checkout()
{
    if (this->state == nullptr)
    { return; }

    // clear all tracked items
    {
        Array<TrackedItem *> itemsToClear;

        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            TrackedItem *ti = this->targetVcsItemsSource.getTrackedItem(i);

            if (this->state->getItemWithUuid(ti->getUuid()) != nullptr)
            {
                itemsToClear.add(ti);
            }
        }

        for (auto i : itemsToClear)
        {
            this->targetVcsItemsSource.deleteTrackedItem(i);
        }
    }

    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        this->checkoutItem(stateItem);
    }

    this->targetVcsItemsSource.onResetState();
}

void Head::cherryPick(const Array<Uuid> uuids)
{
    if (this->state == nullptr)
    { return; }

    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));

        // если этот айтем состояния выбран юзером, то чекаут.
        for (int j = 0; j < uuids.size(); ++j)
        {
            if (stateItem->getUuid() == uuids.getUnchecked(j))
            {
                this->checkoutItem(stateItem);
                break;
            }
        }
    }

    this->targetVcsItemsSource.onResetState();
}

void Head::cherryPickAll()
{
    if (this->state == nullptr)
    { return; }
    
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        this->checkoutItem(stateItem);
    }

    this->targetVcsItemsSource.onResetState();
}

bool Head::resetChanges(const Array<RevisionItem::Ptr> &changes)
{
    if (this->state == nullptr)
    { return false; }

    for (const auto &item : changes)
    {
        this->resetChangedItemToState(item);
    }

    this->targetVcsItemsSource.onResetState();
    return true;
}

void Head::checkoutItem(RevisionItem::Ptr stateItem)
{
    // Changed и Added RevisionItem'ы нужно применять через resetStateTo
    TrackedItem *targetItem = nullptr;
    
    // ищем в проекте айтем с соответствующим уидом
    for (int j = 0; j < this->targetVcsItemsSource.getNumTrackedItems(); ++j)
    {
        TrackedItem *item = this->targetVcsItemsSource.getTrackedItem(j);

        if (item->getUuid() == stateItem->getUuid())
        {
            targetItem = item;
            break;
        }
    }

    if (stateItem->getType() == RevisionItem::Changed)
    {
        if (targetItem)
        {
            targetItem->resetStateTo(*stateItem);
        }
    }
    else if (stateItem->getType() == RevisionItem::Added)
    {
        // айтем не в проекте - добавляем
        if (!targetItem)
        {
            
            const Identifier logicType(stateItem->getDiffLogic()->getType());
            const Uuid id(stateItem->getUuid());
            this->targetVcsItemsSource.initTrackedItem(logicType, id, *stateItem);
        }
        else
        {
            targetItem->resetStateTo(*stateItem);
        }
    }
    else if (stateItem->getType() == RevisionItem::Removed)
    {
        if (targetItem)
        {
            this->targetVcsItemsSource.deleteTrackedItem(targetItem);
        }
    }
}


void Head::rebuildDiffIfNeeded()
{
    if (this->isDiffOutdated() && !this->isThreadRunning())
    {
        this->startThread(5);
    }
}

void Head::rebuildDiffNow()
{
    if (this->isThreadRunning())
    {
        // Better to wait for a couple of seconds here than kill a thread by force
        // and have leaked Diff and/or race due to hanging read-write locks
        this->stopThread(DIFF_BUILD_THREAD_STOP_TIMEOUT);
    }

    this->startThread(9);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Head::serialize() const
{
    ValueTree tree(Serialization::VCS::head);
    ValueTree snapshotNode(Serialization::VCS::snapshot);

    {
        const ScopedReadLock lock(this->stateLock);
        
        for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
            const auto serializedItem = stateItem->serialize();
            snapshotNode.appendChild(serializedItem, nullptr);
        }
    }
    
    tree.appendChild(snapshotNode, nullptr);
    return tree;
}

void Head::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::VCS::head) ?
        tree : tree.getChildWithName(Serialization::VCS::head);

    if (!root.isValid()) { return; }
    
    const auto snapshotNode = root.getChildWithName(Serialization::VCS::snapshot);
    if (!snapshotNode.isValid()) { return; }

    // A temporary workaround, see the comment in VersionControl::deserialize()
    DeltaDataLookup deltaDataLookup;
    const auto snapshotDataNode = root.getChildWithName(Serialization::VCS::snapshotData);
    forEachValueTreeChildWithType(snapshotDataNode, dataElement, Serialization::VCS::packItem)
    {
        const String deltaId = dataElement.getProperty(Serialization::VCS::packItemDeltaId);
        const auto deltaData = dataElement.getChild(0);
        jassert(deltaData.isValid());
        deltaDataLookup[deltaId] = deltaData;
    }

    forEachValueTreeChildWithType(snapshotNode, stateElement, Serialization::VCS::revisionItem)
    {
        RevisionItem::Ptr snapshotItem(new RevisionItem(RevisionItem::Added, nullptr));
        snapshotItem->deserialize(stateElement, deltaDataLookup);
        this->state->addItem(snapshotItem);
    }
}

void Head::reset()
{
    this->state.reset(new Snapshot());
    this->setDiffOutdated(true);
}


//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void Head::changeListenerCallback(ChangeBroadcaster *source)
{
    this->setDiffOutdated(true);
}


//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void Head::run()
{
    if (this->state == nullptr)
    { return; }
    
    this->setRebuildingDiffMode(true);
    this->sendChangeMessage();

    {
        const ScopedWriteLock lock(this->diffLock);
        this->diff->reset();
    }

    const ScopedReadLock threadStateLock(this->stateLock);

    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        if (this->threadShouldExit())
        {
            this->setRebuildingDiffMode(false);
            this->sendChangeMessage();
            return;
        }

        bool foundItemInTarget = false;
        const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));

        // will check `removed` records later
        if (stateItem->getType() == RevisionItem::Removed) { continue; }

        for (int j = 0; j < this->targetVcsItemsSource.getNumTrackedItems(); ++j)
        {
            if (this->threadShouldExit())
            {
                this->setRebuildingDiffMode(false);
                this->sendChangeMessage();
                return;
            }

            TrackedItem *targetItem = this->targetVcsItemsSource.getTrackedItem(j); // i.e. LayerTreeItem

            // state item exists in project, adding `changed` record, if needed
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInTarget = true;

                ScopedPointer<Diff> itemDiff(targetItem->getDiffLogic()->createDiff(*stateItem));

                if (itemDiff->hasAnyChanges())
                {
                    RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Changed, itemDiff));
                    const ScopedWriteLock itemDiffLock(this->diffLock);
                    this->diff->addItem(revisionRecord);
                }

                break;
            }
        }

        // state item was not found in project, adding `removed` record
        if (! foundItemInTarget)
        {
            ScopedPointer<Diff> emptyDiff(new Diff(*stateItem));
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Removed, emptyDiff));
            const ScopedWriteLock emptyDiffLock(this->diffLock);
            this->diff->addItem(revisionRecord);
        }
    }

    // search for project item that are missing (or deleted) in the state
    for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
    {
        if (this->threadShouldExit())
        {
            this->setRebuildingDiffMode(false);
            this->sendChangeMessage();
            return;
        }

        bool foundItemInState = false;
        TrackedItem *targetItem = this->targetVcsItemsSource.getTrackedItem(i);

        for (int j = 0; j < this->state->getNumTrackedItems(); ++j)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(j));

            if (stateItem->getType() == RevisionItem::Removed) { continue; }

            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInState = true;
                break;
            }
        }

        // copy deltas from targetItem and add `added` record
        if (! foundItemInState)
        {
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Added, targetItem));
            const ScopedWriteLock lock(this->diffLock);
            this->diff->addItem(revisionRecord);
        }
    }

    this->setDiffOutdated(false);
    this->setRebuildingDiffMode(false);
    this->sendChangeMessage();
}

// FIXME: lots of duplicate code form Head::run
void Head::rebuildDiffSynchronously()
{
    if (this->state == nullptr)
    { return; }
    
    if (this->isRebuildingDiff())
    { return; }
    
    this->setRebuildingDiffMode(true);
    
    {
        const ScopedWriteLock lock(this->diffLock);
        this->diff->reset();
    }
    
    const ScopedReadLock rebuildStateLock(this->stateLock);
    
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        bool foundItemInTarget = false;
        const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        
        // will check `removed` records later
        if (stateItem->getType() == RevisionItem::Removed) { continue; }
        
        for (int j = 0; j < this->targetVcsItemsSource.getNumTrackedItems(); ++j)
        {
            TrackedItem *targetItem = this->targetVcsItemsSource.getTrackedItem(j); // i.e. LayerTreeItem
            
            // state item exists in project, adding `changed` record, if needed
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInTarget = true;
                
                ScopedPointer<Diff> itemDiff(targetItem->getDiffLogic()->createDiff(*stateItem));
                
                if (itemDiff->hasAnyChanges())
                {
                    RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Changed, itemDiff));
                    const ScopedWriteLock lock(this->diffLock);
                    this->diff->addItem(revisionRecord);
                }
                
                break;
            }
        }
        
        // state item was not found in project, adding `removed` record
        if (! foundItemInTarget)
        {
            ScopedPointer<Diff> emptyDiff(new Diff(*stateItem));
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Removed, emptyDiff));
            const ScopedWriteLock lock(this->diffLock);
            this->diff->addItem(revisionRecord);
        }
    }
    
    // search for project item that are missing (or deleted) in the state
    for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
    {
        bool foundItemInState = false;
        TrackedItem *targetItem = this->targetVcsItemsSource.getTrackedItem(i);
        
        for (int j = 0; j < this->state->getNumTrackedItems(); ++j)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(j));
            
            if (stateItem->getType() == RevisionItem::Removed) { continue; }
            
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInState = true;
                break;
            }
        }
        
        // copy deltas from targetItem and add `added` record
        if (! foundItemInState)
        {
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Added, targetItem));
            const ScopedWriteLock lock(this->diffLock);
            this->diff->addItem(revisionRecord);
        }
    }
    
    this->setDiffOutdated(false);
    this->setRebuildingDiffMode(false);
    this->sendChangeMessage();
}
