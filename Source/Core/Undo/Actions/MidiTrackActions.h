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

class MidiTrackSource;

#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

class MidiTrackRenameAction final : public UndoAction
{
public:

    explicit MidiTrackRenameAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    MidiTrackRenameAction(MidiTrackSource &source,
        const String &trackId, const String &xPath) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    String trackId;
    
    String xPathBefore;
    String xPathAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackRenameAction)
};

//===----------------------------------------------------------------------===//
// Change Colour
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

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    String instrumentIdBefore;
    String instrumentIdAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeInstrumentAction)
};

//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

class MidiTrackMuteAction final : public UndoAction
{
public:

    explicit MidiTrackMuteAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    MidiTrackMuteAction(MidiTrackSource &source,
        const String &trackId, bool shouldBeMuted) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    bool muteStateBefore;
    bool muteStateAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackMuteAction)
};
