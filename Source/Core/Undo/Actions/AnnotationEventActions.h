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

#pragma once

class AnnotationsSequence;
class MidiTrackSource;

#include "AnnotationEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AnnotationEventInsertAction final : public UndoAction
{
public:
    
    explicit AnnotationEventInsertAction(MidiTrackSource &source) :
        UndoAction(source) {}

    AnnotationEventInsertAction(MidiTrackSource &source,
        const String &trackId, const AnnotationEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    AnnotationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AnnotationEventRemoveAction final : public UndoAction
{
public:
    
    explicit AnnotationEventRemoveAction(MidiTrackSource &source) :
        UndoAction(source) {}

    AnnotationEventRemoveAction(MidiTrackSource &source,
        const String &trackId, const AnnotationEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    AnnotationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class AnnotationEventChangeAction final : public UndoAction
{
public:
    
    explicit AnnotationEventChangeAction(MidiTrackSource &source) :
        UndoAction(source) {}

    AnnotationEventChangeAction(MidiTrackSource &source, const String &trackId,
        const AnnotationEvent &target, const AnnotationEvent &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    
    AnnotationEvent eventBefore;
    AnnotationEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventChangeAction)

};
