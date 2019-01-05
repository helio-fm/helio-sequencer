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

#define MIDDLE_C 60

class Note final : public MidiEvent
{
public:

    using Key = int;

    Note() noexcept;

    Note(const Note &other) noexcept = default;
    Note &operator= (const Note &other) = default;

    Note(Note &&other) noexcept = default;
    Note &operator= (Note &&other) noexcept = default;

    Note(WeakReference<MidiSequence> owner, const Note &parametersToCopy) noexcept;
    explicit Note(WeakReference<MidiSequence> owner,
         int keyVal = 0, float beatVal = 0.f,
         float lengthVal = 1.f, float velocityVal = 1.f) noexcept;

    void exportMessages(MidiMessageSequence &outSequence,
        const Clip &clip, double timeOffset, double timeFactor) const override;
    
    Note copyWithNewId(WeakReference<MidiSequence> owner = nullptr) const noexcept;
    Note withKey(Key newKey) const noexcept;
    Note withBeat(float newBeat) const noexcept;
    Note withKeyBeat(Key newKey, float newBeat) const noexcept;
    Note withKeyLength(Key newKey, float newLength) const noexcept;
    Note withDeltaBeat(float deltaPosition) const noexcept;
    Note withDeltaKey(Key deltaKey) const noexcept;
    Note withLength(float newLength) const noexcept;
    Note withDeltaLength(float deltaLength) const noexcept;
    Note withVelocity(float newVelocity) const noexcept;
    Note withParameters(const ValueTree &parameters) const noexcept;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    int getKey() const noexcept;
    float getLength() const noexcept;
    float getVelocity() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const noexcept override;
    void deserialize(const ValueTree &tree) noexcept override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    void applyChanges(const Note &parameters) noexcept;

    static inline int compareElements(const MidiEvent *const first, const MidiEvent *const second) noexcept
    {
        return MidiEvent::compareElements(first, second);
    }

    static inline int compareElements(const Note &first, const Note &second) noexcept
    {
        return Note::compareElements(&first, &second);
    }

    static int compareElements(const Note *const first, const Note *const second) noexcept;

protected:

    Key key;
    float length;
    float velocity;

private:

    JUCE_LEAK_DETECTOR(Note);
};
