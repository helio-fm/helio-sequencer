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
*/

#include "Common.h"
#include "MetronomeSynth.h"
#include "DocumentHelpers.h"
#include "SerializationKeys.h"
#include "BinaryData.h"

//===----------------------------------------------------------------------===//
// TickSample
//===----------------------------------------------------------------------===//

MetronomeSynth::TickSample::TickSample(int rootKey, const char *sourceData, int sourceDataSize) :
    sourceData(sourceData),
    sourceDataSize(sourceDataSize),
    midiNoteForNormalPitch(rootKey)
{
    this->midiNotes.setBit(rootKey);
}

MetronomeSynth::TickSample::TickSample(int rootKey,
    const String &customSample) :
    customSamplePath(customSample),
    midiNoteForNormalPitch(rootKey)
{
    this->midiNotes.setBit(rootKey);
}

AudioFormatReader *MetronomeSynth::TickSample::createReader()
{
    jassert(this->customSamplePath.isNotEmpty() || this->sourceData != nullptr);

    if (this->sourceData != nullptr)
    {
        static WavAudioFormat wavReader;
        return wavReader.createReaderFor(new MemoryInputStream(this->sourceData, this->sourceDataSize, false), true);
    }

    File sampleFile(this->customSamplePath);
    if (!sampleFile.existsAsFile())
    {
        // iOS hack: the `documents` path will change between launches
        sampleFile = DocumentHelpers::getDocumentSlot(sampleFile.getFileName());

        if (!sampleFile.existsAsFile())
        {
            jassertfalse;
            return nullptr;
        }
    }

    if (this->customSamplePath.endsWithIgnoreCase(".wav"))
    {
        static WavAudioFormat wavFileReader;
        return wavFileReader.createReaderFor(new FileInputStream(sampleFile), true);
    }
    else if (this->customSamplePath.endsWithIgnoreCase(".flac"))
    {
        static FlacAudioFormat flacFileReader;
        return flacFileReader.createReaderFor(new FileInputStream(sampleFile), true);
    }

    jassertfalse;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// MetronomeSynth
//===----------------------------------------------------------------------===//

void MetronomeSynth::initVoices()
{
    if (this->getNumVoices() == MetronomeSynth::numVoices)
    {
        return;
    }

    this->clearVoices();

    for (int i = MetronomeSynth::numVoices; i --> 0 ;)
    {
        this->addVoice(new SamplerVoice());
    }
}

Note::Key MetronomeSynth::getKeyForSyllable(MetronomeScheme::Syllable syllable)
{
    switch (syllable)
    {
        // it doesn't matter which keys are used here, as long as they are different:
        case MetronomeScheme::Syllable::Oo: return 60;
        case MetronomeScheme::Syllable::na: return 61;
        case MetronomeScheme::Syllable::Pa: return 62;
        case MetronomeScheme::Syllable::pa: return 63;
    }

    return {};
}

void MetronomeSynth::initSampler(const SamplerParameters &params)
{
    this->clearSounds();

    Array<TickSample> samples;
    const auto syllables = MetronomeScheme::getAllOrdered();

    for (int i = 0; i < syllables.size(); ++i)
    {
        const auto syllable = syllables.getUnchecked(i);
        const auto key = MetronomeSynth::getKeyForSyllable(syllable);
        const auto customSample = params.customSamples.find(syllable);

        if (customSample != params.customSamples.end())
        {
            samples.add(TickSample(key, customSample->second));
        }
        else
        {
            int sampleDataSize = 0;
            const auto assumedFileName = "builtInMetronome" + String(i + 1) + "_wav";
            if (auto *sampleData = BinaryData::getNamedResource(assumedFileName.toRawUTF8(), sampleDataSize))
            {
                samples.add(TickSample(key, sampleData, sampleDataSize));
            }
        }
    }

    for (auto &sample : samples)
    {
        UniquePointer<AudioFormatReader> sampleSoundReader(sample.createReader());
        if (sampleSoundReader != nullptr)
        {
            this->addSound(new SamplerSound({}, *sampleSoundReader,
                sample.midiNotes, sample.midiNoteForNormalPitch,
                TickSample::attackTime, TickSample::releaseTime,
                TickSample::maxPlaybackTime));
        }
    }
}

void MetronomeSynth::handleSustainPedal(int midiChannel, bool isDown) {}
void MetronomeSynth::handleSostenutoPedal(int midiChannel, bool isDown) {}

SerializedData MetronomeSynth::SamplerParameters::serialize() const
{
    using namespace Serialization::Audio;

    SerializedData data(Metronome::metronomeConfig);

    for (const auto &it : this->customSamples)
    {
        SerializedData sampleNode(Metronome::customSample);
        sampleNode.setProperty(Metronome::syllableName, MetronomeScheme::syllableToString(it.first));
        sampleNode.setProperty(Metronome::filePath, it.second);
        data.appendChild(sampleNode);
    }

    return data;
}

void MetronomeSynth::SamplerParameters::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization::Audio;

    const auto root = data.hasType(Metronome::metronomeConfig) ?
        data : data.getChildWithName(Metronome::metronomeConfig);

    if (!root.isValid())
    {
        return;
    }

    for (int i = 0 ; i < root.getNumChildren(); ++i)
    {
        const String syllableString = root.getChild(i).getProperty(Metronome::syllableName);
        const String sampleFilePath = root.getChild(i).getProperty(Metronome::filePath);
        const auto syllable = MetronomeScheme::syllableFromString(syllableString);
        this->customSamples[syllable] = sampleFilePath;
    }
}

void MetronomeSynth::SamplerParameters::reset()
{
    this->customSamples.clear();
}
