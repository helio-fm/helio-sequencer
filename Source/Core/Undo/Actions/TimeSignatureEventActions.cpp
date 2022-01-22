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
#include "TimeSignatureEventActions.h"
#include "TimeSignaturesSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

TimeSignatureEventInsertAction::TimeSignatureEventInsertAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(event) {}

bool TimeSignatureEventInsertAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool TimeSignatureEventInsertAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int TimeSignatureEventInsertAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent);
}

SerializedData TimeSignatureEventInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::timeSignatureEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void TimeSignatureEventInsertAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void TimeSignatureEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

TimeSignatureEventRemoveAction::TimeSignatureEventRemoveAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(target) {}

bool TimeSignatureEventRemoveAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool TimeSignatureEventRemoveAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int TimeSignatureEventRemoveAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent);
}

SerializedData TimeSignatureEventRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::timeSignatureEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void TimeSignatureEventRemoveAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void TimeSignatureEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

TimeSignatureEventChangeAction::TimeSignatureEventChangeAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &target,
    const TimeSignatureEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    eventBefore(target),
    eventAfter(newParameters) {}

bool TimeSignatureEventChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool TimeSignatureEventChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int TimeSignatureEventChangeAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent) * 2;
}

UndoAction *TimeSignatureEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<TimeSignatureEventChangeAction *>(nextAction))
    {
        const bool idsAreEqual =
            (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                this->trackId == nextChanger->trackId);

        if (idsAreEqual)
        {
            return new TimeSignatureEventChangeAction(this->source,
                this->trackId, this->eventBefore, nextChanger->eventAfter);
        }
    }

    (void) nextAction;
    return nullptr;
}

SerializedData TimeSignatureEventChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::timeSignatureEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData timeSignatureBeforeChild(Serialization::Undo::timeSignatureBefore);
    timeSignatureBeforeChild.appendChild(this->eventBefore.serialize());
    tree.appendChild(timeSignatureBeforeChild);
    
    SerializedData timeSignatureAfterChild(Serialization::Undo::timeSignatureAfter);
    timeSignatureAfterChild.appendChild(this->eventAfter.serialize());
    tree.appendChild(timeSignatureAfterChild);
    
    return tree;
}

void TimeSignatureEventChangeAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto timeSignatureBeforeChild = data.getChildWithName(Serialization::Undo::timeSignatureBefore);
    const auto timeSignatureAfterChild = data.getChildWithName(Serialization::Undo::timeSignatureAfter);
    
    this->eventBefore.deserialize(timeSignatureBeforeChild.getChild(0));
    this->eventAfter.deserialize(timeSignatureAfterChild.getChild(0));
}

void TimeSignatureEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}
