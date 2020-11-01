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

BuiltInSynth::BuiltInSynth()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

const String BuiltInSynth::getName() const
{
    return "Helio Piano";
}

void BuiltInSynth::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    // forgive me father, for I have synthed...
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void BuiltInSynth::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    // todo init synth?
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void BuiltInSynth::reset()
{
    this->synth.allNotesOff(0, true);
}

void BuiltInSynth::initSynth()
{
    //for (int i = BUILTIN_SYNTH_NUM_VOICES; --i >= 0;)
    //{
    //    this->synth.addVoice(new SamplerVoice());
    //}

    //this->synth.clearSounds();
    //this->synth.addSound(todo);
}

const String BuiltInSynth::instrumentId = "<piano>";
