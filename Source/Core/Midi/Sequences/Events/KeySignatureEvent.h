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
    explicit KeySignatureEvent(MidiSequence *owner,
        float newBeat = 0.f,
        Note::Key key = 0,
        Scale scale = Scale());

    ~KeySignatureEvent() override;

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

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Stuff for hash tables
    //===------------------------------------------------------------------===//

    KeySignatureEvent &operator=(const KeySignatureEvent &right);
    friend inline bool operator==(const KeySignatureEvent &lhs, const KeySignatureEvent &rhs)
    {
        return (lhs.getId() == rhs.getId());
    }

    int hashCode() const noexcept;

    // Refers to the same key and scale, but defines different equality and hash code functions.
    // Only used in piano roll to keep track of pre-rendered highlighting patterns.
    struct HighlightingScheme
    {
        HighlightingScheme();
        HighlightingScheme(const HighlightingScheme &other);
        HighlightingScheme(int rootKey, const Scale &scale);
        HighlightingScheme(int rootKey, const Array<int> &keys);
        friend inline bool operator==(const HighlightingScheme &lhs, const HighlightingScheme &rhs)
        { return (lhs.rootKey == rhs.rootKey && lhs.scale.isEquivalentTo(rhs.scale)); }
        int hashCode() const;
        Scale scale;
        int rootKey;
        JUCE_LEAK_DETECTOR(HighlightingScheme)
    };

    inline const HighlightingScheme &getHighlightingScheme() const noexcept
    { return this->scheme; }

protected:

    Note::Key rootKey;
    Scale scale;

    HighlightingScheme scheme;

private:

    JUCE_LEAK_DETECTOR(KeySignatureEvent);

};

class KeySignatureEventHashFunction
{
public:
    static int generateHash(const KeySignatureEvent &key, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>(key.hashCode())) % static_cast<uint32>(upperLimit));
    }
};

class HighlightingSchemeHashFunction
{
public:
    static int generateHash(const KeySignatureEvent::HighlightingScheme &key, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>(key.hashCode())) % static_cast<uint32>(upperLimit));
    }
};
