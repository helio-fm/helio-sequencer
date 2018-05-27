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

class KeySignaturesSequence;
class MidiTrackSource;

#include "KeySignatureEvent.h"
#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class KeySignatureEventInsertAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    KeySignatureEventInsertAction(MidiTrackSource &source,
        const String &trackId, const KeySignatureEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    KeySignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class KeySignatureEventRemoveAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    KeySignatureEventRemoveAction(MidiTrackSource &source,
        const String &trackId, const KeySignatureEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    KeySignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class KeySignatureEventChangeAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    KeySignatureEventChangeAction(MidiTrackSource &source, const String &trackId,
        const KeySignatureEvent &target, const KeySignatureEvent &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    
    KeySignatureEvent eventBefore;
    KeySignatureEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventChangeAction)
};

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class KeySignatureEventsGroupInsertAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventsGroupInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    KeySignatureEventsGroupInsertAction(MidiTrackSource &source,
        const String &trackId, Array<KeySignatureEvent> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<KeySignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventsGroupInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class KeySignatureEventsGroupRemoveAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventsGroupRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    KeySignatureEventsGroupRemoveAction(MidiTrackSource &source,
        const String &trackId, Array<KeySignatureEvent> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<KeySignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventsGroupRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class KeySignatureEventsGroupChangeAction final : public UndoAction
{
public:
    
    explicit KeySignatureEventsGroupChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    KeySignatureEventsGroupChangeAction(MidiTrackSource &source, const String &trackId,
        const Array<KeySignatureEvent> state1, const Array<KeySignatureEvent> state2) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    
    Array<KeySignatureEvent> eventsBefore;
    Array<KeySignatureEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventsGroupChangeAction)
};

