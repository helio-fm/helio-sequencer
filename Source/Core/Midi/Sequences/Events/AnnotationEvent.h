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
    
    void exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
        const KeyboardMapping &keyMap, double timeFactor) const noexcept override;

    AnnotationEvent withDeltaBeat(float beatOffset) const noexcept;
    AnnotationEvent withBeat(float newBeat) const noexcept;
    AnnotationEvent withLength(float newLength) const noexcept;
    AnnotationEvent withDescription(const String &newDescription) const noexcept;
    AnnotationEvent withColour(const Colour &newColour) const noexcept;
    AnnotationEvent withParameters(const SerializedData &parameters) const noexcept;

    AnnotationEvent withNewId() const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    String getDescription() const noexcept;
    Colour getColour() const noexcept;
    float getLength() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const AnnotationEvent &parameters) noexcept;

private:

    String description;
    Colour colour;
    float length = 0.f;

    JUCE_LEAK_DETECTOR(AnnotationEvent);
};
