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
#include "Pattern.h"
#include "PatternActions.h"
#include "ProjectTreeItem.h"
#include "ProjectEventDispatcher.h"
#include "UndoStack.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

struct ClipIdGenerator
{
    static String generateId(uint8 length = 2)
    {
        String id;
        Random r;
        r.setSeedRandomly();
        static const char idChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        for (size_t i = 0; i < length; ++i)
        {
            id += idChars[r.nextInt(62)];
        }
        return id;
    }
};

Pattern::Pattern(MidiTrack &parentTrack,
    ProjectEventDispatcher &dispatcher) :
    track(parentTrack),
    eventDispatcher(dispatcher),
    lastStartBeat(0.f),
    lastEndBeat(0.f)
{
    // Add default single instance (we need to have at least one clip on a pattern):
    this->clips.add(new Clip(this));
}

Pattern::~Pattern()
{
    this->masterReference.clear();
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

ProjectTreeItem *Pattern::getProject()
{
    return this->eventDispatcher.getProject();
}

UndoStack *Pattern::getUndoStack()
{
    return this->eventDispatcher.getProject()->getUndoStack();
}

MidiTrack *Pattern::getTrack() const noexcept
{
    return &this->track;
}

float Pattern::getFirstBeat() const noexcept
{
    if (this->clips.size() == 0)
    {
        return FLT_MAX;
    }

    return this->clips.getFirst()->getStartBeat();
}

float Pattern::getLastBeat() const noexcept
{
    if (this->clips.size() == 0)
    {
        return -FLT_MAX;
    }

    return this->clips.getLast()->getStartBeat();
}

void Pattern::sort()
{
    if (this->clips.size() > 0)
    {
        this->clips.sort(*this->clips.getFirst());
    }
}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//===----------------------------------------------------------------------===//

void Pattern::checkpoint()
{
    this->getUndoStack()->beginNewTransaction(String::empty);
}

void Pattern::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void Pattern::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void Pattern::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Clip Actions
//===----------------------------------------------------------------------===//

OwnedArray<Clip> &Pattern::getClips() noexcept
{
    return this->clips;
}

void Pattern::silentImport(const Clip &clip)
{
    if (this->usedClipIds.contains(clip.getId()))
    {
        return;
    }

    const auto storedClip = new Clip(this, clip);
    this->clips.addSorted(*storedClip, storedClip);
    this->usedClipIds.insert(storedClip->getId());
    this->updateBeatRange(false);
}

bool Pattern::insert(Clip clipParams, const bool undoable)
{
    if (this->usedClipIds.contains(clipParams.getId()))
    {
        return false;
    }

    if (undoable)
    {
        this->getUndoStack()->
            perform(new PatternClipInsertAction(*this->getProject(),
                this->getTrackId(), clipParams));
    }
    else
    {
        const auto ownedClip = new Clip(this, clipParams);
        this->clips.addSorted(*ownedClip, ownedClip);
        this->usedClipIds.insert(ownedClip->getId());
        this->notifyClipAdded(*ownedClip);
        this->updateBeatRange(true);
    }

    return true;
}

bool Pattern::remove(Clip clipParams, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new PatternClipRemoveAction(*this->getProject(),
                this->getTrackId(), clipParams));
    }
    else
    {
        const int index = this->clips.indexOfSorted(clipParams, &clipParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            Clip *const removedClip = this->clips[index];
            jassert(removedClip->isValid());
            this->notifyClipRemoved(*removedClip);
            this->usedClipIds.erase(removedClip->getId());
            this->clips.remove(index, true);
            this->updateBeatRange(true);
            return true;
        }

        return false;
    }

    return true;
}

bool Pattern::change(Clip oldParams, Clip newParams, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new PatternClipChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->clips.indexOfSorted(oldParams, &oldParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            const auto changedClip = this->clips[index];
            changedClip->applyChanges(newParams);
            this->clips.remove(index, false);
            this->clips.addSorted(*changedClip, changedClip);
            this->notifyClipChanged(oldParams, *changedClip);
            return true;
        }

        return false;
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Events change listener
//===----------------------------------------------------------------------===//

void Pattern::notifyClipChanged(const Clip &oldClip, const Clip &newClip)
{
    this->eventDispatcher.dispatchChangeClip(oldClip, newClip);
}

void Pattern::notifyClipAdded(const Clip &clip)
{
    this->eventDispatcher.dispatchAddClip(clip);
}

void Pattern::notifyClipRemoved(const Clip &clip)
{
    this->eventDispatcher.dispatchRemoveClip(clip);
}

void Pattern::notifyClipRemovedPostAction()
{
    this->eventDispatcher.dispatchPostRemoveClip(this);
}

void Pattern::updateBeatRange(bool shouldNotifyIfChanged)
{
    if (this->lastStartBeat == this->getFirstBeat() &&
        this->lastEndBeat == this->getLastBeat())
    {
        return;
    }

    this->lastStartBeat = this->getFirstBeat();
    this->lastEndBeat = this->getLastBeat();

    if (shouldNotifyIfChanged)
    {
        this->eventDispatcher.dispatchChangeProjectBeatRange();
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Pattern::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::pattern);

    for (int i = 0; i < this->clips.size(); ++i)
    {
        xml->prependChildElement(this->clips.getUnchecked(i)->serialize());
    }

    return xml;
}

void Pattern::deserialize(const ValueTree &tree)
{
    this->reset();

    const XmlElement *root =
        (tree.getTagName() == Serialization::Core::pattern) ?
        &tree : tree.getChildByName(Serialization::Core::pattern);

    if (root == nullptr)
    {
        return;
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::clip)
    {
        auto clip = new Clip(this);
        clip->deserialize(*e);
        this->clips.add(clip); // sorted later
        this->usedClipIds.insert(clip->getId());
    }

    // Fallback to single clip at zero bar, if no clips found
    if (this->clips.size() == 0)
    {
        this->clips.add(new Clip(this));
    }

    this->sort();
    this->updateBeatRange(false);
}

void Pattern::reset()
{
    this->clips.clear(true);
    this->usedClipIds.clear();
}

String Pattern::createUniqueClipId() const noexcept
{
    uint8 length = 2;
    String eventId = ClipIdGenerator::generateId(length);
    while (this->usedClipIds.contains(eventId))
    {
        length++;
        eventId = ClipIdGenerator::generateId(length);
    }
    return eventId;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String Pattern::getTrackId() const noexcept
{
    return this->track.getTrackId().toString();
}

int Pattern::hashCode() const noexcept
{
    return this->getTrackId().hashCode();
}
