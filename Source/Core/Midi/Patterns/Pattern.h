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

#include "Clip.h"
#include "UndoActionIDs.h"

class ProjectEventDispatcher;
class ProjectNode;
class UndoStack;
class MidiTrack;

class Pattern final : public Serializable
{
public:

    Pattern(MidiTrack &track, ProjectEventDispatcher &eventDispatcher);
    
    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    float getFirstBeat() const noexcept;
    float getLastBeat() const noexcept;
    MidiTrack *getTrack() const noexcept;
    int indexOfSorted(const Clip *target) const;

    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    UndoActionId getLastUndoActionId() const;
    void checkpoint(UndoActionId id = 0);
    void undo();
    void redo();

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // This one is for import and checkout procedures.
    // Does not notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.
    void silentImport(const Clip &clipToImport);

    bool insert(const Clip &clip, bool undoable);
    bool remove(const Clip &clip, bool undoable);
    bool change(const Clip &clip, const Clip &newClip, bool undoable);

    bool insertGroup(Array<Clip> &clips, bool undoable);
    bool removeGroup(Array<Clip> &clips, bool undoable);
    bool changeGroup(Array<Clip> &clipsBefore, Array<Clip> &clipsAfter, bool undoable);

    // Batch actions:
    void transposeAll(int keyDelta, bool checkpoint);

    //===------------------------------------------------------------------===//
    // Array wrapper
    //===------------------------------------------------------------------===//

    void sort();

    inline int size() const noexcept
    { return this->clips.size(); }
    
    inline Clip *getUnchecked(int index) const noexcept
    { return this->clips.getUnchecked(index); }
    
    inline const OwnedArray<Clip> &getClips() const noexcept
    { return this->clips; }

    bool hasSoloClips() const noexcept;

    //===------------------------------------------------------------------===//
    // Events change listener
    //===------------------------------------------------------------------===//

    void notifyClipChanged(const Clip &oldClip, const Clip &newClip);
    void notifyClipAdded(const Clip &clip);
    void notifyClipRemoved(const Clip &clip);
    void notifyClipRemovedPostAction();
    void updateBeatRange(bool shouldNotifyIfChanged);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    Clip::Id createUniqueClipId() const noexcept;
    const String &getTrackId() const noexcept;

    friend inline bool operator==(const Pattern &lhs, const Pattern &rhs)
    {
        return (&lhs == &rhs);
    }

    int hashCode() const noexcept;

protected:

    // keeps track of beat range to avoid
    // sending unnecessary "beat range changed" events:
    float lastEndBeat = 0.f;
    float lastStartBeat = 0.f;

    ProjectNode *getProject() const noexcept;
    UndoStack *getUndoStack() const noexcept;

    OwnedArray<Clip> clips;
    mutable FlatHashSet<Clip::Id> usedClipIds;

private:
    
    MidiTrack &track;
    ProjectEventDispatcher &eventDispatcher;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pattern)
    JUCE_DECLARE_WEAK_REFERENCEABLE(Pattern)
};

class PatternHashFunction final
{
public:

    static int generateHash(const Pattern *pattern, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>(pattern->hashCode())) % static_cast<uint32>(upperLimit));
    }
};
