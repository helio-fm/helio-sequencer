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

#define KEY_C5 60

class Note : public MidiEvent
{
public:

    typedef int Key;

    Note();
    Note(const Note &other);
    Note(WeakReference<MidiSequence> owner, const Note &parametersToCopy);

    explicit Note(WeakReference<MidiSequence> owner,
         int keyVal = 0, float beatVal = 0.f,
         float lengthVal = 1.f, float velocityVal = 1.f);

    Array<MidiMessage> toMidiMessages() const override;
    
    Note copyWithNewId(WeakReference<MidiSequence> owner = nullptr) const;
    Note withBeat(float newBeat) const;
    Note withKeyBeat(int newKey, float newBeat) const;
    Note withDeltaBeat(float deltaPosition) const;
    Note withDeltaKey(int deltaKey) const;
    Note withLength(float newLength) const;
    Note withDeltaLength(float deltaLength) const;
    Note withVelocity(float newVelocity) const;
    Note withParameters(const XmlElement &xml) const;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    int getKey() const noexcept;
    float getLength() const noexcept;
    float getVelocity() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    void applyChanges(const Note &parameters);

    static int compareElements(const MidiEvent *const first, const MidiEvent *const second);
    static int compareElements(const Note *const first, const Note *const second);
    static int compareElements(const Note &first, const Note &second);

protected:

    Key key;
    float length;
    float velocity;

private:

    JUCE_LEAK_DETECTOR(Note);

};
