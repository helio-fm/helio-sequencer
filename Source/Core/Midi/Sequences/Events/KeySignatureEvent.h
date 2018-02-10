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
#include "Scale.h"
#include "Note.h"

class KeySignatureEvent : public MidiEvent
{
public:

    KeySignatureEvent();
    KeySignatureEvent(const KeySignatureEvent &other);
    KeySignatureEvent(WeakReference<MidiSequence> owner,
        const KeySignatureEvent &parametersToCopy);

    explicit KeySignatureEvent(WeakReference<MidiSequence> owner,
        float newBeat = 0.f,
        Note::Key key = 0,
        Scale scale = Scale());

    String toString() const;
    Array<MidiMessage> toMidiMessages() const override;
    
    KeySignatureEvent copyWithNewId() const;
    KeySignatureEvent withDeltaBeat(float beatOffset) const;
    KeySignatureEvent withBeat(float newBeat) const;
    KeySignatureEvent withRootKey(Note::Key key) const;
    KeySignatureEvent withScale(Scale scale) const;
    KeySignatureEvent withParameters(const XmlElement &xml) const;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Note::Key getRootKey() const noexcept;
    const Scale &getScale() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    void applyChanges(const KeySignatureEvent &parameters);

protected:

    Note::Key rootKey;
    Scale scale;

private:

    JUCE_LEAK_DETECTOR(KeySignatureEvent);

};
