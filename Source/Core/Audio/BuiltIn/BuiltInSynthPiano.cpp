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

#define ATTACK_TIME 0.0
#define RELEASE_TIME 1.0
#define MAX_PLAY_TIME 5.0

BuiltInSynthPiano::BuiltInSynthPiano(bool empty /*= false*/)
{
    if (! empty)
    {
        this->initSamples();
        this->initVoices();
    }

    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

BuiltInSynthPiano::~BuiltInSynthPiano()
{
    this->samples.clear();
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
        Logger::writeToLog("BuiltInSynthPiano deferred init.");
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

    for (const auto &s : this->samples)
    {
        this->synth.addSound(new SamplerSound(s->name,
            *s->reader,
            s->midiNotes,
            s->midiNoteForNormalPitch,
            ATTACK_TIME,
            RELEASE_TIME,
            MAX_PLAY_TIME));
    }
}

void BuiltInSynthPiano::initSamples()
{
    this->samples.clear();

    this->samples.add(new GrandSample("A0v9", 21, 22, 22, BinaryData::A0v9_ogg, BinaryData::A0v9_oggSize));
    this->samples.add(new GrandSample("C1v9", 23, 25, 24, BinaryData::C1v9_ogg, BinaryData::C1v9_oggSize));
    this->samples.add(new GrandSample("D#1v9", 26, 28, 27, BinaryData::D1v9_ogg, BinaryData::D1v9_oggSize));
    this->samples.add(new GrandSample("F#1v9", 29, 31, 30, BinaryData::F1v9_ogg, BinaryData::F1v9_oggSize));

    this->samples.add(new GrandSample("A1v9", 32, 34, 33, BinaryData::A1v9_ogg, BinaryData::A1v9_oggSize));
    this->samples.add(new GrandSample("C2v9", 35, 37, 36, BinaryData::C2v9_ogg, BinaryData::C2v9_oggSize));
    this->samples.add(new GrandSample("D#2v9", 38, 40, 39, BinaryData::D2v9_ogg, BinaryData::D2v9_oggSize));
    this->samples.add(new GrandSample("F#2v9", 41, 43, 42, BinaryData::F2v9_ogg, BinaryData::F2v9_oggSize));

    this->samples.add(new GrandSample("A2v9", 44, 46, 45, BinaryData::A2v9_ogg, BinaryData::A2v9_oggSize));
    this->samples.add(new GrandSample("C3v9", 47, 49, 48, BinaryData::C3v9_ogg, BinaryData::C3v9_oggSize));
    this->samples.add(new GrandSample("D#3v9", 50, 52, 51, BinaryData::D3v9_ogg, BinaryData::D3v9_oggSize));
    this->samples.add(new GrandSample("F#3v9", 53, 55, 54, BinaryData::F3v9_ogg, BinaryData::F3v9_oggSize));

    this->samples.add(new GrandSample("A3v9", 56, 58, 57, BinaryData::A3v9_ogg, BinaryData::A3v9_oggSize));
    this->samples.add(new GrandSample("C4v9", 59, 61, 60, BinaryData::C4v9_ogg, BinaryData::C4v9_oggSize));
    this->samples.add(new GrandSample("D#4v9", 62, 64, 63, BinaryData::D4v9_ogg, BinaryData::D4v9_oggSize));
    this->samples.add(new GrandSample("F#4v9", 65, 67, 66, BinaryData::F4v9_ogg, BinaryData::F4v9_oggSize));

    this->samples.add(new GrandSample("A4v9", 68, 70, 69, BinaryData::A4v9_ogg, BinaryData::A4v9_oggSize));
    this->samples.add(new GrandSample("C5v9", 71, 73, 72, BinaryData::C5v9_ogg, BinaryData::C5v9_oggSize));
    this->samples.add(new GrandSample("D#5v9", 74, 76, 75, BinaryData::D5v9_ogg, BinaryData::D5v9_oggSize));
    this->samples.add(new GrandSample("F#5v9", 77, 79, 78, BinaryData::F5v9_ogg, BinaryData::F5v9_oggSize));

    this->samples.add(new GrandSample("A5v9", 80, 82, 81, BinaryData::A5v9_ogg, BinaryData::A5v9_oggSize));
    this->samples.add(new GrandSample("C6v9", 83, 85, 84, BinaryData::C6v9_ogg, BinaryData::C6v9_oggSize));
    this->samples.add(new GrandSample("D#6v9", 86, 88, 87, BinaryData::D6v9_ogg, BinaryData::D6v9_oggSize));
    this->samples.add(new GrandSample("F#6v9", 89, 91, 90, BinaryData::F6v9_ogg, BinaryData::F6v9_oggSize));

    this->samples.add(new GrandSample("A6v9", 92, 94, 93, BinaryData::A6v9_ogg, BinaryData::A6v9_oggSize));
    this->samples.add(new GrandSample("C7v9", 95, 97, 96, BinaryData::C7v9_ogg, BinaryData::C7v9_oggSize));
    this->samples.add(new GrandSample("D#7v9", 98, 100, 99, BinaryData::D7v9_ogg, BinaryData::D7v9_oggSize));
    this->samples.add(new GrandSample("F#7v9", 101, 103, 102, BinaryData::F7v9_ogg, BinaryData::F7v9_oggSize));

    this->samples.add(new GrandSample("A7v9", 104, 106, 105, BinaryData::A7v9_ogg, BinaryData::A7v9_oggSize));
    this->samples.add(new GrandSample("C8v9", 107, 108, 108, BinaryData::C8v9_ogg, BinaryData::C8v9_oggSize));
}

//sample=A0v9.ogg   lokey=21    hikey=22    pitch_keycenter=21
//sample=C1v9.ogg   lokey=23    hikey=25    pitch_keycenter=24
//sample=D#1v9.ogg  lokey=26    hikey=28    pitch_keycenter=27
//sample=F#1v9.ogg  lokey=29    hikey=31    pitch_keycenter=30
//sample=A1v9.ogg   lokey=32    hikey=34    pitch_keycenter=33
//sample=C2v9.ogg   lokey=35    hikey=37    pitch_keycenter=36
//sample=D#2v9.ogg  lokey=38    hikey=40    pitch_keycenter=39
//sample=F#2v9.ogg  lokey=41    hikey=43    pitch_keycenter=42
//sample=A2v9.ogg   lokey=44    hikey=46    pitch_keycenter=45
//sample=C3v9.ogg   lokey=47    hikey=49    pitch_keycenter=48
//sample=D#3v9.ogg  lokey=50    hikey=52    pitch_keycenter=51
//sample=F#3v9.ogg  lokey=53    hikey=55    pitch_keycenter=54
//sample=A3v9.ogg   lokey=56    hikey=58    pitch_keycenter=57
//sample=C4v9.ogg   lokey=59    hikey=61    pitch_keycenter=60
//sample=D#4v9.ogg  lokey=62    hikey=64    pitch_keycenter=63
//sample=F#4v9.ogg  lokey=65    hikey=67    pitch_keycenter=66
//sample=A4v9.ogg   lokey=68    hikey=70    pitch_keycenter=69
//sample=C5v9.ogg   lokey=71    hikey=73    pitch_keycenter=72
//sample=D#5v9.ogg  lokey=74    hikey=76    pitch_keycenter=75
//sample=F#5v9.ogg  lokey=77    hikey=79    pitch_keycenter=78
//sample=A5v9.ogg   lokey=80    hikey=82    pitch_keycenter=81
//sample=C6v9.ogg   lokey=83    hikey=85    pitch_keycenter=84
//sample=D#6v9.ogg  lokey=86    hikey=88    pitch_keycenter=87
//sample=F#6v9.ogg  lokey=89    hikey=91    pitch_keycenter=90
//sample=A6v9.ogg   lokey=92    hikey=94    pitch_keycenter=93
//sample=C7v9.ogg   lokey=95    hikey=97    pitch_keycenter=96
//sample=D#7v9.ogg  lokey=98    hikey=100   pitch_keycenter=99
//sample=F#7v9.ogg  lokey=101   hikey=103   pitch_keycenter=102
//sample=A7v9.ogg   lokey=104   hikey=106   pitch_keycenter=105
//sample=C8v9.ogg   lokey=107   hikey=108   pitch_keycenter=108

// In case I ever consider to cut the number of samples in a half:
//sample=C1v9.ogg   lokey=21    hikey=27    pitch_keycenter=24
//sample=F#1v9.ogg  lokey=28    hikey=33    pitch_keycenter=30
//sample=C2v9.ogg   lokey=34    hikey=39    pitch_keycenter=36
//sample=F#2v9.ogg  lokey=40    hikey=45    pitch_keycenter=42
//sample=C3v9.ogg   lokey=46    hikey=51    pitch_keycenter=48
//sample=F#3v9.ogg  lokey=52    hikey=57    pitch_keycenter=54
//sample=C4v9.ogg   lokey=58    hikey=63    pitch_keycenter=60
//sample=F#4v9.ogg  lokey=64    hikey=69    pitch_keycenter=66
//sample=C5v9.ogg   lokey=70    hikey=75    pitch_keycenter=72
//sample=F#5v9.ogg  lokey=76    hikey=81    pitch_keycenter=78
//sample=C6v9.ogg   lokey=82    hikey=87    pitch_keycenter=84
//sample=F#6v9.ogg  lokey=88    hikey=93    pitch_keycenter=90
//sample=C7v9.ogg   lokey=94    hikey=99    pitch_keycenter=96
//sample=F#7v9.ogg  lokey=100   hikey=108   pitch_keycenter=102
