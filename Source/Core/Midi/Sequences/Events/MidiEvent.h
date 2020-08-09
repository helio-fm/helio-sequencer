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

class Clip;
class MidiSequence;

class MidiEvent : public Serializable
{
public:

    using Id = int32;

    // Non-serialized field to be used instead of expensive dynamic casts:
    enum class Type : uint8 
    {
        Note = 1,
        Auto = 2,
        Annotation = 3,
        TimeSignature = 4,
        KeySignature = 5
    };

    inline Type getType() const noexcept { return this->type; }
    inline bool isTypeOf(Type val) const noexcept { return this->type == val; }

    MidiEvent(const MidiEvent &other) noexcept = default;
    MidiEvent &operator= (const MidiEvent &other) = default;

    MidiEvent(MidiEvent &&other) noexcept = default;
    MidiEvent &operator= (MidiEvent &&other) noexcept = default;

    // Used to create new events, generates new id that is unique within a track
    MidiEvent(WeakReference<MidiSequence> owner, Type type, float beat) noexcept;

    // Doesn't create new id, only used in undo/redo actions to insert events
    // with custom parameters (assumes the id is already valid and unique)
    MidiEvent(WeakReference<MidiSequence> owner, const MidiEvent &parameters) noexcept;

    virtual void exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
        double timeOffset, double timeFactor, int periodSize) const noexcept = 0;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isValid() const noexcept;

    MidiSequence *getSequence() const noexcept;

    int getTrackControllerNumber() const noexcept;
    int getTrackChannel() const noexcept;
    Colour getTrackColour() const noexcept;

    const Id getId() const noexcept;
    float getBeat() const noexcept;

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

    static int compareElements(const MidiEvent *const first, const MidiEvent *const second) noexcept;

protected:

    WeakReference<MidiSequence> sequence;

    Id id = 0;
    Type type;
    float beat = 0.f;

    Id createId() const noexcept;
    static String packId(MidiEvent::Id id);
    static Id unpackId(const String &str);

    friend struct MidiEventHash;
    friend class LegacyEventFormatSupportTests;

};

struct MidiEventHash
{
    inline HashCode operator()(const MidiEvent &key) const noexcept
    {
        return static_cast<HashCode>(key.id);
    }
};
