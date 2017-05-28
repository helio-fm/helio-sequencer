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
#include "MidiLayerOwner.h"
#include "MidiEvent.h"

class LayerTreeItem;
class UndoStack;

#define MIDI_IMPORT_SCALE 48

class MidiLayer : public Serializable
{
public:

    explicit MidiLayer(MidiLayerOwner &parent);
    ~MidiLayer() override;

    String getXPath() const;
    MidiLayerOwner *getOwner() const;
    
    void allNotesOff();
    void allSoundOff();
    void allControllersOff();
    
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
	// Instances
	//===------------------------------------------------------------------===//

	class Instance : public Serializable
	{
	public:

		Instance() : startBeat(0.f)
		{
			Uuid uuid;
			id = uuid.toString();
		}

		float getStartBeat() const noexcept
		{ return this->startBeat; }

		String getId() const noexcept
		{ return this->id; }

		void reset()
		{ this->startBeat = 0.f; }

		XmlElement *serialize() const override;
		void deserialize(const XmlElement &xml) override;
		void reset() override;

		static int compareElements(const Instance *const first, const Instance *const second)
		{
			if (first == second) { return 0; }

			const float diff = first->startBeat - second->startBeat;
			const int diffResult = (diff > 0.f) - (diff < 0.f);
			if (diffResult != 0) { return diffResult; }

			return first->id.compare(second->id);
		}

	private:

		float startBeat;
		String id;

	};

	Array<Instance> getInstances() const noexcept;
	bool insertInstance(Instance instance, const bool undoable);
	bool removeInstance(Instance instance, const bool undoable);
	bool changeInstance(Instance instance, Instance newInstance, const bool undoable);

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

    int getChannel() const;
    void setChannel(int val);

    Colour getColour() const;
    void setColour(Colour val);
    
    bool isMuted() const;
    String getMuteStateAsString() const;
    void setMuted(bool shouldBeMuted);
    static bool isMuted(const String &muteState);

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
    // Misc
    //===------------------------------------------------------------------===//

    void sendMidiMessage(const MidiMessage &message);

    String getInstrumentId() const noexcept;
    void setInstrumentId(const String &val);

    int getControllerNumber() const noexcept;
    void setControllerNumber(int val);
    
    bool isTempoLayer() const noexcept;
    bool isSustainPedalLayer() const noexcept;
    bool isOnOffLayer() const noexcept;

    // используется в плеере для связки с инструментами
    Uuid getLayerId() const noexcept;
    String getLayerIdAsString() const;

protected:

    void setLayerId(const String &id);

    float lastEndBeat;
    float lastStartBeat;
    
    ProjectTreeItem *getProject();
    UndoStack *getUndoStack();
    
    Colour colour;
    int channel;
    bool muted;

    String instrumentId;
    int controllerNumber;
    Uuid layerId;

    OwnedArray<MidiEvent> midiEvents;

	Array<Instance> instances;

private:

    mutable MidiMessageSequence cachedSequence;
    mutable bool cacheIsOutdated;

    MidiLayerOwner &owner;

private:
    
    WeakReference<MidiLayer>::Master masterReference;
    friend class WeakReference<MidiLayer>;

};
