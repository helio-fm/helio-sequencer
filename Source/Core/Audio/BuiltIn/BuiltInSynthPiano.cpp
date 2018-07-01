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
#define RELEASE_TIME (1.0)
#define MAX_PLAY_TIME (5.0)

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
        //Logger::writeToLog("BuiltInSynthPiano deferred init.");
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

    OwnedArray<GrandSample> samples;
    samples.add(new GrandSample("C1v9", 21, 27, 24, BinaryData::C1v9_flac, BinaryData::C1v9_flacSize));
    samples.add(new GrandSample("F#1v9", 28, 33, 30, BinaryData::F1v9_flac, BinaryData::F1v9_flacSize));

    samples.add(new GrandSample("C2v9", 34, 39, 36, BinaryData::C2v9_flac, BinaryData::C2v9_flacSize));
    samples.add(new GrandSample("F#2v9", 40, 45, 42, BinaryData::F2v9_flac, BinaryData::F2v9_flacSize));

    samples.add(new GrandSample("C3v9", 46, 51, 48, BinaryData::C3v9_flac, BinaryData::C3v9_flacSize));
    samples.add(new GrandSample("F#3v9", 52, 57, 54, BinaryData::F3v9_flac, BinaryData::F3v9_flacSize));

    samples.add(new GrandSample("C4v9", 58, 63, 60, BinaryData::C4v9_flac, BinaryData::C4v9_flacSize));
    samples.add(new GrandSample("F#4v9", 64, 69, 66, BinaryData::F4v9_flac, BinaryData::F4v9_flacSize));

    samples.add(new GrandSample("C5v9", 70, 75, 72, BinaryData::C5v9_flac, BinaryData::C5v9_flacSize));
    samples.add(new GrandSample("F#5v9", 76, 81, 78, BinaryData::F5v9_flac, BinaryData::F5v9_flacSize));

    samples.add(new GrandSample("C6v9", 82, 87, 84, BinaryData::C6v9_flac, BinaryData::C6v9_flacSize));
    samples.add(new GrandSample("F#6v9", 88, 93, 90, BinaryData::F6v9_flac, BinaryData::F6v9_flacSize));

    samples.add(new GrandSample("C7v9", 94, 99, 96, BinaryData::C7v9_flac, BinaryData::C7v9_flacSize));
    samples.add(new GrandSample("F#7v9", 100, 108, 102, BinaryData::F7v9_flac, BinaryData::F7v9_flacSize));

    for (const auto &s : samples)
    {
        auto reader = s->createReader();
        this->synth.addSound(new SamplerSound(s->name,
            *reader,
            s->midiNotes,
            s->midiNoteForNormalPitch,
            ATTACK_TIME,
            RELEASE_TIME,
            MAX_PLAY_TIME));
    }
}
