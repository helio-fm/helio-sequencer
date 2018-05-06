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
#include "VersionControlEditorDefault.h"
#include "TrackedItem.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"
#include "SerializationKeys.h"

using namespace VCS;

VersionControl::VersionControl(WeakReference<VCS::TrackedItemsSource> parent,
                               const String &existingId,
                               const String &existingKeyBase64) :
    pack(new Pack()),
    stashes(new StashesRepository(pack)),
    head(pack, parent),
    rootRevision(Revision::create(pack, "root")),
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

    this->rootRevision = Revision::create(this->pack, TRANS("defaults::newproject::firstcommit"));

    MessageManagerLock lock;
    this->addChangeListener(&this->head);
    this->head.moveTo(this->rootRevision);
}

VersionControl::~VersionControl()
{
    MessageManagerLock lock;
    this->removeChangeListener(&this->head);
}

VersionControlEditor *VersionControl::createEditor()
{
    //if (App::isRunningOnPhone())
    //{
    //    return new VersionControlEditorPhone(*this);
    //}

    return new VersionControlEditorDefault(*this);
}


//===----------------------------------------------------------------------===//
// Push-pull stuff
//===----------------------------------------------------------------------===//

MD5 VersionControl::calculateHash() const
{
    // StringArray и sort - чтоб не зависеть от порядка чайлдов.
    StringArray ids(this->recursiveGetHashes(this->rootRevision));
    ids.sort(true);
    return MD5(ids.joinIntoString("").toUTF8());
}

StringArray VersionControl::recursiveGetHashes(const ValueTree revision) const
{
    StringArray sum;

    for (int i = 0; i < revision.getNumChildren(); ++i)
    {
        ValueTree child(revision.getChild(i));
        sum.addArray(this->recursiveGetHashes(child));
    }

    const String revisionSum(Revision::calculateHash(revision).toHexString());
    sum.add(revisionSum);
    return sum;
}

void VersionControl::mergeWith(VersionControl &remoteHistory)
{
    this->recursiveTreeMerge(this->getRoot(), remoteHistory.getRoot());

    this->publicId = remoteHistory.getPublicId();
    this->historyMergeVersion = remoteHistory.getVersion();

    ValueTree newHeadRevision(this->getRevisionById(this->rootRevision,
        Revision::getUuid(remoteHistory.getHead().getHeadingRevision())));

    if (! Revision::isEmpty(newHeadRevision))
    {
        this->head.moveTo(newHeadRevision);
    }

    this->pack->flush();
    this->sendChangeMessage();
}

void VersionControl::recursiveTreeMerge(ValueTree localRevision,
    ValueTree remoteRevision)
{
    // сначала мерж двух ревизий.
    // проход по чайлдам идет потом, чтоб head.moveTo у чайлда имел дело
    // с уже смерженным родителем.

    if (Revision::calculateHash(localRevision) != Revision::calculateHash(remoteRevision))
    {
        Revision::copyProperties(localRevision, remoteRevision);
        Revision::flush(localRevision);

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
        ValueTree remoteChild(remoteRevision.getChild(i));
        bool remoteChildExistsInLocal = false;

        for (int j = 0; j < localRevision.getNumChildren(); ++j)
        {
            ValueTree localChild(localRevision.getChild(j));
            if (Revision::getUuid(localChild) == Revision::getUuid(remoteChild))
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
            ValueTree newLocalChild(Revision::create(this->pack));
            Revision::copyProperties(newLocalChild, remoteChild);
            Revision::flush(newLocalChild);
            localRevision.appendChild(newLocalChild, nullptr);
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

void VersionControl::moveHead(const ValueTree revision)
{
    if (! Revision::isEmpty(revision))
    {
        this->head.moveTo(revision);
        this->sendChangeMessage();
    }
}

void VersionControl::checkout(const ValueTree revision)
{
    if (! Revision::isEmpty(revision))
    {
        this->head.moveTo(revision);
        this->head.checkout();
        this->sendChangeMessage();
    }
}

void VersionControl::cherryPick(const ValueTree revision, const Array<Uuid> uuids)
{
    if (! Revision::isEmpty(revision))
    {
        ValueTree headRevision(this->head.getHeadingRevision());
        this->head.moveTo(revision);
        this->head.cherryPick(uuids);
        this->head.moveTo(headRevision);
        this->sendChangeMessage();
    }
}

void VersionControl::quickAmendItem(TrackedItem *targetItem)
{
    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
    this->head.getHeadingRevision().setProperty(revisionRecord->getUuid().toString(), var(revisionRecord), nullptr);
    this->head.moveTo(this->head.getHeadingRevision());
    Revision::flush(this->head.getHeadingRevision());
    this->pack->flush();
    this->sendChangeMessage();
}

bool VersionControl::resetChanges(SparseSet<int> selectedItems)
{
    if (selectedItems.size() == 0) { return false; }

    ValueTree allChanges(this->head.getDiff());
    Array<RevisionItem::Ptr> changesToReset;

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];

        if (index >= allChanges.getNumProperties()) { return false; }

        const Identifier id(allChanges.getPropertyName(index));
        const var property(allChanges.getProperty(id));

        if (RevisionItem *item = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            changesToReset.add(item);
        }
    }

    this->head.resetChanges(changesToReset);
    return true;
}

bool VersionControl::resetAllChanges()
{
    ValueTree allChanges(this->head.getDiff());
    Array<RevisionItem::Ptr> changesToReset;

    for (int i = 0; i < allChanges.getNumProperties(); ++i)
    {
        const Identifier id(allChanges.getPropertyName(i));
        const var property(allChanges.getProperty(id));
        if (RevisionItem *item = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            changesToReset.add(item);
        }
    }
    
    this->head.resetChanges(changesToReset);
    return true;
}

bool VersionControl::commit(SparseSet<int> selectedItems, const String &message)
{
    if (selectedItems.size() == 0) { return false; }

    ValueTree newRevision(Revision::create(this->pack, message));
    ValueTree allChanges(this->head.getDiff().createCopy());

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];

        if (index >= allChanges.getNumProperties()) { return false; }

        const Identifier id(allChanges.getPropertyName(index));
        const var property(allChanges.getProperty(id));

        newRevision.setProperty(id, property, nullptr);
    }

    ValueTree headingRevision(this->head.getHeadingRevision());

    if (!headingRevision.isValid()) { return false; }

    headingRevision.appendChild(newRevision, nullptr);
    this->head.moveTo(newRevision);

    Revision::flush(newRevision);
    this->pack->flush();

    this->sendChangeMessage();
    return true;
}


//===----------------------------------------------------------------------===//
// Stashes
//===----------------------------------------------------------------------===//

bool VersionControl::stash(SparseSet<int> selectedItems,
    const String &message, bool shouldKeepChanges)
{
    if (selectedItems.size() == 0) { return false; }
    
    ValueTree newRevision(Revision::create(this->pack, message));
    ValueTree allChanges(this->head.getDiff().createCopy());
    
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
    
    this->sendChangeMessage();
    return true;
}

bool VersionControl::applyStash(const ValueTree stash, bool shouldKeepStash)
{
    if (! Revision::isEmpty(stash))
    {
        ValueTree headRevision(this->head.getHeadingRevision());
        this->head.moveTo(stash);
        this->head.cherryPickAll();
        this->head.moveTo(headRevision);
        
        if (! shouldKeepStash)
        {
            this->stashes->removeStash(stash);
        }
        
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

    ValueTree allChanges(this->head.getDiff().createCopy());
    this->stashes->storeQuickStash(allChanges);
    this->resetAllChanges();

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
    
    this->sendChangeMessage();
    return true;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree VersionControl::serialize() const
{
    ValueTree tree(Serialization::Core::versionControl);

    tree.setProperty(Serialization::VCS::vcsHistoryVersion, String(this->historyMergeVersion), nullptr);
    tree.setProperty(Serialization::VCS::vcsHistoryId, this->publicId, nullptr);
    tree.setProperty(Serialization::VCS::headRevisionId, Revision::getUuid(this->head.getHeadingRevision()), nullptr);
    
    tree.appendChild(this->key.serialize(), nullptr);
    tree.appendChild(Revision::serialize(this->rootRevision), nullptr);
    tree.appendChild(this->stashes->serialize(), nullptr);
    tree.appendChild(this->pack->serialize(), nullptr);
    tree.appendChild(this->head.serialize(), nullptr);
    
    return tree;
}

void VersionControl::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Core::versionControl) ?
        tree : tree.getChildWithName(Serialization::Core::versionControl);

    if (!root.isValid()) { return; }

    const String timeStamp = root.getProperty(Serialization::VCS::vcsHistoryVersion);
    this->historyMergeVersion = timeStamp.getLargeIntValue();

    this->publicId = root.getProperty(Serialization::VCS::vcsHistoryId, this->publicId);

    const String headId = root.getProperty(Serialization::VCS::headRevisionId);
    Logger::writeToLog("Head ID is " + headId);

    this->key.deserialize(root);
    Revision::deserialize(this->rootRevision, root);
    this->stashes->deserialize(root);
    this->pack->deserialize(root);

    {
        const double h1 = Time::getMillisecondCounterHiRes();
        this->head.deserialize(root);
        const double h2 = Time::getMillisecondCounterHiRes();
        Logger::writeToLog("Loading index done in " + String(h2 - h1) + "ms");
    }
    
    ValueTree headRevision(this->getRevisionById(this->rootRevision, headId));

    // здесь мы раньше полностью десериализовали состояние хэда.
    // если дерево истории со временеи становится большим, moveTo со всеми мержами занимает кучу времени.
    // если работать в десятками тысяч событий, загрузка индекса длится ~2ms, а пересборка индекса - ~500ms
    // поэтому moveTo убираем, оставляем pointTo
    
    if (! Revision::isEmpty(headRevision))
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
    Revision::reset(this->rootRevision);
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

ValueTree VersionControl::getRevisionById(const ValueTree startFrom, const String &id) const
{
    //Logger::writeToLog("getRevisionById, iterating " + startFrom.getUuid());

    if (Revision::getUuid(startFrom) == id)
    {
        return startFrom;
    }

    for (int i = 0; i < startFrom.getNumChildren(); ++i)
    {
        ValueTree child(startFrom.getChild(i));
        ValueTree search(this->getRevisionById(child, id));

        if (! Revision::isEmpty(search))
        {
            //Logger::writeToLog("search ok, returning " + search.getUuid());
            return search;
        }
    }

    return Revision::create(this->pack);
}
