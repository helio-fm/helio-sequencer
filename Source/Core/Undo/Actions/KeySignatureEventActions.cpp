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

SerializedData KeySignatureEventInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::keySignatureEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void KeySignatureEventInsertAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void KeySignatureEventInsertAction::reset()
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

SerializedData KeySignatureEventRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::keySignatureEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void KeySignatureEventRemoveAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void KeySignatureEventRemoveAction::reset()
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

    (void) nextAction;
    return nullptr;
}

SerializedData KeySignatureEventChangeAction::serialize() const
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

void KeySignatureEventChangeAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto keySignatureBeforeChild = data.getChildWithName(Serialization::Undo::keySignatureBefore);
    const auto keySignatureAfterChild = data.getChildWithName(Serialization::Undo::keySignatureAfter);
    
    this->eventBefore.deserialize(keySignatureBeforeChild.getChild(0));
    this->eventAfter.deserialize(keySignatureAfterChild.getChild(0));
}

void KeySignatureEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}
