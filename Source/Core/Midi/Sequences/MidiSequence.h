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

#include "MidiEvent.h"

class ProjectTreeItem;
class ProjectEventDispatcher;
class MidiTrack;
class UndoStack;

#define MIDI_IMPORT_SCALE 48

class MidiSequence : public Serializable
{
public:

    explicit MidiSequence(MidiTrack &track,
        ProjectEventDispatcher &eventDispatcher) noexcept;
    
    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    void checkpoint() noexcept;
    void undo();
    void redo();
    void clearUndoHistory();

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    MidiMessageSequence exportMidi() const;
    virtual void importMidi(const MidiMessageSequence &sequence) = 0;
    
    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // This one is for import and checkout procedures.
    // Does not notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.
    virtual void silentImport(const MidiEvent &eventToImport) = 0;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    virtual float getFirstBeat() const noexcept;
    virtual float getLastBeat() const noexcept;
    float getLengthInBeats() const noexcept;
    MidiTrack *getTrack() const noexcept;

    //===------------------------------------------------------------------===//
    // OwnedArray wrapper
    //===------------------------------------------------------------------===//

    void sort();

    inline int size() const noexcept
    { return this->midiEvents.size(); }

    template<typename T>
    inline T **begin() const noexcept
    { return this->midiEvents.begin(); }
    
    template<typename T>
    inline T **end() const noexcept
    { return this->midiEvents.end(); }
    
    inline MidiEvent *getUnchecked(const int index) const noexcept
    { return this->midiEvents.getUnchecked(index); }

    // fixme fix comparators! problematic behaviour here
    inline int indexOfSorted(const MidiEvent *const event) const noexcept
    {
        jassert(this->midiEvents[this->midiEvents.indexOfSorted(*event, event)] == event);
        return this->midiEvents.indexOfSorted(*event, event);
    }

    //===------------------------------------------------------------------===//
    // Events change listener
    //===------------------------------------------------------------------===//

    void notifyEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent);
    void notifyEventAdded(const MidiEvent &event);
    void notifyEventRemoved(const MidiEvent &event);
    void notifyEventRemovedPostAction();

    void invalidateSequenceCache();
    void updateBeatRange(bool shouldNotifyIfChanged);

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    String createUniqueEventId() const noexcept;
    String getTrackId() const noexcept;
    int getChannel() const noexcept;

    friend inline bool operator==(const MidiSequence &lhs, const MidiSequence &rhs)
    {
        return &lhs == &rhs;
    }

private:

    MidiTrack &track;
    ProjectEventDispatcher &eventDispatcher;

protected:

    float lastEndBeat;
    float lastStartBeat;
    
    ProjectTreeItem *getProject() const noexcept;
    UndoStack *getUndoStack() const noexcept;

    OwnedArray<MidiEvent> midiEvents;
    mutable SparseHashSet<MidiEvent::Id, StringHash> usedEventIds;

private:

    mutable MidiMessageSequence cachedSequence;
    mutable bool cacheIsOutdated;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSequence)
    JUCE_DECLARE_WEAK_REFERENCEABLE(MidiSequence)
};
