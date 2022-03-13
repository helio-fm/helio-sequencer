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
#include "MetronomeSynth.h"
#include "BinaryData.h"

struct MetronomeSynth::TickSample final
{
    static constexpr auto attackTime = 0.0;
    static constexpr auto releaseTime = 0.5;
    static constexpr auto maxPlaybackTime = 4.5;

    MetronomeSynth::TickSample() = default;
    MetronomeSynth::TickSample(const MetronomeSynth::TickSample &other) :
        sourceData(other.sourceData),
        sourceDataSize(other.sourceDataSize),
        midiNoteForNormalPitch(other.midiNoteForNormalPitch),
        midiNotes(other.midiNotes) {}

    MetronomeSynth::TickSample(int lowKey, int highKey, int rootKey,
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

    AudioFormatReader *createReader()
    {
        static FlacAudioFormat flac;
        return flac.createReaderFor(new MemoryInputStream(sourceData, sourceDataSize, false), true);
    }

    const char *sourceData;
    const int sourceDataSize;
    BigInteger midiNotes;
    const int midiNoteForNormalPitch;

    JUCE_LEAK_DETECTOR(MetronomeSynth::TickSample)
};

void MetronomeSynth::initVoices()
{
    for (int i = MetronomeSynth::numVoices; i --> 0 ;)
    {
        this->addVoice(new SamplerVoice());
    }
}

void MetronomeSynth::initSampler()
{
    this->clearSounds();

    Array<TickSample> samples;
    //samples.add({ 26, 39, 36, BinaryData::todo_flac, BinaryData::todo_flacSize });

    for (auto &s : samples)
    {
        UniquePointer<AudioFormatReader> reader(s.createReader());
        this->addSound(new SamplerSound({}, *reader,
            s.midiNotes, s.midiNoteForNormalPitch,
            TickSample::attackTime, TickSample::releaseTime, TickSample::maxPlaybackTime));
    }
}

void MetronomeSynth::handleSustainPedal(int midiChannel, bool isDown) {}
void MetronomeSynth::handleSostenutoPedal(int midiChannel, bool isDown) {}
