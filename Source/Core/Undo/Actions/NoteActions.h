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

class PianoSequence;
class MidiTrackSource;

#include "Note.h"
#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class NoteInsertAction final : public UndoAction
{
public:

    explicit NoteInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    NoteInsertAction(MidiTrackSource &source,
        const String &trackId, const Note &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    String trackId;
    Note note;

    JUCE_DECLARE_NON_COPYABLE(NoteInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class NoteRemoveAction final : public UndoAction
{
public:
    
    explicit NoteRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    NoteRemoveAction(MidiTrackSource &source,
        const String &trackId, const Note &target) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;
    Note note;

    JUCE_DECLARE_NON_COPYABLE(NoteRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class NoteChangeAction final : public UndoAction
{
public:
    
    explicit NoteChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    NoteChangeAction(MidiTrackSource &source, const String &trackId,
        const Note &note, const Note &newParameters) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    Note noteBefore;
    Note noteAfter;

    JUCE_DECLARE_NON_COPYABLE(NoteChangeAction)
};

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class NotesGroupInsertAction final : public UndoAction
{
public:
    
    explicit NotesGroupInsertAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    NotesGroupInsertAction(MidiTrackSource &source,
        const String &trackId, Array<Note> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<Note> notes;
    
    JUCE_DECLARE_NON_COPYABLE(NotesGroupInsertAction)
};

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class NotesGroupRemoveAction final : public UndoAction
{
public:
    
    explicit NotesGroupRemoveAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}
    
    NotesGroupRemoveAction(MidiTrackSource &source,
        const String &trackId, Array<Note> &target) noexcept;
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<Note> notes;
    
    JUCE_DECLARE_NON_COPYABLE(NotesGroupRemoveAction)
};

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class NotesGroupChangeAction final : public UndoAction
{
public:
    
    explicit NotesGroupChangeAction(MidiTrackSource &source) noexcept :
        UndoAction(source) {}

    NotesGroupChangeAction(MidiTrackSource &source, const String &trackId,
        Array<Note> &state1, Array<Note> &state2) noexcept;

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String trackId;

    Array<Note> notesBefore;
    Array<Note> notesAfter;

    JUCE_DECLARE_NON_COPYABLE(NotesGroupChangeAction)
};
