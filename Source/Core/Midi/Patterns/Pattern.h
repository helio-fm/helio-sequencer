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

class ProjectEventDispatcher;
class ProjectNode;
class UndoStack;
class MidiTrack;

// A sorted array of clips
class Pattern final : public Serializable
{
public:

    explicit Pattern(MidiTrack &track,
        ProjectEventDispatcher &eventDispatcher);
    
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

    String getLastUndoDescription() const;
    void checkpoint(const String &transactionName = {});
    void undo();
    void redo();

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // This one is for import and checkout procedures.
    // Does not notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.
    void silentImport(const Clip &clipToImport);

    bool insert(Clip clip, bool undoable);
    bool remove(Clip clip, bool undoable);
    bool change(Clip clip, Clip newClip, bool undoable);

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
    
    inline Clip *getUnchecked(const int index) const noexcept
    { return this->clips.getUnchecked(index); }
    
    inline const OwnedArray<Clip> &getClips() const noexcept
    { return this->clips; }

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

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    String createUniqueClipId() const noexcept;
    const String &getTrackId() const noexcept;

    friend inline bool operator==(const Pattern &lhs, const Pattern &rhs)
    {
        return (&lhs == &rhs);
    }

    int hashCode() const noexcept;

protected:

    void clearQuick();

    float lastEndBeat;
    float lastStartBeat;

    ProjectNode *getProject() const noexcept;
    UndoStack *getUndoStack() const noexcept;

    OwnedArray<Clip> clips;
    mutable FlatHashSet<Clip::Id, StringHash> usedClipIds;

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
