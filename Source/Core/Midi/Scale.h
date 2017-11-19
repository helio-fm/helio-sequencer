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

#define CHROMATIC_SCALE_SIZE 12

class Scale final : public Serializable
{
public:

    Scale();
    Scale(const Scale &other);
    explicit Scale(const String &name);

    Scale withName(const String &name) const;
    Scale withKeys(const Array<int> &keys) const;

    inline static StringArray getKeyNames(bool sharps = true)
    {
        return sharps ?
            StringArray("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B") :
            StringArray("C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B");
    }

    // These names only make sense in diatonic scales:
    enum Function {
        Tonic = 0,
        Supertonic = 1,
        Mediant = 2,
        Subdominant = 3,
        Dominant = 4,
        Submediant = 5,
        Subtonic = 6
    };
    
    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    bool isChromatic() const noexcept;
    bool isValid() const noexcept;
    int getSize() const noexcept;
    String getName() const noexcept;
    String getLocalizedName() const;

    // Render target scale chords into chromatic scale (tonic = 0)
    Array<int> getPowerChord(Function fun, bool restrictToOneOctave) const;
    Array<int> getTriad(Function fun, bool restrictToOneOctave) const;
    Array<int> getSeventhChord(Function fun, bool restrictToOneOctave) const;

    Array<int> getUpScale() const;
    Array<int> getDownScale() const;

    // Flat third considered "minor"-ish (like Aeolian, Phrygian, Locrian etc.)
    bool seemsMinor() const;

    // True if specified chromatic key is in the scale
    bool hasKey(int key) const;

    //===------------------------------------------------------------------===//
    // Hard-coded defaults
    //===------------------------------------------------------------------===//

    static Scale getChromaticScale();
    static Scale getNaturalMiniorScale();
    static Scale getNaturalMajorScale();

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Operators
    //===------------------------------------------------------------------===//

    Scale &operator=(const Scale &other);
    friend bool operator==(const Scale &l, const Scale &r);
    friend bool operator!=(const Scale &l, const Scale &r);

    // Used to compare scales in version control
    // (there may be lots of synonyms for the same sets of notes,
    // e.g. Phrygian is called Zokuso in Japan and Ousak in Greece)
    bool isEquivalentTo(const Scale &other) const;

    int hashCode() const noexcept;

private:

    // Key (input and returned) starts from 0
    int getKey(int key, bool shouldRestrictToOneOctave = false) const;

    String name;

    // Simply holds key indices for chromatic scale
    // accessed by index in target scale,
    // e.g. for Ionian: keys[0] = 0, keys[1] = 2, keys[2] = 4, etc
    Array<int> keys;

    JUCE_LEAK_DETECTOR(Scale)
};
