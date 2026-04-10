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
#include "KeySignatureEventActions.h"
#include "KeySignaturesSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

KeySignatureEventInsertAction::KeySignatureEventInsertAction(MidiTrackSource &source,
    const String &trackId, const KeySignatureEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(event) {}

bool KeySignatureEventInsertAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }

    return false;
}

bool KeySignatureEventInsertAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }

    return false;
}

int KeySignatureEventInsertAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent);
}

SerializedData KeySignatureEventInsertAction::serialize() const noexcept
{
    SerializedData tree(Serialization::Undo::keySignatureEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void KeySignatureEventInsertAction::deserialize(const SerializedData &data) noexcept
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void KeySignatureEventInsertAction::reset() noexcept
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

KeySignatureEventRemoveAction::KeySignatureEventRemoveAction(MidiTrackSource &source,
    const String &trackId, const KeySignatureEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(target) {}

bool KeySignatureEventRemoveAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }

    return false;
}

bool KeySignatureEventRemoveAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }

    return false;
}

int KeySignatureEventRemoveAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent);
}

SerializedData KeySignatureEventRemoveAction::serialize() const noexcept
{
    SerializedData tree(Serialization::Undo::keySignatureEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void KeySignatureEventRemoveAction::deserialize(const SerializedData &data) noexcept
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void KeySignatureEventRemoveAction::reset() noexcept
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

KeySignatureEventChangeAction::KeySignatureEventChangeAction(MidiTrackSource &source,
    const String &trackId, const KeySignatureEvent &target,
    const KeySignatureEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    eventBefore(target),
    eventAfter(newParameters) {}

bool KeySignatureEventChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }

    return false;
}

bool KeySignatureEventChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }

    return false;
}

int KeySignatureEventChangeAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent) * 2;
}

UndoAction *KeySignatureEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<KeySignatureEventChangeAction *>(nextAction))
    {
        const bool idsAreEqual =
            (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                this->trackId == nextChanger->trackId);

        if (idsAreEqual)
        {
            return new KeySignatureEventChangeAction(this->source,
                this->trackId, this->eventBefore, nextChanger->eventAfter);
        }
    }

    (void)nextAction;
    return nullptr;
}

SerializedData KeySignatureEventChangeAction::serialize() const noexcept
{
    SerializedData tree(Serialization::Undo::keySignatureEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);

    SerializedData keySignatureBeforeChild(Serialization::Undo::keySignatureBefore);
    keySignatureBeforeChild.appendChild(this->eventBefore.serialize());
    tree.appendChild(keySignatureBeforeChild);

    SerializedData keySignatureAfterChild(Serialization::Undo::keySignatureAfter);
    keySignatureAfterChild.appendChild(this->eventAfter.serialize());
    tree.appendChild(keySignatureAfterChild);

    return tree;
}

void KeySignatureEventChangeAction::deserialize(const SerializedData &data) noexcept
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);

    const auto keySignatureBeforeChild = data.getChildWithName(Serialization::Undo::keySignatureBefore);
    const auto keySignatureAfterChild = data.getChildWithName(Serialization::Undo::keySignatureAfter);

    this->eventBefore.deserialize(keySignatureBeforeChild.getChild(0));
    this->eventAfter.deserialize(keySignatureAfterChild.getChild(0));
}

void KeySignatureEventChangeAction::reset() noexcept
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

KeySignaturesGroupChangeAction::KeySignaturesGroupChangeAction(MidiTrackSource &source,
    const String &trackId, Array<KeySignatureEvent> &state1, Array<KeySignatureEvent> &state2) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->groupBefore.swapWith(state1);
    this->groupAfter.swapWith(state2);
}

bool KeySignaturesGroupChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->groupBefore, this->groupAfter, false);
    }

    return false;
}

bool KeySignaturesGroupChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->groupAfter, this->groupBefore, false);
    }

    return false;
}

int KeySignaturesGroupChangeAction::getSizeInUnits()
{
    return (sizeof(KeySignatureEvent) * this->groupBefore.size()) +
        (sizeof(KeySignatureEvent) * this->groupAfter.size());
}

UndoAction *KeySignaturesGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<KeySignaturesGroupChangeAction *>(nextAction))
    {
        if (nextChanger->trackId != this->trackId)
        {
            return nullptr;
        }

        if (this->groupBefore.size() != nextChanger->groupAfter.size())
        {
            return nullptr;
        }

        for (int i = 0; i < this->groupBefore.size(); ++i)
        {
            if (this->groupBefore.getUnchecked(i).getId() !=
                nextChanger->groupAfter.getUnchecked(i).getId())
            {
                return nullptr;
            }
        }

        return new KeySignaturesGroupChangeAction(this->source,
            this->trackId, this->groupBefore, nextChanger->groupAfter);
    }

    (void)nextAction;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData KeySignaturesGroupChangeAction::serialize() const noexcept
{
    SerializedData tree(Serialization::Undo::keySignaturesGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);

    SerializedData groupBeforeChild(Serialization::Undo::groupBefore);
    SerializedData groupAfterChild(Serialization::Undo::groupAfter);

    for (int i = 0; i < this->groupBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->groupBefore.getUnchecked(i).serialize());
    }

    for (int i = 0; i < this->groupAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->groupAfter.getUnchecked(i).serialize());
    }

    tree.appendChild(groupBeforeChild);
    tree.appendChild(groupAfterChild);

    return tree;
}

void KeySignaturesGroupChangeAction::deserialize(const SerializedData &data) noexcept
{
    this->reset();

    this->trackId = data.getProperty(Serialization::Undo::trackId);

    const auto groupBeforeChild = data.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = data.getChildWithName(Serialization::Undo::groupAfter);

    for (const auto &props : groupBeforeChild)
    {
        KeySignatureEvent ks;
        ks.deserialize(props);
        this->groupBefore.add(ks);
    }

    for (const auto &props : groupAfterChild)
    {
        KeySignatureEvent ks;
        ks.deserialize(props);
        this->groupAfter.add(ks);
    }
}

void KeySignaturesGroupChangeAction::reset() noexcept
{
    this->groupBefore.clear();
    this->groupAfter.clear();
    this->trackId.clear();
}
