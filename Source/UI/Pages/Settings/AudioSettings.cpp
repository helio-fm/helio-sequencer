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

#include "Common.h"
#include "AudioSettings.h"
#include "AudioCore.h"

AudioSettings::AudioSettings(AudioCore &core) : audioCore(core)
{
    this->midiInputsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->midiInputsCombo.get());

    this->sampleRateCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->sampleRateCombo.get());

    this->bufferSizeCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->bufferSizeCombo.get());

    this->deviceTypeCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->deviceTypeCombo.get());

    this->deviceCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->deviceCombo.get());

    this->deviceTypeEditor = make<TextEditor>();
    this->addAndMakeVisible(this->deviceTypeEditor.get());
    this->deviceTypeEditor->setReadOnly(true);
    this->deviceTypeEditor->setScrollbarsShown(false);
    this->deviceTypeEditor->setCaretVisible(false);
    this->deviceTypeEditor->setPopupMenuEnabled(false);
    this->deviceTypeEditor->setInterceptsMouseClicks(false, true);
    this->deviceTypeEditor->setFont(Globals::UI::Fonts::M);

    this->deviceEditor = make<TextEditor>();
    this->addAndMakeVisible(this->deviceEditor.get());
    this->deviceEditor->setReadOnly(true);
    this->deviceEditor->setScrollbarsShown(false);
    this->deviceEditor->setCaretVisible(false);
    this->deviceEditor->setPopupMenuEnabled(false);
    this->deviceEditor->setInterceptsMouseClicks(false, true);
    this->deviceEditor->setFont(Globals::UI::Fonts::M);

    this->sampleRateEditor = make<TextEditor>();
    this->addAndMakeVisible(this->sampleRateEditor.get());
    this->sampleRateEditor->setReadOnly(true);
    this->sampleRateEditor->setScrollbarsShown(false);
    this->sampleRateEditor->setCaretVisible(false);
    this->sampleRateEditor->setPopupMenuEnabled(false);
    this->sampleRateEditor->setInterceptsMouseClicks(false, true);
    this->sampleRateEditor->setFont(Globals::UI::Fonts::M);

    this->bufferSizeEditor = make<TextEditor>();
    this->addAndMakeVisible(this->bufferSizeEditor.get());
    this->bufferSizeEditor->setReadOnly(true);
    this->bufferSizeEditor->setScrollbarsShown(false);
    this->bufferSizeEditor->setCaretVisible(false);
    this->bufferSizeEditor->setPopupMenuEnabled(false);
    this->bufferSizeEditor->setInterceptsMouseClicks(false, true);
    this->bufferSizeEditor->setFont(Globals::UI::Fonts::M);

    this->midiInputEditor = make<TextEditor>();
    this->addAndMakeVisible(this->midiInputEditor.get());
    this->midiInputEditor->setReadOnly(true);
    this->midiInputEditor->setScrollbarsShown(false);
    this->midiInputEditor->setCaretVisible(false);
    this->midiInputEditor->setPopupMenuEnabled(false);
    this->midiInputEditor->setInterceptsMouseClicks(false, true);
    this->midiInputEditor->setFont(Globals::UI::Fonts::M);

    this->setSize(550, 244);

    MenuPanel::Menu emptyMenu;
    this->deviceTypeCombo->initWith(this->deviceTypeEditor.get(), emptyMenu);
    this->deviceCombo->initWith(this->deviceEditor.get(), emptyMenu);
    this->sampleRateCombo->initWith(this->sampleRateEditor.get(), emptyMenu);
    this->bufferSizeCombo->initWith(this->bufferSizeEditor.get(), emptyMenu);
    this->midiInputsCombo->initWith(this->midiInputEditor.get(), emptyMenu);
}

AudioSettings::~AudioSettings() = default;

void AudioSettings::resized()
{
    const Rectangle<int> comboBounds(4, 4, this->getWidth() - 8, this->getHeight() - 8);

    this->midiInputsCombo->setBounds(comboBounds);
    this->sampleRateCombo->setBounds(comboBounds);
    this->bufferSizeCombo->setBounds(comboBounds);
    this->deviceTypeCombo->setBounds(comboBounds);
    this->deviceCombo->setBounds(comboBounds);

    const Rectangle<int> editorBounds(16, 0, this->getWidth() - 32, 32);

    this->deviceTypeEditor->setBounds(editorBounds.withY(12));
    this->deviceEditor->setBounds(editorBounds.withY(60));
    this->sampleRateEditor->setBounds(editorBounds.withY(108));
    this->bufferSizeEditor->setBounds(editorBounds.withY(156));
    this->midiInputEditor->setBounds(editorBounds.withY(200));
}

void AudioSettings::parentHierarchyChanged()
{
    if (this->isShowing())
    {
        auto &deviceManager = this->audioCore.getDevice();
        this->syncDeviceTypesList(deviceManager);
        this->syncDevicesList(deviceManager);
        this->syncSampleRatesList(deviceManager);
        this->syncBufferSizesList(deviceManager);
        this->syncMidiInputsList(deviceManager);
    }
}

void AudioSettings::handleCommandMessage(int commandId)
{
    auto &deviceManager = this->audioCore.getDevice();
    const auto &deviceTypes = deviceManager.getAvailableDeviceTypes();

    const int deviceTypeIndex = commandId - CommandIDs::SelectAudioDeviceType;
    if (deviceTypeIndex >= 0 && deviceTypeIndex < deviceTypes.size())
    {
        this->applyDeviceType(deviceManager, deviceTypes[deviceTypeIndex]->getTypeName());
        return;
    }

    if (const auto *currentType = deviceManager.getCurrentDeviceTypeObject())
    {
        const auto &deviceNames = currentType->getDeviceNames();
        const int deviceIndex = commandId - CommandIDs::SelectAudioDevice;
        if (deviceIndex >= 0 && deviceIndex < deviceNames.size())
        {
            this->applyDevice(deviceManager, deviceNames[deviceIndex]);
            return;
        }
    }

    if (auto *currentDevice = deviceManager.getCurrentAudioDevice())
    {
        const auto sampleRates = currentDevice->getAvailableSampleRates();
        const int index = commandId - CommandIDs::SelectSampleRate;
        if (index >= 0 && index < sampleRates.size())
        {
            this->applySampleRate(deviceManager, sampleRates[index]);
            return;
        }

        const auto bufferSizes = currentDevice->getAvailableBufferSizes();
        const int bufferIndex = commandId - CommandIDs::SelectBufferSize;
        if (bufferIndex >= 0 && bufferIndex < bufferSizes.size())
        {
            this->applyBufferSize(deviceManager, bufferSizes[bufferIndex]);
            return;
        }
    }

    const auto &midiDevices = MidiInput::getAvailableDevices();
    const int miniInputDeviceIndex = commandId - CommandIDs::SelectMidiInputDevice;
    if (miniInputDeviceIndex >= 0 && miniInputDeviceIndex < midiDevices.size())
    {
        this->applyMidiInput(deviceManager, midiDevices[miniInputDeviceIndex].identifier);
        return;
    }
}

void AudioSettings::applyDeviceType(AudioDeviceManager &deviceManager, const String &deviceTypeName)
{
    deviceManager.setCurrentAudioDeviceType(deviceTypeName, true);
    this->syncDeviceTypesList(deviceManager);

    auto *currentType = deviceManager.getCurrentDeviceTypeObject();
    jassert(currentType != nullptr);
    currentType->scanForDevices();
    const auto &deviceNames = currentType->getDeviceNames();

    const auto *currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice == nullptr && !deviceNames.isEmpty())
    {
        // for whatever reason, at this point there are devices available,
        // but nothing is set as the current device (happens with JACK)
        this->applyDevice(deviceManager, deviceNames[0]);
    }
    else
    {
        this->syncDevicesList(deviceManager);
        this->syncSampleRatesList(deviceManager);
        this->syncBufferSizesList(deviceManager);
        this->syncMidiInputsList(deviceManager);
    }
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
    this->syncMidiInputsList(deviceManager);
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

void AudioSettings::applyMidiInput(AudioDeviceManager &deviceManager, const String &deviceId)
{
    this->audioCore.setActiveMidiPlayer({}, false);

    for (const auto &device : MidiInput::getAvailableDevices())
    {
        const bool shouldBeEnabled = device.identifier == deviceId;
        deviceManager.setMidiInputDeviceEnabled(device.identifier, shouldBeEnabled);
    }

    this->syncMidiInputsList(deviceManager);
}

void AudioSettings::syncDeviceTypesList(AudioDeviceManager &deviceManager)
{
    this->deviceTypeEditor->setText({}, dontSendNotification);

    const auto currentTypeName = deviceManager.getCurrentAudioDeviceType();
    const auto &types = deviceManager.getAvailableDeviceTypes();

    MenuPanel::Menu menu;
    for (int i = 0; i < types.size(); ++i)
    {
        const String &typeName = types[i]->getTypeName();
        const bool isSelected = typeName == currentTypeName;
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectAudioDeviceType + i, typeName));

        if (isSelected)
        {
            this->deviceTypeEditor->setText(TRANS(I18n::Settings::audioDevice) +
                ": " + typeName, dontSendNotification);
        }
    }

    this->deviceTypeCombo->updateMenu(menu);
}

void AudioSettings::syncDevicesList(AudioDeviceManager &deviceManager)
{
    MenuPanel::Menu menu;
    this->deviceEditor->setText({}, dontSendNotification);

    const auto *currentType = deviceManager.getCurrentDeviceTypeObject();
    if (currentType == nullptr)
    {
        this->deviceCombo->updateMenu(menu);
        return;
    }

    const auto &devices = currentType->getDeviceNames();
    const auto *currentDevice = deviceManager.getCurrentAudioDevice();

    for (int i = 0; i < devices.size(); ++i)
    {
        const auto &deviceName = devices[i];
        const bool isSelected = (currentDevice != nullptr) && (deviceName == currentDevice->getName());
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectAudioDevice + i, deviceName));

        if (isSelected)
        {
            this->deviceEditor->setText(TRANS(I18n::Settings::audioDriver) +
                ": " + deviceName, dontSendNotification);
        }
    }

    this->deviceCombo->updateMenu(menu);
}

void AudioSettings::syncSampleRatesList(AudioDeviceManager &deviceManager)
{
    MenuPanel::Menu menu;
    this->sampleRateEditor->setText({}, dontSendNotification);

    auto *currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice == nullptr)
    {
        this->sampleRateCombo->updateMenu(menu);
        return;
    }

    const auto sampleRates = currentDevice->getAvailableSampleRates();

    for (int i = 0; i < sampleRates.size(); ++i)
    {
        const double sampleRate = sampleRates[i];
        const bool isSelected = sampleRate == currentDevice->getCurrentSampleRate();
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectSampleRate + i, String(sampleRate)));

        if (isSelected)
        {
            this->sampleRateEditor->setText(TRANS(I18n::Settings::audioSampleRate) +
                ": " + String(sampleRate), dontSendNotification);
        }
    }

    this->sampleRateCombo->updateMenu(menu);
}

void AudioSettings::syncBufferSizesList(AudioDeviceManager &deviceManager)
{
    MenuPanel::Menu menu;
    this->bufferSizeEditor->setText({}, dontSendNotification);

    auto *currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice == nullptr)
    {
        this->bufferSizeCombo->updateMenu(menu);
        return;
    }

    const auto bufferSizes = currentDevice->getAvailableBufferSizes();
    const int currentBufferSize = currentDevice->getCurrentBufferSizeSamples();

    for (int i = 0; i < bufferSizes.size(); ++i)
    {
        const int &bufferSize = bufferSizes[i];
        const bool isSelected = bufferSize == currentBufferSize;
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectBufferSize + i, String(bufferSize)));

        if (isSelected)
        {
            this->bufferSizeEditor->setText(TRANS(I18n::Settings::audioBufferSize) +
                ": " + String(bufferSize), dontSendNotification);
        }
    }

    this->bufferSizeCombo->updateMenu(menu);
}

static bool areNoMidiInputsSelected(AudioDeviceManager &deviceManager,
    const Array<MidiDeviceInfo> &devices)
{
    for (const auto &device : devices)
    {
        if (deviceManager.isMidiInputDeviceEnabled(device.identifier))
        {
            return false;
        }
    }

    return true; // yup, even if the list is empty
}

// gets the first one, but the caller assumes either all are selected, or just one of them is selected
static String getFirstSelectedMidiInputDevice(AudioDeviceManager &deviceManager,
    const Array<MidiDeviceInfo> &devices)
{
    for (const auto &device : devices)
    {
        if (deviceManager.isMidiInputDeviceEnabled(device.identifier))
        {
            return device.name;
        }
    }

    return {};
}

void AudioSettings::syncMidiInputsList(AudioDeviceManager &deviceManager)
{
    MenuPanel::Menu menu;
    const auto &devices = MidiInput::getAvailableDevices();

    for (int i = 0; i < devices.size(); ++i)
    {
        const bool isEnabled = deviceManager.isMidiInputDeviceEnabled(devices[i].identifier);
        menu.add(MenuItem::item(isEnabled ? Icons::apply : Icons::empty,
            CommandIDs::SelectMidiInputDevice + i, devices[i].name));
    }

    const auto selectedDeviceName = areNoMidiInputsSelected(deviceManager, devices) ?
        TRANS(I18n::Settings::midiNoInputDevices) :
        getFirstSelectedMidiInputDevice(deviceManager, devices);

    this->midiInputEditor->setText(TRANS(I18n::Settings::midiRecord) +
        ": " + selectedDeviceName, dontSendNotification);

    this->midiInputsCombo->updateMenu(menu);
}
