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

class AutomationSequence;
class MidiTrackSource;

#include "AutomationEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AutomationEventInsertAction : public UndoAction
{
public:
    
    explicit AutomationEventInsertAction(MidiTrackSource &source) :
    UndoAction(source) {}

    AutomationEventInsertAction(MidiTrackSource &source,
                                String trackId,
                                const AutomationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    AutomationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AutomationEventRemoveAction : public UndoAction
{
public:
    
    explicit AutomationEventRemoveAction(MidiTrackSource &source) :
    UndoAction(source) {}

    AutomationEventRemoveAction(MidiTrackSource &source,
                                String trackId,
                                const AutomationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    AutomationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class AutomationEventChangeAction : public UndoAction
{
public:
    
    explicit AutomationEventChangeAction(MidiTrackSource &source) :
    UndoAction(source) {}

    AutomationEventChangeAction(MidiTrackSource &source,
                                String trackId,
                                const AutomationEvent &target,
                                const AutomationEvent &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    AutomationEvent eventBefore;
    AutomationEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventChangeAction)

};


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupInsertAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupInsertAction(MidiTrackSource &source) :
    UndoAction(source) {}
    
    AutomationEventsGroupInsertAction(MidiTrackSource &source,
                                      String trackId,
                                      Array<AutomationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AutomationEvent> events;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupRemoveAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupRemoveAction(MidiTrackSource &source) :
    UndoAction(source) {}
    
    AutomationEventsGroupRemoveAction(MidiTrackSource &source,
                                      String trackId,
                                      Array<AutomationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AutomationEvent> events;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupChangeAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupChangeAction(MidiTrackSource &source) :
    UndoAction(source) {}

    AutomationEventsGroupChangeAction(MidiTrackSource &source,
                                      String trackId,
                                      const Array<AutomationEvent> state1,
                                      const Array<AutomationEvent> state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    Array<AutomationEvent> eventsBefore;
    Array<AutomationEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupChangeAction)

};
