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

#include "Serializable.h"

class MidiSequence;

class MidiEvent : public Serializable
{
public:

    using Id = String;

    // Non-serialized field to be used instead of expensive dynamic casts:
    enum Type { Note = 1, Auto = 2, Annotation = 3, TimeSignature = 4, KeySignature = 5 };
    inline Type getType() const noexcept { return this->type; }
    inline bool isTypeOf(Type val) const noexcept { return this->type == val; }

    MidiEvent(WeakReference<MidiSequence> owner, Type type, float beat);
    ~MidiEvent() override;

    virtual Array<MidiMessage> toMidiMessages() const = 0;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isValid() const noexcept;

    MidiSequence *getSequence() const noexcept;
    Colour getColour() const noexcept;

    int getControllerNumber() const noexcept;
    int getChannel() const noexcept;

    Id getId() const noexcept;
    float getBeat() const noexcept;
    
    inline HashCode hashCode() const noexcept
    {
        const HashCode code =
            static_cast<HashCode>(this->beat)
            + static_cast<HashCode>(this->getId().hashCode());
        return code;
    }

    friend inline bool operator==(const MidiEvent &l, const MidiEvent &r)
    {
        // Events are considered equal when they have the same id,
        // and they are either owned by the same track, or one of them
        // does not belong any track, i.e. is just a set of parameters
        // for undo/redo action or VCS delta.
        return (&l == &r ||
            ((l.sequence == nullptr || r.sequence == nullptr) && l.id == r.id) ||
            (l.sequence != nullptr && l.sequence == r.sequence && l.id == r.id));
    }

    static int compareElements(const MidiEvent *const first, const MidiEvent *const second)
    {
        if (first == second) { return 0; }
        
        const float diff = first->getBeat() - second->getBeat();
        const int diffResult = (diff > 0.f) - (diff < 0.f);
        if (diffResult != 0) { return diffResult; }
        
        return first->getId().compare(second->getId());
    }

protected:

    WeakReference<MidiSequence> sequence;

    Id id;
    Type type;
    float beat;

    Id createId() const noexcept;

};

struct MidiEventHash
{
    inline HashCode operator()(const MidiEvent &key) const noexcept
    {
        return key.hashCode() % HASH_CODE_MAX;
    }
};
