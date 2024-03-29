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

#pragma once

#include "Temperament.h"

class DefaultSynth final : public Synthesiser
{
public:

    DefaultSynth();

    // what we want here is to make all built-in temperaments
    // work out of the box with the built-in instrument, so that
    // all features are easily previewed even before the user
    // sets up any instruments and keyboard mappings;
    // for that we need to let the instrument know which temperament
    // the project is currently in, and even simpler - we only need
    // an octave size, because this is what matters from the sequencer's
    // perspective, and we will only support EDOs, because all built-in
    // temperaments and EDO's; hopefully someday I'll come up with
    // a better approach, or just get rid of this hack;
    void setTemperament(Temperament::Ptr temperament);

protected:

    struct Sound final : public SynthesiserSound
    {
        bool appliesToNote(int midiNoteNumber) override { return true; }
        bool appliesToChannel(int midiChannel) override { return true; }
    };

    class Voice final : public SynthesiserVoice
    {
    public:

        Voice();

        bool canPlaySound(SynthesiserSound *) override;
        void setCurrentPlaybackSampleRate(double sampleRate) override;
        void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int) override;
        void stopNote(float, bool allowTailOff) override;
        bool isVoiceActive() const override;
        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}
        void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

        using SynthesiserVoice::renderNextBlock;

        void setTemperament(Temperament::Ptr temperament) noexcept;

    private:

        float currentAngle = 0.f;
        float angleDelta = 0.f;
        float level = 0.f;

        int periodSize = Globals::twelveTonePeriodSize;
        double periodRange = 2.0;
        int middleA = 69;

        ADSR adsr;
        Reverb reverb;

        double getNoteInHertz(int noteNumber, double frequencyOfA = 440.0) noexcept;
        int getCurrentChannel() const noexcept;
    };

    void handleSustainPedal(int midiChannel, bool isDown) override;
    void handleSostenutoPedal(int midiChannel, bool isDown) override;

#if PLATFORM_DESKTOP
    static constexpr auto numVoices = 16;
#elif PLATFORM_MOBILE
    static constexpr auto numVoices = 8;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DefaultSynth)
};
