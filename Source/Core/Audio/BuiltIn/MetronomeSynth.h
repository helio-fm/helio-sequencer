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

#pragma once

#include "Note.h"
#include "Meter.h"

class MetronomeSynth final : public Synthesiser
{
public:

    MetronomeSynth() = default;

    struct SamplerParameters final : Serializable
    {
        FlatHashMap<MetronomeScheme::Syllable,
            String, MetronomeScheme::SyllableHash> customSamples;

        SerializedData serialize() const override;
        void deserialize(const SerializedData &data) override;
        void reset() override;
    };

    void initVoices();
    void initSampler(const SamplerParameters &params);

    static Note::Key getKeyForSyllable(MetronomeScheme::Syllable syllable);

protected:

    void handleSustainPedal(int midiChannel, bool isDown) override;
    void handleSostenutoPedal(int midiChannel, bool isDown) override;

    static constexpr auto numVoices = 16;

    struct TickSample final
    {
        static constexpr auto attackTime = 0.0;
        static constexpr auto releaseTime = 0.5;
        static constexpr auto maxPlaybackTime = 4.5;

        TickSample() = delete;
        TickSample(int rootKey, const char *sourceData, int sourceDataSize);
        TickSample(int rootKey, const String &customSample);

        AudioFormatReader *createReader();

        const char *sourceData = nullptr;
        const int sourceDataSize = 0;

        const String customSamplePath;

        BigInteger midiNotes;
        const int midiNoteForNormalPitch = 0;

        JUCE_LEAK_DETECTOR(TickSample)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomeSynth)
};
