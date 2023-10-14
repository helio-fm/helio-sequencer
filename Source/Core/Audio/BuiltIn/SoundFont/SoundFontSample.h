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

class SharedAudioSampleBuffer final : public ReferenceCountedObject, public AudioSampleBuffer
{
public:

    using Ptr = ReferenceCountedObjectPtr<SharedAudioSampleBuffer>;

    explicit SharedAudioSampleBuffer(int numChannels, int numSamples) :
        AudioSampleBuffer(numChannels, numSamples) {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SharedAudioSampleBuffer)
};

class SoundFontSample final
{
public:

    explicit SoundFontSample(const File &fileIn) :
        file(fileIn) {}

    explicit SoundFontSample(double sampleRateIn) :
        sampleRate(sampleRateIn) {}

    File getFile() const noexcept { return this->file; }
    String getShortName() const noexcept { return this->file.getFileName(); }

    const AudioSampleBuffer *getBuffer() const noexcept { return this->buffer.get(); }
    void setBuffer(SharedAudioSampleBuffer::Ptr newBuffer)
    {
        this->buffer = newBuffer;
        if (this->buffer != nullptr)
        {
            this->sampleLength = this->buffer->getNumSamples();
        }
        else
        {
            this->sampleLength = 0;
        }
    }

    double getSampleRate() const noexcept { return this->sampleRate; }
    uint64 getSampleLength() const noexcept { return this->sampleLength; }
    uint64 getLoopStart() const noexcept { return this->loopStart; }
    uint64 getLoopEnd() const noexcept { return this->loopEnd; }
    
    bool load(AudioFormatManager &formatManager)
    {
        UniquePointer<AudioFormatReader> reader(formatManager.createReaderFor(this->file));
        if (reader == nullptr)
        {
            return false;
        }

        this->sampleRate = reader->sampleRate;
        this->sampleLength = reader->lengthInSamples;

        // Read some extra samples, which will be filled with zeros, so interpolation
        // can be done without having to check for the edge all the time.
        jassert(this->sampleLength < std::numeric_limits<int>::max());

        this->buffer = new SharedAudioSampleBuffer(reader->numChannels, static_cast<int>(this->sampleLength + 4));
        reader->read(this->buffer.get(), 0, static_cast<int>(this->sampleLength + 4), 0, true, true);

        const auto *metadata = &reader->metadataValues;
        const int numLoops = metadata->getValue("NumSampleLoops", "0").getIntValue();
        if (numLoops > 0)
        {
            this->loopStart = metadata->getValue("Loop0Start", "0").getLargeIntValue();
            this->loopEnd = metadata->getValue("Loop0End", "0").getLargeIntValue();
        }

        return true;
    }

private:

    File file;

    // all samples share the single buffer:
    SharedAudioSampleBuffer::Ptr buffer;

    double sampleRate = 0.0;
    uint64 sampleLength = 0;
    uint64 loopStart = 0;
    uint64 loopEnd = 0;

    JUCE_DECLARE_WEAK_REFERENCEABLE(SoundFontSample)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontSample)
};
