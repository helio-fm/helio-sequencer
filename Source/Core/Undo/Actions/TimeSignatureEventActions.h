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

class TimeSignaturesSequence;
class MidiTrackSource;

#include "TimeSignatureEvent.h"
#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class TimeSignatureEventInsertAction final : public UndoAction
{
public:
    
    explicit TimeSignatureEventInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    TimeSignatureEventInsertAction(MidiTrackSource &source,
        const String &trackId, const TimeSignatureEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class TimeSignatureEventRemoveAction final : public UndoAction
{
public:
    
    explicit TimeSignatureEventRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    TimeSignatureEventRemoveAction(MidiTrackSource &source,
        const String &trackId, const TimeSignatureEvent &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class TimeSignatureEventChangeAction final : public UndoAction
{
public:
    
    explicit TimeSignatureEventChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    TimeSignatureEventChangeAction(MidiTrackSource &source, const String &trackId,
        const TimeSignatureEvent &target, const TimeSignatureEvent &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    
    TimeSignatureEvent eventBefore;
    TimeSignatureEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventChangeAction)
};
