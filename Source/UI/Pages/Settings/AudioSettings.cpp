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

#include "Common.h"
#include "AudioSettings.h"
#include "AudioCore.h"
#include "Workspace.h"
#include "HelioTheme.h"

AudioSettings::AudioSettings(AudioCore &core) : audioCore(core)
{
    this->deviceTypeEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->deviceTypeEditor.get());

    this->deviceEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->deviceEditor.get());

    this->sampleRateEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->sampleRateEditor.get());

    this->bufferSizeEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->bufferSizeEditor.get());

    this->midiInputEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->midiInputEditor.get());

    this->midiInputRemappingCheckbox = make<ToggleButton>(TRANS(I18n::Settings::midiRemap12ToneKeyboard));
    this->addAndMakeVisible(this->midiInputRemappingCheckbox.get());
    const auto isFilteringMidi = App::Workspace().getAudioCore().isFilteringMidiInput();
    this->midiInputRemappingCheckbox->setToggleState(isFilteringMidi, dontSendNotification);
    this->midiInputRemappingCheckbox->onClick = [this]
    {
        const auto shouldFilterMidi = this->midiInputRemappingCheckbox->getToggleState();
        App::Workspace().getAudioCore().setFilteringMidiInput(shouldFilterMidi);
    };

    this->midiOutputEditor = HelioTheme::makeSingleLineTextEditor(false);
    this->addAndMakeVisible(this->midiOutputEditor.get());

    MenuPanel::Menu emptyMenu;

    this->deviceTypeCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->deviceTypeCombo.get());
    this->deviceTypeCombo->initWith(this->deviceTypeEditor.get(), emptyMenu);

    this->deviceCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->deviceCombo.get());
    this->deviceCombo->initWith(this->deviceEditor.get(), emptyMenu);

    this->sampleRateCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->sampleRateCombo.get());
    this->sampleRateCombo->initWith(this->sampleRateEditor.get(), emptyMenu);

    this->bufferSizeCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->bufferSizeCombo.get());
    this->bufferSizeCombo->initWith(this->bufferSizeEditor.get(), emptyMenu);

    this->midiInputsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->midiInputsCombo.get());
    this->midiInputsCombo->initWith(this->midiInputEditor.get(), emptyMenu);

    this->midiOutputsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->midiOutputsCombo.get());
    this->midiOutputsCombo->initWith(this->midiOutputEditor.get(), emptyMenu);

    this->setSize(550, 310);
}

AudioSettings::~AudioSettings() = default;

void AudioSettings::resized()
{
    const Rectangle<int> comboBounds(4, 4, this->getWidth() - 8, this->getHeight() - 8);

    this->midiInputsCombo->setBounds(comboBounds);
    this->midiOutputsCombo->setBounds(comboBounds);
    this->sampleRateCombo->setBounds(comboBounds);
    this->bufferSizeCombo->setBounds(comboBounds);
    this->deviceTypeCombo->setBounds(comboBounds);
    this->deviceCombo->setBounds(comboBounds);

    constexpr auto marginX = 16;
    constexpr auto marginY = 16;
    constexpr auto rowSpacing = 10;
    Rectangle<int> editorBounds(marginX, marginY,
        this->getWidth() - (marginX * 2), this->getHeight() - (marginY * 2));

    this->deviceTypeEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));

    editorBounds.removeFromTop(rowSpacing);
    this->deviceEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));

    editorBounds.removeFromTop(rowSpacing);
    this->sampleRateEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));

    editorBounds.removeFromTop(rowSpacing);
    this->bufferSizeEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));

    editorBounds.removeFromTop(rowSpacing);
    this->midiInputEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));

    editorBounds.removeFromTop(rowSpacing / 2);
    this->midiInputRemappingCheckbox->setBounds(editorBounds
        .removeFromTop(Globals::UI::textEditorHeight).translated(4, 0));

    editorBounds.removeFromTop(rowSpacing);
    this->midiOutputEditor->setBounds(editorBounds.removeFromTop(Globals::UI::textEditorHeight));
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
        this->syncMidiOutputsList(deviceManager);
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

    const auto midiInputs = MidiInput::getAvailableDevices();
    const int miniInputDeviceIndex = commandId - CommandIDs::SelectMidiInputDevice;
    if (miniInputDeviceIndex >= 0 && miniInputDeviceIndex < midiInputs.size())
    {
        this->applyMidiInput(deviceManager, midiInputs[miniInputDeviceIndex].identifier);
        return;
    }

    if (commandId == CommandIDs::SelectMidiNoOutputDevice)
    {
        this->applyMidiOutput(deviceManager, {});
        return;
    }

    const auto midiOutputs = MidiOutput::getAvailableDevices();
    const int miniOutputDeviceIndex = commandId - CommandIDs::SelectMidiOutputDevice;
    if (miniOutputDeviceIndex >= 0 && miniOutputDeviceIndex < midiOutputs.size())
    {
        this->applyMidiOutput(deviceManager, midiOutputs[miniOutputDeviceIndex].identifier);
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
        this->syncMidiOutputsList(deviceManager);
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
    this->syncMidiOutputsList(deviceManager);
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

void AudioSettings::applyMidiOutput(AudioDeviceManager &deviceManager, const String &deviceId)
{
    deviceManager.setDefaultMidiOutputDevice(deviceId);
    this->syncMidiOutputsList(deviceManager);
}

void AudioSettings::applyMidiInput(AudioDeviceManager &deviceManager, const String &deviceId)
{
    this->audioCore.resetActiveMidiPlayer();

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
    const auto devices = MidiInput::getAvailableDevices();

    for (int i = 0; i < devices.size(); ++i)
    {
        const bool isEnabled = deviceManager.isMidiInputDeviceEnabled(devices[i].identifier);
        menu.add(MenuItem::item(isEnabled ? Icons::apply : Icons::empty,
            CommandIDs::SelectMidiInputDevice + i, devices[i].name));
    }

    const auto selectedDeviceName = areNoMidiInputsSelected(deviceManager, devices) ?
        TRANS(I18n::Settings::midiNoDevicesFound) :
        getFirstSelectedMidiInputDevice(deviceManager, devices);

    this->midiInputEditor->setText(TRANS(I18n::Settings::midiRecord) +
        ": " + selectedDeviceName, dontSendNotification);

    this->midiInputsCombo->updateMenu(menu);
}

void AudioSettings::syncMidiOutputsList(AudioDeviceManager &deviceManager)
{
    MenuPanel::Menu menu;
    const auto devices = MidiOutput::getAvailableDevices();
    const auto defaultOutputDeviceId = deviceManager.getDefaultMidiOutputIdentifier();

    // "don't send midi" option
    menu.add(MenuItem::item(defaultOutputDeviceId.isEmpty() ? Icons::apply : Icons::empty,
        CommandIDs::SelectMidiNoOutputDevice, TRANS(I18n::Settings::midiOutputNone)));

    auto defaultOutputDeviceName = MidiOutput::getDefaultDevice().name;

    for (int i = 0; i < devices.size(); ++i)
    {
        const bool isSelected = defaultOutputDeviceId == devices[i].identifier;
        if (isSelected)
        {
            defaultOutputDeviceName = devices[i].name;
        }
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectMidiOutputDevice + i, devices[i].name));
    }

    const auto selectedDeviceName = defaultOutputDeviceId.isEmpty() ?
        TRANS(I18n::Settings::midiOutputNone) : defaultOutputDeviceName;

    this->midiOutputEditor->setText(TRANS(I18n::Settings::midiOutput) +
        ": " + selectedDeviceName, dontSendNotification);

    this->midiOutputsCombo->updateMenu(menu);
}
