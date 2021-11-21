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
#include "ProjectNode.h"
#include "UndoStack.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

struct ClipIdGenerator final
{
    static Clip::Id generateId(int length = 2)
    {
        jassert(length <= 4);
        Clip::Id id = 0;
        static Random r;
        r.setSeedRandomly();
        static const char idChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < length; ++i)
        {
            id |= idChars[r.nextInt(62)] << (i * CHAR_BIT);
        }
        return id;
    }
};

Pattern::Pattern(MidiTrack &parentTrack,
    ProjectEventDispatcher &dispatcher) :
    track(parentTrack),
    eventDispatcher(dispatcher) {}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

ProjectNode *Pattern::getProject() const noexcept
{
    return this->eventDispatcher.getProject();
}

UndoStack *Pattern::getUndoStack() const noexcept
{
    return this->eventDispatcher.getProject()->getUndoStack();
}

MidiTrack *Pattern::getTrack() const noexcept
{
    return &this->track;
}

int Pattern::indexOfSorted(const Clip *target) const
{
    static Clip comparator;
    jassert(target->getPattern() == this);
    return this->clips.indexOfSorted(comparator, target);
}

void Pattern::sort()
{
    if (this->clips.size() > 0)
    {
        this->clips.sort(*this->clips.getFirst());
    }
}

bool Pattern::hasSoloClips() const noexcept
{
    for (const auto *clip : this->clips)
    {
        if (clip->isSoloed())
        {
            return true;
        }
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//===----------------------------------------------------------------------===//

UndoActionId Pattern::getLastUndoActionId() const
{
    return this->getUndoStack()->getUndoActionId();
}

void Pattern::checkpoint(UndoActionId id)
{
    this->getUndoStack()->beginNewTransaction(id);
}

void Pattern::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint({});
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

//===----------------------------------------------------------------------===//
// Clip Actions
//===----------------------------------------------------------------------===//

void Pattern::silentImport(const Clip &clip)
{
    if (this->usedClipIds.contains(clip.getId()))
    {
        jassertfalse;
        return;
    }

    auto *storedClip = new Clip(this, clip);
    this->clips.addSorted(*storedClip, storedClip);
    this->usedClipIds.insert(storedClip->getId());
    this->updateBeatRange(false);
}

bool Pattern::insert(const Clip &clipParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipInsertAction(*this->getProject(),
                this->getTrackId(), clipParams));
    }
    else
    {
        auto *ownedClip = new Clip(this, clipParams);
        this->clips.addSorted(*ownedClip, ownedClip);
        this->notifyClipAdded(*ownedClip);
        this->updateBeatRange(true);
    }

    return true;
}

bool Pattern::remove(const Clip &clipParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipRemoveAction(*this->getProject(),
                this->getTrackId(), clipParams));
    }
    else
    {
        const int index = this->clips.indexOfSorted(clipParams, &clipParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            const auto *removedClip = this->clips.getUnchecked(index);
            jassert(removedClip->isValid());
            this->notifyClipRemoved(*removedClip);
            this->clips.remove(index, true);
            this->updateBeatRange(true);
            return true;
        }

        return false;
    }

    return true;
}

bool Pattern::change(const Clip &oldParams, const Clip &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->clips.indexOfSorted(oldParams, &oldParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            auto *changedClip = this->clips.getUnchecked(index);
            changedClip->applyChanges(newParams);
            this->clips.remove(index, false);
            this->clips.addSorted(*changedClip, changedClip);
            this->notifyClipChanged(oldParams, *changedClip);
            this->updateBeatRange(true);
            return true;
        }

        return false;
    }

    return true;
}

bool Pattern::insertGroup(Array<Clip> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipsGroupInsertAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const Clip &eventParams = group.getReference(i);
            auto *ownedClip = new Clip(this, eventParams);
            this->clips.addSorted(*ownedClip, ownedClip);
            this->notifyClipAdded(*ownedClip);
        }

        this->updateBeatRange(true);
    }

    return true;
}

bool Pattern::removeGroup(Array<Clip> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipsGroupRemoveAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const Clip &clip = group.getReference(i);
            const int index = this->clips.indexOfSorted(clip, &clip);
            jassert(index >= 0);
            if (index >= 0)
            {
                auto *removedClip = this->clips.getUnchecked(index);
                this->notifyClipRemoved(*removedClip);
                this->clips.remove(index, true);
            }
        }

        this->updateBeatRange(true);
        this->notifyClipRemovedPostAction();
    }

    return true;
}

bool Pattern::changeGroup(Array<Clip> &groupBefore, Array<Clip> &groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new ClipsGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const Clip &oldParams = groupBefore.getReference(i);
            const Clip &newParams = groupAfter.getReference(i);
            const int index = this->clips.indexOfSorted(oldParams, &oldParams);
            jassert(index >= 0);
            if (index >= 0)
            {
                auto *changedClip = this->clips.getUnchecked(index);
                changedClip->applyChanges(newParams);
                this->clips.remove(index, false);
                this->clips.addSorted(*changedClip, changedClip);
                this->notifyClipChanged(oldParams, *changedClip);
            }
        }

        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Batch actions
//===----------------------------------------------------------------------===//

void Pattern::transposeAll(int keyDelta, bool shouldCheckpoint)
{
    if (this->size() > 0)
    {
        Array<Clip> groupBefore, groupAfter;

        for (int i = 0; i < this->size(); ++i)
        {
            const Clip clip = *this->clips.getUnchecked(i);
            groupBefore.add(clip);
            groupAfter.add(clip.withDeltaKey(keyDelta));
        }

        if (shouldCheckpoint)
        {
            this->checkpoint();
        }

        this->changeGroup(groupBefore, groupAfter, true);
    }
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
        this->eventDispatcher.dispatchChangeTrackBeatRange();
        this->eventDispatcher.dispatchChangeProjectBeatRange();
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Pattern::serialize() const
{
    SerializedData tree(Serialization::Midi::pattern);

    for (int i = 0; i < this->clips.size(); ++i)
    {
        tree.appendChild(this->clips.getUnchecked(i)->serialize());
    }

    return tree;
}

void Pattern::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root =
        data.hasType(Serialization::Midi::pattern) ?
        data : data.getChildWithName(Serialization::Midi::pattern);

    if (!root.isValid())
    {
        return;
    }

    forEachChildWithType(root, e, Serialization::Midi::clip)
    {
        auto clip = new Clip(this);
        clip->deserialize(e);
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

Clip::Id Pattern::createUniqueClipId() const noexcept
{
    int length = 2;
    auto clipId = ClipIdGenerator::generateId(length);
    while (this->usedClipIds.contains(clipId))
    {
        length = jmin(4, length + 1);
        clipId = ClipIdGenerator::generateId(length);
    }
    
    this->usedClipIds.insert(clipId);
    return clipId;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

const String &Pattern::getTrackId() const noexcept
{
    return this->track.getTrackId();
}

int Pattern::hashCode() const noexcept
{
    return this->getTrackId().hashCode();
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class LegacyClipFormatSupportTests final : public UnitTest
{
public:
    LegacyClipFormatSupportTests() : UnitTest("Legacy clip id format support tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        beginTest("Legacy clip id serialization");

        const auto id2 = ClipIdGenerator::generateId(2);
        const auto id3 = ClipIdGenerator::generateId(3);
        const auto id4 = ClipIdGenerator::generateId(4);

        const auto p2 = Clip::packId(id2);
        expectEquals(p2.length(), 2);

        const auto p3 = Clip::packId(id3);
        expectEquals(p3.length(), 3);

        const auto p4 = Clip::packId(id4);
        expectEquals(p4.length(), 4);

        const auto u2 = Clip::unpackId(p2);
        expectEquals(id2, u2);

        const auto u3 = Clip::unpackId(p3);
        expectEquals(id3, u3);

        const auto u4 = Clip::unpackId(p4);
        expectEquals(id4, u4);

        const String s = "xZ0";
        const auto u = Clip::unpackId(s);
        const auto p = Clip::packId(u);
        expectEquals(p, s);
    }
};

static LegacyClipFormatSupportTests legacyClipFormatSupportTests;

#endif
