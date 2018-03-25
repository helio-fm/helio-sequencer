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

#include "Note.h"
#include "Scale.h"

// Arpeggiators are created by user simply as a sequences within a scale.
// Depending on arp type, it has a mapping from that sequence's space into target chord space,
// so that the chord transformation becomes as straightforward as
// iterating through arp's sequence and inserting a transformed note for each arp's key.

// This effectively allows to use any melody/sequence as an arpeggiator
// (the only assumption is that it has no non-scale keys within, which will be ignored),
// with no restrictions on a target chord's scale.

// Which opens huge possibilities for experimentation,
// like using an arpeggiated chord as a new arpeggiator, and so on.
// See DiatonicArpMapper implementation for the most common mapping example.

class Arp final : public Serializable
{
public:

    Arp(const String &name, 
        const Scale &scale,
        const Array<Note> &sequence,
        Note::Key rootKey);

    Uuid getId() const noexcept { return this->id; }
    String getName() const noexcept { return this->name; };
    bool isValid() const noexcept { return !this->keys.isEmpty(); }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
    struct Key final
    {
        // Key, relative to the root note, and translated into scale, may be negative
        const int key;
        // Octave number, relative to root key, may be negative
        // We cannot keep this info in a key, as target chord's scale might have different period
        const int period;
        // Velocity and length parameters is the same as for note
        const float velocity;
        const float length;
        // Beat is relative to sequence start (i.e. first one == 0)
        const float beat;
    };

    class Mapper
    {
    public:

        Mapper() = default;
        virtual ~Mapper() = default;

        virtual Note::Key mapArpKeyIntoChordSpace(Arp::Key arpKey, const Array<Note> &chord,
            const Scale &chordScale, Note::Key chordRoot) const = 0;

    protected:

        Note::Key getChordKey(const Array<Note> &chord, int index) const
        {
            const auto safeIndex = index % chord.size();
            return chord.getUnchecked(safeIndex).getKey();
        }

        Note::Key getChordKeyPlus(const Array<Note> &chord, const Scale &chordScale,
            Note::Key chordRoot, int index, int scaleOffset) const
        {
            const int relativeChordKey = this->getChordKey(chord, index) - chordRoot;
            const int nextScaleKey = chordScale.getScaleKey(relativeChordKey) + scaleOffset;
            return chordScale.getChromaticKey(nextScaleKey) + chordRoot;
        }
    };

protected:

    Uuid id;
    String name;
    Identifier type;
    Array<Arp::Key> keys;
    ScopedPointer<Arp::Mapper> mapper;

    JUCE_LEAK_DETECTOR(Arp)
};



class Arpeggiator final : public Serializable
{
public:
    
    Arpeggiator();

    String getId() const;
    String getName() const;
    String exportSequenceAsTrack() const;
    
    bool isReversed() const;
    bool hasRelativeMapping() const;
    bool limitsToChord() const;
    bool isEmpty() const;
    float getScale() const;
    
    Arpeggiator withName(const String &newName) const;
    Arpeggiator withSequence(const Array<Note> &arpSequence) const;
    Arpeggiator withSequenceFromString(const String &data) const;
    Arpeggiator reversed(bool shouldBeReversed) const;
    Arpeggiator mappedRelative(bool shouldBeMappedRelative) const;
    Arpeggiator limitedToChord(bool shouldLimitToChord) const;
    Arpeggiator withScale(float newScale) const;
    
    struct Key
    {
        int octaveShift;
        int keyIndex;
        float velocity;
        // for chord-length mapping
        float absStart;
        float absLength;
        // for arp-length mapping
        float beatStart;
        float beatLength;
        float sequenceLength;
    };
    
    Array<Key> createArpKeys() const;
    
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    Array<Note> sequence;
    bool reversedMode;
    bool relativeMappingMode;
    bool limitToChordMode;
    float scale;
    
    String name;
    Uuid id;
    
    JUCE_LEAK_DETECTOR(Arpeggiator)
    
};
