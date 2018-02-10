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
#include "Serializable.h"

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
    Arpeggiator withSequenceFromXml(const XmlElement &xml) const;
    Arpeggiator withSequence(const Array<Note> &arpSequence) const;
    Arpeggiator reversed(bool shouldBeReversed) const;
    Arpeggiator mappedRelative(bool shouldBeMappedRelative) const;
    Arpeggiator limitedToChord(bool shouldLimitToChord) const;
    Arpeggiator withScale(float newScale) const;
    
    struct Key
    {
        int octaveShift;
        int keyIndex;
        float absStart;
        float absLength;
        float beatStart;
        float beatLength;
        float velocity;
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
