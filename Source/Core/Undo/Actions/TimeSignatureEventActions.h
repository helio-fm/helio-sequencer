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

class TimeSignaturesSequence;
class MidiTrackSource;

#include "TimeSignatureEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class TimeSignatureEventInsertAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventInsertAction(MidiTrackSource &source) :
    UndoAction(source) {}

    TimeSignatureEventInsertAction(MidiTrackSource &source,
                                   String trackId,
                                   const TimeSignatureEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class TimeSignatureEventRemoveAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventRemoveAction(MidiTrackSource &source) :
    UndoAction(source) {}

    TimeSignatureEventRemoveAction(MidiTrackSource &source,
                                   String trackId,
                                   const TimeSignatureEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class TimeSignatureEventChangeAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventChangeAction(MidiTrackSource &source) :
    UndoAction(source) {}

    TimeSignatureEventChangeAction(MidiTrackSource &source,
                                   String trackId,
                                   const TimeSignatureEvent &target,
                                   const TimeSignatureEvent &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    
    TimeSignatureEvent eventBefore;
    TimeSignatureEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventChangeAction)

};


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupInsertAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupInsertAction(MidiTrackSource &source) :
    UndoAction(source) {}
    
    TimeSignatureEventsGroupInsertAction(MidiTrackSource &source,
                                         String trackId,
                                         Array<TimeSignatureEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<TimeSignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupRemoveAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupRemoveAction(MidiTrackSource &source) :
    UndoAction(source) {}
    
    TimeSignatureEventsGroupRemoveAction(MidiTrackSource &source,
                                         String trackId,
                                         Array<TimeSignatureEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<TimeSignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupChangeAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupChangeAction(MidiTrackSource &source) :
    UndoAction(source) {}

    TimeSignatureEventsGroupChangeAction(MidiTrackSource &source,
                                         String trackId,
                                         const Array<TimeSignatureEvent> state1,
                                         const Array<TimeSignatureEvent> state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    
    Array<TimeSignatureEvent> eventsBefore;
    Array<TimeSignatureEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupChangeAction)

};

