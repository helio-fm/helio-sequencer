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

#include "BuiltInSynthAudioPlugin.h"

struct GrandSample
{
    GrandSample() = default;

    GrandSample(const String &keyName,
        int lowKey, int highKey, int rootKey,
        const void *sourceData, size_t sourceDataSize) :
        sourceData(sourceData),
        sourceDataSize(sourceDataSize),
        name(keyName),
        midiNoteForNormalPitch(rootKey)
    {
        for (int i = lowKey; i <= highKey; ++i)
        { this->midiNotes.setBit(i); }
    }

    ScopedPointer<AudioFormatReader> createReader()
    {
        FlacAudioFormat flac;
        return flac.createReaderFor(new MemoryInputStream(sourceData, sourceDataSize, false), true);
    }

    const void *sourceData;
    size_t sourceDataSize;
    String name;
    BigInteger midiNotes;
    int midiNoteForNormalPitch;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrandSample)
};

class BuiltInSynthPiano : public BuiltInSynthAudioPlugin
{
public:

    explicit BuiltInSynthPiano();

    const String getName() const override;
    void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages) override;
    void reset() override;

protected:

    void initVoices() override;
    void initSampler() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BuiltInSynthPiano)
};
