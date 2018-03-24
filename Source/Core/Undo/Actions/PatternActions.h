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

class PatternClipInsertAction final : public UndoAction
{
public:

    explicit PatternClipInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    PatternClipInsertAction(MidiTrackSource &source,
        String trackId, const Clip &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(PatternClipInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

class PatternClipRemoveAction final : public UndoAction
{
public:

    explicit PatternClipRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    PatternClipRemoveAction(MidiTrackSource &source,
        String trackId, const Clip &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Clip clip;

    JUCE_DECLARE_NON_COPYABLE(PatternClipRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

class PatternClipChangeAction final : public UndoAction
{
public:

    explicit PatternClipChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    PatternClipChangeAction(MidiTrackSource &source, String trackId,
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

    JUCE_DECLARE_NON_COPYABLE(PatternClipChangeAction)
};
