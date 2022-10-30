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

#pragma once

class SoundFontSynth final : public Synthesiser
{
public:

    SoundFontSynth() = default;

    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override;

    //===------------------------------------------------------------------===//
    // Synth parameters
    //===------------------------------------------------------------------===//

    struct Parameters final : Serializable
    {
        String filePath;
        int programIndex = 0;

        Parameters withSoundFontFile(const String &newFilePath) const noexcept;
        Parameters withProgramIndex(int newProgramIndex) const noexcept;

        SerializedData serialize() const override;
        void deserialize(const SerializedData &data) override;
        void reset() override;
    };

    void initSynth(const Parameters &parameters);

    //===------------------------------------------------------------------===//
    // Presets
    //===------------------------------------------------------------------===//

    int getNumPrograms() const;
    int getCurrentProgram() const;
    void setCurrentProgram(int index);
    const String getProgramName(int index) const;
    void changeProgramName(int index, const String &newName);

private:

    static constexpr auto numVoices = 16;

    int noteVelocities[128];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontSynth)
};
