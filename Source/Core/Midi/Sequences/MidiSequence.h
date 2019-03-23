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
#include "MidiEvent.h"
#include "ProjectEventDispatcher.h"

class ProjectNode;
class MidiTrack;
class UndoStack;

class MidiSequence : public Serializable
{
public:

    explicit MidiSequence(MidiTrack &track,
        ProjectEventDispatcher &eventDispatcher) noexcept;
    
    //===------------------------------------------------------------------===//
    // Undoing
    //===------------------------------------------------------------------===//

    String getLastUndoDescription() const;
    void checkpoint(const String &transactionName = {}) noexcept;
    void undo();
    void redo();
    void clearUndoHistory();

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    static float midiTicksToBeats(double ticks, int timeFormat) noexcept;
    virtual void importMidi(const MidiMessageSequence &sequence, short timeFormat) = 0;
    void exportMidi(MidiMessageSequence &outSequence, const Clip &clip,
        double timeAdjustment, double timeFactor) const;

    //===------------------------------------------------------------------===//
    // Track editing
    //===------------------------------------------------------------------===//

    // Methods for import and checkout.
    // Have different assumptions on event ids.
    // Don't notify anybody to prevent notification hell.
    // Always call notifyLayerChanged() when you're done using it.

    template<typename T>
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

    template<typename T>
    void checkoutEvent(const ValueTree &parameters)
    {
        static T empty;
        ScopedPointer<T> event(new T(this, empty));
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

    inline MidiEvent **begin() const noexcept
    { return this->midiEvents.begin(); }
    
    inline MidiEvent **end() const noexcept
    { return this->midiEvents.end(); }
    
    inline MidiEvent *getUnchecked(const int index) const noexcept
    { return this->midiEvents.getUnchecked(index); }

    inline int indexOfSorted(const MidiEvent *const event) const noexcept
    {
        jassert(this->midiEvents[this->midiEvents.indexOfSorted(*event, event)] == event);
        return this->midiEvents.indexOfSorted(*event, event);
    }

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void updateBeatRange(bool shouldNotifyIfChanged);

    String createUniqueEventId() const noexcept;
    const String &getTrackId() const noexcept;
    int getChannel() const noexcept;

    friend inline bool operator==(const MidiSequence &lhs, const MidiSequence &rhs)
    {
        return &lhs == &rhs;
    }

private:

    MidiTrack &track;

protected:

    float lastEndBeat;
    float lastStartBeat;

    ProjectEventDispatcher &eventDispatcher;
    ProjectNode *getProject() const noexcept;
    UndoStack *getUndoStack() const noexcept;

    OwnedArray<MidiEvent> midiEvents;
    mutable FlatHashSet<MidiEvent::Id, StringHash> usedEventIds;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSequence)
    JUCE_DECLARE_WEAK_REFERENCEABLE(MidiSequence)
};
