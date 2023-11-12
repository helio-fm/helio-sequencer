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
    void setPeriodSizeAndRange(int periodSize, double periodRange);

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

        void setPeriodSize(int size) noexcept;
        void setPeriodRange(double periodRange) noexcept;
        void setFrequency(double frequency) noexcept;

        inline float getNextSample() noexcept
        {
            const auto index0 = int(this->currentIndex);
            // the table is 1 sample larger than DefaultSynth::tableSize
            // so we don't have to do another bounds check here:
            const auto index1 = index0 + 1;
 
            const auto frac = this->currentIndex - float(index0);
 
            const auto *table = DefaultSynth::waveTable.getReadPointer(0);
            const auto value0 = table[index0];
            const auto value1 = table[index1];
 
            const auto currentSample = value0 + frac * (value1 - value0);
 
            if ((this->currentIndex += this->waveTableDelta) > DefaultSynth::tableSize)
            {
                this->currentIndex -= DefaultSynth::tableSize;
            }
 
            return currentSample;
        }

    private:

        float currentIndex = 0.f;
        float waveTableDelta = 0.f;
        double level = 0.0;

        int periodSize = Globals::twelveTonePeriodSize;
        double periodRange = 2.0;
        int middleC = Temperament::periodNumForMiddleC * Globals::twelveTonePeriodSize;

        ADSR adsr;
        Reverb reverb;

        double getNoteInHertz(int noteNumber, double frequencyOfA = 440.0) noexcept;
        int getCurrentChannel() const noexcept;
    };

    void handleSustainPedal(int midiChannel, bool isDown) override;
    void handleSostenutoPedal(int midiChannel, bool isDown) override;

#if PLATFORM_DESKTOP
    static constexpr auto numVoices = 32;
#elif PLATFORM_MOBILE
    static constexpr auto numVoices = 16;
#endif

    static AudioSampleBuffer waveTable;

#if PLATFORM_DESKTOP
    static constexpr auto tableSize = 512;
#elif PLATFORM_MOBILE
    static constexpr auto tableSize = 256;
#endif

    void initWaveTable();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DefaultSynth)
};
