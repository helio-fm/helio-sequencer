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
        double timeOffset, double timeFactor, int periodSize) const noexcept override;

    TimeSignatureEvent copyWithNewId() const noexcept;
    TimeSignatureEvent withDeltaBeat(float beatOffset) const noexcept;
    TimeSignatureEvent withBeat(float newBeat) const noexcept;
    TimeSignatureEvent withNumerator(const int newNumerator) const noexcept;
    TimeSignatureEvent withDenominator(const int newDenominator) const noexcept;
    TimeSignatureEvent withParameters(const SerializedData &parameters) const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    int getNumerator() const noexcept;
    int getDenominator() const noexcept;
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

protected:

    int numerator;
    int denominator;

private:

    JUCE_LEAK_DETECTOR(TimeSignatureEvent);

};
