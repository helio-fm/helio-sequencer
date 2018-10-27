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
#include "ProjectTreeItem.h"
#include "TrackedItem.h"
#include "App.h"

#include "Diff.h"
#include "DiffLogic.h"

using namespace VCS;

#define DIFF_BUILD_THREAD_STOP_TIMEOUT 5000

Head::Head(const Head &other) :
    Thread("Diff Thread"),
    targetVcsItemsSource(other.targetVcsItemsSource),
    pack(other.pack),
    diffOutdated(other.diffOutdated),
    rebuildingDiffMode(false),
    diff(other.diff),
    headingAt(other.headingAt),
    state(new Snapshot(other.state.get())) {}

Head::Head(Pack::Ptr packPtr, TrackedItemsSource &targetProject) :
    Thread("Diff Thread"),
    targetVcsItemsSource(targetProject),
    pack(packPtr),
    diffOutdated(false),
    rebuildingDiffMode(false),
    diff(new Revision(packPtr)),
    headingAt(new Revision(packPtr)),
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
    Logger::writeToLog("Head::mergeStateWith " + changes->getUuid());

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

bool VCS::Head::moveTo(const Revision::Ptr revision)
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
        Logger::writeToLog("VCS head moved to " + rev->getUuid());

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


bool Head::resetChangedItemToState(const VCS::RevisionItem::Ptr diffItem)
{
    if (this->state == nullptr)
    { return false; }

    // на входе - один из айтемов диффа
    VCS::TrackedItem *sourceItem = nullptr;

    // ищем в собранном состоянии айтем с соответствующим уидом
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        VCS::TrackedItem *item = this->state->getTrackedItem(i);

        if (item->getUuid() == diffItem->getUuid())
        {
            sourceItem = item;
            break;
        }
    }

    // обработать тип - добавлено, удалено, изменено
    if (diffItem->getType() == RevisionItem::Changed)
    {
        VCS::TrackedItem *targetItem = nullptr;

        // ищем в проекте айтем с соответствующим уидом
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            VCS::TrackedItem *item = this->targetVcsItemsSource.getTrackedItem(i);

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
        VCS::TrackedItem *targetItem = nullptr;

        // снова ищем исходный с тем же уидом и вызываем deleteTrackedItem
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            VCS::TrackedItem *item = this->targetVcsItemsSource.getTrackedItem(i);

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

bool VCS::Head::resetChanges(const Array<RevisionItem::Ptr> &changes)
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

void Head::checkoutItem(VCS::RevisionItem::Ptr stateItem)
{
    // Changed и Added RevisionItem'ы нужно применять через resetStateTo
    TrackedItem *targetItem = nullptr;

    //Logger::writeToLog(stateItem->getVCSName());
    
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

ValueTree VCS::Head::serialize() const
{
    ValueTree tree(Serialization::VCS::head);
    ValueTree stateNode(Serialization::VCS::snapshot);
    ValueTree stateDataNode(Serialization::VCS::snapshotData);

    {
        const ScopedReadLock lock(this->stateLock);
        
        for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
            const auto serializedItem = stateItem->serialize();
            stateNode.appendChild(serializedItem, nullptr);
            
            // exports also deltas data
            for (int j = 0; j < stateItem->getNumDeltas(); ++j)
            {
                const auto deltaData = stateItem->serializeDeltaData(j);
                
                ValueTree packItem(Serialization::VCS::packItem);
                packItem.setProperty(Serialization::VCS::packItemRevId, stateItem->getUuid().toString(), nullptr);
                packItem.setProperty(Serialization::VCS::packItemDeltaId, stateItem->getDelta(j)->getUuid().toString(), nullptr);
                packItem.appendChild(deltaData, nullptr);
                
                stateDataNode.appendChild(packItem, nullptr);
            }
        }
    }
    
    tree.appendChild(stateNode, nullptr);
    tree.appendChild(stateDataNode, nullptr);
    return tree;
}

void VCS::Head::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto headRoot = tree.hasType(Serialization::VCS::head) ?
        tree : tree.getChildWithName(Serialization::VCS::head);
    if (!headRoot.isValid()) { return; }
    
    const auto indexRoot = headRoot.getChildWithName(Serialization::VCS::snapshot);
    if (!indexRoot.isValid()) { return; }

    const auto dataRoot = headRoot.getChildWithName(Serialization::VCS::snapshotData);
    if (!dataRoot.isValid()) { return; }
    
    forEachValueTreeChildWithType(indexRoot, stateElement, Serialization::VCS::revisionItem)
    {
        RevisionItem::Ptr stateItem(new RevisionItem(this->pack, RevisionItem::Added, nullptr));
        stateItem->deserialize(stateElement);

        //Logger::writeToLog("- " + stateItem->getVCSName());
        
        // import deltas data
        forEachValueTreeChildWithType(dataRoot, dataElement, Serialization::VCS::packItem)
        {
            const String packItemRevId = dataElement.getProperty(Serialization::VCS::packItemRevId);
            const String packItemDeltaId = dataElement.getProperty(Serialization::VCS::packItemDeltaId);
            const auto deltaData = dataElement.getChild(0);
            
            if (packItemRevId == stateItem->getUuid().toString())
            {
                stateItem->importDataForDelta(deltaData, packItemDeltaId);
                //Logger::writeToLog("+ " + String(packItemDeltaId->getNumChildElements()));
            }
        }
        
        this->state->addItem(stateItem);
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
                    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Changed, itemDiff));
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
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Removed, emptyDiff));
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
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
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
                    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Changed, itemDiff));
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
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Removed, emptyDiff));
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
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
            const ScopedWriteLock lock(this->diffLock);
            this->diff->addItem(revisionRecord);
        }
    }
    
    this->setDiffOutdated(false);
    this->setRebuildingDiffMode(false);
    this->sendChangeMessage();
}
