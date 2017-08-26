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
class ProjectTreeItem;

#include "Note.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class NoteInsertAction : public UndoAction
{
public:

    explicit NoteInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    NoteInsertAction(ProjectTreeItem &project,
                     String trackId,
                     const Note &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    String trackId;
    Note note;

    JUCE_DECLARE_NON_COPYABLE(NoteInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class NoteRemoveAction : public UndoAction
{
public:
    
    explicit NoteRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    NoteRemoveAction(ProjectTreeItem &project,
                     String trackId,
                     const Note &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    Note note;

    JUCE_DECLARE_NON_COPYABLE(NoteRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class NoteChangeAction : public UndoAction
{
public:
    
    explicit NoteChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    NoteChangeAction(ProjectTreeItem &project,
                     String trackId,
                     const Note &note,
                     const Note &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
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

class NotesGroupInsertAction : public UndoAction
{
public:
    
    explicit NotesGroupInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    NotesGroupInsertAction(ProjectTreeItem &project,
                           String trackId,
                           Array<Note> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<Note> notes;
    
    JUCE_DECLARE_NON_COPYABLE(NotesGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class NotesGroupRemoveAction : public UndoAction
{
public:
    
    explicit NotesGroupRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    NotesGroupRemoveAction(ProjectTreeItem &project,
                           String trackId,
                           Array<Note> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<Note> notes;
    
    JUCE_DECLARE_NON_COPYABLE(NotesGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class NotesGroupChangeAction : public UndoAction
{
public:
    
    explicit NotesGroupChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    NotesGroupChangeAction(ProjectTreeItem &project,
                           String trackId,
                           Array<Note> &state1,
                           Array<Note> &state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;

    Array<Note> notesBefore;
    Array<Note> notesAfter;

    JUCE_DECLARE_NON_COPYABLE(NotesGroupChangeAction)

};
