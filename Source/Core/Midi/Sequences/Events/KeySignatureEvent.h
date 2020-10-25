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
        Note::Key key = 0) noexcept;

    String toString(const StringArray &keyNames) const;

    void exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
        const KeyboardMapping &keyMap, double timeOffset, double timeFactor) const noexcept override;
    
    KeySignatureEvent copyWithNewId() const noexcept;
    KeySignatureEvent withDeltaBeat(float beatOffset) const noexcept;
    KeySignatureEvent withBeat(float newBeat) const noexcept;
    KeySignatureEvent withRootKey(Note::Key key) const noexcept;
    KeySignatureEvent withScale(Scale::Ptr scale) const noexcept;
    KeySignatureEvent withParameters(const SerializedData &parameters) const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Note::Key getRootKey() const noexcept;
    const Scale::Ptr getScale() const noexcept;

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

protected:

    Note::Key rootKey = 0;
    Scale::Ptr scale;

private:

    JUCE_LEAK_DETECTOR(KeySignatureEvent);

};
