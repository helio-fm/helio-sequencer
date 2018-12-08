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

#include "Common.h"
#include "BuiltInSynthPiano.h"
#include "BinaryData.h"

#define ATTACK_TIME (0.0)
#define RELEASE_TIME (0.5)
#define MAX_PLAY_TIME (4.5)

struct PianoSample final
{
    PianoSample() = default;
    PianoSample(const PianoSample &other) :
        sourceData(other.sourceData),
        sourceDataSize(other.sourceDataSize),
        midiNoteForNormalPitch(other.midiNoteForNormalPitch),
        midiNotes(other.midiNotes) {}

    PianoSample(int lowKey, int highKey, int rootKey,
        const char *sourceData, int sourceDataSize) :
        sourceData(sourceData),
        sourceDataSize(sourceDataSize),
        midiNoteForNormalPitch(rootKey)
    {
        for (int i = lowKey; i <= highKey; ++i)
        {
            this->midiNotes.setBit(i);
        }
    }

    ScopedPointer<AudioFormatReader> createReader()
    {
        static FlacAudioFormat flac;
        return flac.createReaderFor(new MemoryInputStream(sourceData, sourceDataSize, false), true);
    }

    const char *sourceData;
    int sourceDataSize;
    String name;
    BigInteger midiNotes;
    int midiNoteForNormalPitch;

    JUCE_LEAK_DETECTOR(PianoSample)
};

BuiltInSynthPiano::BuiltInSynthPiano()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

const String BuiltInSynthPiano::getName() const
{
    return "Helio Piano";
}

void BuiltInSynthPiano::initVoices()
{
    for (int i = BUILTIN_SYNTH_NUM_VOICES; --i >= 0;)
    {
        this->synth.addVoice(new SamplerVoice());
    }
}

void BuiltInSynthPiano::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    if (this->synth.getNumSounds() == 0 && midiMessages.getNumEvents() > 0)
    {
        // Initialization takes about 400ms (i.e. slows app loading way down),
        // and consumes a lot of RAM (though user might never use the built-in piano).
        // So let's do a lazy initialization on first use - i.e. here in processBlock
        this->initVoices();
        this->initSampler();
    }
    
    BuiltInSynthAudioPlugin::processBlock(buffer, midiMessages);
}

void BuiltInSynthPiano::reset()
{
    this->synth.allNotesOff(0, true);
}

void BuiltInSynthPiano::initSampler()
{
    this->synth.clearSounds();

    Array<PianoSample> samples;

    samples.add({ 26, 39, 36, BinaryData::C2v9_flac, BinaryData::C2v9_flacSize });
    samples.add({ 40, 45, 42, BinaryData::F2v9_flac, BinaryData::F2v9_flacSize });

    samples.add({ 46, 51, 48, BinaryData::C3v9_flac, BinaryData::C3v9_flacSize });
    samples.add({ 52, 57, 54, BinaryData::F3v9_flac, BinaryData::F3v9_flacSize });

    samples.add({ 58, 63, 60, BinaryData::C4v9_flac, BinaryData::C4v9_flacSize });
    samples.add({ 64, 69, 66, BinaryData::F4v9_flac, BinaryData::F4v9_flacSize });

    samples.add({ 70, 75, 72, BinaryData::C5v9_flac, BinaryData::C5v9_flacSize });
    samples.add({ 76, 81, 78, BinaryData::F5v9_flac, BinaryData::F5v9_flacSize });

    samples.add({ 82, 87, 84, BinaryData::C6v9_flac, BinaryData::C6v9_flacSize });
    samples.add({ 88, 100, 90, BinaryData::F6v9_flac, BinaryData::F6v9_flacSize });

    for (auto &s : samples)
    {
        auto reader = s.createReader();
        this->synth.addSound(new SamplerSound({}, *reader,
            s.midiNotes, s.midiNoteForNormalPitch,
            ATTACK_TIME, RELEASE_TIME, MAX_PLAY_TIME));
    }
}
