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
#include "VersionControlEditor.h"
#include "TrackedItem.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"
#include "SerializationKeys.h"
#include "ResourceSyncService.h"
#include "App.h"

using namespace VCS;

VersionControl::VersionControl(VCS::TrackedItemsSource &parent) :
    parent(parent),
    pack(new Pack()),
    head(pack, parent),
    stashes(new StashesRepository(pack)),
    rootRevision(new Revision(pack, TRANS("defaults::newproject::firstcommit")))
{
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
    return new VersionControlEditor(*this);
}

//===----------------------------------------------------------------------===//
// VCS
//===----------------------------------------------------------------------===//

void VersionControl::moveHead(const Revision::Ptr revision)
{
    if (! revision->isEmpty())
    {
        this->head.moveTo(revision);
        this->sendChangeMessage();
    }
}

void VersionControl::checkout(const Revision::Ptr revision)
{
    if (! revision->isEmpty())
    {
        this->head.moveTo(revision);
        this->head.checkout();
        this->sendChangeMessage();
    }
}

void VersionControl::cherryPick(const Revision::Ptr revision, const Array<Uuid> uuids)
{
    if (! revision->isEmpty())
    {
        auto headRevision(this->head.getHeadingRevision());
        this->head.moveTo(revision);
        this->head.cherryPick(uuids);
        this->head.moveTo(headRevision);
        this->sendChangeMessage();
    }
}

void VersionControl::appendSubtree(const VCS::Revision::Ptr subtree, const String &appendRevisionId)
{
    if (appendRevisionId.isEmpty())
    {
        // if appendRevisionId is empty - replace root?
        // todo make clear how to clone projects
        //DBG("Warning: replacing history remote tree");
        //this->rootRevision = subtree;
    }
    else
    {
        if (auto targetRevision = this->getRevisionById(this->rootRevision, appendRevisionId))
        {
            targetRevision->addChild(subtree);
            this->sendChangeMessage();
        }
    }
}

Revision::Ptr VersionControl::updateShallowRevisionData(const String &id, const ValueTree &data)
{
    if (auto targetRevision = this->getRevisionById(this->rootRevision, id))
    {
        targetRevision->deserialize(data);
        targetRevision->flush();
        this->sendChangeMessage();
        return targetRevision;
    }

    return nullptr;
}

void VersionControl::quickAmendItem(TrackedItem *targetItem)
{
    RevisionItem::Ptr revisionRecord(new RevisionItem(this->pack, RevisionItem::Added, targetItem));
    this->head.getHeadingRevision()->addItem(revisionRecord);
    this->head.moveTo(this->head.getHeadingRevision());
    this->head.getHeadingRevision()->flush();
    this->pack->flush();
    this->sendChangeMessage();
}

bool VersionControl::resetChanges(SparseSet<int> selectedItems)
{
    if (selectedItems.size() == 0) { return false; }

    Revision::Ptr allChanges(this->head.getDiff());
    Array<RevisionItem::Ptr> changesToReset;

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        if (index >= allChanges->getItems().size()) { return false; }
        if (auto *item = allChanges->getItems()[index].get())
        {
            changesToReset.add(item);
        }
    }

    this->head.resetChanges(changesToReset);
    return true;
}

bool VersionControl::resetAllChanges()
{
    Revision::Ptr allChanges(this->head.getDiff());
    Array<RevisionItem::Ptr> changesToReset;

    for (auto *item : allChanges->getItems())
    {
        changesToReset.add(item);
    }
    
    this->head.resetChanges(changesToReset);
    return true;
}

bool VersionControl::commit(SparseSet<int> selectedItems, const String &message)
{
    if (selectedItems.size() == 0) { return false; }

    Revision::Ptr newRevision(new Revision(this->pack, message));
    Revision::Ptr allChanges(this->head.getDiff());

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        if (index >= allChanges->getItems().size()) { return false; }
        if (auto *item = allChanges->getItems()[index].get())
        {
            newRevision->addItem(item);
        }
    }

    Revision::Ptr headingRevision(this->head.getHeadingRevision());
    if (headingRevision == nullptr) { return false; }

    headingRevision->addChild(newRevision);
    this->head.moveTo(newRevision);

    newRevision->flush();
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
    
    Revision::Ptr newRevision(new Revision(this->pack, message));
    Revision::Ptr allChanges(this->head.getDiff());
    
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        if (index >= allChanges->getItems().size()) { return false; }
        newRevision->addItem(allChanges->getItems()[index]);
    }
    
    this->stashes->addStash(newRevision);

    if (! shouldKeepChanges)
    {
        this->resetChanges(selectedItems);
    }
    
    this->sendChangeMessage();
    return true;
}

bool VersionControl::applyStash(const Revision::Ptr stash, bool shouldKeepStash)
{
    if (! stash->isEmpty())
    {
        Revision::Ptr headRevision(this->head.getHeadingRevision());
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

    Revision::Ptr allChanges(this->head.getDiff());
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

    tree.setProperty(Serialization::VCS::headRevisionId, this->head.getHeadingRevision()->getUuid(), nullptr);
    
    tree.appendChild(this->rootRevision->serialize(), nullptr);
    tree.appendChild(this->stashes->serialize(), nullptr);
    tree.appendChild(this->pack->serialize(), nullptr);
    tree.appendChild(this->head.serialize(), nullptr);
    tree.appendChild(this->remoteCache.serialize(), nullptr);

    return tree;
}

void VersionControl::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Core::versionControl) ?
        tree : tree.getChildWithName(Serialization::Core::versionControl);

    if (!root.isValid()) { return; }

    const String headId = root.getProperty(Serialization::VCS::headRevisionId);
    DBG("Head ID is " + headId);

    this->rootRevision->deserialize(root);
    this->stashes->deserialize(root);
    this->remoteCache.deserialize(root);
    this->pack->deserialize(root);

    {
        const double h1 = Time::getMillisecondCounterHiRes();
        this->head.deserialize(root);
        const double h2 = Time::getMillisecondCounterHiRes();
        DBG("Loading VCS snapshot done in " + String(h2 - h1) + "ms");
    }
    
    if (auto headRevision = this->getRevisionById(this->rootRevision, headId))
    {
        this->head.pointTo(headRevision);
    }
}

void VersionControl::reset()
{
    this->rootRevision->reset();
    this->head.reset();
    this->remoteCache.reset();
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
// Network
//===----------------------------------------------------------------------===//

void VersionControl::syncAllRevisions()
{
    App::Helio().getResourceSyncService()->syncRevisions(this,
        this->parent.getVCSId(), this->parent.getVCSName(), {});
}

void VersionControl::syncRevision(const VCS::Revision::Ptr revision)
{
    App::Helio().getResourceSyncService()->syncRevisions(this,
        this->parent.getVCSId(), this->parent.getVCSName(),
        { revision->getUuid() });
}

void VersionControl::updateLocalSyncCache(const VCS::Revision::Ptr revision)
{
    this->remoteCache.updateForLocalRevision(revision);
    this->sendChangeMessage();
}

void VersionControl::updateRemoteSyncCache(const Array<RevisionDto> &revisions)
{
    this->remoteCache.updateForRemoteRevisions(revisions);
    this->sendChangeMessage();
}

Revision::SyncState VersionControl::getRevisionSyncState(const Revision::Ptr revision) const
{
    if (!revision->isShallowCopy() && this->remoteCache.hasRevisionTracked(revision))
    {
        return Revision::FullSync;
    }
    else if (revision->isShallowCopy())
    {
        return Revision::ShallowCopy;
    }

    return Revision::NoSync;
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Revision::Ptr VersionControl::getRevisionById(const Revision::Ptr startFrom, const String &id) const
{
    if (startFrom->getUuid() == id)
    {
        return startFrom;
    }

    for (auto *child : startFrom->getChildren())
    {
        if (auto search = this->getRevisionById(child, id))
        {
            return search;
        }
    }

    return nullptr;
}
