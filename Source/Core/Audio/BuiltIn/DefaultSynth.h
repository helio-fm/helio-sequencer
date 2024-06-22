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

        Temperament::Ptr temperament;

        ADSR adsr;
        Reverb reverb;
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
