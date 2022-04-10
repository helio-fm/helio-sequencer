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

#include "MobileComboBox.h"

class AudioCore;

class AudioSettings final : public Component
{
public:

    explicit AudioSettings(AudioCore &core);
    ~AudioSettings() override;

    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    void applyDeviceType(AudioDeviceManager &deviceManager, const String &deviceTypeName);
    void applyDevice(AudioDeviceManager &deviceManager, const String &deviceName);
    void applySampleRate(AudioDeviceManager &deviceManager, double sampleRate);
    void applyBufferSize(AudioDeviceManager &deviceManager, int bufferSize);
    void applyMidiInput(AudioDeviceManager &deviceManager, const String &deviceId);
    void applyMidiOutput(AudioDeviceManager &deviceManager, const String &deviceId);

    void syncDeviceTypesList(AudioDeviceManager &deviceManager);
    void syncDevicesList(AudioDeviceManager &deviceManager);
    void syncSampleRatesList(AudioDeviceManager &deviceManager);
    void syncBufferSizesList(AudioDeviceManager &deviceManager);
    void syncMidiInputsList(AudioDeviceManager &deviceManager);
    void syncMidiOutputsList(AudioDeviceManager &deviceManager);

    AudioCore &audioCore;

    UniquePointer<MobileComboBox::Container> midiInputsCombo;
    UniquePointer<MobileComboBox::Container> midiOutputsCombo;
    UniquePointer<MobileComboBox::Container> sampleRateCombo;
    UniquePointer<MobileComboBox::Container> bufferSizeCombo;
    UniquePointer<MobileComboBox::Container> deviceTypeCombo;
    UniquePointer<MobileComboBox::Container> deviceCombo;
    UniquePointer<TextEditor> deviceTypeEditor;
    UniquePointer<TextEditor> deviceEditor;
    UniquePointer<TextEditor> sampleRateEditor;
    UniquePointer<TextEditor> bufferSizeEditor;
    UniquePointer<TextEditor> midiInputEditor;
    UniquePointer<TextEditor> midiOutputEditor;
    UniquePointer<ToggleButton> midiInputRemappingCheckbox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettings)
};
