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
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    
    AnnotationEvent eventBefore;
    AnnotationEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventChangeAction)

};

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupInsertAction final : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupInsertAction(MidiTrackSource &source) :
        UndoAction(source) {}
    
    AnnotationEventsGroupInsertAction(MidiTrackSource &source,
        const String &trackId, Array<AnnotationEvent> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AnnotationEvent> annotations;
    
    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupInsertAction)
    
};

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupRemoveAction final : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupRemoveAction(MidiTrackSource &source) :
        UndoAction(source) {}
    
    AnnotationEventsGroupRemoveAction(MidiTrackSource &source,
        const String &trackId, Array<AnnotationEvent> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AnnotationEvent> annotations;
    
    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupRemoveAction)
    
};

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupChangeAction final : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupChangeAction(MidiTrackSource &source) :
        UndoAction(source) {}

    AnnotationEventsGroupChangeAction(MidiTrackSource &source, const String &trackId,
        const Array<AnnotationEvent> state1, const Array<AnnotationEvent> state2) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    
    Array<AnnotationEvent> eventsBefore;
    Array<AnnotationEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupChangeAction)

};

