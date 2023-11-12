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
#include "DefaultSynth.h"

//===----------------------------------------------------------------------===//
// Voice
//===----------------------------------------------------------------------===//

DefaultSynth::Voice::Voice()
{
    ADSR::Parameters ap;
    ap.attack = 0.001f;
    ap.decay = 2.0f;
    ap.sustain = 0.5f;
    ap.release = 0.35f;
    this->adsr.setParameters(ap);

    Reverb::Parameters rp;
    rp.roomSize = 0.0f;
    rp.damping = 0.0f;
    rp.wetLevel = 0.23f;
    rp.dryLevel = 0.73f;
    rp.width = 0.1f;
    rp.freezeMode = 0.4f;
    this->reverb.setParameters(rp);
}

bool DefaultSynth::Voice::canPlaySound(SynthesiserSound *)
{
    return true; // just assume correct usage
}

void DefaultSynth::Voice::setCurrentPlaybackSampleRate(double sampleRate)
{
    if (sampleRate > 0)
    {
        this->adsr.setSampleRate(sampleRate);
        this->reverb.setSampleRate(sampleRate);
        SynthesiserVoice::setCurrentPlaybackSampleRate(sampleRate);
    }
}

void DefaultSynth::Voice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int)
{
    const auto channel = this->getCurrentChannel();

    // for microtonal temperaments try to figure out the key
    // assuming the default multi-channel keyboard mapping:
    const int adjustedNoteNumber =
        this->periodSize > Globals::twelveTonePeriodSize ?
        midiNoteNumber + Globals::twelveToneKeyboardSize * (channel - 1) :
        midiNoteNumber;

    this->level = velocity * 0.2; // hopefully not too loud

    this->setFrequency(this->getNoteInHertz(adjustedNoteNumber));

    this->reverb.reset();
    this->adsr.noteOn();
}

void DefaultSynth::Voice::stopNote(float, bool allowTailOff)
{
    // always stopping a note with tail to avoid nasty clicks in the chord tool,
    // the release duration is set to be small for this reason
    //if (allowTailOff)
    {
        this->adsr.noteOff();
    }
    //else
    //{
    //    this->adsr.reset();
    //    this->reverb.reset();
    //    this->clearCurrentNote();
    //}
}

bool DefaultSynth::Voice::isVoiceActive() const
{
    return this->adsr.isActive();
}

void DefaultSynth::Voice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    if (this->adsr.isActive())
    {
        while (--numSamples >= 0)
        {
            const auto amplitude = this->adsr.getNextSample();
            auto currentSample = float(this->getNextSample() * this->level * amplitude);

#if PLATFORM_DESKTOP
            // even this seems to be too slow for realtime playback on many phones
            this->reverb.processMono(&currentSample, 1);
#endif

            for (auto i = outputBuffer.getNumChannels(); i --> 0 ;)
            {
                outputBuffer.addSample(i, startSample, currentSample);
            }

            ++startSample;
        }
    }
}

void DefaultSynth::Voice::setPeriodSize(int size) noexcept
{
    this->periodSize = size;
    this->middleC = Temperament::periodNumForMiddleC * this->periodSize;
}

void DefaultSynth::Voice::setPeriodRange(double periodRange) noexcept
{
    this->periodRange = periodRange;
}

void DefaultSynth::Voice::setFrequency(double frequency) noexcept
{
    const auto tableSizeOverSampleRate = double(DefaultSynth::tableSize) / this->getSampleRate();
    this->waveTableDelta = float(frequency * tableSizeOverSampleRate);
}

double DefaultSynth::Voice::getNoteInHertz(int noteNumber, double frequencyOfA /*= 440.0*/) noexcept
{
    return frequencyOfA * std::pow(this->periodRange,
        double(noteNumber - this->middleC) / double(this->periodSize));
}

int DefaultSynth::Voice::getCurrentChannel() const noexcept
{
    // the only way to access channel info in SynthesizerVoice :(
    for (int i = 1; i <= Globals::numChannels; ++i)
    {
        if (this->isPlayingChannel(i))
        {
            return i;
        }
    }

    jassertfalse;
    return 1;
}

//===----------------------------------------------------------------------===//
// DefaultSynth
//===----------------------------------------------------------------------===//

DefaultSynth::DefaultSynth()
{
    this->initWaveTable();

    for (int i = DefaultSynth::numVoices; i --> 0 ;)
    {
        this->addVoice(new DefaultSynth::Voice());
    }

    this->addSound(new DefaultSynth::Sound());
}

AudioSampleBuffer DefaultSynth::waveTable;

void DefaultSynth::initWaveTable()
{
//#if DEBUG
//    const double initStart = Time::getMillisecondCounterHiRes();
//#endif

    // just following JUCE tutorial on wavetables here:
    // https://docs.juce.com/master/tutorial_wavetable_synth.html

    this->waveTable.setSize(1, DefaultSynth::tableSize + 1);
    this->waveTable.clear();
 
    auto *samples = this->waveTable.getWritePointer(0);

    constexpr float harmonics[] = { 1.f, 0.5f, 3.f };
    constexpr float harmonicWeights[] = { 0.75f, 0.05f, 0.025f };
 
    for (auto harmonic = 0; harmonic < numElementsInArray(harmonics); ++harmonic)
    {
        auto currentAngle = 0.0;
        const auto angleDelta = MathConstants<double>::twoPi /
            double(DefaultSynth::tableSize - 1) * harmonics[harmonic];

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            const auto sample = std::sin(currentAngle);
            samples[i] += float(sample) * harmonicWeights[harmonic];
            currentAngle += angleDelta;
        }
    }
 
    samples[tableSize] = samples[0];

    //DBG("Initialized default synth's wavetable in " +
    //    String(Time::getMillisecondCounterHiRes() - initStart) + "ms");
}

void DefaultSynth::setPeriodSizeAndRange(int periodSize, double periodRange)
{
    //DBG("Setting octave size for the default synth: " + String(periodSize));
    for (int i = 0; i < this->getNumVoices(); ++i)
    {
        if (auto *voice = dynamic_cast<DefaultSynth::Voice *>(this->getVoice(i)))
        {
            voice->stopNote(1.f, false);
            voice->setPeriodSize(periodSize);
            voice->setPeriodRange(periodRange);
        }
    }
}

// the built-in synth doesn't have pedals.
// the built-in synth doesn't need pedals!
void DefaultSynth::handleSustainPedal(int midiChannel, bool isDown) {}
// seriously, just want to make sure that once I send a note-off event,
// the BuiltInSynthVoice shuts up regardless of controller states
void DefaultSynth::handleSostenutoPedal(int midiChannel, bool isDown) {}
