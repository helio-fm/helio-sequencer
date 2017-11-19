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
#include "HeadState.h"
#include "App.h"

#include "Diff.h"
#include "DiffLogic.h"

using namespace VCS;

Head::Head(const Head &other) :
    Thread("Diff Thread"),
    targetVcsItemsSource(other.targetVcsItemsSource),
    pack(other.pack),
    diffOutdated(other.diffOutdated),
    rebuildingDiffMode(false),
    diff(other.diff),
    headingAt(other.headingAt),
    state(new HeadState(other.state))
{
}

Head::Head(Pack::Ptr packPtr, WeakReference<TrackedItemsSource> targetProject) :
    Thread("Diff Thread"),
    targetVcsItemsSource(std::move(targetProject)),
    pack(packPtr),
    diffOutdated(false),
    rebuildingDiffMode(false),
    diff(Revision::create(packPtr)),
    headingAt(Revision::create(packPtr)),
    state(nullptr)
{
    if (targetVcsItemsSource != nullptr)
    {
        this->state = new HeadState();
    }
}

Head::~Head()
{
}

ValueTree Head::getHeadingRevision() const
{
    return this->headingAt;
}

ValueTree Head::getDiff() const
{
    ScopedReadLock lock(this->diffLock);
    return this->diff;
}

bool Head::hasAnythingOnTheStage() const
{
    const int numProps = this->getDiff().getNumProperties();
    return (numProps > 0);
}

bool Head::hasTrackedItemsOnTheStage() const
{
    const int numProps = this->getDiff().getNumProperties();
    
    for (int i = 0; i < numProps; ++i)
    {
        const Identifier id = this->getDiff().getPropertyName(i);
        const var property = this->getDiff().getProperty(id);
        
        if (RevisionItem *revRecord = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            if (revRecord->getType() != RevisionItem::Added)
            {
                return true;
            }
        }
    }
    
    return false;
}


bool Head::isDiffOutdated() const
{
    ScopedReadLock lock(this->outdatedMarkerLock);
    return this->diffOutdated;
}

void Head::setDiffOutdated(bool isOutdated)
{
    ScopedWriteLock lock(this->outdatedMarkerLock);
    this->diffOutdated = isOutdated;
}

bool Head::isRebuildingDiff() const
{
    ScopedReadLock lock(this->rebuildingDiffLock);
    return this->rebuildingDiffMode;
}

void Head::setRebuildingDiffMode(bool isBuildingNow)
{
    ScopedWriteLock lock(this->rebuildingDiffLock);
    this->rebuildingDiffMode = isBuildingNow;
}

void Head::mergeStateWith(ValueTree changes)
{
    Logger::writeToLog("Head::mergeStateWith " + Revision::getUuid(changes));

    ValueTree headRevision(this->getHeadingRevision());

    for (int i = 0; i < changes.getNumProperties(); ++i)
    {
        Identifier id = changes.getPropertyName(i);
        const var property = changes.getProperty(id);

        if (RevisionItem *changesItem = dynamic_cast<RevisionItem *>(property.getObject()))
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
            else { jassertfalse; }
        }
    }
}

bool VCS::Head::moveTo(const ValueTree revision)
{
    if (this->isThreadRunning())
    {
        this->stopThread(100);
    }

    if (this->targetVcsItemsSource != nullptr)
    {
        // здесь надо будет пройтись до корня и запомнить все ревизии
        Array<ValueTree> treePath;
        ValueTree currentRevision(revision);

        // сначала обнуляем состояние
        {
            ScopedWriteLock lock(this->stateLock);
            this->state = new HeadState();
        }

        Logger::writeToLog("Head::moveTo " + Revision::getUuid(currentRevision));

        while (currentRevision.isValid())
        {
            treePath.insert(0, currentRevision);
            currentRevision = currentRevision.getParent();
        }

        // затем, идти по ним в обратном порядке - от корня
        for (auto && i : treePath)
        {
            const ValueTree rev(i);

            Logger::writeToLog("Head::moveTo -> " + Revision::getUuid(rev));

            // собираем все дельты и применяем их к текущему состоянию
            for (int j = 0; j < rev.getNumProperties(); ++j)
            {
                Identifier id = rev.getPropertyName(j);
                const var &property = rev.getProperty(id);

                if (RevisionItem *item = dynamic_cast<RevisionItem *>(property.getObject()))
                {
                    if (item->getType() == RevisionItem::Added)
                    {
                        // ::Ptr сам создастся конструктором из указателя и увеличит его счетчик ссылок
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
        }
    }

    this->headingAt = revision;
    this->setDiffOutdated(true);
    return true;
}

void Head::pointTo(const ValueTree revision)
{
    this->headingAt = revision;
    this->setDiffOutdated(true);
}


bool Head::resetChangedItemToState(const VCS::RevisionItem::Ptr diffItem)
{
    if (this->targetVcsItemsSource == nullptr)
    { return false; }

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
        for (int i = 0; i < this->targetVcsItemsSource->getNumTrackedItems(); ++i)
        {
            VCS::TrackedItem *item = this->targetVcsItemsSource->getTrackedItem(i);

            if (item->getUuid() == diffItem->getUuid())
            {
                targetItem = item;
                break;
            }
        }

        if (targetItem)
        {
            targetItem->getDiffLogic()->resetStateTo(*sourceItem);
            return true;
        }
    }
    else if (diffItem->getType() == RevisionItem::Added)
    {
        VCS::TrackedItem *targetItem = nullptr;

        // снова ищем исходный с тем же уидом и вызываем deleteTrackedItem
        for (int i = 0; i < this->targetVcsItemsSource->getNumTrackedItems(); ++i)
        {
            VCS::TrackedItem *item = this->targetVcsItemsSource->getTrackedItem(i);

            if (item->getUuid() == diffItem->getUuid())
            {
                targetItem = item;
                break;
            }
        }

        if (targetItem)
        {
            return this->targetVcsItemsSource->deleteTrackedItem(targetItem);
        }
    }
    else if (diffItem->getType() == RevisionItem::Removed)
    {
        const String logicType(sourceItem->getDiffLogic()->getType());
        const Uuid id(sourceItem->getUuid());

        TrackedItem *newItem =
            this->targetVcsItemsSource->initTrackedItem(logicType, id);

        if (newItem)
        {
            newItem->getDiffLogic()->resetStateTo(*sourceItem);
        }
        return true;
    }

    return false;
}

void Head::checkout()
{
    if (this->targetVcsItemsSource == nullptr)
    { return; }

    if (this->state == nullptr)
    { return; }

    // clear all tracked items
    {
        Array<TrackedItem *> itemsToClear;

        for (int i = 0; i < this->targetVcsItemsSource->getNumTrackedItems(); ++i)
        {
            TrackedItem *ti = this->targetVcsItemsSource->getTrackedItem(i);

            if (this->state->getItemWithUuid(ti->getUuid()) != nullptr)
            {
                itemsToClear.add(ti);
            }
        }

        for (auto i : itemsToClear)
        {
            this->targetVcsItemsSource->deleteTrackedItem(i);
        }
    }

    //this->targetVcsItemsSource->clearAllTrackedItems();


    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        this->checkoutItem(stateItem);
    }
}

void Head::cherryPick(const Array<Uuid> uuids)
{
    if (this->targetVcsItemsSource == nullptr)
    { return; }

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
}

void Head::cherryPickAll()
{
    if (this->targetVcsItemsSource == nullptr)
    { return; }
    
    if (this->state == nullptr)
    { return; }
    
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        this->checkoutItem(stateItem);
    }
}

void Head::checkoutItem(VCS::RevisionItem::Ptr stateItem)
{
    if (this->targetVcsItemsSource == nullptr)
    { return; }

    // Changed и Added RevisionItem'ы нужно применять через resetStateTo
    TrackedItem *targetItem = nullptr;

    //Logger::writeToLog(stateItem->getVCSName());
    
    // ищем в проекте айтем с соответствующим уидом
    for (int j = 0; j < this->targetVcsItemsSource->getNumTrackedItems(); ++j)
    {
        TrackedItem *item = this->targetVcsItemsSource->getTrackedItem(j);

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
            targetItem->getDiffLogic()->resetStateTo(*stateItem);
        }
    }
    else if (stateItem->getType() == RevisionItem::Added)
    {
        // айтем не в проекте - добавляем
        if (!targetItem)
        {
            
            const String logicType(stateItem->getDiffLogic()->getType());
            const Uuid id(stateItem->getUuid());

            //Logger::writeToLog("Create tracked item of type: " + logicType);

            TrackedItem *newItem =
                this->targetVcsItemsSource->initTrackedItem(logicType, id);

            if (newItem)
            {
                newItem->getDiffLogic()->resetStateTo(*stateItem);
            }
        }
        else
        {
            targetItem->getDiffLogic()->resetStateTo(*stateItem);
        }
    }
    else if (stateItem->getType() == RevisionItem::Removed)
    {
        if (targetItem)
        {
            this->targetVcsItemsSource->deleteTrackedItem(targetItem);
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
        this->stopThread(50);
    }

    this->startThread(9);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Head::serialize() const
{
    auto xml = new XmlElement(Serialization::VCS::head);
    auto stateXml = new XmlElement(Serialization::VCS::headIndex);
    auto stateDataXml = new XmlElement(Serialization::VCS::headIndexData);

    {
        ScopedReadLock lock(this->stateLock);
        
        for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
        {
            const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
            XmlElement *serializedItem = stateItem->serialize();
            stateXml->addChildElement(serializedItem);
            
            // exports also deltas data
            for (int j = 0; j < stateItem->getNumDeltas(); ++j)
            {
                XmlElement *deltaData = stateItem->createDeltaDataFor(j);
                
                auto packItem = new XmlElement(Serialization::VCS::packItem);
                packItem->setAttribute(Serialization::VCS::packItemRevId, stateItem->getUuid().toString());
                packItem->setAttribute(Serialization::VCS::packItemDeltaId, stateItem->getDelta(j)->getUuid().toString());
                packItem->addChildElement(deltaData);
                
                stateDataXml->prependChildElement(packItem);
            }
        }
    }
    
    xml->addChildElement(stateXml);
    xml->addChildElement(stateDataXml);
    return xml;
}

void Head::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *headRoot = xml.hasTagName(Serialization::VCS::head) ?
        &xml : xml.getChildByName(Serialization::VCS::head);
    if (headRoot == nullptr) { return; }
    
    const XmlElement *indexRoot = headRoot->getChildByName(Serialization::VCS::headIndex);
    if (indexRoot == nullptr) { return; }

    const XmlElement *dataRoot = headRoot->getChildByName(Serialization::VCS::headIndexData);
    if (dataRoot == nullptr) { return; }
    
    forEachXmlChildElementWithTagName(*indexRoot, stateElement, Serialization::VCS::revisionItem)
    {
        RevisionItem::Ptr stateItem(new RevisionItem(this->pack, RevisionItem::Added, nullptr));
        stateItem->deserialize(*stateElement);

        //Logger::writeToLog("- " + stateItem->getVCSName());
        
        // import deltas data
        forEachXmlChildElementWithTagName(*dataRoot, dataElement, Serialization::VCS::packItem)
        {
            const String packItemRevId = dataElement->getStringAttribute(Serialization::VCS::packItemRevId);
            const String packItemDeltaId = dataElement->getStringAttribute(Serialization::VCS::packItemDeltaId);
            const XmlElement *deltaData = dataElement->getFirstChildElement();
            
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
    this->state = new HeadState();
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
    if (this->targetVcsItemsSource == nullptr)
    { return; }

    if (this->state == nullptr)
    { return; }
    
    this->setRebuildingDiffMode(true);
    this->sendChangeMessage();

    {
        ScopedWriteLock lock(this->diffLock);
        this->diff.removeAllChildren(nullptr);
        this->diff.removeAllProperties(nullptr);
    }

    ScopedReadLock threadStateLock(this->stateLock);

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

        // записи удаления рассматриваем позже
        if (stateItem->getType() == RevisionItem::Removed) { continue; }

        for (int j = 0; j < this->targetVcsItemsSource->getNumTrackedItems(); ++j)
        {
            if (this->threadShouldExit())
            {
                this->setRebuildingDiffMode(false);
                this->sendChangeMessage();
                return;
            }

            TrackedItem *targetItem = this->targetVcsItemsSource->getTrackedItem(j); // i.e. LayerTreeItem

            // айтем из состояния - существует в проекте. добавляем запись changed, если нужно.
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInTarget = true;

                ScopedPointer<Diff> itemDiff(targetItem->getDiffLogic()->createDiff(*stateItem));

                if (itemDiff->hasAnyChanges())
                {
                    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Changed, itemDiff));
                    var revisionVar(revisionRecord);

                    ScopedWriteLock itemDiffLock(this->diffLock);
                    this->diff.setProperty(stateItem->getUuid().toString(), revisionVar, nullptr);
                }

                break;
            }
        }

        // айтем из состояния - в проекте не найден. добавляем запись removed.
        if (! foundItemInTarget)
        {
            ScopedPointer<Diff> emptyDiff(new Diff(*stateItem));

            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Removed, emptyDiff));
            var revisionVar(revisionRecord);

            ScopedWriteLock emptyDiffLock(this->diffLock);
            this->diff.setProperty(stateItem->getUuid().toString(), revisionVar, nullptr);
        }
    }

    // теперь ищем айтемы в проекте, которые отсутствуют - или удалены - в состоянии
    for (int i = 0; i < this->targetVcsItemsSource->getNumTrackedItems(); ++i)
    {
        if (this->threadShouldExit())
        {
            this->setRebuildingDiffMode(false);
            this->sendChangeMessage();
            return;
        }

        bool foundItemInState = false;
        TrackedItem *targetItem = this->targetVcsItemsSource->getTrackedItem(i);

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

        // и добавляем запись - added, с дельтами, которые тупо копируем у targetItem
        if (! foundItemInState)
        {
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
            var revisionVar(revisionRecord);

            ScopedWriteLock lock(this->diffLock);
            this->diff.setProperty(targetItem->getUuid().toString(), revisionVar, nullptr);
        }
    }

    this->setDiffOutdated(false);
    this->setRebuildingDiffMode(false);
    this->sendChangeMessage();
}

// warning код практически дублирует Head::run
void Head::rebuildDiffSynchronously()
{
    if (this->targetVcsItemsSource == nullptr)
    { return; }
    
    if (this->state == nullptr)
    { return; }
    
    if (this->isRebuildingDiff())
    { return; }
    
    this->setRebuildingDiffMode(true);
    
    {
        ScopedWriteLock lock(this->diffLock);
        this->diff.removeAllChildren(nullptr);
        this->diff.removeAllProperties(nullptr);
    }
    
    ScopedReadLock rebuildStateLock(this->stateLock);
    
    for (int i = 0; i < this->state->getNumTrackedItems(); ++i)
    {
        bool foundItemInTarget = false;
        const RevisionItem::Ptr stateItem = static_cast<RevisionItem *>(this->state->getTrackedItem(i));
        
        // записи удаления рассматриваем позже
        if (stateItem->getType() == RevisionItem::Removed) { continue; }
        
        for (int j = 0; j < this->targetVcsItemsSource->getNumTrackedItems(); ++j)
        {
            TrackedItem *targetItem = this->targetVcsItemsSource->getTrackedItem(j); // i.e. LayerTreeItem
            
            // айтем из состояния - существует в проекте. добавляем запись changed, если нужно.
            if (stateItem->getUuid() == targetItem->getUuid())
            {
                foundItemInTarget = true;
                
                ScopedPointer<Diff> itemDiff(targetItem->getDiffLogic()->createDiff(*stateItem));
                
                if (itemDiff->hasAnyChanges())
                {
                    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Changed, itemDiff));
                    var revisionVar(revisionRecord);
                    
                    ScopedWriteLock lock(this->diffLock);
                    this->diff.setProperty(stateItem->getUuid().toString(), revisionVar, nullptr);
                }
                
                break;
            }
        }
        
        // айтем из состояния - в проекте не найден. добавляем запись removed.
        if (! foundItemInTarget)
        {
            ScopedPointer<Diff> emptyDiff(new Diff(*stateItem));
            
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Removed, emptyDiff));
            var revisionVar(revisionRecord);
            
            ScopedWriteLock lock(this->diffLock);
            this->diff.setProperty(stateItem->getUuid().toString(), revisionVar, nullptr);
        }
    }
    
    // теперь ищем айтемы в проекте, которые отсутствуют - или удалены - в состоянии
    for (int i = 0; i < this->targetVcsItemsSource->getNumTrackedItems(); ++i)
    {
        bool foundItemInState = false;
        TrackedItem *targetItem = this->targetVcsItemsSource->getTrackedItem(i);
        
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
        
        // и добавляем запись - added, с дельтами, которые тупо копируем у targetItem
        if (! foundItemInState)
        {
            RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
            var revisionVar(revisionRecord);
            
            ScopedWriteLock lock(this->diffLock);
            this->diff.setProperty(targetItem->getUuid().toString(), revisionVar, nullptr);
        }
    }
    
    this->setDiffOutdated(false);
    this->setRebuildingDiffMode(false);
    this->sendChangeMessage();
}
