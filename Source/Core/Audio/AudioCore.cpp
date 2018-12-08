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
#include "AudioCore.h"
#include "InternalPluginFormat.h"
#include "BuiltInSynthFormat.h"
#include "PluginWindow.h"
#include "OrchestraPit.h"
#include "Instrument.h"
#include "SerializationKeys.h"
#include "AudioMonitor.h"

void AudioCore::initAudioFormats(AudioPluginFormatManager &formatManager)
{
    formatManager.addDefaultFormats();
    formatManager.addFormat(new InternalPluginFormat());
    formatManager.addFormat(new BuiltInSynthFormat());
}

AudioCore::AudioCore()
{
    this->audioMonitor = new AudioMonitor();
    this->deviceManager.addAudioCallback(this->audioMonitor);
    AudioCore::initAudioFormats(this->formatManager);
}

AudioCore::~AudioCore()
{
    this->deviceManager.removeAudioCallback(this->audioMonitor);
    this->audioMonitor = nullptr;
    this->deviceManager.closeAudioDevice();
}

void AudioCore::mute()
{
    for (auto instrument : this->instruments)
    {
        this->removeInstrumentFromDevice(instrument);
    }
}

void AudioCore::unmute()
{
    this->mute(); // на всякий случай, чтоб 2 раза инструменты не добавлялись

    for (auto instrument : this->instruments)
    {
        this->addInstrumentToDevice(instrument);
    }
}

AudioDeviceManager &AudioCore::getDevice() noexcept
{
    return this->deviceManager;
}

AudioPluginFormatManager &AudioCore::getFormatManager() noexcept
{
    return this->formatManager;
}

AudioMonitor *AudioCore::getMonitor() const noexcept
{
    return this->audioMonitor;
}

//===----------------------------------------------------------------------===//
// Instruments
//===----------------------------------------------------------------------===//

void AudioCore::addInstrument(const PluginDescription &pluginDescription,
    const String &name, Instrument::InitializationCallback callback)
{
    auto *instrument = this->instruments.add(new Instrument(formatManager, name));
    this->addInstrumentToDevice(instrument);
    instrument->initializeFrom(pluginDescription,
        [this, callback](Instrument *instrument)
        {
            jassert(instrument);
            this->broadcastInstrumentAdded(instrument);
            callback(instrument);
            DBG("Loaded " + instrument->getName());
        });
}

void AudioCore::removeInstrument(Instrument *instrument)
{
    this->broadcastInstrumentRemoved(instrument);

    this->removeInstrumentFromDevice(instrument);
    this->instruments.removeObject(instrument, true);

    this->broadcastInstrumentRemovedPostAction();
}

void AudioCore::addInstrumentToDevice(Instrument *instrument)
{
    this->deviceManager.addAudioCallback(&instrument->getProcessorPlayer());
    this->deviceManager.addMidiInputCallback({}, &instrument->getProcessorPlayer().getMidiMessageCollector());
}

void AudioCore::removeInstrumentFromDevice(Instrument *instrument)
{
    this->deviceManager.removeAudioCallback(&instrument->getProcessorPlayer());
    this->deviceManager.removeMidiInputCallback({}, &instrument->getProcessorPlayer().getMidiMessageCollector());
}

//===----------------------------------------------------------------------===//
// OrchestraPit
//===----------------------------------------------------------------------===//

Array<Instrument *> AudioCore::getInstruments() const
{
    Array<Instrument *> result;
    result.addArray(this->instruments);
    return result;
}

Instrument *AudioCore::findInstrumentById(const String &id) const
{
    // check by ids
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        Instrument *instrument = this->instruments.getUnchecked(i);

        if (id.contains(instrument->getInstrumentID()))
        {
            return instrument;
        }
    }

    // check by hashes
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        Instrument *instrument = this->instruments.getUnchecked(i);

        if (id.contains(instrument->getInstrumentHash()))
        {
            return instrument;
        }
    }

    return nullptr;
}

void AudioCore::initDefaultInstrument()
{
    OwnedArray<PluginDescription> descriptions;

    BuiltInSynthFormat format;
    format.findAllTypesForFile(descriptions, BuiltInSynth::pianoId);

    PluginDescription desc(*descriptions[0]);
    this->addInstrument(desc, "Helio Piano", [](Instrument *) {});
}

//===----------------------------------------------------------------------===//
// Setup
//===----------------------------------------------------------------------===//

void AudioCore::autodetectDeviceSetup()
{
    DBG("AudioCore::autodetectDeviceSetup");
    
    // requesting 0 inputs and only 2 outputs because of freaking alsa
    this->deviceManager.initialise(0, 2, nullptr, true);

    const auto deviceType =
        this->deviceManager.getCurrentDeviceTypeObject();

    const auto device =
        this->deviceManager.getCurrentAudioDevice();

    if (!deviceType || !device)
    {
        const OwnedArray<AudioIODeviceType> &types =
            this->deviceManager.getAvailableDeviceTypes();

        AudioIODeviceType *const type = types[0];

        this->deviceManager.setCurrentAudioDeviceType(type->getTypeName(), true);

        type->scanForDevices();

        AudioDeviceManager::AudioDeviceSetup deviceSetup;
        this->deviceManager.getAudioDeviceSetup(deviceSetup);
        this->deviceManager.setAudioDeviceSetup(deviceSetup, true);
    }
}

ValueTree AudioCore::serializeDeviceManager() const
{
    using namespace Serialization;

    ValueTree tree(Audio::audioDevice);
    AudioDeviceManager::AudioDeviceSetup currentSetup;
    this->deviceManager.getAudioDeviceSetup(currentSetup);

    tree.setProperty(Audio::audioDeviceType, this->deviceManager.getCurrentAudioDeviceType(), nullptr);
    tree.setProperty(Audio::audioOutputDeviceName, currentSetup.outputDeviceName, nullptr);
    tree.setProperty(Audio::audioInputDeviceName, currentSetup.inputDeviceName, nullptr);

    const auto currentAudioDevice = this->deviceManager.getCurrentAudioDevice();
    if (currentAudioDevice != nullptr)
    {
        tree.setProperty(Audio::audioDeviceRate,
            currentAudioDevice->getCurrentSampleRate(), nullptr);

        if (currentAudioDevice->getDefaultBufferSize() !=
            currentAudioDevice->getCurrentBufferSizeSamples())
        {
            tree.setProperty(Audio::audioDeviceBufferSize,
                currentAudioDevice->getCurrentBufferSizeSamples(), nullptr);
        }

        if (!currentSetup.useDefaultInputChannels)
        {
            tree.setProperty(Audio::audioDeviceInputChannels,
                currentSetup.inputChannels.toString(2), nullptr);
        }

        if (!currentSetup.useDefaultOutputChannels)
        {
            tree.setProperty(Audio::audioDeviceOutputChannels,
                currentSetup.outputChannels.toString(2), nullptr);
        }
    }

    const StringArray availableMidiDevices(MidiInput::getDevices());
    for (const auto &midiInputName : availableMidiDevices)
    {
        if (this->deviceManager.isMidiInputEnabled(midiInputName))
        {
            ValueTree midiInputNode(Audio::midiInput);
            midiInputNode.setProperty(Audio::midiInputName, midiInputName, nullptr);
            tree.appendChild(midiInputNode, nullptr);
        }
    }

    // Add any midi devices that have been enabled before, but which aren't currently
    // open because the device has been disconnected:
    if (!this->customMidiInputs.isEmpty())
    {
        for (const auto &midiInputName : this->customMidiInputs)
        {
            if (!availableMidiDevices.contains(midiInputName, true))
            {
                ValueTree midiInputNode(Audio::midiInput);
                midiInputNode.setProperty(Audio::midiInputName, midiInputName, nullptr);
                tree.appendChild(midiInputNode, nullptr);
            }
        }
    }

    const String defaultMidiOutput(this->deviceManager.getDefaultMidiOutputName());
    if (defaultMidiOutput.isNotEmpty())
    {
        tree.setProperty(Audio::defaultMidiOutput, defaultMidiOutput, nullptr);
    }

    return tree;
}

void AudioCore::deserializeDeviceManager(const ValueTree &tree)
{
    using namespace Serialization;

    const auto root = tree.hasType(Audio::audioDevice) ?
        tree : tree.getChildWithName(Audio::audioDevice);

    if (!root.isValid())
    {
        this->autodetectDeviceSetup();
        return;
    }

    // A hack: this will call scanDevicesIfNeeded():
    const auto &availableDeviceTypes = this->deviceManager.getAvailableDeviceTypes();

    String error;
    AudioDeviceManager::AudioDeviceSetup setup;
    setup.inputDeviceName = root.getProperty(Audio::audioInputDeviceName);
    setup.outputDeviceName = root.getProperty(Audio::audioOutputDeviceName);

    String currentDeviceType = root.getProperty(Audio::audioDeviceType);
    AudioIODeviceType *foundType = nullptr;

    for (const auto availableType : availableDeviceTypes)
    {
        if (availableType->getTypeName() == currentDeviceType)
        {
            foundType = availableType;
        }
    }

    if (foundType == nullptr && !availableDeviceTypes.isEmpty())
    {
        // TODO search for device types with the same i/o device names?
        currentDeviceType = availableDeviceTypes.getFirst()->getTypeName();
    }

    this->deviceManager.setCurrentAudioDeviceType(currentDeviceType, true);

    setup.bufferSize = root.getProperty(Audio::audioDeviceBufferSize, setup.bufferSize);
    setup.sampleRate = root.getProperty(Audio::audioDeviceRate, setup.sampleRate);

    const var defaultTwoChannels("11");
    const String inputChannels = root.getProperty(Audio::audioDeviceInputChannels, defaultTwoChannels);
    const String outputChannels = root.getProperty(Audio::audioDeviceOutputChannels, defaultTwoChannels);
    setup.inputChannels.parseString(inputChannels, 2);
    setup.outputChannels.parseString(outputChannels, 2);

    setup.useDefaultInputChannels = !root.hasProperty(Audio::audioDeviceInputChannels);
    setup.useDefaultOutputChannels = !root.hasProperty(Audio::audioDeviceOutputChannels);

    error = this->deviceManager.setAudioDeviceSetup(setup, true);

    this->customMidiInputs.clearQuick();
    forEachValueTreeChildWithType(root, c, Audio::midiInput)
    {
        this->customMidiInputs.add(c.getProperty(Audio::midiInputName));
    }

    const StringArray allMidiIns(MidiInput::getDevices());
    for (const auto &midiIn : allMidiIns)
    {
        this->deviceManager.setMidiInputEnabled(midiIn,
            this->customMidiInputs.contains(midiIn));
    }

    if (error.isNotEmpty())
    {
        error = this->deviceManager.initialise(0, 2, nullptr, false);
    }

    this->deviceManager.setDefaultMidiOutput(root.getProperty(Audio::defaultMidiOutput));
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AudioCore::serialize() const
{
    DBG("AudioCore::serialize");
    using namespace Serialization;

    // serializes all settings and instruments (with their graphs)
    // deviceManager's graph is not serialized but managed dynamically

    ValueTree tree(Audio::audioCore);
    ValueTree orchestra(Audio::orchestra);
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        Instrument *instrument = this->instruments.getUnchecked(i);
        orchestra.appendChild(instrument->serialize(), nullptr);
    }

    tree.appendChild(orchestra, nullptr);

    const auto deviceState(this->serializeDeviceManager());
    tree.appendChild(deviceState, nullptr);
    return tree;
}

void AudioCore::deserialize(const ValueTree &tree)
{
    DBG("AudioCore::deserialize");
    using namespace Serialization;

    // re-creates deviceManager's graph each time on deserialization
    this->reset();

    const auto root = tree.hasType(Audio::audioCore) ?
        tree : tree.getChildWithName(Audio::audioCore);

    if (!root.isValid())
    {
        this->autodetectDeviceSetup();
        return;
    }

    this->deserializeDeviceManager(root);

    const auto orchestra = root.getChildWithName(Audio::orchestra);
    if (orchestra.isValid())
    {
        for (const auto &instrumentNode : orchestra)
        {
            ScopedPointer<Instrument> instrument(new Instrument(this->formatManager, {}));
            instrument->deserialize(instrumentNode);
            if (instrument->isValid())
            {
                this->addInstrumentToDevice(instrument);
                this->instruments.add(instrument.release());
            }
        }
    }

    if (this->instruments.isEmpty())
    {
        this->initDefaultInstrument();
    }
}

void AudioCore::reset()
{
    while (this->instruments.size() > 0)
    {
        this->removeInstrument(this->instruments[0]);
    }
}
