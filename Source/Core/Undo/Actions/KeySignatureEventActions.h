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
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
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
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
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
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;
    
    KeySignatureEvent eventBefore;
    KeySignatureEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(KeySignatureEventChangeAction)
};
