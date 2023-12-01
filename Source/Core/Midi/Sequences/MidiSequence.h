/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Clip.h"
#include "MidiEvent.h"
#include "ProjectEventDispatcher.h"
#include "UndoActionIDs.h"

class ProjectNode;
class MidiTrack;
class UndoStack;
class KeyboardMapping;

class MidiSequence : public Serializable
{
public:

    explicit MidiSequence(MidiTrack &track,
        ProjectEventDispatcher &eventDispatcher) noexcept;
    
    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    UndoActionId getLastUndoActionId() const;
    void checkpoint(UndoActionId id = UndoActionIDs::None) noexcept;
    void undo();
    void undoCurrentTransactionOnly();
    void redo();
    void clearUndoHistory();

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    static float midiTicksToBeats(double ticks, int timeFormat) noexcept;
    virtual void importMidi(const MidiMessageSequence &sequence, short timeFormat) = 0;
    virtual void exportMidi(MidiMessageSequence &outSequence,
        const Clip &clip, const KeyboardMapping &keyMap,
        bool soloPlaybackMode, bool exportMetronome,
        float projectFirstBeat, float projectLastBeat,
        double timeFactor = 1.0) const;

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // Methods for import and checkout.
    // Have different assumptions on event ids.
    // Don't notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.

    template <typename T>
    void importMidiEvent(const MidiEvent &eventToImport)
    {
        const auto &event = static_cast<const T &>(eventToImport);
        jassert(event.isValid());

        if (!this->usedEventIds.contains(event.getId()))
        {
            jassertfalse;
            return;
        }

        static T comparator;
        this->midiEvents.addSorted(comparator, new T(this, event));
    }

    template <typename T>
    void checkoutEvent(const SerializedData &parameters)
    {
        static T empty;
        UniquePointer<T> event(new T(this, empty));
        event->deserialize(parameters);

        if (this->usedEventIds.contains(event->getId()))
        {
            jassertfalse;
            return;
        }

        static T comparator;
        this->usedEventIds.insert(event->getId());
        this->midiEvents.addSorted(comparator, event.release());
    }

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    // first/last beat relative to sequence, i.e. first beat is typically 0
    float getFirstBeat() const noexcept { return this->sequenceStartBeat; }
    float getLastBeat() const noexcept { return this->sequenceEndBeat; }
    float getLengthInBeats() const noexcept;
    MidiTrack *getTrack() const noexcept;

    //===------------------------------------------------------------------===//
    // OwnedArray wrapper
    //===------------------------------------------------------------------===//

    template <typename T>
    void sort()
    {
        static T comparator;
        this->midiEvents.sort(comparator);
    }

    inline bool isEmpty() const noexcept
    { return this->midiEvents.isEmpty(); }

    inline bool isNotEmpty() const noexcept
    { return !this->midiEvents.isEmpty(); }

    inline int size() const noexcept
    { return this->midiEvents.size(); }

    inline MidiEvent **begin() noexcept
    { return this->midiEvents.begin(); }

    inline MidiEvent *const *begin() const noexcept
    { return this->midiEvents.begin(); }
    
    inline MidiEvent **end() noexcept
    { return this->midiEvents.end(); }

    inline MidiEvent *const *end() const noexcept
    { return this->midiEvents.end(); }

    inline MidiEvent **data() noexcept
    { return this->begin(); }

    inline MidiEvent *const *data() const noexcept
    { return this->begin(); }

    inline MidiEvent *getUnchecked(const int index) const noexcept
    { return this->midiEvents.getUnchecked(index); }

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    virtual void updateBeatRange(bool shouldNotifyIfChanged);

    MidiEvent::Id createUniqueEventId() const noexcept;
    const String &getTrackId() const noexcept;
    int getChannel() const noexcept;

    friend inline bool operator==(const MidiSequence &lhs, const MidiSequence &rhs)
    {
        return &lhs == &rhs;
    }

    JUCE_DECLARE_WEAK_REFERENCEABLE(MidiSequence)

private:

    MidiTrack &track;

    float sequenceEndBeat = 0.f;
    float sequenceStartBeat = 0.f;

protected:

    virtual float findFirstBeat() const noexcept;
    virtual float findLastBeat() const noexcept;

    ProjectEventDispatcher &eventDispatcher;
    ProjectNode *getProject() const noexcept;
    UndoStack *getUndoStack() const noexcept;
    int getPeriodSize() const noexcept;

    OwnedArray<MidiEvent> midiEvents;
    mutable FlatHashSet<MidiEvent::Id> usedEventIds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSequence)
};
