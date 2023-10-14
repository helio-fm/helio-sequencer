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

#include "Common.h"
#include "Head.h"
#include "Diff.h"

namespace VCS
{

Head::Head(const Head &other) :
    targetVcsItemsSource(other.targetVcsItemsSource),
    isDiffOutdated(other.isDiffOutdated),
    diff(other.diff),
    headingAt(other.headingAt),
    state(make<Snapshot>(other.state.get())) {}

Head::Head(TrackedItemsSource &targetProject) :
    targetVcsItemsSource(targetProject),
    diff(new Revision()),
    headingAt(new Revision()),
    state(make<Snapshot>()) {}

Revision::Ptr Head::getHeadingRevision() const
{
    return this->headingAt;
}

Revision::Ptr Head::getDiff() const
{
    const ScopedReadLock lock(this->diffLock);
    return this->diff;
}

bool Head::diffHasChanges() const
{
    return !this->getDiff()->getItems().isEmpty();
}

void Head::setDiffOutdated(bool isOutdated)
{
    this->isDiffOutdated = isOutdated;
}

void Head::mergeStateWith(Revision::Ptr changes)
{
    Revision::Ptr headRevision(this->getHeadingRevision());
    for (auto *changesItem : changes->getItems())
    {
        if (changesItem->getType() == RevisionItem::Type::Added)
        {
            if (this->state != nullptr)
            {
                this->state->addItem(changesItem);
            }
        }
        else if (changesItem->getType() == RevisionItem::Type::Removed)
        {
            if (this->state != nullptr)
            {
                this->state->removeItem(changesItem);
            }
        }
        else if (changesItem->getType() == RevisionItem::Type::Changed)
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
    // first, reset the snapshot state
    {
        const ScopedWriteLock lock(this->stateLock);
        this->state = make<Snapshot>();
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
            if (item->getType() == RevisionItem::Type::Added)
            {
                this->state->addItem(item);
            }
            else if (item->getType() == RevisionItem::Type::Removed)
            {
                this->state->removeItem(item);
            }
            else if (item->getType() == RevisionItem::Type::Changed)
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
        auto *item = this->state->getTrackedItem(i);

        if (item->getUuid() == diffItem->getUuid())
        {
            sourceItem = item;
            break;
        }
    }

    // обработать тип - добавлено, удалено, изменено
    if (diffItem->getType() == RevisionItem::Type::Changed)
    {
        TrackedItem *targetItem = nullptr;

        // ищем в проекте айтем с соответствующим уидом
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            auto *item = this->targetVcsItemsSource.getTrackedItem(i);

            if (item->getUuid() == diffItem->getUuid())
            {
                targetItem = item;
                break;
            }
        }

        if (targetItem != nullptr && sourceItem != nullptr)
        {
            targetItem->resetStateTo(*sourceItem);
            return true;
        }
    }
    else if (diffItem->getType() == RevisionItem::Type::Added)
    {
        TrackedItem *targetItem = nullptr;

        // снова ищем исходный с тем же уидом и вызываем deleteTrackedItem
        for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
        {
            auto *item = this->targetVcsItemsSource.getTrackedItem(i);

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
    else if (diffItem->getType() == RevisionItem::Type::Removed)
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
    {
        return;
    }

    this->targetVcsItemsSource.onBeforeResetState();

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
    {
        return;
    }

    this->targetVcsItemsSource.onBeforeResetState();

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
    {
        return;
    }

    this->targetVcsItemsSource.onBeforeResetState();

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
    {
        return false;
    }

    this->targetVcsItemsSource.onBeforeResetState();

    for (const auto &item : changes)
    {
        this->resetChangedItemToState(item);
    }

    this->targetVcsItemsSource.onResetState();
    return true;
}

void Head::checkoutItem(RevisionItem::Ptr stateItem)
{
    TrackedItem *targetItem = nullptr;
    
    for (int i = 0; i < this->targetVcsItemsSource.getNumTrackedItems(); ++i)
    {
        auto *item = this->targetVcsItemsSource.getTrackedItem(i);

        if (item->getUuid() == stateItem->getUuid())
        {
            targetItem = item;
            break;
        }
    }

    if (stateItem->getType() == RevisionItem::Type::Changed)
    {
        if (targetItem)
        {
            targetItem->resetStateTo(*stateItem);
        }
    }
    else if (stateItem->getType() == RevisionItem::Type::Added)
    {
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
    else if (stateItem->getType() == RevisionItem::Type::Removed)
    {
        if (targetItem)
        {
            this->targetVcsItemsSource.deleteTrackedItem(targetItem);
        }
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Head::serialize() const
{
    SerializedData tree(Serialization::VCS::head);
    SerializedData snapshotNode(Serialization::VCS::snapshot);

    {
        const ScopedReadLock lock(this->stateLock);
        
        for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
            const auto serializedItem = stateItem->serialize();
            snapshotNode.appendChild(serializedItem);
        }
    }
    
    tree.appendChild(snapshotNode);
    return tree;
}

void Head::deserialize(const SerializedData &data)
{
    this->reset();
    
    const auto root = data.hasType(Serialization::VCS::head) ?
        data : data.getChildWithName(Serialization::VCS::head);

    if (!root.isValid()) { return; }

    const auto snapshotNode = root.getChildWithName(Serialization::VCS::snapshot);
    if (!snapshotNode.isValid()) { return; }

    forEachChildWithType(snapshotNode, stateElement, Serialization::VCS::revisionItem)
    {
        RevisionItem::Ptr snapshotItem(new RevisionItem(RevisionItem::Type::Added, nullptr));
        snapshotItem->deserialize(stateElement);
        this->state->addItem(snapshotItem);
    }
}

void Head::reset()
{
    this->state = make<Snapshot>();
    this->setDiffOutdated(true);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void Head::changeListenerCallback(ChangeBroadcaster *source)
{
    this->setDiffOutdated(true); // the VCS has changed
}

//===----------------------------------------------------------------------===//
// Rebuilding the diff
//===----------------------------------------------------------------------===//

void Head::rebuildDiffIfNeeded()
{
    if (this->state == nullptr)
    {
        return;
    }

    if (!this->isDiffOutdated.get())
    {
        return;
    }

    //DBG("VCS: rebuilding the diff");

    const ScopedWriteLock scopedDiffLock(this->diffLock);
    this->diff->reset();
    
    const ScopedReadLock scopedStateLock(this->stateLock);
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        bool foundItemInTarget = false;
        const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        
        // will check `removed` records later
        if (stateItem->getType() == RevisionItem::Type::Removed) { continue; }
        
        for (int j = 0; j < this->targetVcsItemsSource.getNumTrackedItems(); ++j)
        {
            auto *targetItem = this->targetVcsItemsSource.getTrackedItem(j); // i.e. MidiTrckNode
            
            // state item exists in project, adding `changed` record, if needed
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInTarget = true;
                
                UniquePointer<Diff> itemDiff(targetItem->getDiffLogic()->createDiff(*stateItem));
                
                if (itemDiff->hasAnyChanges())
                {
                    RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Type::Changed, itemDiff.get()));
                    this->diff->addItem(revisionRecord);
                }
                
                break;
            }
        }
        
        // state item was not found in project, adding `removed` record
        if (! foundItemInTarget)
        {
            auto emptyDiff = make<Diff>(*stateItem);
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Type::Removed, emptyDiff.get()));
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
            
            if (stateItem->getType() == RevisionItem::Type::Removed) { continue; }
            
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInState = true;
                break;
            }
        }
        
        // copy deltas from targetItem and add `added` record
        if (! foundItemInState)
        {
            RevisionItem::Ptr revisionRecord(new RevisionItem(RevisionItem::Type::Added, targetItem));
            this->diff->addItem(revisionRecord);
        }
    }
    
    this->setDiffOutdated(false);
}

}
