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
#include "PatternActions.h"
#include "MidiTrackSource.h"
#include "MidiSequence.h"
#include "TreeNode.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert Clip
//===----------------------------------------------------------------------===//

ClipInsertAction::ClipInsertAction(MidiTrackSource &source,
    const String &trackId, const Clip &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    clip(target) {}

bool ClipInsertAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insert(this->clip, false);
    }

    return false;
}

bool ClipInsertAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->remove(this->clip, false);
    }

    return false;
}

int ClipInsertAction::getSizeInUnits()
{
    return sizeof(Clip);
}

ValueTree ClipInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->clip.serialize(), nullptr);
    return tree;
}

void ClipInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->clip.deserialize(tree.getChild(0));
}

void ClipInsertAction::reset()
{
    this->clip.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

ClipRemoveAction::ClipRemoveAction(MidiTrackSource &source,
    const String &trackId, const Clip &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    clip(target) {}

bool ClipRemoveAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->remove(this->clip, false);
    }

    return false;
}

bool ClipRemoveAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insert(this->clip, false);
    }

    return false;
}

int ClipRemoveAction::getSizeInUnits()
{
    return sizeof(Clip);
}

ValueTree ClipRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->clip.serialize(), nullptr);
    return tree;
}

void ClipRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->clip.deserialize(tree.getChild(0));
}

void ClipRemoveAction::reset()
{
    this->clip.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

ClipChangeAction::ClipChangeAction(MidiTrackSource &source,
    const String &trackId, const Clip &target, const Clip &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    clipBefore(target),
    clipAfter(newParameters)
{
    jassert(target.getId() == newParameters.getId());
}

bool ClipChangeAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->change(this->clipBefore, this->clipAfter, false);
    }

    return false;
}

bool ClipChangeAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->change(this->clipAfter, this->clipBefore, false);
    }

    return false;
}

int ClipChangeAction::getSizeInUnits()
{
    return sizeof(Clip) * 2;
}

UndoAction *ClipChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        if (ClipChangeAction *nextChanger =
            dynamic_cast<ClipChangeAction *>(nextAction))
        {
            const bool idsAreEqual =
                (this->clipBefore.getId() == nextChanger->clipAfter.getId() &&
                this->trackId == nextChanger->trackId);

            if (idsAreEqual)
            {
                return new ClipChangeAction(this->source,
                    this->trackId, this->clipBefore, nextChanger->clipAfter);
            }
        }
    }

    (void)nextAction;
    return nullptr;
}

ValueTree ClipChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);

    ValueTree instanceBeforeChild(Serialization::Undo::instanceBefore);
    instanceBeforeChild.appendChild(this->clipBefore.serialize(), nullptr);
    tree.appendChild(instanceBeforeChild, nullptr);

    ValueTree instanceAfterChild(Serialization::Undo::instanceAfter);
    instanceAfterChild.appendChild(this->clipAfter.serialize(), nullptr);
    tree.appendChild(instanceAfterChild, nullptr);

    return tree;
}

void ClipChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);

    auto instanceBeforeChild = tree.getChildWithName(Serialization::Undo::instanceBefore);
    auto instanceAfterChild = tree.getChildWithName(Serialization::Undo::instanceAfter);

    this->clipBefore.deserialize(instanceBeforeChild.getChild(0));
    this->clipAfter.deserialize(instanceAfterChild.getChild(0));
}

void ClipChangeAction::reset()
{
    this->clipBefore.reset();
    this->clipAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

ClipsGroupInsertAction::ClipsGroupInsertAction(MidiTrackSource &source,
    const String &trackId, Array<Clip> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->clips.swapWith(target);
}

bool ClipsGroupInsertAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insertGroup(this->clips, false);
    }

    return false;
}

bool ClipsGroupInsertAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->removeGroup(this->clips, false);
    }

    return false;
}

int ClipsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(Clip) * this->clips.size());
}

ValueTree ClipsGroupInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipsGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);

    for (int i = 0; i < this->clips.size(); ++i)
    {
        tree.appendChild(this->clips.getUnchecked(i).serialize(), nullptr);
    }

    return tree;
}

void ClipsGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);

    for (const auto &props : tree)
    {
        Clip n;
        n.deserialize(props);
        this->clips.add(n);
    }
}

void ClipsGroupInsertAction::reset()
{
    this->clips.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

ClipsGroupRemoveAction::ClipsGroupRemoveAction(MidiTrackSource &source,
    const String &trackId, Array<Clip> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->clips.swapWith(target);
}

bool ClipsGroupRemoveAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->removeGroup(this->clips, false);
    }

    return false;
}

bool ClipsGroupRemoveAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insertGroup(this->clips, false);
    }

    return false;
}

int ClipsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(Clip) * this->clips.size());
}

ValueTree ClipsGroupRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipsGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);

    for (int i = 0; i < this->clips.size(); ++i)
    {
        tree.appendChild(this->clips.getUnchecked(i).serialize(), nullptr);
    }

    return tree;
}

void ClipsGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);

    for (const auto &props : tree)
    {
        Clip n;
        n.deserialize(props);
        this->clips.add(n);
    }
}

void ClipsGroupRemoveAction::reset()
{
    this->clips.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

ClipsGroupChangeAction::ClipsGroupChangeAction(MidiTrackSource &source,
    const String &trackId, Array<Clip> &state1, Array<Clip> &state2) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->clipsBefore.swapWith(state1);
    this->clipsAfter.swapWith(state2);
}

bool ClipsGroupChangeAction::perform()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->changeGroup(this->clipsBefore, this->clipsAfter, false);
    }

    return false;
}

bool ClipsGroupChangeAction::undo()
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->changeGroup(this->clipsAfter, this->clipsBefore, false);
    }

    return false;
}

int ClipsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(Clip) * this->clipsBefore.size()) +
        (sizeof(Clip) * this->clipsAfter.size());
}

UndoAction *ClipsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (Pattern *pattern = this->source.findPatternByTrackId(this->trackId))
    {
        if (auto nextChanger = dynamic_cast<ClipsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }

            if (this->clipsBefore.size() != nextChanger->clipsAfter.size())
            {
                return nullptr;
            }

            for (int i = 0; i < this->clipsBefore.size(); ++i)
            {
                if (this->clipsBefore.getUnchecked(i).getId() !=
                    nextChanger->clipsAfter.getUnchecked(i).getId())
                {
                    return nullptr;
                }
            }

            return new ClipsGroupChangeAction(this->source,
                this->trackId, this->clipsBefore, nextChanger->clipsAfter);
        }
    }

    (void)nextAction;
    return nullptr;
}

ValueTree ClipsGroupChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::clipsGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);

    ValueTree groupBeforeChild(Serialization::Undo::groupBefore);
    ValueTree groupAfterChild(Serialization::Undo::groupAfter);

    for (int i = 0; i < this->clipsBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->clipsBefore.getUnchecked(i).serialize(), nullptr);
    }

    for (int i = 0; i < this->clipsAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->clipsAfter.getUnchecked(i).serialize(), nullptr);
    }

    tree.appendChild(groupBeforeChild, nullptr);
    tree.appendChild(groupAfterChild, nullptr);

    return tree;
}

void ClipsGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();

    this->trackId = tree.getProperty(Serialization::Undo::trackId);

    const auto groupBeforeChild = tree.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = tree.getChildWithName(Serialization::Undo::groupAfter);

    for (const auto &props : groupBeforeChild)
    {
        Clip n;
        n.deserialize(props);
        this->clipsBefore.add(n);
    }

    for (const auto &props : groupAfterChild)
    {
        Clip n;
        n.deserialize(props);
        this->clipsAfter.add(n);
    }
}

void ClipsGroupChangeAction::reset()
{
    this->clipsBefore.clear();
    this->clipsAfter.clear();
    this->trackId.clear();
}
