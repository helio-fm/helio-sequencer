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
    addAndMakeVisible (deviceTypes = new ComboBox (String()));
    deviceTypes->setEditableText (false);
    deviceTypes->setJustificationType (Justification::centredLeft);
    deviceTypes->setTextWhenNothingSelected (String());
    deviceTypes->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    deviceTypes->addItem (TRANS("DirectSound"), 1);
    deviceTypes->addItem (TRANS("ASIO"), 2);
    deviceTypes->addListener (this);

    addAndMakeVisible (deviceTypesLabel = new Label (String(),
                                                     TRANS("settings::audio::device")));
    deviceTypesLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    deviceTypesLabel->setJustificationType (Justification::centredLeft);
    deviceTypesLabel->setEditable (false, false, false);
    deviceTypesLabel->setColour (Label::textColourId, Colour (0xbcffffff));
    deviceTypesLabel->setColour (TextEditor::textColourId, Colours::black);
    deviceTypesLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (devices = new ComboBox (String()));
    devices->setEditableText (false);
    devices->setJustificationType (Justification::centredLeft);
    devices->setTextWhenNothingSelected (String());
    devices->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    devices->addListener (this);

    addAndMakeVisible (devicesLabel = new Label (String(),
                                                 TRANS("settings::audio::driver")));
    devicesLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    devicesLabel->setJustificationType (Justification::centredLeft);
    devicesLabel->setEditable (false, false, false);
    devicesLabel->setColour (Label::textColourId, Colour (0xbcffffff));
    devicesLabel->setColour (TextEditor::textColourId, Colours::black);
    devicesLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (sampleRates = new ComboBox (String()));
    sampleRates->setEditableText (false);
    sampleRates->setJustificationType (Justification::centredLeft);
    sampleRates->setTextWhenNothingSelected (String());
    sampleRates->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    sampleRates->addListener (this);

    addAndMakeVisible (sampleRatesLabel = new Label (String(),
                                                     TRANS("settings::audio::samplerate")));
    sampleRatesLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    sampleRatesLabel->setJustificationType (Justification::centredLeft);
    sampleRatesLabel->setEditable (false, false, false);
    sampleRatesLabel->setColour (Label::textColourId, Colour (0xbcffffff));
    sampleRatesLabel->setColour (TextEditor::textColourId, Colours::black);
    sampleRatesLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (latencyLabel = new Label (String(),
                                                 TRANS("settings::audio::buffersize")));
    latencyLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    latencyLabel->setJustificationType (Justification::centredLeft);
    latencyLabel->setEditable (false, false, false);
    latencyLabel->setColour (Label::textColourId, Colour (0xbcffffff));
    latencyLabel->setColour (TextEditor::textColourId, Colours::black);
    latencyLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (latency = new ComboBox (String()));
    latency->setEditableText (false);
    latency->setJustificationType (Justification::centredLeft);
    latency->setTextWhenNothingSelected (String());
    latency->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    latency->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (550, 300);

    //[Constructor]
    //[/Constructor]
}

AudioSettings::~AudioSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    deviceTypes = nullptr;
    deviceTypesLabel = nullptr;
    devices = nullptr;
    devicesLabel = nullptr;
    sampleRates = nullptr;
    sampleRatesLabel = nullptr;
    latencyLabel = nullptr;
    latency = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AudioSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    deviceTypes->setBounds (16, 39, getWidth() - 38, 26);
    deviceTypesLabel->setBounds (8, 8, getWidth() - 22, 24);
    devices->setBounds (16, (getHeight() / 2) + -26 - (26 / 2), getWidth() - 38, 26);
    devicesLabel->setBounds (8, 80, getWidth() - 22, 24);
    sampleRates->setBounds (16, (getHeight() / 2) + 46 - (26 / 2), getWidth() - 38, 26);
    sampleRatesLabel->setBounds (8, 152, getWidth() - 22, 24);
    latencyLabel->setBounds (8, 236 - (24 / 2), getWidth() - 22, 24);
    latency->setBounds (16, (getHeight() / 2) + 119 - (26 / 2), getWidth() - 38, 26);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioSettings::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == deviceTypes)
    {
        //[UserComboBoxCode_deviceTypes] -- add your combo box handling code here..

        AudioDeviceManager &deviceManager = this->audioCore.getDevice();
        const int &selectedIndex = this->deviceTypes->getSelectedItemIndex();
        const String &selectedText = this->deviceTypes->getItemText(selectedIndex);

        this->applyDeviceType(deviceManager, selectedText);

        //[/UserComboBoxCode_deviceTypes]
    }
    else if (comboBoxThatHasChanged == devices)
    {
        //[UserComboBoxCode_devices] -- add your combo box handling code here..

        AudioDeviceManager &deviceManager = this->audioCore.getDevice();
        const int &selectedIndex = this->devices->getSelectedItemIndex();
        const String &selectedText = this->devices->getItemText(selectedIndex);
        this->applyDevice(deviceManager, selectedText);

        //[/UserComboBoxCode_devices]
    }
    else if (comboBoxThatHasChanged == sampleRates)
    {
        //[UserComboBoxCode_sampleRates] -- add your combo box handling code here..

        AudioDeviceManager &deviceManager = this->audioCore.getDevice();
        const int &selectedIndex = this->sampleRates->getSelectedItemIndex();
        const String &selectedText = this->sampleRates->getItemText(selectedIndex);
        this->applySampleRate(deviceManager, selectedText.getDoubleValue());

        //[/UserComboBoxCode_sampleRates]
    }
    else if (comboBoxThatHasChanged == latency)
    {
        //[UserComboBoxCode_latency] -- add your combo box handling code here..
        AudioDeviceManager &deviceManager = this->audioCore.getDevice();
        const int &selectedIndex = this->latency->getSelectedItemIndex();
        const String &selectedText = this->latency->getItemText(selectedIndex);
        this->applyLatency(deviceManager, selectedText.getIntValue());
        //[/UserComboBoxCode_latency]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
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
        this->syncLatencySlider(deviceManager);
    }
    //[/UserCode_visibilityChanged]
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
    this->syncLatencySlider(deviceManager);
}

void AudioSettings::applyDevice(AudioDeviceManager &deviceManager, const String &deviceName)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.outputDeviceName = deviceName;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);

    this->syncDevicesList(deviceManager);
    this->syncSampleRatesList(deviceManager);
    this->syncLatencySlider(deviceManager);
}

void AudioSettings::applySampleRate(AudioDeviceManager &deviceManager, double sampleRate)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.sampleRate = sampleRate;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);

    this->syncSampleRatesList(deviceManager);
    this->syncLatencySlider(deviceManager);
}

void AudioSettings::applyLatency(AudioDeviceManager &deviceManager, const int &newBufferSize)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    deviceManager.getAudioDeviceSetup(deviceSetup);

    deviceSetup.bufferSize = newBufferSize;
    deviceManager.setAudioDeviceSetup(deviceSetup, true);

    //this->syncLatencySlider(deviceManager);
}


void AudioSettings::syncDeviceTypesList(AudioDeviceManager &deviceManager)
{
    this->deviceTypes->clear(dontSendNotification);
    const String &currentTypeName = deviceManager.getCurrentAudioDeviceType();
    const OwnedArray<AudioIODeviceType> &types = deviceManager.getAvailableDeviceTypes();

    for (int i = 0; i < types.size(); ++i)
    {
        const String &typeName = types[i]->getTypeName();
        const int typeId = (i + 1); // not 0
        this->deviceTypes->addItem(typeName, typeId);

        if (typeName == currentTypeName)
        {
            this->deviceTypes->setSelectedId(typeId, dontSendNotification);
        }
    }
}

void AudioSettings::syncDevicesList(AudioDeviceManager &deviceManager)
{
    this->devices->clear(dontSendNotification);
    const AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();
    const AudioIODeviceType *currentType = deviceManager.getCurrentDeviceTypeObject();

    if (!currentType) { return; }

    const StringArray &devices = currentType->getDeviceNames();

    for (int i = 0; i < devices.size(); ++i)
    {
        const String &deviceName = devices[i];
        const int deviceId = (i + 1); // not 0
        this->devices->addItem(deviceName, deviceId);

        if (currentDevice)
        {
            if (deviceName == currentDevice->getName())
            {
                this->devices->setSelectedId(deviceId, dontSendNotification);
            }
        }
    }
}

void AudioSettings::syncSampleRatesList(AudioDeviceManager &deviceManager)
{
    this->sampleRates->clear(dontSendNotification);
    AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();

    if (!currentDevice) { return; }

    const Array<double> rates(currentDevice->getAvailableSampleRates());

    for (int i = 0; i < rates.size(); ++i)
    {
        const double &sampleRate = rates[i];
        const int rateId = (i + 1); // not 0
        this->sampleRates->addItem(String(sampleRate), rateId);

        if (sampleRate == currentDevice->getCurrentSampleRate())
        {
            this->sampleRates->setSelectedId(rateId, dontSendNotification);
        }
    }
}

void AudioSettings::syncLatencySlider(AudioDeviceManager &deviceManager)
{
    this->latency->clear(dontSendNotification);
    AudioIODevice *currentDevice = deviceManager.getCurrentAudioDevice();

    if (!currentDevice) { return; }

    const Array<int> buffers(currentDevice->getAvailableBufferSizes());
    const int currentBufferSize = currentDevice->getCurrentBufferSizeSamples();

    for (int i = 0; i < buffers.size(); ++i)
    {
        const int &buffer = buffers[i];
        const int bufferId = (i + 1); // not 0
        this->latency->addItem(String(buffer), bufferId);

        if (buffer == currentBufferSize)
        {
            this->latency->setSelectedId(bufferId, dontSendNotification);
        }
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioSettings" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="AudioCore &amp;core"
                 variableInitialisers="audioCore(core)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="550"
                 initialHeight="300">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="4d4d4d"/>
  <COMBOBOX name="" id="31ccb7cf6ce25097" memberName="deviceTypes" virtualName=""
            explicitFocusOrder="0" pos="16 39 38M 26" editable="0" layout="33"
            items="DirectSound&#10;ASIO" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="a76abc06dc30e785" memberName="deviceTypesLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 22M 24" textCol="bcffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="settings::audio::device" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="" id="c40779f2a1d82e01" memberName="devices" virtualName=""
            explicitFocusOrder="0" pos="16 -26Cc 38M 26" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="1a168043cfef2cb0" memberName="devicesLabel" virtualName=""
         explicitFocusOrder="0" pos="8 80 22M 24" textCol="bcffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="settings::audio::driver" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="" id="6266c722bfb5105a" memberName="sampleRates" virtualName=""
            explicitFocusOrder="0" pos="16 46Cc 38M 26" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="" id="46a4ed98959925ad" memberName="sampleRatesLabel" virtualName=""
         explicitFocusOrder="0" pos="8 152 22M 24" textCol="bcffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="settings::audio::samplerate"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="21" kerning="0"
         bold="0" italic="0" justification="33"/>
  <LABEL name="" id="29668fa4fff702a8" memberName="latencyLabel" virtualName=""
         explicitFocusOrder="0" pos="8 236c 22M 24" textCol="bcffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="settings::audio::buffersize"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="21" kerning="0"
         bold="0" italic="0" justification="33"/>
  <COMBOBOX name="" id="5961361c40500925" memberName="latency" virtualName=""
            explicitFocusOrder="0" pos="16 119Cc 38M 26" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
