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

class MidiTrackSource;

#include "UndoAction.h"
#include "TimeSignatureEvent.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

class MidiTrackRenameAction final : public UndoAction
{
public:

    explicit MidiTrackRenameAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    MidiTrackRenameAction(MidiTrackSource &source,
        const String &trackId, const String &path) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;
    
private:

    String trackId;
    
    String pathBefore;
    String pathAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackRenameAction)
};

//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

class MidiTrackChangeChannelAction final : public UndoAction
{
public:

    explicit MidiTrackChangeChannelAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    MidiTrackChangeChannelAction(MidiTrackSource &source,
        const String &trackId, const int newChannel) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;

    int channelBefore = 1;
    int channelAfter = 1;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeChannelAction)
};

//===----------------------------------------------------------------------===//
// Change Channel
//===----------------------------------------------------------------------===//

class MidiTrackChangeColourAction final : public UndoAction
{
public:

    explicit MidiTrackChangeColourAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    MidiTrackChangeColourAction(MidiTrackSource &source,
        const String &trackId, const Colour &newColour) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;

    Colour colourBefore;
    Colour colourAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeColourAction)
};

//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

class MidiTrackChangeInstrumentAction final : public UndoAction
{
public:

    explicit MidiTrackChangeInstrumentAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    MidiTrackChangeInstrumentAction(MidiTrackSource &source,
        const String &trackId, const String &instrumentId) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;

    String instrumentIdBefore;
    String instrumentIdAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeInstrumentAction)
};

//===----------------------------------------------------------------------===//
// Change Time Signature
//===----------------------------------------------------------------------===//

class MidiTrackChangeTimeSignatureAction final : public UndoAction
{
public:

    explicit MidiTrackChangeTimeSignatureAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    MidiTrackChangeTimeSignatureAction(MidiTrackSource &source,
        const String &trackId, const TimeSignatureEvent &newParameters) noexcept;

    MidiTrackChangeTimeSignatureAction(MidiTrackSource &source, const String &trackId,
        const TimeSignatureEvent &oldParameters, const TimeSignatureEvent &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    UndoAction *createCoalescedAction(UndoAction *nextAction) override;

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    String trackId;

    TimeSignatureEvent timeSignatureBefore;
    TimeSignatureEvent timeSignatureAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeTimeSignatureAction)
};
