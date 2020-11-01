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
    
};
