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
#include "BuiltInSynth.h"
#include "BinaryData.h"

#define ATTACK_TIME (0.0)
#define RELEASE_TIME (0.5)
#define MAX_PLAY_TIME (4.5)

struct BuiltInSynthSound final : public SynthesiserSound
{
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }
};

class BuiltInSynthVoice final : public SynthesiserVoice
{
public:

    BuiltInSynthVoice()
    {
        ADSR::Parameters ap;
        ap.attack = 0.001f;
        ap.decay = 1.0f;
        ap.sustain = 0.2f;
        ap.release = 0.5f;
        this->adsr.setParameters(ap);

        Reverb::Parameters rp;
        rp.roomSize = 0.0f;
        rp.damping = 0.0f;
        rp.wetLevel = 0.23f;
        rp.dryLevel = 0.73f;
        rp.width = 0.0f;
        rp.freezeMode = 0.4f;
        this->reverb.setParameters(rp);
    }

    bool canPlaySound(SynthesiserSound *) override
    {
        return true; // just assume correct usage
    }

    void setCurrentPlaybackSampleRate(double sampleRate) override
    {
        if (sampleRate > 0)
        {
            this->adsr.setSampleRate(sampleRate);
            this->reverb.setSampleRate(sampleRate);
            SynthesiserVoice::setCurrentPlaybackSampleRate(sampleRate);
        }
    }

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int) override
    {
        this->currentAngle = 0.0;
        this->level = velocity * 0.15;

        const auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        const auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        this->angleDelta = cyclesPerSample * MathConstants<double>::twoPi;

        this->adsr.noteOn();
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            this->adsr.noteOff();
        }
        else
        {
            this->clearCurrentNote();
            this->adsr.reset();
            this->reverb.reset();
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (this->adsr.isActive())
        {
            while (--numSamples >= 0)
            {
                const auto amplitude = this->adsr.getNextSample();
                auto currentSample = (float)(std::sin(currentAngle) * level * amplitude);
                this->reverb.processMono(&currentSample, 1);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                {
                    outputBuffer.addSample(i, startSample, currentSample);
                }

                this->currentAngle += this->angleDelta;
                ++startSample;
            }
        }
    }

    using SynthesiserVoice::renderNextBlock;

private:

    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;

    ADSR adsr;
    Reverb reverb;

};

BuiltInSynth::BuiltInSynth()
{
    this->initSynth(); // fixme
    this->setPlayConfigDetails(0, 2,
        this->getSampleRate(), this->getBlockSize());
}

const String BuiltInSynth::getName() const
{
    return "Helio Piano";
}

void BuiltInSynth::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void BuiltInSynth::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void BuiltInSynth::reset()
{
    this->synth.allNotesOff(0, true);
}

// todo set temperament info here
void BuiltInSynth::initSynth()
{
    this->synth.clearVoices();
    this->synth.clearSounds();

    for (int i = BuiltInSynth::numVoices; --i >= 0;)
    {
        this->synth.addVoice(new BuiltInSynthVoice());
    }

    this->synth.addSound(new BuiltInSynthSound());
}

const String BuiltInSynth::instrumentId = "<piano>";
