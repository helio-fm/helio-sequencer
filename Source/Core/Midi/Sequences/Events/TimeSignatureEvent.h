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

// The default is, obviously, common time (4/4)
#define TIME_SIGNATURE_DEFAULT_NUMERATOR 4
#define TIME_SIGNATURE_DEFAULT_DENOMINATOR 4

class TimeSignatureEvent : public MidiEvent
{
public:

    TimeSignatureEvent();
    TimeSignatureEvent(const TimeSignatureEvent &other);
    TimeSignatureEvent(WeakReference<MidiSequence> owner,
        const TimeSignatureEvent &parametersToCopy);

    explicit TimeSignatureEvent(WeakReference<MidiSequence> owner,
        float newBeat = 0.f,
        int newNumerator = TIME_SIGNATURE_DEFAULT_NUMERATOR,
        int newDenominator = TIME_SIGNATURE_DEFAULT_DENOMINATOR);

    static void parseString(const String &data, int &numerator, int &denominator);
    
    Array<MidiMessage> toMidiMessages() const override;
    TimeSignatureEvent copyWithNewId() const;
    TimeSignatureEvent withDeltaBeat(float beatOffset) const;
    TimeSignatureEvent withBeat(float newBeat) const;
    TimeSignatureEvent withNumerator(const int newNumerator) const;
    TimeSignatureEvent withDenominator(const int newDenominator) const;
    TimeSignatureEvent withParameters(const XmlElement &xml) const;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    int getNumerator() const noexcept;
    int getDenominator() const noexcept;
    String toString() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const TimeSignatureEvent &parameters);

protected:

    int numerator;
    int denominator;

private:

    JUCE_LEAK_DETECTOR(TimeSignatureEvent);

};
