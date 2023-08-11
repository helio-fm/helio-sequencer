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

    This SoundFont implementation is based on SFZero,
    written by Steve Folta and extended by Leo Olivers and Cognitone,
    distributed under MIT license, see README.md for details.
*/

#include "Common.h"
#include "SoundFontSynth.h"
#include "SoundFontSound.h"
#include "SoundFont2Sound.h"
#include "SoundFontRegion.h"
#include "SoundFontSample.h"
#include "SerializationKeys.h"

class SoundFontEnvelope final
{
public:

    SoundFontEnvelope() = default;

    void setExponentialDecay(bool newExponentialDecay);
    void startNote(const EGParameters *parameters,
        float floatVelocity, double sampleRate,
        const EGParameters *velMod = nullptr);

    void nextSegment();
    void noteOff();
    void fastRelease();
    bool isDone() const { return (this->segment == Segment::Done); }
    bool isReleasing() const { return (this->segment == Segment::Release); }
    int segmentIndex() const { return static_cast<int>(this->segment); }
    float getLevel() const { return this->level; }
    void setLevel(float v) { this->level = v; }
    float getSlope() const { return this->slope; }
    void setSlope(float v) { this->slope = v; }
    int getSamplesUntilNextSegment() const { return this->samplesUntilNextSegment; }
    void setSamplesUntilNextSegment(int v) { this->samplesUntilNextSegment = v; }
    bool getSegmentIsExponential() const { return this->segmentIsExponential; }
    void setSegmentIsExponential(bool v) { this->segmentIsExponential = v; }

private:

    enum class Segment
    {
        Delay,
        Attack,
        Hold,
        Decay,
        Sustain,
        Release,
        Done
    };

    void startDelay();
    void startAttack();
    void startHold();
    void startDecay();
    void startSustain();
    void startRelease();

    Segment segment = Segment::Done;
    EGParameters parameters;
    double sampleRate = 0.0;
    bool exponentialDecay = false;
    float level = 0.f;
    float slope = 0.f;
    int samplesUntilNextSegment = 0;
    bool segmentIsExponential = false;
    static const float BottomLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontEnvelope)
};

static const float fastReleaseTime = 0.01f;

void SoundFontEnvelope::setExponentialDecay(bool newExponentialDecay)
{
    this->exponentialDecay = newExponentialDecay;
}

void SoundFontEnvelope::startNote(const EGParameters *newParameters, float floatVelocity, double newSampleRate,
    const EGParameters *velMod)
{
    this->parameters = *newParameters;
    if (velMod)
    {
        this->parameters.delay += floatVelocity * velMod->delay;
        this->parameters.attack += floatVelocity * velMod->attack;
        this->parameters.hold += floatVelocity * velMod->hold;
        this->parameters.decay += floatVelocity * velMod->decay;
        this->parameters.sustain += floatVelocity * velMod->sustain;
        if (this->parameters.sustain < 0.0)
        {
            this->parameters.sustain = 0.0;
        }
        else if (this->parameters.sustain > 100.0)
        {
            this->parameters.sustain = 100.0;
        }
        this->parameters.release += floatVelocity * velMod->release;
    }

    this->sampleRate = newSampleRate;
    this->startDelay();
}

void SoundFontEnvelope::nextSegment()
{
    switch (this->segment)
    {
        case Segment::Delay:
            this->startAttack();
            break;
        case Segment::Attack:
            this->startHold();
            break;
        case Segment::Hold:
            this->startDecay();
            break;
        case Segment::Decay:
            this->startSustain();
            break;
        case Segment::Sustain:
            jassertfalse;
            break;
        case Segment::Release:
        default:
            this->segment = Segment::Done;
            break;
    }
}

void SoundFontEnvelope::noteOff()
{
    this->startRelease();
}

void SoundFontEnvelope::fastRelease()
{
    this->segment = Segment::Release;
    this->samplesUntilNextSegment = int(fastReleaseTime * this->sampleRate);
    this->slope = -this->level / this->samplesUntilNextSegment;
    this->segmentIsExponential = false;
}

void SoundFontEnvelope::startDelay()
{
    if (this->parameters.delay <= 0)
    {
        this->startAttack();
    }
    else
    {
        this->segment = Segment::Delay;
        this->level = 0.0;
        this->slope = 0.0;
        this->samplesUntilNextSegment = int(this->parameters.delay * this->sampleRate);
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startAttack()
{
    if (this->parameters.attack <= 0)
    {
        this->startHold();
    }
    else
    {
        this->segment = Segment::Attack;
        this->level = this->parameters.start / 100.0f;
        this->samplesUntilNextSegment = static_cast<int>(this->parameters.attack * this->sampleRate);
        this->slope = 1.0f / this->samplesUntilNextSegment;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startHold()
{
    if (this->parameters.hold <= 0)
    {
        this->level = 1.0;
        this->startDecay();
    }
    else
    {
        this->segment = Segment::Hold;
        this->samplesUntilNextSegment = static_cast<int>(this->parameters.hold * this->sampleRate);
        this->level = 1.0;
        this->slope = 0.0;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startDecay()
{
    if (this->parameters.decay <= 0)
    {
        this->startSustain();
    }
    else
    {
        this->segment = Segment::Decay;
        this->samplesUntilNextSegment = static_cast<int>(this->parameters.decay * this->sampleRate);
        this->level = 1.0;
        if (this->exponentialDecay)
        {
            // I don't truly understand this; just following what LinuxSampler does.
            float mysterySlope = -9.226f / this->samplesUntilNextSegment;
            this->slope = exp(mysterySlope);
            this->segmentIsExponential = true;
            if (this->parameters.sustain > 0.0)
            {
                // Again, this is following LinuxSampler's example, which is similar to
                // SF2-style decay, where "decay" specifies the time it would take to
                // get to zero, not to the sustain level.  The SFZ spec is not that
                // specific about what "decay" means, so perhaps it's really supposed
                // to specify the time to reach the sustain level.
                this->samplesUntilNextSegment = static_cast<int>(log((this->parameters.sustain / 100.0) / this->level) / mysterySlope);
                if (this->samplesUntilNextSegment <= 0)
                {
                    this->startSustain();
                }
            }
        }
        else
        {
            this->slope = (this->parameters.sustain / 100.0f - 1.0f) / this->samplesUntilNextSegment;
            this->segmentIsExponential = false;
        }
    }
}

void SoundFontEnvelope::startSustain()
{
    if (this->parameters.sustain <= 0)
    {
        this->startRelease();
    }
    else
    {
        this->segment = Segment::Sustain;
        this->level = this->parameters.sustain / 100.0f;
        this->slope = 0.0;
        this->samplesUntilNextSegment = 0x7FFFFFFF;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startRelease()
{
    float release = this->parameters.release;
    if (release <= 0)
    {
        // Enforce a short release, to prevent clicks.
        release = fastReleaseTime;
    }

    this->segment = Segment::Release;
    this->samplesUntilNextSegment = static_cast<int>(release * this->sampleRate);
    if (this->exponentialDecay)
    {
        // I don't truly understand this; just following what LinuxSampler does.
        const float mysterySlope = -9.226f / this->samplesUntilNextSegment;
        this->slope = exp(mysterySlope);
        this->segmentIsExponential = true;
    }
    else
    {
        this->slope = -this->level / this->samplesUntilNextSegment;
        this->segmentIsExponential = false;
    }
}

const float SoundFontEnvelope::BottomLevel = 0.001f;

//===----------------------------------------------------------------------===//
// SoundFontVoice
//===----------------------------------------------------------------------===//

class SoundFontVoice final : public SynthesiserVoice
{
public:

    SoundFontVoice()
    {
        this->envelope.setExponentialDecay(true);
    }

    bool canPlaySound(SynthesiserSound *sound) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void stopNoteForGroup();
    void stopNoteQuick();
    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override {}
    void renderNextBlock(AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override;
    bool isPlayingNoteDown();
    bool isPlayingOneShot();

    int getGroup();
    uint64 getOffBy();

    // Set the region to be used by the next startNote().
    void setRegion(SoundFontRegion *nextRegion);

    String infoString();

private:

    SoundFontRegion *region = nullptr;
    int trigger = 0;
    int currentMidiNote = 0;
    int currentPitchWheel = 0;
    double pitchRatio = 0.0;
    float noteGainLeft = 0.f;
    float noteGainRight = 0.f;
    double sourceSamplePosition = 0.0;
    SoundFontEnvelope envelope;
    int64 sampleEnd = 0;
    int64 loopStart = 0;
    int64 loopEnd = 0;

    // Info only.
    int numLoops = 0;
    int currentVelocity = 0;

    void calcPitchRatio();
    void killNote();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontVoice)
};

static const float globalGain = -1.0;

bool SoundFontVoice::canPlaySound(SynthesiserSound *sound)
{
    return dynamic_cast<SoundFontSound *>(sound) != nullptr;
}

void SoundFontVoice::startNote(int midiNoteNumber, float floatVelocity, SynthesiserSound *soundIn,
    int currentPitchWheelPosition)
{
    const auto *sound = dynamic_cast<SoundFontSound *>(soundIn);

    if (sound == nullptr)
    {
        this->killNote();
        return;
    }

    const int velocity = static_cast<int>(floatVelocity * 127.0);
    this->currentVelocity = velocity;
    if (this->region == nullptr)
    {
        this->region = sound->getRegionFor(midiNoteNumber, velocity);
    }

    if ((this->region == nullptr) ||
        (this->region->sample == nullptr) ||
        (this->region->sample->getBuffer() == nullptr))
    {
        this->killNote();
        return;
    }

    if (this->region->negativeEnd)
    {
        this->killNote();
        return;
    }

    // Pitch.
    this->currentMidiNote = midiNoteNumber;
    this->currentPitchWheel = currentPitchWheelPosition;
    this->calcPitchRatio();

    // Gain.
    double noteGainDB = globalGain + this->region->volume;
    // Thanks to <http:://www.drealm.info/sfz/plj-sfz.xhtml> for explaining the
    // velocity curve in a way that I could understand, although they mean
    // "log10" when they say "log".
    double velocityGainDB = -20.0 * log10((127.0 * 127.0) / (velocity * velocity));
    velocityGainDB *= this->region->ampVelTrack / 100.0;
    noteGainDB += velocityGainDB;
    this->noteGainLeft = this->noteGainRight = static_cast<float>(Decibels::decibelsToGain(noteGainDB));
    // The SFZ spec is silent about the pan curve, but a 3dB pan law seems
    // common.  This sqrt() curve matches what Dimension LE does; Alchemy Free
    // seems closer to sin(adjustedPan * pi/2).
    const double adjustedPan = (this->region->pan + 100.0) / 200.0;
    this->noteGainLeft *= static_cast<float>(sqrt(1.0 - adjustedPan));
    this->noteGainRight *= static_cast<float>(sqrt(adjustedPan));
    this->envelope.startNote(&this->region->ampeg, floatVelocity,
        this->getSampleRate(), &this->region->ampegVelTrack);

    // Offset/end.
    this->sourceSamplePosition = static_cast<double>(this->region->offset);
    this->sampleEnd = this->region->sample->getSampleLength();
    if ((this->region->end > 0) && (this->region->end < this->sampleEnd))
    {
        this->sampleEnd = this->region->end + 1;
    }

    // Loop.
    this->loopStart = this->loopEnd = 0;
    auto loopMode = this->region->loopMode;
    if (loopMode == SoundFontRegion::LoopMode::sampleLoop)
    {
        if (this->region->sample->getLoopStart() < this->region->sample->getLoopEnd())
        {
            loopMode = SoundFontRegion::LoopMode::loopContinuous;
        }
        else
        {
            loopMode = SoundFontRegion::LoopMode::noLoop;
        }
    }
    if ((loopMode != SoundFontRegion::LoopMode::noLoop) && (loopMode != SoundFontRegion::LoopMode::oneShot))
    {
        if (this->region->loopStart < this->region->loopEnd)
        {
            this->loopStart = this->region->loopStart;
            this->loopEnd = this->region->loopEnd;
        }
        else
        {
            this->loopStart = this->region->sample->getLoopStart();
            this->loopEnd = this->region->sample->getLoopEnd();
        }
    }

    this->numLoops = 0;
}

void SoundFontVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (!allowTailOff || (this->region == nullptr))
    {
        this->killNote();
        return;
    }

    if (this->region->loopMode != SoundFontRegion::LoopMode::oneShot)
    {
        this->envelope.noteOff();
    }
    if (this->region->loopMode == SoundFontRegion::LoopMode::loopSustain)
    {
        // Continue playing, but stop looping.
        this->loopEnd = this->loopStart;
    }
}

void SoundFontVoice::stopNoteForGroup()
{
    if (this->region->offMode == SoundFontRegion::OffMode::fast)
    {
        this->envelope.fastRelease();
    }
    else
    {
        this->envelope.noteOff();
    }
}

void SoundFontVoice::stopNoteQuick()
{
    this->envelope.fastRelease();
}

void SoundFontVoice::pitchWheelMoved(int newValue)
{
    if (this->region == nullptr)
    {
        return;
    }

    this->currentPitchWheel = newValue;
    this->calcPitchRatio();
}

void SoundFontVoice::renderNextBlock(AudioSampleBuffer &outputBuffer, int startSample, int numSamples)
{
    if (this->region == nullptr)
    {
        return;
    }

    const auto *buffer = this->region->sample->getBuffer();
    if (buffer == nullptr)
    {
        jassertfalse;
        return;
    }

    const float *inL = buffer->getReadPointer(0, 0);
    const float *inR = buffer->getNumChannels() > 1 ? buffer->getReadPointer(1, 0) : nullptr;

    float *outL = outputBuffer.getWritePointer(0, startSample);
    float *outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

    const int bufferNumSamples = buffer->getNumSamples();

    // Cache some values, to give them at least some chance of ending up in registers.
    double sourceSamplePosition = this->sourceSamplePosition;
    float ampegGain = this->envelope.getLevel();
    float ampegSlope = this->envelope.getSlope();
    int samplesUntilNextAmpSegment = this->envelope.getSamplesUntilNextSegment();
    bool ampSegmentIsExponential = this->envelope.getSegmentIsExponential();

    const float loopStart = float(this->loopStart);
    const float loopEnd = float(this->loopEnd);
    const float sampleEnd = float(this->sampleEnd);

    while (--numSamples >= 0)
    {
        const int pos = int(sourceSamplePosition);
        jassert(pos >= 0 && pos < bufferNumSamples);
        const float alpha = float(sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        int nextPos = pos + 1;
        if ((loopStart < loopEnd) && (nextPos > loopEnd))
        {
            nextPos = int(loopStart);
        }

        // Simple linear interpolation with buffer overrun check
        const float nextL = nextPos < bufferNumSamples ? inL[nextPos] : inL[pos];
        const float nextR = inR ? (nextPos < bufferNumSamples ? inR[nextPos] : inR[pos]) : nextL;
        float l = (inL[pos] * invAlpha + nextL * alpha);
        float r = inR ? (inR[pos] * invAlpha + nextR * alpha) : l;

        const float gainLeft = this->noteGainLeft * ampegGain;
        const float gainRight = this->noteGainRight * ampegGain;
        l *= gainLeft;
        r *= gainRight;
        // Shouldn't we dither here?

        if (outR)
        {
            *outL++ += l;
            *outR++ += r;
        }
        else
        {
            *outL++ += (l + r) * 0.5f;
        }

        // Next sample.
        sourceSamplePosition += this->pitchRatio;
        if ((loopStart < loopEnd) && (sourceSamplePosition > loopEnd))
        {
            sourceSamplePosition = loopStart;
            this->numLoops += 1;
        }

        // Update EG.
        if (ampSegmentIsExponential)
        {
            ampegGain *= ampegSlope;
        }
        else
        {
            ampegGain += ampegSlope;
        }
        if (--samplesUntilNextAmpSegment < 0)
        {
            this->envelope.setLevel(ampegGain);
            this->envelope.nextSegment();
            ampegGain = this->envelope.getLevel();
            ampegSlope = this->envelope.getSlope();
            samplesUntilNextAmpSegment = this->envelope.getSamplesUntilNextSegment();
            ampSegmentIsExponential = this->envelope.getSegmentIsExponential();
        }

        if ((sourceSamplePosition >= sampleEnd) || this->envelope.isDone())
        {
            this->killNote();
            break;
        }
    }

    this->sourceSamplePosition = sourceSamplePosition;
    this->envelope.setLevel(ampegGain);
    this->envelope.setSamplesUntilNextSegment(samplesUntilNextAmpSegment);
}

bool SoundFontVoice::isPlayingNoteDown()
{
    return this->region && this->region->trigger != SoundFontRegion::Trigger::release;
}

bool SoundFontVoice::isPlayingOneShot()
{
    return this->region && this->region->loopMode == SoundFontRegion::LoopMode::oneShot;
}

int SoundFontVoice::getGroup()
{
    return this->region ? this->region->group : 0;
}

uint64 SoundFontVoice::getOffBy()
{
    return this->region ? this->region->offBy : 0;
}

void SoundFontVoice::setRegion(SoundFontRegion *nextRegion)
{
    this->region = nextRegion;
}

String SoundFontVoice::infoString()
{
    const char *egSegmentNames[] = {"delay", "attack", "hold", "decay", "sustain", "release", "done"};

    const static int numEGSegments(sizeof(egSegmentNames) / sizeof(egSegmentNames[0]));

    const char *egSegmentName = "-Invalid-";
    int egSegmentIndex = this->envelope.segmentIndex();
    if ((egSegmentIndex >= 0) && (egSegmentIndex < numEGSegments))
    {
        egSegmentName = egSegmentNames[egSegmentIndex];
    }

    String info;
    info << "note: " << this->currentMidiNote << ", vel: " << this->currentVelocity
         << ", pan: " << this->region->pan << ", eg: " << egSegmentName
         << ", loops: " << this->numLoops;

    return info;
}

void SoundFontVoice::calcPitchRatio()
{
    double note = this->currentMidiNote;

    note += this->region->transpose;
    note += this->region->tune / 100.0;

    double adjustedPitch = this->region->pitchKeyCenter +
                           (note - this->region->pitchKeyCenter) * (this->region->pitchKeyTrack / 100.0);

    if (this->currentPitchWheel != 8192)
    {
        double wheel = ((2.0 * this->currentPitchWheel / 16383.0) - 1.0);
        if (wheel > 0)
        {
            adjustedPitch += wheel * this->region->bendUp / 100.0;
        }
        else
        {
            adjustedPitch += wheel * this->region->bendDown / -100.0;
        }
    }

    // todo someday: support equal temperaments like the default synth does?
    const auto fractionalMidiNoteInHz = [](double note, double freqOfA = 440.0) {
        // Like MidiMessage::getMidiNoteInHertz(), but with a float note.
        note -= 69;
        // Now 0 = A
        return freqOfA * pow(2.0, note / 12.0);
    };

    const double targetFreq = fractionalMidiNoteInHz(adjustedPitch);
    const double naturalFreq = MidiMessage::getMidiNoteInHertz(this->region->pitchKeyCenter);
    this->pitchRatio = (targetFreq * this->region->sample->getSampleRate()) / (naturalFreq * getSampleRate());
}

void SoundFontVoice::killNote()
{
    this->region = nullptr;
    this->clearCurrentNote();
}

//===----------------------------------------------------------------------===//
// SoundFontSynth
//===----------------------------------------------------------------------===//

void SoundFontSynth::initSynth(const Parameters &parameters)
{
    const File file(parameters.filePath);
    if (!file.existsAsFile())
    {
        return;
    }

    ScopedLock locker(this->lock);

    this->allNotesOff(0, false);

    this->clearVoices();
    for (int i = SoundFontSynth::numVoices; i --> 0 ;)
    {
        this->addVoice(new SoundFontVoice());
    }

    this->clearSounds();

    AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats();

    if (file.getFullPathName().endsWithIgnoreCase("sf2"))
    {
        auto sound = make<SoundFont2Sound>(file);
        sound->loadRegions();
        sound->loadSamples(audioFormatManager);
        this->sounds.add(sound.release());
    }
    else if (file.getFullPathName().endsWithIgnoreCase("sbk"))
    {
        auto sound = make<SoundFontSound>(file);
        sound->loadRegions();
        sound->loadSamples(audioFormatManager);
        this->sounds.add(sound.release());
    }

    // new file has been loaded so we need to set the program anyway
    jassert(parameters.programIndex < this->getNumPrograms());
    this->setCurrentProgram(parameters.programIndex);
}

void SoundFontSynth::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    int i;

    const ScopedLock locker(this->lock);

    int midiVelocity = static_cast<int>(velocity * 127);

    // First, stop any currently-playing sounds in the group.
    // Currently, this only pays attention to the first matching region.
    int group = 0;
    auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get());
    if (sound != nullptr)
    {
        if (auto *region = sound->getRegionFor(midiNoteNumber, midiVelocity))
        {
            group = region->group;
        }
    }

    if (group != 0)
    {
        for (auto *v : this->voices)
        {
            jassert(dynamic_cast<SoundFontVoice *>(v));
            auto *voice = static_cast<SoundFontVoice *>(v);
            if (voice->getOffBy() == group)
            {
                voice->stopNoteForGroup();
            }
        }
    }

    // Are any notes playing?  (Needed for first/legato trigger handling.)
    // Also stop any voices still playing this note.
    bool anyNotesPlaying = false;
    for (auto *v : this->voices)
    {
        jassert(dynamic_cast<SoundFontVoice *>(v));
        auto *voice = static_cast<SoundFontVoice *>(v);

        if (voice->isPlayingChannel(midiChannel) && voice->isPlayingNoteDown())
        {
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
            {
                if (!voice->isPlayingOneShot())
                {
                    voice->stopNoteQuick();
                }
            }
            else
            {
                anyNotesPlaying = true;
            }
        }
    }

    // Play *all* matching regions.
    if (sound != nullptr)
    {
        const auto trigger = anyNotesPlaying ? SoundFontRegion::Trigger::legato : SoundFontRegion::Trigger::first;
        for (i = 0; i < sound->getNumRegions(); ++i)
        {
            auto *region = sound->regionAt(i);
            if (region->matches(midiNoteNumber, midiVelocity, trigger))
            {
                if (auto *voice = dynamic_cast<SoundFontVoice *>(this->findFreeVoice(sound,
                    midiNoteNumber, midiChannel, this->isNoteStealingEnabled())))
                {
                    // This check duplicates what the Synthesiser's startVoice method does,
                    // but we have to do it here, before assigning the region reference to the voice,
                    // because the stopNote method will end up resetting that region reference,
                    // and the voice will later fallback to picking the first matching region,
                    // so when this loop runs out of unused voices, it ends up not playing
                    // all matching regions while playing some of them twice:
                    if (voice->getCurrentlyPlayingSound() != nullptr)
                    {
                        voice->stopNote(0.0f, false);
                    }

                    voice->setRegion(region);
                    this->startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
                }
            }
        }
    }

    this->noteVelocities[midiNoteNumber] = midiVelocity;
}

void SoundFontSynth::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
{
    const ScopedLock locker(this->lock);

    Synthesiser::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);

    // Start release region.
    if (auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get()))
    {
        if (auto *region = sound->getRegionFor(midiNoteNumber,
            this->noteVelocities[midiNoteNumber], SoundFontRegion::Trigger::release))
        {
            if (auto *voice = dynamic_cast<SoundFontVoice *>(this->findFreeVoice(sound, midiNoteNumber, midiChannel, false)))
            {
                if (voice->getCurrentlyPlayingSound() != nullptr)
                {
                    voice->stopNote(0.0f, false);
                }

                // Synthesiser is too locked-down (ivars are private rt protected), so
                // we have to use a "setRegion()" mechanism.
                voice->setRegion(region);
                this->startVoice(voice, sound, midiChannel, midiNoteNumber, this->noteVelocities[midiNoteNumber] / 127.0f);
            }
        }
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

int SoundFontSynth::getNumPrograms() const
{
    if (auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get()))
    {
        return sound->getNumPresets();
    }

    return 0;
}

int SoundFontSynth::getCurrentProgram() const
{
    if (auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get()))
    {
        return sound->getSelectedPreset();
    }

    return 0;
}

void SoundFontSynth::setCurrentProgram(int index)
{
    if (auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get()))
    {
        return sound->setSelectedPreset(index < this->getNumPrograms() ? index : 0);
    }
}

const String SoundFontSynth::getProgramName(int index) const
{
    if (auto *sound = dynamic_cast<SoundFontSound *>(this->getSound(0).get()))
    {
        return sound->getPresetName(index);
    }

    return {};
}

void SoundFontSynth::changeProgramName(int index, const String &newName)
{
    jassertfalse;
}

//===----------------------------------------------------------------------===//
// Synth parameters
//===----------------------------------------------------------------------===//

SoundFontSynth::Parameters SoundFontSynth::Parameters::withSoundFontFile(const String &newFilePath) const noexcept
{
    Parameters other(*this);
    other.filePath = newFilePath;
    return other;
}

SoundFontSynth::Parameters SoundFontSynth::Parameters::withProgramIndex(int newProgramIndex) const noexcept
{
    Parameters other(*this);
    other.programIndex = newProgramIndex;
    return other;
}

SerializedData SoundFontSynth::Parameters::serialize() const
{
    using namespace Serialization::Audio;

    SerializedData data(SoundFont::soundFontConfig);
    data.setProperty(SoundFont::filePath, this->filePath);
    data.setProperty(SoundFont::programIndex, this->programIndex);

    return data;
}

void SoundFontSynth::Parameters::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization::Audio;

    const auto root = data.hasType(SoundFont::soundFontConfig) ?
        data : data.getChildWithName(SoundFont::soundFontConfig);

    if (!root.isValid())
    {
        return;
    }

    this->filePath = root.getProperty(SoundFont::filePath);
    this->programIndex = root.getProperty(SoundFont::programIndex);
}

void SoundFontSynth::Parameters::reset()
{
    this->filePath.clear();
    this->programIndex = 0;
}
