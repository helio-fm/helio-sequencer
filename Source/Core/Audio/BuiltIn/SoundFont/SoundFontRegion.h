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

#include "SoundFontSample.h"

// SoundFontRegion is designed to be able to be bitwise-copied.

struct EGParameters final
{
    float delay = 0.f;
    float start = 0.f;
    float attack = 0.f;
    float hold = 0.f;
    float decay = 0.f;
    float sustain = 0.f;
    float release = 0.f;

    void clear()
    {
        this->delay = this->start = this->attack = this->hold = this->decay = this->release = 0.0;
        this->sustain = 100.0;
    }

    void clearMod()
    {
        // Clear for velocity or other modification.
        this->delay = this->start = this->attack = this->hold = this->decay = this->sustain = this->release = 0.0;
    }
};

struct SoundFontRegion final
{
    enum class Trigger
    {
        attack,
        release,
        first,
        legato
    };

    enum class LoopMode
    {
        sampleLoop,
        noLoop,
        oneShot,
        loopContinuous,
        loopSustain
    };

    enum class OffMode
    {
        fast,
        normal
    };

    SoundFontRegion()
    {
        this->clear();
    }

    void clear()
    {
        memset(this, 0, sizeof(*this));
        this->hikey = 127;
        this->hivel = 127;
        this->pitchKeyCenter = 60; // C4
        this->pitchKeyTrack = 100;
        this->bendUp = 200;
        this->bendDown = -200;
        this->volume = this->pan = 0.f;
        this->ampVelTrack = 100.f;
        this->ampeg.clear();
        this->ampegVelTrack.clearMod();
    }

    void clearForSF2()
    {
        this->clear();
        this->pitchKeyCenter = -1;
        this->loopMode = LoopMode::noLoop;

        // SF2 defaults in timecents.
        this->ampeg.delay = -12000.f;
        this->ampeg.attack = -12000.f;
        this->ampeg.hold = -12000.f;
        this->ampeg.decay = -12000.f;
        this->ampeg.sustain = 0.f;
        this->ampeg.release = -12000.f;
    }

    void clearForRelativeSF2()
    {
        this->clear();
        this->pitchKeyTrack = 0;
        this->ampVelTrack = 0.f;
        this->ampeg.sustain = 0.f;
    }

    void addForSF2(SoundFontRegion *other)
    {
        this->offset += other->offset;
        this->end += other->end;
        this->loopStart += other->loopStart;
        this->loopEnd += other->loopEnd;
        this->transpose += other->transpose;
        this->tune += other->tune;
        this->pitchKeyTrack += other->pitchKeyTrack;
        this->volume += other->volume;
        this->pan += other->pan;

        this->ampeg.delay += other->ampeg.delay;
        this->ampeg.attack += other->ampeg.attack;
        this->ampeg.hold += other->ampeg.hold;
        this->ampeg.decay += other->ampeg.decay;
        this->ampeg.sustain += other->ampeg.sustain;
        this->ampeg.release += other->ampeg.release;
    }

    void sf2ToSFZ()
    {
        const auto timeCents2Secs = [](int timeCents)
        {
            return float(pow(2.0, timeCents / 1200.0));
        };

        // EG times need to be converted from timecents to seconds.
        this->ampeg.delay = timeCents2Secs(int(this->ampeg.delay));
        this->ampeg.attack = timeCents2Secs(int(this->ampeg.attack));
        this->ampeg.hold = timeCents2Secs(int(this->ampeg.hold));
        this->ampeg.decay = timeCents2Secs(int(this->ampeg.decay));
        this->ampeg.release = timeCents2Secs(int(this->ampeg.release));

        this->ampeg.start = jlimit(0.f, 100.f, this->ampeg.start);
        this->ampeg.sustain = jmax(this->ampeg.sustain, 0.f);
        this->ampeg.sustain = 100.f * Decibels::decibelsToGain(-this->ampeg.sustain / 10.f);

        // Restrict to min/max useful values according to specs:
        this->ampeg.delay = jlimit(0.001f, 20.f, this->ampeg.delay);
        this->ampeg.attack = jlimit(0.001f, 100.f, this->ampeg.attack);
        this->ampeg.hold = jlimit(0.001f, 20.f, this->ampeg.hold);
        this->ampeg.decay = jlimit(0.001f, 100.f, this->ampeg.decay);
        this->ampeg.release = jlimit(0.001f, 100.f, this->ampeg.release);

        // Pin values to their ranges.
        this->pan = jlimit(-100.f, 100.f, this->pan);
    }

    String dump()
    {
        String info = String::formatted("%d - %d, vel %d - %d", lokey, hikey, lovel, hivel);
        if (sample)
        {
            info << sample->getShortName();
        }
        info << "\n";
        return info;
    }

    bool matches(int note, int velocity, Trigger trigger, int periodSize) const noexcept
    {
        int mappedNote = note;
        if (periodSize != Globals::twelveTonePeriodSize)
        {
            mappedNote = int(double(note * Globals::twelveTonePeriodSize) / double(periodSize));
        }

        return (mappedNote >= this->lokey && mappedNote <= this->hikey &&
            velocity >= this->lovel && velocity <= this->hivel &&
            (trigger == this->trigger ||
                (this->trigger == Trigger::attack &&
                    (trigger == Trigger::first || trigger == Trigger::legato))));
    }

    WeakReference<SoundFontSample> sample;

    int lokey, hikey;
    int lovel, hivel;
    Trigger trigger;
    int group;
    int64 offBy;
    OffMode offMode;

    int64 offset;
    int64 end;
    bool negativeEnd;
    LoopMode loopMode;
    int64 loopStart, loopEnd;
    int transpose;
    int tune;
    int pitchKeyCenter, pitchKeyTrack;
    int bendUp, bendDown;

    float volume, pan;
    float ampVelTrack;

    EGParameters ampeg, ampegVelTrack;

    JUCE_LEAK_DETECTOR(SoundFontRegion)
};
