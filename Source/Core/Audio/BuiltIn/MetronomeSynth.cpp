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
#include "SerializationKeys.h"
#include "BinaryData.h"

struct MetronomeSynth::TickSample final
{
    static constexpr auto attackTime = 0.0;
    static constexpr auto releaseTime = 0.5;
    static constexpr auto maxPlaybackTime = 4.5;

    MetronomeSynth::TickSample() = default;
    MetronomeSynth::TickSample(const MetronomeSynth::TickSample &other) = default;

    MetronomeSynth::TickSample(int rootKey,
        const char *sourceData, int sourceDataSize) :
        sourceData(sourceData),
        sourceDataSize(sourceDataSize),
        midiNoteForNormalPitch(rootKey)
    {
        this->midiNotes.setBit(rootKey);
    }

    MetronomeSynth::TickSample(int rootKey,
        const String &customSample) :
        customSamplePath(customSample),
        midiNoteForNormalPitch(rootKey)
    {
        this->midiNotes.setBit(rootKey);
    }

    AudioFormatReader *createReader()
    {
        jassert(this->customSamplePath.isNotEmpty() || this->sourceData != nullptr);

        if (this->sourceData != nullptr)
        {
            static FlacAudioFormat flacReader;
            return flacReader.createReaderFor(new MemoryInputStream(this->sourceData, this->sourceDataSize, false), true);
        }

        File sampleFile(this->customSamplePath);
        if (!sampleFile.existsAsFile())
        {
            jassertfalse;
            return nullptr;
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

    const char *sourceData = nullptr;
    const int sourceDataSize = 0;

    const String customSamplePath;

    BigInteger midiNotes;
    const int midiNoteForNormalPitch = 0;

    JUCE_LEAK_DETECTOR(MetronomeSynth::TickSample)
};

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
        // any better options for keys?
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

    for (auto &syllable : MetronomeScheme::getAllOrdered())
    {
        const auto key = MetronomeSynth::getKeyForSyllable(syllable);
        const auto customSample = params.customSamples.find(syllable);

        if (customSample != params.customSamples.end())
        {
            samples.add({ key, customSample->second });
        }
        else
        {
            // using the built-in metronome
            int sampleDataSize = 0;
            // fixme: add the actual samples
            const auto assumedFileName = "metronome_" + MetronomeScheme::syllableToString(syllable) + "_flac";
            auto *sampleData = BinaryData::getNamedResource(assumedFileName.toRawUTF8(), sampleDataSize);
            samples.add({ key, sampleData, sampleDataSize });
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
