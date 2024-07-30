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

#include "Common.h"
#include "SoundFontSynth.h"
#include "SoundFontSound.h"
#include "SoundFont2Sound.h"
#include "SoundFontRegion.h"
#include "SoundFontSample.h"
#include "KeyboardMapping.h"
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

    static constexpr float bottomLevel = 0.001f;
    static constexpr float fastReleaseTime = 0.01f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontEnvelope)
};

void SoundFontEnvelope::setExponentialDecay(bool newExponentialDecay)
{
    this->exponentialDecay = newExponentialDecay;
}

void SoundFontEnvelope::startNote(const EGParameters *newParameters,
    float floatVelocity, double newSampleRate, const EGParameters *velMod)
{
    this->parameters = *newParameters;
    if (velMod)
    {
        this->parameters.delay += floatVelocity * velMod->delay;
        this->parameters.attack += floatVelocity * velMod->attack;
        this->parameters.hold += floatVelocity * velMod->hold;
        this->parameters.decay += floatVelocity * velMod->decay;
        this->parameters.sustain += floatVelocity * velMod->sustain;
        this->parameters.sustain = jlimit(0.f, 100.f, this->parameters.sustain);
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
    this->samplesUntilNextSegment = int(SoundFontEnvelope::fastReleaseTime * this->sampleRate);
    this->slope = -this->level / this->samplesUntilNextSegment;
    this->segmentIsExponential = false;
}

void SoundFontEnvelope::startDelay()
{
    if (this->parameters.delay <= 0.f)
    {
        this->startAttack();
    }
    else
    {
        this->segment = Segment::Delay;
        this->level = 0.f;
        this->slope = 0.f;
        this->samplesUntilNextSegment = int(this->parameters.delay * this->sampleRate);
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startAttack()
{
    if (this->parameters.attack <= 0.f)
    {
        this->startHold();
    }
    else
    {
        this->segment = Segment::Attack;
        this->level = jlimit(0.f, 1.f, this->parameters.start / 100.f);
        this->samplesUntilNextSegment = static_cast<int>(this->parameters.attack * this->sampleRate);
        this->slope = (1.f - this->level) / this->samplesUntilNextSegment;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startHold()
{
    if (this->parameters.hold <= 0.f)
    {
        this->level = 1.f;
        this->startDecay();
    }
    else
    {
        this->segment = Segment::Hold;
        this->samplesUntilNextSegment = static_cast<int>(this->parameters.hold * this->sampleRate);
        this->level = 1.f;
        this->slope = 0.f;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startDecay()
{
    if (this->parameters.decay <= 0.f)
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
            const float mysterySlope = -9.226f / this->samplesUntilNextSegment;
            this->slope = exp(mysterySlope);
            this->segmentIsExponential = true;
            if (this->parameters.sustain > 0.f)
            {
                // Again, this is following LinuxSampler's example, which is similar to
                // SF2-style decay, where "decay" specifies the time it would take to
                // get to zero, not to the sustain level.  The SFZ spec is not that
                // specific about what "decay" means, so perhaps it's really supposed
                // to specify the time to reach the sustain level.
                this->samplesUntilNextSegment = int(log((this->parameters.sustain / 100.f) / this->level) / mysterySlope);
                if (this->samplesUntilNextSegment <= 0)
                {
                    this->startSustain();
                }
            }
        }
        else
        {
            this->slope = (jmax(SoundFontEnvelope::bottomLevel, this->parameters.sustain) / 100.f - 1.f) / this->samplesUntilNextSegment;
            this->segmentIsExponential = false;
        }
    }
}

void SoundFontEnvelope::startSustain()
{
    if (this->parameters.sustain <= 0.f)
    {
        this->startRelease();
    }
    else
    {
        this->segment = Segment::Sustain;
        this->level = this->parameters.sustain / 100.f;
        this->slope = 0.f;
        this->samplesUntilNextSegment = 0x7FFFFFFF;
        this->segmentIsExponential = false;
    }
}

void SoundFontEnvelope::startRelease()
{
    // Pin to short release to prevent clicks
    const float release = jmax(this->parameters.release, SoundFontEnvelope::fastReleaseTime);

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

    void setTemperament(Temperament::Ptr temperament) noexcept
    {
        this->temperament = temperament;
    }

    bool canPlaySound(SynthesiserSound *sound) override;
    void startNote(int midiNoteNumber, float velocity,
        SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void stopNoteForGroup();
    void stopNoteQuick();
    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override {}
    void renderNextBlock(AudioSampleBuffer &outputBuffer,
        int startSample, int numSamples) override;
    bool isPlayingNoteDown();
    bool isPlayingOneShot();

    int getGroup();
    uint64 getOffBy();

    // Set the region to be used by the next startNote().
    void setRegion(SoundFontRegion *nextRegion);

private:

    SoundFontRegion *region = nullptr;

    Temperament::Ptr temperament;

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

    static constexpr float globalGainDB = -0.1f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontVoice)
};

bool SoundFontVoice::canPlaySound(SynthesiserSound *sound)
{
    return dynamic_cast<SoundFontSound *>(sound) != nullptr;
}

void SoundFontVoice::startNote(int midiNoteNumber, float floatVelocity,
    SynthesiserSound *soundIn, int currentPitchWheelPosition)
{
    if (this->temperament == nullptr)
    {
        jassertfalse;
        return;
    }

    const auto *sound = dynamic_cast<SoundFontSound *>(soundIn);
    if (sound == nullptr)
    {
        this->killNote();
        return;
    }

    const int actualNoteNumber =
        this->temperament->unmapMicrotonalNote(midiNoteNumber,
            this->getCurrentPlayingChannel());

    const int velocity = int(floatVelocity * 127.f);
    this->currentVelocity = velocity;
    if (this->region == nullptr)
    {
        this->region = sound->getRegionFor(actualNoteNumber, velocity);
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
    this->currentMidiNote = actualNoteNumber;
    this->currentPitchWheel = currentPitchWheelPosition;
    this->calcPitchRatio();

    // Gain.
    double noteGainDB = SoundFontVoice::globalGainDB + this->region->volume;
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

void SoundFontVoice::calcPitchRatio()
{
    if (this->temperament == nullptr)
    {
        jassertfalse;
        return;
    }

    double note = this->currentMidiNote;
    note += this->temperament->getEquivalentOfTwelveToneInterval(Semitones(this->region->transpose));
    note += this->region->tune / 100.0;

    const double adjustedPitchKeyCenter =
        double(this->region->pitchKeyCenter * this->temperament->getPeriodSize()) /
            double(Globals::twelveTonePeriodSize); 

    double adjustedPitch = adjustedPitchKeyCenter +
        (note - adjustedPitchKeyCenter) * (this->region->pitchKeyTrack / 100.0);

    if (this->currentPitchWheel != 8192)
    {
        const double wheel = ((2.0 * this->currentPitchWheel / 16383.0) - 1.0);
        if (wheel > 0)
        {
            adjustedPitch += wheel * this->region->bendUp / 100.0;
        }
        else
        {
            adjustedPitch += wheel * this->region->bendDown / -100.0;
        }
    }

    const double targetFreq = this->temperament->getNoteInHertz(adjustedPitch);
    const double naturalFreq = MidiMessage::getMidiNoteInHertz(this->region->pitchKeyCenter);
    this->pitchRatio =
        (targetFreq * this->region->sample->getSampleRate()) /
        (naturalFreq * this->getSampleRate());
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

    const ScopedLock locker(this->lock);

    this->allNotesOff(0, false);

    this->clearVoices();
    for (int i = SoundFontSynth::numVoices; i --> 0 ;)
    {
        auto voice = make<SoundFontVoice>();
        voice->setTemperament(this->temperament);
        this->addVoice(voice.release());
    }

    this->clearSounds();

    AudioFormatManager audioFormatManager;
    audioFormatManager.registerBasicFormats();

    const auto fullPath = file.getFullPathName();
    if (fullPath.endsWithIgnoreCase("sf2"))
    {
        auto sound = make<SoundFont2Sound>(file);
        sound->loadRegions();
        sound->loadSamples(audioFormatManager);
        sound->setTemperament(this->temperament);
        this->sounds.add(sound.release());
    }
    else if (fullPath.endsWithIgnoreCase("sf3") || fullPath.endsWithIgnoreCase("sf4"))
    {
        auto sound = make<SoundFont3Sound>(file);
        sound->loadRegions();
        sound->loadSamples(audioFormatManager);
        sound->setTemperament(this->temperament);
        this->sounds.add(sound.release());
    }
    else if (fullPath.endsWithIgnoreCase("sbk"))
    {
        auto sound = make<SoundFontSound>(file);
        sound->loadRegions();
        sound->loadSamples(audioFormatManager);
        sound->setTemperament(this->temperament);
        this->sounds.add(sound.release());
    }

    // new file has been loaded so we need to set the program anyway
    jassert(parameters.programIndex < this->getNumPrograms());
    this->setCurrentProgram(parameters.programIndex);
}

void SoundFontSynth::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    const ScopedLock locker(this->lock);

    if (this->temperament == nullptr)
    {
        jassertfalse;
        return;
    }

    const int actualNoteNumber =
        this->temperament->unmapMicrotonalNote(midiNoteNumber, midiChannel);

    const int midiVelocity = int(velocity * 127.f);

    // First, stop any currently-playing sounds in the group.
    // Currently, this only pays attention to the first matching region.
    int group = 0;
    auto *sound = this->getSoundFontSound();
    if (sound != nullptr)
    {
        if (auto *region = sound->getRegionFor(actualNoteNumber, midiVelocity))
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
        const auto periodSize = this->temperament->getPeriodSize();
        const auto trigger = anyNotesPlaying ?
            SoundFontRegion::Trigger::legato : SoundFontRegion::Trigger::first;

        for (int i = 0; i < sound->getNumRegions(); ++i)
        {
            auto *region = sound->regionAt(i);
            if (region->matches(actualNoteNumber, midiVelocity, trigger, periodSize))
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
                        voice->stopNote(0.f, false);
                    }

                    voice->setRegion(region);
                    this->startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
                }
            }
        }
    }

    this->noteVelocities[actualNoteNumber] = midiVelocity;
}

void SoundFontSynth::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
{
    const ScopedLock locker(this->lock);

    Synthesiser::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);

    // Start release region.
    if (auto *sound = this->getSoundFontSound())
    {
        if (this->temperament == nullptr)
        {
            jassertfalse;
            return;
        }

        const int actualNoteNumber =
            this->temperament->unmapMicrotonalNote(midiNoteNumber, midiChannel);

        if (auto *region = sound->getRegionFor(actualNoteNumber,
            this->noteVelocities[actualNoteNumber], SoundFontRegion::Trigger::release))
        {
            if (auto *voice = dynamic_cast<SoundFontVoice *>(this->findFreeVoice(sound, midiNoteNumber, midiChannel, false)))
            {
                if (voice->getCurrentlyPlayingSound() != nullptr)
                {
                    voice->stopNote(0.f, false);
                }

                // Synthesiser is too locked-down (ivars are private rt protected), so
                // we have to use a "setRegion()" mechanism.
                voice->setRegion(region);
                this->startVoice(voice, sound, midiChannel, midiNoteNumber,
                    this->noteVelocities[actualNoteNumber] / 127.f);
            }
        }
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

int SoundFontSynth::getNumPrograms() const
{
    if (auto *sound = this->getSoundFontSound())
    {
        return sound->getNumPresets();
    }

    return 0;
}

int SoundFontSynth::getCurrentProgram() const
{
    if (auto *sound = this->getSoundFontSound())
    {
        return sound->getSelectedPreset();
    }

    return 0;
}

void SoundFontSynth::setCurrentProgram(int index)
{
    if (auto *sound = this->getSoundFontSound())
    {
        return sound->setSelectedPreset(index < this->getNumPrograms() ? index : 0);
    }
}

const String SoundFontSynth::getProgramName(int index) const
{
    if (auto *sound = this->getSoundFontSound())
    {
        return sound->getPresetName(index);
    }

    return {};
}

void SoundFontSynth::changeProgramName(int index, const String &newName)
{
    jassertfalse;
}

void SoundFontSynth::setTemperament(Temperament::Ptr temperament)
{
    if (this->temperament == temperament)
    {
        return;
    }

    this->temperament = temperament;

    for (auto *v : this->voices)
    {
        jassert(dynamic_cast<SoundFontVoice *>(v));
        auto *voice = static_cast<SoundFontVoice *>(v);
        voice->stopNote(0.f, false);
        voice->setTemperament(temperament);
    }

    if (auto *sound = this->getSoundFontSound())
    {
        sound->setTemperament(temperament);
    }
}

SoundFontSound *SoundFontSynth::getSoundFontSound() const noexcept
{
    if (this->getNumSounds() == 0)
    {
        return nullptr;
    }

    jassert(dynamic_cast<SoundFontSound *>(this->getSound(0).get()));
    return static_cast<SoundFontSound *>(this->getSound(0).get());
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
