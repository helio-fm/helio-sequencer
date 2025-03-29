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
#include "Scale.h"
#include "Note.h"
#include "Temperament.h"

class KeySignatureEvent final : public MidiEvent
{
public:

    KeySignatureEvent() noexcept;
    KeySignatureEvent(const KeySignatureEvent &other) noexcept;
    KeySignatureEvent(WeakReference<MidiSequence> owner,
        const KeySignatureEvent &parametersToCopy) noexcept;

    explicit KeySignatureEvent(WeakReference<MidiSequence> owner,
        Scale::Ptr scale = nullptr,
        float newBeat = 0.f,
        Note::Key key = 0,
        const String &keyName = {}) noexcept;

    String toString(const Temperament::Period &defaultKeyNames) const;

    void exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
        const KeyboardMapping &keyMap, double timeFactor) const noexcept override;
    
    KeySignatureEvent withDeltaBeat(float beatOffset) const noexcept;
    KeySignatureEvent withBeat(float newBeat) const noexcept;
    KeySignatureEvent withRootKey(Note::Key key, const String &keyName) const noexcept;
    KeySignatureEvent withScale(Scale::Ptr scale) const noexcept;
    KeySignatureEvent withParameters(const SerializedData &parameters) const noexcept;

    KeySignatureEvent withNewId() const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Note::Key getRootKey() const noexcept;
    
    // this is optional and can be empty;
    // if empty, indicates the default key name:
    const String &getRootKeyName() const noexcept;

    const String getRootKeyNameOfDefault(const
        Temperament::Period &defaultKeyNames) const noexcept;

    const Scale::Ptr getScale() const noexcept;

    // this returns correct results only for diatonic scales in 12-edo,
    // if positive, indicates the number of sharps,
    // if negative, the number of flats:
    int getNumFlatsSharps() const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const KeySignatureEvent &parameters) noexcept;

private:

    Note::Key rootKey = 0; // # of semitones modulo period
    String rootKeyName; // one of enharmonic equivalents

    Scale::Ptr scale;

    JUCE_LEAK_DETECTOR(KeySignatureEvent)
};
