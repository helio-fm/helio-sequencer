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
#include "App.h"
#include "VersionControl.h"
#include "VersionControlEditorPhone.h"
#include "VersionControlEditorDefault.h"
#include "TrackedItem.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"
#include "Client.h"
#include "Supervisor.h"
#include "SerializationKeys.h"

using namespace VCS;


VersionControl::VersionControl(WeakReference<VCS::TrackedItemsSource> parent,
                               const String &existingId,
                               const String &existingKeyBase64) :
    pack(new Pack()),
    stashes(new StashesRepository(pack)),
    head(pack, parent),
    root(pack, "root"),
    parentItem(parent),
    historyMergeVersion(1)
{
    // both existing id and key should be empty or not at the same time.
    jassert((existingId.isEmpty() && existingKeyBase64.isEmpty()) ||
            (existingId.isNotEmpty() && existingKeyBase64.isNotEmpty()));
    
    if (existingId.isNotEmpty())
    {
        this->publicId = existingId;
    }
    else
    {
        Uuid id1;
        Uuid id2;
        this->publicId = id1.toString() + id2.toString();
    }
    
    if (existingKeyBase64.isNotEmpty())
    {
        this->key.restoreFromBase64(existingKeyBase64);
    }

    this->root = Revision(this->pack, TRANS("defaults::newproject::firstcommit"));

    this->remote = new Client(*this);

    MessageManagerLock lock;
    this->addChangeListener(&this->head);
    this->head.moveTo(this->root);
}

VersionControl::~VersionControl()
{
    MessageManagerLock lock;
    this->removeChangeListener(&this->head);
}

VersionControlEditor *VersionControl::createEditor()
{
    if (App::isRunningOnPhone())
    {
        return new VersionControlEditorPhone(*this);
    }
    else
    {
        return new VersionControlEditorDefault(*this);
    }
    
    return nullptr;
}


//===----------------------------------------------------------------------===//
// Push-pull stuff
//===----------------------------------------------------------------------===//

MD5 VersionControl::calculateHash() const
{
    // StringArray и sort - чтоб не зависеть от порядка чайлдов.
    StringArray ids(this->recursiveGetHashes(this->root));
    ids.sort(true);
    return MD5(ids.joinIntoString("").toUTF8());
}

StringArray VersionControl::recursiveGetHashes(const Revision revision) const
{
    StringArray sum;

    for (int i = 0; i < revision.getNumChildren(); ++i)
    {
        Revision child(revision.getChild(i));
        sum.addArray(this->recursiveGetHashes(child));
    }

    const String revisionSum(revision.calculateHash().toHexString());
    sum.add(revisionSum);
    return sum;
}

void VersionControl::mergeWith(VersionControl &remoteHistory)
{
    this->recursiveTreeMerge(this->getRoot(), remoteHistory.getRoot());

    this->publicId = remoteHistory.getPublicId();
    this->historyMergeVersion = remoteHistory.getVersion();

    Revision newHeadRevision(this->getRevisionById(this->root,
                             remoteHistory.getHead().getHeadingRevision().getUuid()));

    if (!newHeadRevision.isEmpty())
    {
        this->head.moveTo(newHeadRevision);
    }

    this->pack->flush();
    this->sendChangeMessage();
}

void VersionControl::recursiveTreeMerge(Revision localRevision,
                                        Revision remoteRevision)
{
    // сначала мерж двух ревизий.
    // проход по чайлдам идет потом, чтоб head.moveTo у чайлда имел дело
    // с уже смерженным родителем.

    if (localRevision.calculateHash() != remoteRevision.calculateHash())
    {
        localRevision.copyPropertiesFrom(remoteRevision);
        localRevision.flushData();

        // amend не работает
        //Revision headRevision(this->head.getHeadingRevision());
        //this->head.moveTo(localRevision);

        //this->head.mergeHeadWith(remoteRevision,
        //                         remoteRevision.getMessage(),
        //                         remoteRevision.getTimeStamp());

        //localRevision.setProperty(Serialization::VCS::commitId, remoteRevision.getUuid(), nullptr);
        //localRevision.setProperty(Serialization::VCS::commitVersion, remoteRevision.getVersion(), nullptr);
        //localRevision.flushData();

        //this->head.moveTo(headRevision);
    }

    // затем пройтись по чайлдам.
    // аналогичные - смержить этой же процедурой.
    // несуществующие локально - скопировать.
    // новые локально - оставить в покое.

    for (int i = 0; i < remoteRevision.getNumChildren(); ++i)
    {
        Revision remoteChild(remoteRevision.getChild(i));
        bool remoteChildExistsInLocal = false;

        for (int j = 0; j < localRevision.getNumChildren(); ++j)
        {
            Revision localChild(localRevision.getChild(j));

            if (localChild.getUuid() == remoteChild.getUuid())
            {
                this->recursiveTreeMerge(localChild, remoteChild);
                remoteChildExistsInLocal = true;
                break;
            }
        }

        // копируем, тоже рекурсией.
        if (!remoteChildExistsInLocal)
        {
            // скопировать все свойства, кроме пака. только свойства, не чайлдов.
            Revision newLocalChild(this->pack, "");
            newLocalChild.copyPropertiesFrom(remoteChild);
            newLocalChild.flushData();
            localRevision.addChild(newLocalChild, -1, nullptr);
            this->recursiveTreeMerge(newLocalChild, remoteChild);
        }
    }

    // должно работать.

    // остается вопрос, как мы отличим up to date
    // от new/outdated?
    // на сервер отдается хэш от уидов всех айтемов.
    // тогда мы точно сможем сказать, изменилась ли история.
    // если изменилась, то ни версия, ни таймштамп истории здесь не критерий.
    // качаем, и - что дальше?
    // ступор.
    //===------------------------------------------------------------------===//
    // что, если увеличивать счетчик только после мержа двух историй?
    // окей. представь 2 девайса:
    // 1   1   1
    // 1a  1   1b
    // 1a<<1   1b   должен выдать ошибку (разные хэши, одинаковые версии)
    // 1a>>1   1b   разные хэши, должен смержить серверную версию с локальной, увеличить счетчики у обеих
    // 2   2   1b   так?
    // 2   2 <<1b   ошибка, нельзя пушить на более позднюю версию
    // 2   2 >>1b   должен сделать пулл, смержить локальную с серверной, увеличить счетчик у локальной
    // 2   2   2b
    // 2   2 <<2b   разные хэши, должен смержить серверную версию с локальной, увеличить счетчики у обеих
    // 2   3   3    так?
    // 2c  3   3    сделал изменения
    // 2c>>3   3    ошибка, нельзя пушить на более позднюю версию
    // 2с<<3   3    должен сделать пулл, смержить локальную с серверной, увеличить счетчик у локальной
    // 3с  3   3
    // и так далее, да, схема должна работать.
    //===------------------------------------------------------------------===//
    // итак, пулл разрешен только если серверная версия больше.
    // если версии равны и равны хэши - up to date
    // остальное - ошибка.
    //===------------------------------------------------------------------===//
    // пуш разрешен, только если локальная версия больше, либо версии равны, но не равны хэши
    // если версии и хэши равны - up to date
    // остальное - ошибка.
    //===------------------------------------------------------------------===//
}


//===----------------------------------------------------------------------===//
// VCS
//===----------------------------------------------------------------------===//

void VersionControl::moveHead(const VCS::Revision revision)
{
    if (! revision.isEmpty())
    {
        this->head.moveTo(revision);
        this->sendChangeMessage();
    }
}

void VersionControl::checkout(const VCS::Revision revision)
{
    if (! revision.isEmpty())
    {
        this->head.moveTo(revision);
        this->head.checkout();
        Supervisor::track(Serialization::Activities::vcsCheckoutRevision);
        this->sendChangeMessage();
    }
}

void VersionControl::cherryPick(const VCS::Revision revision, const Array<Uuid> uuids)
{
    if (! revision.isEmpty())
    {
        Revision headRevision(this->head.getHeadingRevision());
        this->head.moveTo(revision);
        this->head.cherryPick(uuids);
        this->head.moveTo(headRevision);
        Supervisor::track(Serialization::Activities::vcsCheckoutItems);
        this->sendChangeMessage();
    }
}

void VersionControl::quickAmendItem(TrackedItem *targetItem)
{
    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
    this->head.getHeadingRevision().setProperty(revisionRecord->getUuid().toString(), var(revisionRecord), nullptr);
    this->head.moveTo(this->head.getHeadingRevision());
    this->head.getHeadingRevision().flushData();
    this->pack->flush();
    this->sendChangeMessage();
}

bool VersionControl::resetChanges(SparseSet<int> selectedItems)
{
    if (selectedItems.size() == 0) { return false; }

    Revision allChanges(this->head.getDiff());

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];

        if (index >= allChanges.getNumProperties()) { return false; }

        const Identifier id(allChanges.getPropertyName(index));
        const var property(allChanges.getProperty(id));

        if (RevisionItem *item = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            this->head.resetChangedItemToState(item);
        }
    }

    Supervisor::track(Serialization::Activities::vcsReset);
    //this->sendChangeMessage();

    return true;
}

bool VersionControl::resetAllChanges()
{
    Revision allChanges(this->head.getDiff());
    
    for (int i = 0; i < allChanges.getNumProperties(); ++i)
    {
        const Identifier id(allChanges.getPropertyName(i));
        const var property(allChanges.getProperty(id));
        
        if (RevisionItem *item = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            this->head.resetChangedItemToState(item);
        }
    }
    
    Supervisor::track(Serialization::Activities::vcsReset);
    //this->sendChangeMessage();
    
    return true;
}

bool VersionControl::commit(SparseSet<int> selectedItems, const String &message)
{
    if (selectedItems.size() == 0) { return false; }

    Revision newRevision(this->pack, message);

    Revision allChanges(this->head.getDiff().createCopy());

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];

        if (index >= allChanges.getNumProperties()) { return false; }

        const Identifier id(allChanges.getPropertyName(index));
        const var property(allChanges.getProperty(id));

        newRevision.setProperty(id, property, nullptr);
    }

    Revision headingRevision(this->head.getHeadingRevision());

    if (!headingRevision.isValid()) { return false; }

    headingRevision.addChild(newRevision, -1, nullptr);
    this->head.moveTo(newRevision);

    newRevision.flushData();
    this->pack->flush();

    Supervisor::track(Serialization::Activities::vcsCommit);
    this->sendChangeMessage();

    return true;
}


//===----------------------------------------------------------------------===//
// Stashes
//===----------------------------------------------------------------------===//

bool VersionControl::stash(SparseSet<int> selectedItems, const String &message, bool shouldKeepChanges)
{
    if (selectedItems.size() == 0) { return false; }
    
    Revision newRevision(this->pack, message);
    
    Revision allChanges(this->head.getDiff().createCopy());
    
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        
        if (index >= allChanges.getNumProperties()) { return false; }
        
        const Identifier id(allChanges.getPropertyName(index));
        const var property(allChanges.getProperty(id));
        
        newRevision.setProperty(id, property, nullptr);
    }
    
    this->stashes->addStash(newRevision);

    if (! shouldKeepChanges)
    {
        this->resetChanges(selectedItems);
    }
    
    Supervisor::track(Serialization::Activities::vcsStash);
    this->sendChangeMessage();
    
    return true;
}

bool VersionControl::applyStash(const VCS::Revision stash, bool shouldKeepStash)
{
    if (! stash.isEmpty())
    {
        Revision headRevision(this->head.getHeadingRevision());
        this->head.moveTo(stash);
        this->head.cherryPickAll();
        this->head.moveTo(headRevision);
        
        if (! shouldKeepStash)
        {
            this->stashes->removeStash(stash);
        }
        
        Supervisor::track(Serialization::Activities::vcsApplyStash);
        this->sendChangeMessage();
        return true;
    }
    
    return false;
}

bool VersionControl::applyStash(const String &stashId, bool shouldKeepStash)
{
    return this->applyStash(this->stashes->getUserStashWithName(stashId), shouldKeepStash);
}

bool VersionControl::hasQuickStash() const
{
    return (! this->stashes->hasQuickStash());
}

bool VersionControl::quickStashAll()
{
    if (this->hasQuickStash())
    { return false; }

    Revision allChanges(this->head.getDiff().createCopy());
    this->stashes->storeQuickStash(allChanges);
    this->resetAllChanges();

    Supervisor::track(Serialization::Activities::vcsStash);
    this->sendChangeMessage();
    
    return true;
}

bool VersionControl::applyQuickStash()
{
    if (! this->hasQuickStash())
    { return false; }
    
    Head tempHead(this->head);
    tempHead.mergeStateWith(this->stashes->getQuickStash());
    tempHead.cherryPickAll();
    this->stashes->resetQuickStash();
    
    Supervisor::track(Serialization::Activities::vcsApplyStash);
    this->sendChangeMessage();
    return true;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *VersionControl::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::versionControl);

    xml->setAttribute(Serialization::VCS::vcsHistoryVersion, String(this->historyMergeVersion));
    xml->setAttribute(Serialization::VCS::vcsHistoryId, this->publicId);
    xml->setAttribute(Serialization::VCS::headRevisionId, this->head.getHeadingRevision().getUuid());
    
    xml->addChildElement(this->key.serialize());
    xml->addChildElement(this->root.serialize());
    xml->addChildElement(this->stashes->serialize());
    xml->addChildElement(this->pack->serialize());
    xml->addChildElement(this->head.serialize());
    
    return xml;
}

void VersionControl::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = xml.hasTagName(Serialization::Core::versionControl) ?
                                 &xml : xml.getChildByName(Serialization::Core::versionControl);

    if (mainSlot == nullptr) { return; }

    const String timeStamp = mainSlot->getStringAttribute(Serialization::VCS::vcsHistoryVersion);
    this->historyMergeVersion = timeStamp.getLargeIntValue();

    this->publicId = mainSlot->getStringAttribute(Serialization::VCS::vcsHistoryId, this->publicId);

    const String headId = mainSlot->getStringAttribute(Serialization::VCS::headRevisionId);
    Logger::writeToLog("Head ID is " + headId);

    this->key.deserialize(*mainSlot);
    this->root.deserialize(*mainSlot);
    this->stashes->deserialize(*mainSlot);
    this->pack->deserialize(*mainSlot);

    {
        const double h1 = Time::getMillisecondCounterHiRes();
        this->head.deserialize(*mainSlot);
        const double h2 = Time::getMillisecondCounterHiRes();
        Logger::writeToLog("Loading index done in " + String(h2 - h1) + "ms");
    }
    
    Revision headRevision(this->getRevisionById(this->root, headId));

    // здесь мы раньше полностью десериализовали состояние хэда.
    // если дерево истории со временеи становится большим, moveTo со всеми мержами занимает кучу времени.
    // если работать в десятками тысяч событий, загрузка индекса длится ~2ms, а пересборка индекса - ~500ms
    // поэтому moveTo убираем, оставляем pointTo
    
    if (!headRevision.isEmpty())
    {
        const double t1 = Time::getMillisecondCounterHiRes();

        this->head.pointTo(headRevision);
        //this->head.moveTo(headRevision);

        const double t2 = Time::getMillisecondCounterHiRes();
        //Logger::writeToLog("Building index done in " + String(t2 - t1) + "ms");
    }
//#endif
}

void VersionControl::reset()
{
    this->root.reset();
    this->head.reset();
    this->stashes->reset();
    this->pack->reset();
}


//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void VersionControl::changeListenerCallback(ChangeBroadcaster* source)
{
    // Project changed
    this->getHead().setDiffOutdated(true);
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Revision VersionControl::getRevisionById(const Revision startFrom, const String &id) const
{
    //Logger::writeToLog("getRevisionById, iterating " + startFrom.getUuid());

    if (startFrom.getUuid() == id)
    {
        return startFrom;
    }

    for (int i = 0; i < startFrom.getNumChildren(); ++i)
    {
        Revision child(startFrom.getChild(i));
        Revision search(this->getRevisionById(child, id));

        if (!search.isEmpty())
        {
            //Logger::writeToLog("search ok, returning " + search.getUuid());
            return search;
        }
    }

    return Revision(this->pack, "");
}
