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

// Time signatures are a bit different from other MidiEvents
// in a way that not only they can belong to a Sequence at the timeline,
// but they also can belong to a MidiTrack to override the timeline's
// time signatures when this track is selected.

class TimeSignatureEvent final : public MidiEvent
{
public:

    TimeSignatureEvent() noexcept;
    TimeSignatureEvent(const TimeSignatureEvent &other) noexcept;
    TimeSignatureEvent(WeakReference<MidiSequence> owner,
        const TimeSignatureEvent &parametersToCopy) noexcept;
    explicit TimeSignatureEvent(WeakReference<MidiSequence> owner,
        float newBeat = 0.f,
        int newNumerator = Globals::Defaults::timeSignatureNumerator,
        int newDenominator = Globals::Defaults::timeSignatureDenominator) noexcept;

    static void parseString(const String &data, int &numerator, int &denominator);
    
    void exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
        const KeyboardMapping &keyMap, double timeOffset, double timeFactor) const noexcept override;

    TimeSignatureEvent withDeltaBeat(float beatOffset) const noexcept;
    TimeSignatureEvent withBeat(float newBeat) const noexcept;
    TimeSignatureEvent withNumerator(const int newNumerator) const noexcept;
    TimeSignatureEvent withDenominator(const int newDenominator) const noexcept;
    TimeSignatureEvent withParameters(const SerializedData &parameters) const noexcept;

    TimeSignatureEvent withNewId() const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isValid() const noexcept;

    int getNumerator() const noexcept;
    int getDenominator() const noexcept;

    float getBarLengthInBeats() const noexcept;
    Colour getColour() const;

    String toString() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const TimeSignatureEvent &parameters) noexcept;

    static inline int compareElements(const TimeSignatureEvent &first, const TimeSignatureEvent &second) noexcept
    {
        return TimeSignatureEvent::compareElements(&first, &second);
    }

    static int compareElements(const TimeSignatureEvent *const first, const TimeSignatureEvent *const second) noexcept;

    HashCode getHash() const;

protected:

    WeakReference<MidiTrack> track = nullptr;

    int numerator = 0;
    int denominator = 0;

private:

    JUCE_LEAK_DETECTOR(TimeSignatureEvent);

};

struct TimeSignatureEventHash
{
    inline HashCode operator()(const TimeSignatureEvent &key) const noexcept
    {
        return key.getHash();
    }
};
