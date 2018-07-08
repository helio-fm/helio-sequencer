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

class AnnotationEvent final : public MidiEvent
{
public:

    AnnotationEvent() noexcept;
    AnnotationEvent(const AnnotationEvent &other) noexcept;
    AnnotationEvent(WeakReference<MidiSequence> owner,
        const AnnotationEvent &parametersToCopy) noexcept;
    explicit AnnotationEvent(WeakReference<MidiSequence> owner,
        float newBeat = 0.f,
        const String &description = "",
        const Colour &newColour = Colours::white) noexcept;
    
    void exportMessages(MidiMessageSequence &outSequence,
        const Clip &clip, double timeOffset, double timeFactor) const override;
    
    AnnotationEvent copyWithNewId() const noexcept;
    AnnotationEvent withDeltaBeat(float beatOffset) const noexcept;
    AnnotationEvent withBeat(float newBeat) const noexcept;
    AnnotationEvent withDescription(const String &newDescription) const noexcept;
    AnnotationEvent withColour(const Colour &newColour) const noexcept;
    AnnotationEvent withParameters(const ValueTree &parameters) const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    String getDescription() const noexcept;
    Colour getTrackColour() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const noexcept override;
    void deserialize(const ValueTree &tree) noexcept override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const AnnotationEvent &parameters) noexcept;

protected:

    String description;
    Colour colour;

private:

    JUCE_LEAK_DETECTOR(AnnotationEvent);
};
