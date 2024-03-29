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
    ap.attack = 0.002f;
    ap.decay = 2.0f;
    ap.sustain = 0.45f;
    ap.release = 0.4f;
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

    this->currentAngle = 0.f;
    this->level = velocity * 0.2f; // hopefully not too loud

    const auto cyclesPerSecond = this->getNoteInHertz(adjustedNoteNumber);
    const auto cyclesPerSample = cyclesPerSecond / this->getSampleRate();

    this->angleDelta = float(cyclesPerSample) * MathConstants<float>::twoPi;

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

#if JUCE_LINUX
            // the standard: std::sinf is available since c++11
            // gcc: well yes, but actually no
            auto currentSample = std::sin(this->currentAngle) * this->level * amplitude;
#else
            auto currentSample = std::sinf(this->currentAngle) * this->level * amplitude;
#endif

#if PLATFORM_DESKTOP
            // even this seems to be too slow for realtime playback on many phones
            this->reverb.processMono(&currentSample, 1);
#endif

            for (auto i = outputBuffer.getNumChannels(); i --> 0 ;)
            {
                outputBuffer.addSample(i, startSample, currentSample);
            }

            this->currentAngle += this->angleDelta;
            ++startSample;
        }
    }
}

void DefaultSynth::Voice::setTemperament(Temperament::Ptr temperament) noexcept
{
    this->periodSize = temperament->getPeriodSize();
    this->periodRange = temperament->getPeriodRange();
    this->middleA = Temperament::periodNumForMiddleC * this->periodSize +
        // what the temperament considers to be the equivalent of "la" note:
        temperament->getEquivalentOfTwelveToneInterval(Semitones::MajorSixth);
}

double DefaultSynth::Voice::getNoteInHertz(int noteNumber, double frequencyOfA /*= 440.0*/) noexcept
{
    return frequencyOfA * std::pow(this->periodRange,
        double(noteNumber - this->middleA) / double(this->periodSize));
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
    for (int i = DefaultSynth::numVoices; i --> 0 ;)
    {
        this->addVoice(new DefaultSynth::Voice());
    }

    this->addSound(new DefaultSynth::Sound());
}

void DefaultSynth::setTemperament(Temperament::Ptr temperament)
{
    for (int i = 0; i < this->getNumVoices(); ++i)
    {
        if (auto *voice = dynamic_cast<DefaultSynth::Voice *>(this->getVoice(i)))
        {
            voice->stopNote(1.f, false);
            voice->setTemperament(temperament);
        }
    }
}

// the built-in synth doesn't have pedals.
// the built-in synth doesn't need pedals!
void DefaultSynth::handleSustainPedal(int midiChannel, bool isDown) {}
// seriously, just want to make sure that once I send a note-off event,
// the BuiltInSynthVoice shuts up regardless of controller states
void DefaultSynth::handleSostenutoPedal(int midiChannel, bool isDown) {}
