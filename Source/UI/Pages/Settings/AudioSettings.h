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

//[Headers]
class AudioCore;
#include "MobileComboBox.h"
//[/Headers]


class AudioSettings final : public Component
{
public:

    AudioSettings(AudioCore &core);
    ~AudioSettings();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void visibilityChanged() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    void applyDeviceType(AudioDeviceManager &deviceManager, const String &deviceTypeName);
    void applyDevice(AudioDeviceManager &deviceManager, const String &deviceName);
    void applySampleRate(AudioDeviceManager &deviceManager, double sampleRate);
    void applyBufferSize(AudioDeviceManager &deviceManager, int bufferSize);

    void syncDeviceTypesList(AudioDeviceManager &deviceManager);
    void syncDevicesList(AudioDeviceManager &deviceManager);
    void syncSampleRatesList(AudioDeviceManager &deviceManager);
    void syncBufferSizesList(AudioDeviceManager &deviceManager);

    AudioCore &audioCore;
    //[/UserVariables]

    ScopedPointer<MobileComboBox::Primer> sampleRateComboPrimer;
    ScopedPointer<MobileComboBox::Primer> bufferSizeComboPrimer;
    ScopedPointer<MobileComboBox::Primer> deviceTypeComboPrimer;
    ScopedPointer<MobileComboBox::Primer> deviceComboPrimer;
    ScopedPointer<TextEditor> deviceTypeEditor;
    ScopedPointer<TextEditor> deviceEditor;
    ScopedPointer<TextEditor> sampleRateEditor;
    ScopedPointer<TextEditor> bufferSizeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSettings)
};
