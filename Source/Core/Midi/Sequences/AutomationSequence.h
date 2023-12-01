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

#include "MidiSequence.h"
#include "AutomationEvent.h"

class AutomationSequence final : public MidiSequence
{
public:

    explicit AutomationSequence(MidiTrack &track,
        ProjectEventDispatcher &dispatcher) noexcept;

    float getAverageControllerValue() const;

    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//

    MidiEvent *insert(const AutomationEvent &autoEvent, bool undoable);
    bool remove(const AutomationEvent &autoEvent, bool undoable);
    bool change(const AutomationEvent &autoEvent,
        const AutomationEvent &newAutoEvent,
        bool undoable);
    
    bool insertGroup(Array<AutomationEvent> &events, bool undoable);
    bool removeGroup(Array<AutomationEvent> &events, bool undoable);
    bool changeGroup(Array<AutomationEvent> eventsBefore,
        Array<AutomationEvent> eventsAfter,
        bool undoable);

    int indexOfSorted(const AutomationEvent *const event) const noexcept
    {
        const auto index = this->midiEvents.indexOfSorted(*event, event);
        jassert(this->midiEvents[index] == event); // duplicate events?
        return index;
    }

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence, short timeFormat) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationSequence);
};
