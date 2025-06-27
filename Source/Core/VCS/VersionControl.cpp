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
#include "VersionControl.h"
#include "VersionControlEditor.h"

VersionControl::VersionControl(VCS::TrackedItemsSource &parent) :
    parent(parent),
    head(parent),
    stashes(new VCS::StashesRepository()),
    rootRevision(new VCS::Revision(TRANS(I18n::Defaults::newProjectFirstCommit)))
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

void VersionControl::moveHead(const VCS::Revision::Ptr revision)
{
    if (!revision->isEmpty())
    {
        this->head.moveTo(revision);
        this->sendChangeMessage();
    }
}

void VersionControl::checkout(const VCS::Revision::Ptr revision)
{
    if (!revision->isEmpty())
    {
        this->head.moveTo(revision);
        this->head.checkout();

        // with these two lines it will behave like a hard reset,
        // which is less flexible but seems more convenient:
        this->head.rebuildDiffIfNeeded();
        this->resetAllChanges();

        this->sendChangeMessage();
    }
}

void VersionControl::cherryPick(const VCS::Revision::Ptr revision, const Array<Uuid> uuids)
{
    if (!revision->isEmpty())
    {
        auto headRevision(this->head.getHeadingRevision());
        this->head.moveTo(revision);
        this->head.cherryPick(uuids);
        this->head.moveTo(headRevision);
        this->sendChangeMessage();
    }
}

void VersionControl::quickAmendItem(VCS::TrackedItem *targetItem)
{
    // warning: this is not a fully-functional amend,
    // it is only used when a new tracked item is added to revision;
    // changes and deletions to committed items will not work,
    // this is only used when initializing the first version automatically:
    VCS::RevisionItem::Ptr revisionRecord(new VCS::RevisionItem(VCS::RevisionItem::Type::Added, targetItem));
    this->head.getHeadingRevision()->addItem(revisionRecord);
    this->head.moveTo(this->head.getHeadingRevision());
    this->sendChangeMessage();
}

bool VersionControl::resetChanges(SparseSet<int> selectedItems)
{
    if (selectedItems.isEmpty())
    {
        return false;
    }

    VCS::Revision::Ptr allChanges(this->head.getDiff());
    Array<VCS::RevisionItem::Ptr> changesToReset;

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        if (index >= allChanges->getItems().size()) { return false; }
        if (auto *item = allChanges->getItems()[index].get())
        {
            changesToReset.add(item);
        }
    }

    const auto result = this->head.resetChanges(changesToReset);
    this->sendChangeMessage();
    return result;
}

void VersionControl::resetAllChanges()
{
    VCS::Revision::Ptr allChanges(this->head.getDiff());
    Array<VCS::RevisionItem::Ptr> changesToReset;

    for (auto *item : allChanges->getItems())
    {
        changesToReset.add(item);
    }
    
    this->head.resetChanges(changesToReset);
    this->sendChangeMessage();
}

bool VersionControl::commit(SparseSet<int> selectedItems, const String &message)
{
    if (selectedItems.isEmpty())
    {
        return false;
    }

    VCS::Revision::Ptr newRevision(new VCS::Revision(message));
    VCS::Revision::Ptr allChanges(this->head.getDiff());

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        const int index = selectedItems[i];
        if (index >= allChanges->getItems().size()) { return false; }
        if (auto *item = allChanges->getItems()[index].get())
        {
            newRevision->addItem(item);
        }
    }

    VCS::Revision::Ptr headingRevision(this->head.getHeadingRevision());
    if (headingRevision == nullptr) { return false; }

    headingRevision->addChild(newRevision);
    this->head.moveTo(newRevision);

    this->sendChangeMessage();
    return true;
}


//===----------------------------------------------------------------------===//
// Stashes
//===----------------------------------------------------------------------===//

bool VersionControl::stash(SparseSet<int> selectedItems,
    const String &message, bool shouldKeepChanges)
{
    if (selectedItems.isEmpty())
    {
        return false;
    }
    
    VCS::Revision::Ptr newRevision(new VCS::Revision(message));
    VCS::Revision::Ptr allChanges(this->head.getDiff());
    
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

bool VersionControl::applyStash(const VCS::Revision::Ptr stash, bool shouldKeepStash)
{
    if (stash->isEmpty())
    {
        return false;
    }
    
    VCS::Revision::Ptr headRevision(this->head.getHeadingRevision());
    this->head.moveTo(stash);
    this->head.cherryPickAll();
    this->head.moveTo(headRevision);

    if (!shouldKeepStash)
    {
        this->stashes->removeStash(stash);
    }

    this->sendChangeMessage();
    return true;
}

bool VersionControl::applyStash(const String &stashId, bool shouldKeepStash)
{
    return this->applyStash(this->stashes->getUserStashWithName(stashId), shouldKeepStash);
}

bool VersionControl::hasQuickStash() const
{
    return !this->stashes->hasQuickStash();
}

bool VersionControl::quickStashAll()
{
    if (this->hasQuickStash())
    {
        return false;
    }

    VCS::Revision::Ptr allChanges(this->head.getDiff());
    this->stashes->storeQuickStash(allChanges);
    this->resetAllChanges();

    this->sendChangeMessage();
    return true;
}

bool VersionControl::restoreQuickStash()
{
    if (!this->hasQuickStash())
    {
        return false;
    }
    
    VCS::Head tempHead(this->head);
    tempHead.mergeStateWith(this->stashes->getQuickStash());
    tempHead.cherryPickAll();
    this->stashes->resetQuickStash();
    
    this->sendChangeMessage();
    return true;
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void VersionControl::changeListenerCallback(ChangeBroadcaster *source)
{
    this->getHead().setDiffOutdated(true); // the project has changed
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData VersionControl::serialize() const
{
    SerializedData tree(Serialization::Core::versionControl);

    tree.setProperty(Serialization::VCS::headRevisionId, this->head.getHeadingRevision()->getUuid());
    tree.setProperty(Serialization::VCS::diffFormatVersion, VersionControl::diffFormatVersion);

    tree.appendChild(this->rootRevision->serialize());
    tree.appendChild(this->stashes->serialize());
    tree.appendChild(this->head.serialize());

    return tree;
}

void VersionControl::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root = data.hasType(Serialization::Core::versionControl) ?
        data : data.getChildWithName(Serialization::Core::versionControl);

    if (!root.isValid()) { return; }

    const String headId = root.getProperty(Serialization::VCS::headRevisionId);

    this->rootRevision->deserialize(root);
    this->stashes->deserialize(root);

    {
#if DEBUG
        const double headLoadStart = Time::getMillisecondCounterHiRes();
#endif
        this->head.deserialize(root);
        DBG("Loading VCS snapshot done in " +
            String(Time::getMillisecondCounterHiRes() - headLoadStart) + "ms");
    }
    
    if (auto headRevision = this->getRevisionById(this->rootRevision, headId))
    {
        // head keeps a snapshot node, which is the result of applying all deltas
        // from the start (moveTo() does this), in other words, the "project state" of
        // current head position, and it will be used as a baseline when making a diff;
        // we persist it for performance: rebuilding it from scratch is super slow;
        // as the app development moves forward, the snapshot, being a kind of a cache,
        // can get outdated, i.e. not containing all supported delta types, and needs
        // to be rebuilt to make correct diffs.

        const int snapshotDiffFormatVersion =
            root.getProperty(Serialization::VCS::diffFormatVersion, 0);

        const bool needToRebuildSnapshot =
            snapshotDiffFormatVersion != VersionControl::diffFormatVersion;

        if (needToRebuildSnapshot)
        {
            DBG("Found outdated diff format, rebuilding VCS snapshot");
            this->head.moveTo(headRevision);
        }
        else
        {
            this->head.pointTo(headRevision);
        }
    }
}

void VersionControl::reset()
{
    this->rootRevision->reset();
    this->head.reset();
    this->stashes->reset();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

VCS::Revision::Ptr VersionControl::getRevisionById(const VCS::Revision::Ptr startFrom, const String &id) const
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
