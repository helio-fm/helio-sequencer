/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

    This SoundFont implementation is based on SFZero,
    written by Steve Folta and extended by Leo Olivers and Cognitone,
    distributed under MIT license, see README.md for details.
*/

#pragma once

#include "SoundFontRegion.h"

class SoundFontSample;

class SoundFontSound : public SynthesiserSound
{
public:

    explicit SoundFontSound(const File &file);
    ~SoundFontSound() override;

    using Ptr = ReferenceCountedObjectPtr<SoundFontSound>;

    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;

    virtual void loadRegions();
    virtual void loadSamples(AudioFormatManager &formatManager);

    SoundFontRegion *getRegionFor(int note, int velocity,
        SoundFontRegion::Trigger trigger = SoundFontRegion::Trigger::attack) const;

    int getNumRegions() const;
    SoundFontRegion *regionAt(int index);

    const StringArray &getErrors() const { return this->errors; }
    const StringArray &getWarnings() const { return this->warnings; }

    virtual int getNumPresets() const;
    virtual String getPresetName(int whichSubsound) const;
    virtual void setSelectedPreset(int whichSubsound);
    virtual int getSelectedPreset() const;

    void addError(const String &message);
    void addUnsupportedOpcode(const String &opcode);

    struct Preset final
    {
        const String name;
        const int bank = 0;
        const int preset = 0;

        OwnedArray<SoundFontRegion> regions;

        Preset() = default;
        Preset(String nameIn, int bankIn, int presetIn) :
            name(nameIn), bank(bankIn), preset(presetIn) {}

        void addRegion(UniquePointer<SoundFontRegion> &&region)
        {
            this->regions.add(move(region));
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Preset)
    };

protected:

    File file;

    Array<SoundFontRegion *> regions;

private:

    friend class SoundFontReader;
    void addRegion(UniquePointer<SoundFontRegion> &&region);
    WeakReference<SoundFontSample> addSample(String path, String defaultPath = String());

    UniquePointer<Preset> preset; // a single virtual "preset" to own the regions

    FlatHashMap<String, UniquePointer<SoundFontSample>> samples;

    StringArray errors;
    StringArray warnings;
    FlatHashMap<String, String> unsupportedOpcodes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontSound)
};
