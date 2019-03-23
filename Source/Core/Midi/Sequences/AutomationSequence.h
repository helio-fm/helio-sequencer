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

#include "MidiSequence.h"
#include "AutomationEvent.h"

class AutomationSequence final : public MidiSequence
{
public:

    explicit AutomationSequence(MidiTrack &track,
        ProjectEventDispatcher &dispatcher) noexcept;

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

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence, short timeFormat) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationSequence);
};
