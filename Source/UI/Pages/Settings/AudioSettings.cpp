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

//[Headers]
#include "Common.h"
//[/Headers]

#include "AudioSettings.h"

//[MiscUserDefs]
#include "AudioCore.h"
//[/MiscUserDefs]

AudioSettings::AudioSettings(AudioCore &core)
    : audioCore(core)
{
    addAndMakeVisible(sampleRateComboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible(bufferSizeComboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible(deviceTypeComboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible(deviceComboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible(deviceTypeEditor = new TextEditor(String()));
    deviceTypeEditor->setMultiLine(false);
    deviceTypeEditor->setReturnKeyStartsNewLine(false);
    deviceTypeEditor->setReadOnly(true);
    deviceTypeEditor->setScrollbarsShown(false);
    deviceTypeEditor->setCaretVisible(false);
    deviceTypeEditor->setPopupMenuEnabled(false);
    deviceTypeEditor->setText(String());

    addAndMakeVisible(deviceEditor = new TextEditor(String()));
    deviceEditor->setMultiLine(false);
    deviceEditor->setReturnKeyStartsNewLine(false);
    deviceEditor->setReadOnly(true);
    deviceEditor->setScrollbarsShown(false);
    deviceEditor->setCaretVisible(false);
    deviceEditor->setPopupMenuEnabled(false);
    deviceEditor->setText(String());

    addAndMakeVisible(sampleRateEditor = new TextEditor(String()));
    sampleRateEditor->setMultiLine(false);
    sampleRateEditor->setReturnKeyStartsNewLine(false);
    sampleRateEditor->setReadOnly(true);
    sampleRateEditor->setScrollbarsShown(false);
    sampleRateEditor->setCaretVisible(false);
    sampleRateEditor->setPopupMenuEnabled(false);
    sampleRateEditor->setText(String());

    addAndMakeVisible(bufferSizeEditor = new TextEditor(String()));
    bufferSizeEditor->setMultiLine(false);
    bufferSizeEditor->setReturnKeyStartsNewLine(false);
    bufferSizeEditor->setReadOnly(true);
    bufferSizeEditor->setScrollbarsShown(false);
    bufferSizeEditor->setCaretVisible(false);
    bufferSizeEditor->setPopupMenuEnabled(false);
    bufferSizeEditor->setText(String());


    //[UserPreSize]
    //[/UserPreSize]

    setSize(550, 200);

    //[Constructor]
    this->deviceTypeEditor->setFont(18.f);
    this->deviceEditor->setFont(18.f);
    this->sampleRateEditor->setFont(18.f);
    this->bufferSizeEditor->setFont(18.f);

    this->deviceTypeEditor->setInterceptsMouseClicks(false, true);
    this->deviceEditor->setInterceptsMouseClicks(false, true);
    this->sampleRateEditor->setInterceptsMouseClicks(false, true);
    this->bufferSizeEditor->setInterceptsMouseClicks(false, true);

    MenuPanel::Menu emptyMenu;
    this->deviceTypeComboPrimer->initWith(this->deviceTypeEditor.get(), emptyMenu);
    this->deviceComboPrimer->initWith(this->deviceEditor.get(), emptyMenu);
    this->sampleRateComboPrimer->initWith(this->sampleRateEditor.get(), emptyMenu);
    this->bufferSizeComboPrimer->initWith(this->bufferSizeEditor.get(), emptyMenu);
    //[/Constructor]
}

AudioSettings::~AudioSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    sampleRateComboPrimer = nullptr;
    bufferSizeComboPrimer = nullptr;
    deviceTypeComboPrimer = nullptr;
    deviceComboPrimer = nullptr;
    deviceTypeEditor = nullptr;
    deviceEditor = nullptr;
    sampleRateEditor = nullptr;
    bufferSizeEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AudioSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    g.fillAll (Colours::black);

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void AudioSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    sampleRateComboPrimer->setBounds (4, 4, getWidth() - 8, getHeight() - 8);
    bufferSizeComboPrimer->setBounds (4, 4, getWidth() - 8, getHeight() - 8);
    deviceTypeComboPrimer->setBounds (4, 4, getWidth() - 8, getHeight() - 8);
    deviceComboPrimer->setBounds (4, 4, getWidth() - 8, getHeight() - 8);
    deviceTypeEditor->setBounds (16, 12, getWidth() - 32, 32);
    deviceEditor->setBounds (16, 60, getWidth() - 32, 32);
    sampleRateEditor->setBounds (16, 108, getWidth() - 32, 32);
    bufferSizeEditor->setBounds (16, 156, getWidth() - 32, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioSettings::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isVisible())
    {
        AudioDeviceManager &deviceManager = this->audioCore.getDevice();

        this->syncDeviceTypesList(deviceManager);
        this->syncDevicesList(deviceManager);
        this->syncSampleRatesList(deviceManager);
        this->syncBufferSizesList(deviceManager);
    }
    //[/UserCode_visibilityChanged]
}

void AudioSettings::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    AudioDeviceManager &deviceManager = this->audioCore.getDevice();
    const OwnedArray<AudioIODeviceType> &deviceTypes = deviceManager.getAvailableDeviceTypes();

    const int deviceTypeIndex = commandId - CommandIDs::SelectAudioDeviceType;
    if (deviceTypeIndex >= 0 && deviceTypeIndex < deviceTypes.size())
    {
        this->applyDeviceType(deviceManager, deviceTypes[deviceTypeIndex]->getTypeName());
        return;
    }

    if (const auto currentType = deviceManager.getCurrentDeviceTypeObject())
    {
        const StringArray &deviceNames = currentType->getDeviceNames();
        const int deviceIndex = commandId - CommandIDs::SelectAudioDevice;
        if (deviceIndex >= 0 && deviceIndex < deviceNames.size())
        {
            this->applyDevice(deviceManager, deviceNames[deviceIndex]);
            return;
        }
    }

    if (const auto currentDevice = deviceManager.getCurrentAudioDevice())
    {
        const Array<double> rates(currentDevice->getAvailableSampleRates());
        const int rateIndex = commandId - CommandIDs::SelectSampleRate;
        if (rateIndex >= 0 && rateIndex < rates.size())
        {
            this->applySampleRate(deviceManager, rates[rateIndex]);
            return;
        }

        const Array<int> bufferSizes(currentDevice->getAvailableBufferSizes());
        const int bufferIndex = commandId - CommandIDs::SelectBufferSize;
        if (bufferIndex >= 0 && bufferIndex < bufferSizes.size())
        {
            this->applyBufferSize(deviceManager, bufferSizes[bufferIndex]);
            return;
        }
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

// todo notify all instruments of config change
//if (this->graph_)
//{
//    this->graph_->getProcessorGraph().removeIllegalConnections();
//}

void AudioSettings::applyDeviceType(AudioDeviceManager &deviceManager, const String &deviceTypeName)
{
    deviceManager.setCurrentAudioDeviceType(deviceTypeName, true);
    deviceManager.getCurrentDeviceTypeObject()->scanForDevices();

    this->syncDeviceTypesList(deviceManager);
    this->syncDevicesList(deviceManager);
    this->syncSampleRatesList(deviceManager);
    this->syncBufferSizesList(deviceManager);
}

void AudioSettings::applyDevice(AudioDeviceManager &deviceManager, const String &deviceName)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.outputDeviceName = deviceName;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);

    this->syncDevicesList(deviceManager);
    this->syncSampleRatesList(deviceManager);
    this->syncBufferSizesList(deviceManager);
}

void AudioSettings::applySampleRate(AudioDeviceManager &deviceManager, double sampleRate)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.sampleRate = sampleRate;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);

    this->syncSampleRatesList(deviceManager);
    this->syncBufferSizesList(deviceManager);
}

void AudioSettings::applyBufferSize(AudioDeviceManager &deviceManager, int bufferSize)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.bufferSize = bufferSize;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);
    this->syncBufferSizesList(deviceManager);
}

void AudioSettings::syncDeviceTypesList(AudioDeviceManager &deviceManager)
{
    const String &currentTypeName = deviceManager.getCurrentAudioDeviceType();
    const OwnedArray<AudioIODeviceType> &types = deviceManager.getAvailableDeviceTypes();

    MenuPanel::Menu menu;
    for (int i = 0; i < types.size(); ++i)
    {
        const String &typeName = types[i]->getTypeName();
        const bool isSelected = typeName == currentTypeName;
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectAudioDeviceType + i, typeName));

        if (isSelected)
        {
            this->deviceTypeEditor->setText(TRANS("settings::audio::device") +
                ": " + typeName, dontSendNotification);
        }
    }

    this->deviceTypeComboPrimer->updateMenu(menu);
}

void AudioSettings::syncDevicesList(AudioDeviceManager &deviceManager)
{
    const AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();
    const AudioIODeviceType *currentType = deviceManager.getCurrentDeviceTypeObject();

    if (currentDevice == nullptr || currentType == nullptr) { return; }

    MenuPanel::Menu menu;
    const StringArray &devices = currentType->getDeviceNames();

    for (int i = 0; i < devices.size(); ++i)
    {
        const String &deviceName = devices[i];
        const bool isSelected = deviceName == currentDevice->getName();
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectAudioDevice + i, deviceName));

        if (isSelected)
        {
            this->deviceEditor->setText(TRANS("settings::audio::driver") +
                ": " + deviceName, dontSendNotification);
        }
    }

    this->deviceComboPrimer->updateMenu(menu);
}

void AudioSettings::syncSampleRatesList(AudioDeviceManager &deviceManager)
{
    AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();

    if (currentDevice == nullptr) { return; }

    MenuPanel::Menu menu;
    const Array<double> rates(currentDevice->getAvailableSampleRates());

    for (int i = 0; i < rates.size(); ++i)
    {
        const double &sampleRate = rates[i];
        const bool isSelected = sampleRate == currentDevice->getCurrentSampleRate();
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectSampleRate + i, String(sampleRate)));

        if (isSelected)
        {
            this->sampleRateEditor->setText(TRANS("settings::audio::samplerate") +
                ": " + String(sampleRate), dontSendNotification);
        }
    }

    this->sampleRateComboPrimer->updateMenu(menu);
}

void AudioSettings::syncBufferSizesList(AudioDeviceManager &deviceManager)
{
    AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();

    if (currentDevice == nullptr) { return; }

    MenuPanel::Menu menu;
    const Array<int> bufferSizes(currentDevice->getAvailableBufferSizes());
    const int currentBufferSize = currentDevice->getCurrentBufferSizeSamples();

    for (int i = 0; i < bufferSizes.size(); ++i)
    {
        const int &bufferSize = bufferSizes[i];
        const bool isSelected = bufferSize == currentBufferSize;
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectBufferSize + i, String(bufferSize)));

        if (isSelected)
        {
            this->bufferSizeEditor->setText(TRANS("settings::audio::buffersize") +
                ": " + String(bufferSize), dontSendNotification);
        }
    }

    this->bufferSizeComboPrimer->updateMenu(menu);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioSettings" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="AudioCore &amp;core"
                 variableInitialisers="audioCore(core)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="550"
                 initialHeight="200">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff000000"/>
  <GENERICCOMPONENT name="" id="b8d926f809ea9d18" memberName="sampleRateComboPrimer"
                    virtualName="" explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <GENERICCOMPONENT name="" id="f314396fef5c1957" memberName="bufferSizeComboPrimer"
                    virtualName="" explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <GENERICCOMPONENT name="" id="524df900a9089845" memberName="deviceTypeComboPrimer"
                    virtualName="" explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <GENERICCOMPONENT name="" id="1b5648cb76a38566" memberName="deviceComboPrimer"
                    virtualName="" explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <TEXTEDITOR name="" id="4fd07309a20b15b6" memberName="deviceTypeEditor" virtualName=""
              explicitFocusOrder="0" pos="16 12 32M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <TEXTEDITOR name="" id="fa9f51f63481813" memberName="deviceEditor" virtualName=""
              explicitFocusOrder="0" pos="16 60 32M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <TEXTEDITOR name="" id="269505831c2d23ad" memberName="sampleRateEditor" virtualName=""
              explicitFocusOrder="0" pos="16 108 32M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <TEXTEDITOR name="" id="42608864c0766e06" memberName="bufferSizeEditor" virtualName=""
              explicitFocusOrder="0" pos="16 156 32M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
