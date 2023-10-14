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
#include "AnnotationEventActions.h"
#include "AnnotationsSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AnnotationEventInsertAction::AnnotationEventInsertAction(MidiTrackSource &source,
    const String &trackId, const AnnotationEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(event) {}

bool AnnotationEventInsertAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AnnotationEventInsertAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int AnnotationEventInsertAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

SerializedData AnnotationEventInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::annotationEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void AnnotationEventInsertAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void AnnotationEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AnnotationEventRemoveAction::AnnotationEventRemoveAction(MidiTrackSource &source,
    const String &trackId, const AnnotationEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(target) {}

bool AnnotationEventRemoveAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool AnnotationEventRemoveAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int AnnotationEventRemoveAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

SerializedData AnnotationEventRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::annotationEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void AnnotationEventRemoveAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void AnnotationEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AnnotationEventChangeAction::AnnotationEventChangeAction(MidiTrackSource &source,
    const String &trackId, const AnnotationEvent &target,
    const AnnotationEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    eventBefore(target),
    eventAfter(newParameters) {}

bool AnnotationEventChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AnnotationEventChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AnnotationEventChangeAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent) * 2;
}

UndoAction *AnnotationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<AnnotationEventChangeAction *>(nextAction))
    {
        const bool idsAreEqual =
            (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                this->trackId == nextChanger->trackId);

        if (idsAreEqual)
        {
            return new AnnotationEventChangeAction(this->source,
                this->trackId, this->eventBefore, nextChanger->eventAfter);
        }
    }

    (void) nextAction;
    return nullptr;
}

SerializedData AnnotationEventChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::annotationEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData annotationBeforeChild(Serialization::Undo::annotationBefore);
    annotationBeforeChild.appendChild(this->eventBefore.serialize());
    tree.appendChild(annotationBeforeChild);
    
    SerializedData annotationAfterChild(Serialization::Undo::annotationAfter);
    annotationAfterChild.appendChild(this->eventAfter.serialize());
    tree.appendChild(annotationAfterChild);
    
    return tree;
}

void AnnotationEventChangeAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto annotationBeforeChild = data.getChildWithName(Serialization::Undo::annotationBefore);
    const auto annotationAfterChild = data.getChildWithName(Serialization::Undo::annotationAfter);
    
    this->eventBefore.deserialize(annotationBeforeChild.getChild(0));
    this->eventAfter.deserialize(annotationAfterChild.getChild(0));
}

void AnnotationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}
