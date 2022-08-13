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
#include "InternalIODevicesPluginFormat.h"
#include "BuiltInSynthsPluginFormat.h"
#include "DefaultSynthAudioPlugin.h"
#include "MetronomeSynthAudioPlugin.h"
#include "SerializationKeys.h"
#include "AudioMonitor.h"

void AudioCore::initAudioFormats(AudioPluginFormatManager &formatManager)
{
    formatManager.addDefaultFormats();
    formatManager.addFormat(new InternalIODevicesPluginFormat());
    formatManager.addFormat(new BuiltInSynthsPluginFormat());
}

AudioCore::AudioCore()
{
    this->audioMonitor = make<AudioMonitor>();
    this->deviceManager.addAudioCallback(this->audioMonitor.get());
    AudioCore::initAudioFormats(this->formatManager);
}

AudioCore::~AudioCore()
{
    this->deviceManager.removeAudioCallback(this->audioMonitor.get());
    this->audioMonitor = nullptr;
    this->deviceManager.closeAudioDevice();
}

bool AudioCore::canSleepNow() noexcept
{
    // SleepTimer is used to disconnect all callbacks after some delay (i.e. sleep mode),
    // but first we make sure the device is not making any sound, otherwise we'll wait more:
    return this->deviceManager.getOutputLevelGetter()->getCurrentLevel() == 0.0;
}

void AudioCore::sleepNow()
{
    DBG("Audio core sleeps");
    this->disconnectAllAudioCallbacks();
}

void AudioCore::awakeNow()
{
    this->reconnectAllAudioCallbacks();
}

void AudioCore::disconnectAllAudioCallbacks()
{
    if (!this->isMuted.get())
    {
        this->isMuted = true;

        // Audio monitor is especially CPU-hungry, as it does FFT all the time:
        this->deviceManager.removeAudioCallback(this->audioMonitor.get());

        for (auto *instrument : this->instruments)
        {
            this->removeInstrumentFromAudioDevice(instrument);
        }
    }
}

void AudioCore::reconnectAllAudioCallbacks()
{
    if (this->isMuted.get())
    {
        for (auto *instrument : this->instruments)
        {
            this->addInstrumentToAudioDevice(instrument);
        }

        this->deviceManager.addAudioCallback(this->audioMonitor.get());

        this->isMuted = false;
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
    return this->audioMonitor.get();
}

//===----------------------------------------------------------------------===//
// Instruments
//===----------------------------------------------------------------------===//

void AudioCore::addInstrument(const PluginDescription &pluginDescription,
    const String &name, Instrument::InitializationCallback callback)
{
    auto *instrument = this->instruments.add(new Instrument(formatManager, name));
    this->addInstrumentToAudioDevice(instrument);
    instrument->initializeFrom(pluginDescription,
        [this, callback](Instrument *instrument)
        {
            jassert(instrument);
            if (callback != nullptr)
            {
                callback(instrument);
            }
            this->broadcastAddInstrument(instrument);
            DBG("Loaded " + instrument->getName());
        });
}

void AudioCore::removeInstrument(Instrument *instrument)
{
    this->broadcastRemoveInstrument(instrument);

    this->removeInstrumentFromAudioDevice(instrument);
    this->removeInstrumentFromMidiDevice(instrument);

    this->instruments.removeObject(instrument, true);

    this->broadcastPostRemoveInstrument();
}

//===----------------------------------------------------------------------===//
// Audio & MIDI callbacks and MIDI filtering
//===----------------------------------------------------------------------===//

void AudioCore::addFilteredMidiInputCallback(MidiInputCallback *callbackToAdd,
    int periodSize, Scale::Ptr chromaticMapping)
{
    this->removeFilteredMidiInputCallback(callbackToAdd);

    // update the existing filter
    for (const auto &mc : this->filteredMidiCallbacks)
    {
        if (mc.targetCallback == callbackToAdd)
        {
            mc.filter->update(periodSize, chromaticMapping);
            this->deviceManager.addMidiInputDeviceCallback({}, mc.filter.get());
            return;
        }
    }

    // or add a new one
    auto filter = make<MidiRecordingKeyMapper>(*this,
        callbackToAdd, periodSize, chromaticMapping);

    this->deviceManager.addMidiInputDeviceCallback({}, filter.get());
    this->filteredMidiCallbacks.add({ move(filter), callbackToAdd });
}

void AudioCore::removeFilteredMidiInputCallback(MidiInputCallback *callbackToRemove)
{
    for (const auto &mc : this->filteredMidiCallbacks)
    {
        if (mc.targetCallback == callbackToRemove)
        {
            this->deviceManager.removeMidiInputDeviceCallback({}, mc.filter.get());
        }
    }
}

bool AudioCore::isFilteringMidiInput() const noexcept
{
    return this->isReadjustingMidiInput.get();
}

void AudioCore::setFilteringMidiInput(bool isOn) noexcept
{
    this->isReadjustingMidiInput = isOn;
}

void AudioCore::addInstrumentToMidiDevice(Instrument *instrument,
    int periodSize, Scale::Ptr chromaticMapping)
{
    this->addFilteredMidiInputCallback(
        &instrument->getProcessorPlayer().getMidiMessageCollector(),
        periodSize, chromaticMapping);
}

void AudioCore::removeInstrumentFromMidiDevice(Instrument *instrument)
{
    this->removeFilteredMidiInputCallback(
        &instrument->getProcessorPlayer().getMidiMessageCollector());
}

void AudioCore::addInstrumentToAudioDevice(Instrument *instrument)
{
    this->deviceManager.addAudioCallback(&instrument->getProcessorPlayer());
}

void AudioCore::removeInstrumentFromAudioDevice(Instrument *instrument)
{
    this->deviceManager.removeAudioCallback(&instrument->getProcessorPlayer());
}

void AudioCore::resetActiveMidiPlayer()
{
    this->lastActiveMidiPlayer = {};

    for (auto *instrument : this->instruments)
    {
        this->removeInstrumentFromMidiDevice(instrument);
    }
}

void AudioCore::setActiveMidiPlayer(const String &instrumentId,
    int periodSize, Scale::Ptr chromaticMapping, bool forceReconnect)
{
    if (!forceReconnect &&
        this->lastActiveMidiPlayer.instrumentId == instrumentId &&
        this->lastActiveMidiPlayer.periodSize == periodSize &&
        this->lastActiveMidiPlayer.chromaticMapping == chromaticMapping)
    {
        //DBG("Skip setActiveMidiPlayer for " + instrumentId);
        return;
    }

    for (auto *instrument : this->instruments)
    {
        if (instrumentId.startsWith(instrument->getInstrumentId()))
        {
            //DBG("addInstrumentToMidiDevice for " + instrumentId);
            this->addInstrumentToMidiDevice(instrument, periodSize, chromaticMapping);
        }
        else
        {
            //DBG("removeInstrumentFromMidiDevice for " + instrument->getInstrumentId());
            this->removeInstrumentFromMidiDevice(instrument);
        }
    }

    this->lastActiveMidiPlayer.instrumentId = instrumentId;
    this->lastActiveMidiPlayer.periodSize = periodSize;
    this->lastActiveMidiPlayer.chromaticMapping = chromaticMapping;
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

Array<Instrument *> AudioCore::getInstrumentsExceptInternal() const
{
    Array<Instrument *> result;
    for (auto *instrument : this->instruments)
    {
        if (!instrument->isMetronomeInstrument())
        {
            result.add(instrument);
        }
    }
    return result;
}

Instrument *AudioCore::findInstrumentById(const String &id) const
{
    // check by ids
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        auto *instrument = this->instruments.getUnchecked(i);

        if (id.contains(instrument->getInstrumentId()))
        {
            return instrument;
        }
    }

    // check by hashes
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        auto *instrument = this->instruments.getUnchecked(i);

        if (id.contains(instrument->getInstrumentHash()))
        {
            return instrument;
        }
    }

    return nullptr;
}

Instrument *AudioCore::getDefaultInstrument() const noexcept
{
    jassert(this->defaultInstrument != nullptr || !this->instruments.isEmpty());
    return this->defaultInstrument != nullptr ?
        this->defaultInstrument.get() : this->instruments.getFirst();
}

Instrument *AudioCore::getMetronomeInstrument() const noexcept
{
    jassert(this->metronomeInstrument != nullptr);
    return this->metronomeInstrument != nullptr ?
        this->metronomeInstrument.get() : nullptr;
}

String AudioCore::getMetronomeInstrumentId() const noexcept
{
    const auto *metronome = this->getMetronomeInstrument();
    return metronome != nullptr ? metronome->getInstrumentId() : String();
}

void AudioCore::initBuiltInInstrumentsIfNeeded()
{
    if (this->defaultInstrument == nullptr)
    {
        PluginDescription defaultPluginDescription;
        DefaultSynthAudioPlugin defaultAudioPlugin;
        defaultAudioPlugin.fillInPluginDescription(defaultPluginDescription);

        this->addInstrument(defaultPluginDescription,
            TRANS(I18n::Instruments::defultSynthTitle),
            [this](Instrument *instrument) {
                this->defaultInstrument = instrument;
            });
    }

    if (this->metronomeInstrument == nullptr)
    {
        PluginDescription metronomePluginDescription;
        MetronomeSynthAudioPlugin metronomeAudioPlugin;
        metronomeAudioPlugin.fillInPluginDescription(metronomePluginDescription);

        this->addInstrument(metronomePluginDescription,
            TRANS(I18n::Instruments::metronomeTitle),
            [this](Instrument *instrument) {
                this->metronomeInstrument = instrument;
            });
    }
}

//===----------------------------------------------------------------------===//
// Setup
//===----------------------------------------------------------------------===//

bool AudioCore::autodetectAudioDeviceSetup()
{
    //DBG("AudioCore::autodetectDeviceSetup");
    
    // requesting 0 inputs and only 2 outputs because of freaking alsa
    this->deviceManager.initialise(0, 2, nullptr, true);

    const auto deviceType =
        this->deviceManager.getCurrentDeviceTypeObject();

    const auto device =
        this->deviceManager.getCurrentAudioDevice();

    if (!deviceType || !device)
    {
        const auto &types = this->deviceManager.getAvailableDeviceTypes();
        if (types.size() > 0)
        {
            auto *type = types.getFirst();
            this->deviceManager.setCurrentAudioDeviceType(type->getTypeName(), true);

            type->scanForDevices();

            AudioDeviceManager::AudioDeviceSetup deviceSetup;
            this->deviceManager.getAudioDeviceSetup(deviceSetup);
            this->deviceManager.setAudioDeviceSetup(deviceSetup, true);

            return true;
        }

        return false;
    }

    return true;
}

bool AudioCore::autodetectMidiDeviceSetup()
{
    //DBG("AudioCore::autodetectMidiDeviceSetup");

    int numEnabledDevices = 0;
    const auto allDevices = MidiInput::getAvailableDevices();
    for (const auto &midiInput : allDevices)
    {
        if (this->deviceManager.isMidiInputDeviceEnabled(midiInput.identifier))
        {
            numEnabledDevices++;
        }
    }

    if (numEnabledDevices == 1)
    {
        // force reconnect to instrument
        this->setActiveMidiPlayer(this->lastActiveMidiPlayer.instrumentId,
            this->lastActiveMidiPlayer.periodSize,
            this->lastActiveMidiPlayer.chromaticMapping,
            true);
        return true;
    }

    // nothing is enabled, but there's only one device, so let's just enable it
    if (numEnabledDevices == 0 && allDevices.size() == 1)
    {
        DBG("Found the single available MIDI input, enabling");
        this->deviceManager.setMidiInputDeviceEnabled(allDevices.getFirst().identifier, true);
        this->setActiveMidiPlayer(this->lastActiveMidiPlayer.instrumentId,
            this->lastActiveMidiPlayer.periodSize,
            this->lastActiveMidiPlayer.chromaticMapping,
            true);
        return true;
    }

    return false;
}

SerializedData AudioCore::serializeDeviceManager() const
{
    using namespace Serialization;

    SerializedData tree(Audio::audioDevice);
    AudioDeviceManager::AudioDeviceSetup currentSetup;
    this->deviceManager.getAudioDeviceSetup(currentSetup);

    tree.setProperty(Audio::audioDeviceType, this->deviceManager.getCurrentAudioDeviceType());
    tree.setProperty(Audio::audioOutputDeviceName, currentSetup.outputDeviceName);
    tree.setProperty(Audio::audioInputDeviceName, currentSetup.inputDeviceName);

    const auto currentAudioDevice = this->deviceManager.getCurrentAudioDevice();
    if (currentAudioDevice != nullptr)
    {
        tree.setProperty(Audio::audioDeviceRate,
            currentAudioDevice->getCurrentSampleRate());

        if (currentAudioDevice->getDefaultBufferSize() !=
            currentAudioDevice->getCurrentBufferSizeSamples())
        {
            tree.setProperty(Audio::audioDeviceBufferSize,
                currentAudioDevice->getCurrentBufferSizeSamples());
        }

        if (!currentSetup.useDefaultInputChannels)
        {
            tree.setProperty(Audio::audioDeviceInputChannels,
                currentSetup.inputChannels.toString(2));
        }

        if (!currentSetup.useDefaultOutputChannels)
        {
            tree.setProperty(Audio::audioDeviceOutputChannels,
                currentSetup.outputChannels.toString(2));
        }
    }

    for (const auto &midiInput : MidiInput::getAvailableDevices())
    {
        if (this->deviceManager.isMidiInputDeviceEnabled(midiInput.identifier))
        {
            tree.setProperty(Audio::midiInputName, midiInput.name);
            tree.setProperty(Audio::midiInputId, midiInput.identifier);
            // assume single selection here:
            break;
        }
    }

    tree.setProperty(Audio::midiInputReadjusting,
        this->isReadjustingMidiInput.get());

    if (auto *midiOutput = this->deviceManager.getDefaultMidiOutput())
    {
        tree.setProperty(Audio::midiOutputName, midiOutput->getName());
        tree.setProperty(Audio::midiOutputId, midiOutput->getIdentifier());
    }

    return tree;
}

void AudioCore::deserializeDeviceManager(const SerializedData &tree)
{
    using namespace Serialization;

    const auto root = tree.hasType(Audio::audioDevice) ?
        tree : tree.getChildWithName(Audio::audioDevice);

    if (!root.isValid())
    {
        this->autodetectAudioDeviceSetup();
        this->autodetectMidiDeviceSetup();
        return;
    }

    // A hack: this will call scanDevicesIfNeeded():
    const auto &availableDeviceTypes = this->deviceManager.getAvailableDeviceTypes();

    AudioDeviceManager::AudioDeviceSetup setup;
    setup.inputDeviceName = root.getProperty(Audio::audioInputDeviceName, setup.inputDeviceName);
    setup.outputDeviceName = root.getProperty(Audio::audioOutputDeviceName, setup.outputDeviceName);
    setup.bufferSize = root.getProperty(Audio::audioDeviceBufferSize, setup.bufferSize);
    setup.sampleRate = root.getProperty(Audio::audioDeviceRate, setup.sampleRate);

    String currentDeviceType = root.getProperty(Audio::audioDeviceType);

    AudioIODeviceType *foundType = nullptr;
    for (const auto availableType : availableDeviceTypes)
    {
        if (availableType->getTypeName() == currentDeviceType)
        {
            foundType = availableType;
        }
    }

    const bool settingsSeemValid = foundType != nullptr &&
        setup.outputDeviceName.isNotEmpty() && setup.sampleRate > 0;

    if (!settingsSeemValid)
    {
        this->autodetectAudioDeviceSetup();
        this->autodetectMidiDeviceSetup();
        return;
    }

    this->deviceManager.setCurrentAudioDeviceType(currentDeviceType, true);

    const static var defaultTwoChannels("11");
    const String inputChannels = root.getProperty(Audio::audioDeviceInputChannels, defaultTwoChannels);
    const String outputChannels = root.getProperty(Audio::audioDeviceOutputChannels, defaultTwoChannels);
    setup.inputChannels.parseString(inputChannels, 2);
    setup.outputChannels.parseString(outputChannels, 2);

    setup.useDefaultInputChannels = !root.hasProperty(Audio::audioDeviceInputChannels);
    setup.useDefaultOutputChannels = !root.hasProperty(Audio::audioDeviceOutputChannels);

    const auto initError = this->deviceManager.setAudioDeviceSetup(setup, true);
    if (initError.isNotEmpty())
    {
        this->autodetectAudioDeviceSetup();
        this->autodetectMidiDeviceSetup();
        return;
    }

    const auto midiInputId = root.getProperty(Audio::midiInputId).toString();
    const auto midiInputName = root.getProperty(Audio::midiInputName).toString();

    this->isReadjustingMidiInput = root.getProperty(Audio::midiInputReadjusting,
        this->isReadjustingMidiInput.get());
    
    // first, try to match by device id; if failed, search by name
    bool hasFoundMidiInById = false;
    const auto allMidiInputs = MidiInput::getAvailableDevices();

    if (midiInputId.isNotEmpty())
    {
        for (const auto &midiIn : allMidiInputs)
        {
            const auto shouldEnable = midiIn.identifier == midiInputId;
            this->deviceManager.setMidiInputDeviceEnabled(midiIn.identifier, shouldEnable);
            hasFoundMidiInById = hasFoundMidiInById || shouldEnable;
        }
    }

    if (!hasFoundMidiInById && midiInputName.isNotEmpty())
    {
        for (const auto &midiIn : allMidiInputs)
        {
            const auto shouldEnable = midiIn.name == midiInputName;
            this->deviceManager.setMidiInputDeviceEnabled(midiIn.identifier, shouldEnable);
        }
    }

    // also try to find the MIDI output by device id and then by name
    const auto midiOutputId = root.getProperty(Audio::midiOutputId).toString();
    const auto midiOutputName = root.getProperty(Audio::midiOutputName).toString();

    bool hasFoundMidiOutById = false;
    const auto allMidiOutputs = MidiOutput::getAvailableDevices();

    if (midiOutputId.isNotEmpty())
    {
        for (const auto &midiOut : allMidiOutputs)
        {
            if (midiOut.identifier == midiOutputId)
            {
                this->deviceManager.setDefaultMidiOutputDevice(midiOut.identifier);
                hasFoundMidiOutById = true;
                break;
            }
        }
    }

    if (!hasFoundMidiOutById && midiOutputName.isNotEmpty())
    {
        for (const auto &midiOut : allMidiOutputs)
        {
            if (midiOut.name == midiOutputName)
            {
                this->deviceManager.setDefaultMidiOutputDevice(midiOut.identifier);
                break;
            }
        }
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData AudioCore::serialize() const
{
    using namespace Serialization;

    // serializes all settings and instruments (with their graphs)
    // deviceManager's graph is not serialized but managed dynamically

    SerializedData orchestra(Audio::orchestra);
    for (int i = 0; i < this->instruments.size(); ++i)
    {
        auto *instrument = this->instruments.getUnchecked(i);
        orchestra.appendChild(instrument->serialize());
    }

    SerializedData root(Audio::audioCore);
    root.appendChild(orchestra);
    root.appendChild(this->serializeDeviceManager());
    return root;
}

void AudioCore::deserialize(const SerializedData &data)
{
    using namespace Serialization;

    // re-creates deviceManager's graph each time on deserialization
    this->reset();

    const auto root = data.hasType(Audio::audioCore) ?
        data : data.getChildWithName(Audio::audioCore);

    if (!root.isValid())
    {
        this->autodetectAudioDeviceSetup();
        this->autodetectMidiDeviceSetup();
        return;
    }

    this->deserializeDeviceManager(root);

    const auto orchestra = root.getChildWithName(Audio::orchestra);
    if (orchestra.isValid())
    {
        for (const auto &instrumentNode : orchestra)
        {
            auto instrument = make<Instrument>(this->formatManager, "");

            // it's important to add audio processor to device
            // before actually creating nodes and connections:
            this->addInstrumentToAudioDevice(instrument.get());
            instrument->deserialize(instrumentNode);

            // try to filter out the trash early:
            if (instrument->getName().isEmpty() || instrument->getNumNodes() == 0)
            {
                this->removeInstrumentFromAudioDevice(instrument.get());
            }
            else
            {
                // try to detect the built-in instruments
                if (this->defaultInstrument == nullptr && instrument->isDefaultInstrument())
                {
                    this->defaultInstrument = instrument.get();
                }
                else if (this->metronomeInstrument == nullptr && instrument->isMetronomeInstrument())
                {
                    this->metronomeInstrument = instrument.get();
                }

                this->instruments.add(instrument.release());
                this->broadcastAddInstrument(this->instruments.getLast());
            }
        }
    }

    // add the required built-in instruments if they haven't been found already
    this->initBuiltInInstrumentsIfNeeded();
}

void AudioCore::reset()
{
    this->defaultInstrument = nullptr;
    this->metronomeInstrument = nullptr;

    while (!this->instruments.isEmpty())
    {
        this->removeInstrument(this->instruments[0]);
    }

    this->filteredMidiCallbacks.clear();
}
