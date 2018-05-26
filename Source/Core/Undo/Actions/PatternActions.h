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
#include "Pattern.h"

//===----------------------------------------------------------------------===//
// Insert Clip
//===----------------------------------------------------------------------===//

class ClipInsertAction final : public UndoAction
{
public:

    explicit ClipInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipInsertAction(MidiTrackSource &source,
        const String &trackId, const Clip &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(ClipInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

class ClipRemoveAction final : public UndoAction
{
public:

    explicit ClipRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipRemoveAction(MidiTrackSource &source,
        const String &trackId, const Clip &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(ClipRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

class ClipChangeAction final : public UndoAction
{
public:

    explicit ClipChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipChangeAction(MidiTrackSource &source, const String &trackId,
        const Clip &target, const Clip &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    Clip clipBefore;
    Clip clipAfter;

    JUCE_DECLARE_NON_COPYABLE(ClipChangeAction)
};

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class ClipsGroupInsertAction final : public UndoAction
{
public:

    explicit ClipsGroupInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipsGroupInsertAction(MidiTrackSource &source,
        const String &trackId, Array<Clip> &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Array<Clip> clips;

    JUCE_DECLARE_NON_COPYABLE(ClipsGroupInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class ClipsGroupRemoveAction final : public UndoAction
{
public:

    explicit ClipsGroupRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipsGroupRemoveAction(MidiTrackSource &source,
        const String &trackId, Array<Clip> &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Array<Clip> clips;

    JUCE_DECLARE_NON_COPYABLE(ClipsGroupRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class ClipsGroupChangeAction final : public UndoAction
{
public:

    explicit ClipsGroupChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    ClipsGroupChangeAction(MidiTrackSource &source, const String &trackId,
        Array<Clip> &state1, Array<Clip> &state2) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    Array<Clip> clipsBefore;
    Array<Clip> clipsAfter;

    JUCE_DECLARE_NON_COPYABLE(ClipsGroupChangeAction)
};
