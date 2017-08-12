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

#include "Serializable.h"
#include "MidiEvent.h"

class ProjectTreeItem;
class ProjectEventDispatcher;
class MidiTrack;
class MidiTrackTreeItem;
class UndoStack;

#define MIDI_IMPORT_SCALE 48

// TODO rename as MidiSequence, PianoSequence, AutomationSequence,
// or as MidiTrack, PianoTrack, AutomationTrack, etc.
class MidiLayer : public Serializable
{
public:

    explicit MidiLayer(MidiTrack &parentTrack,
		ProjectEventDispatcher &eventDispatcher);

    ~MidiLayer() override;

    enum DefaultControllers
    {
        sustainPedalController = 64,
        tempoController = 81,
    };
    
    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    void checkpoint();
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

    virtual float getFirstBeat() const;
    virtual float getLastBeat() const;
	float getLengthInBeats() const;
		    
    //===------------------------------------------------------------------===//
    // OwnedArray wrapper
    //===------------------------------------------------------------------===//

    void sort();

    inline int size() const
    { return this->midiEvents.size(); }

    inline MidiEvent** begin() const noexcept
    { return this->midiEvents.begin(); }
    
    inline MidiEvent** end() const noexcept
    { return this->midiEvents.end(); }
    
    inline MidiEvent *getUnchecked(const int index) const
    { return this->midiEvents.getUnchecked(index); }

    // fixme fix comparators! problematic behaviour here
    inline int indexOfSorted(const MidiEvent *const event) const
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
    void notifyLayerChanged();
    void notifyBeatRangeChanged();
    void updateBeatRange(bool shouldNotifyIfChanged);

	//===------------------------------------------------------------------===//
	// Helpers
	//===------------------------------------------------------------------===//

	friend inline bool operator==(const MidiLayer &lhs, const MidiLayer &rhs)
	{
		return (&lhs == &rhs || lhs.layerId == rhs.layerId);
	}

	static int compareElements(const MidiLayer *first,
		const MidiLayer *second);

	int hashCode() const noexcept;

private:

	MidiTrack &track;
	ProjectEventDispatcher &eventDispatcher;

protected:

	// clearQuick the arrays and don't send any notifications
	virtual void clearQuick() = 0;

    void setLayerId(const String &id);

    float lastEndBeat;
    float lastStartBeat;
    
    ProjectTreeItem *getProject();
    UndoStack *getUndoStack();

    OwnedArray<MidiEvent> midiEvents;
	
private:

    mutable MidiMessageSequence cachedSequence;
    mutable bool cacheIsOutdated;

private:
    
    WeakReference<MidiLayer>::Master masterReference;
    friend class WeakReference<MidiLayer>;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiLayer);
};
