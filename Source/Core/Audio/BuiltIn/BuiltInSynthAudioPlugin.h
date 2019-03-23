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

#define BUILTIN_SYNTH_NUM_VOICES 32

class BuiltInSynthAudioPlugin : public AudioPluginInstance
{
public:

    BuiltInSynthAudioPlugin() = default;

    //===------------------------------------------------------------------===//
    // AudioPluginInstance
    //===------------------------------------------------------------------===//

    void fillInPluginDescription(PluginDescription &description) const override;

    //===------------------------------------------------------------------===//
    // AudioProcessor
    //===------------------------------------------------------------------===//

    void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages) override;
    void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) override;
    void releaseResources() override;
    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;

    //===------------------------------------------------------------------===//
    // Editor
    //===------------------------------------------------------------------===//

    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;
    
    //===------------------------------------------------------------------===//
    // Programs
    //===------------------------------------------------------------------===//

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String &newName) override;

    //===------------------------------------------------------------------===//
    // State
    //===------------------------------------------------------------------===//

    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

protected:

    virtual void initVoices() = 0;
    virtual void initSampler() = 0;

    Synthesiser synth;

};
