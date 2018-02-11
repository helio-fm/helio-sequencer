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
#include "DataEncoder.h"
#include "SerializationKeys.h"
#include "AudioMonitor.h"
#include "AudiobusOutput.h"

void AudioCore::initAudioFormats(AudioPluginFormatManager &formatManager)
{
    formatManager.addDefaultFormats();
    formatManager.addFormat(new InternalPluginFormat());
    formatManager.addFormat(new BuiltInSynthFormat());
}

AudioCore::AudioCore()
{
    Logger::writeToLog("AudioCore::AudioCore");

    this->audioMonitor = new AudioMonitor();
    this->deviceManager.addAudioCallback(this->audioMonitor);

    AudioCore::initAudioFormats(this->formatManager);

    // requesting 0 inputs and only 2 outputs because of fucking alsa
    this->deviceManager.initialise(0, 2, nullptr, true);

    this->autodetect();

#if HELIO_AUDIOBUS_SUPPORT
    AudiobusOutput::init();
#endif
}

AudioCore::~AudioCore()
{
#if HELIO_AUDIOBUS_SUPPORT
    AudiobusOutput::shutdown();
#endif

    this->deviceManager.removeAudioCallback(this->audioMonitor);
    this->audioMonitor = nullptr;

    //ScopedPointer<XmlElement> test(this->metaInstrument->serialize());
    //DataEncoder::saveObfuscated(File("111.txt"), test);

    this->deviceManager.closeAudioDevice();
    this->masterReference.clear();
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

Instrument *AudioCore::addInstrument(const PluginDescription &pluginDescription,
                                     const String &name)
{
    auto instrument = new Instrument(formatManager, name);
    this->addInstrumentToDevice(instrument);

    instrument->initializeFrom(pluginDescription);
    this->instruments.add(instrument);

    this->broadcastInstrumentAdded(instrument);

    return instrument;
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
    this->deviceManager.addMidiInputCallback(String::empty, &instrument->getProcessorPlayer().getMidiMessageCollector());
}

void AudioCore::removeInstrumentFromDevice(Instrument *instrument)
{
    this->deviceManager.removeAudioCallback(&instrument->getProcessorPlayer());
    this->deviceManager.removeMidiInputCallback(String::empty, &instrument->getProcessorPlayer().getMidiMessageCollector());
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
    this->addInstrument(desc, "Default");
}

//===----------------------------------------------------------------------===//
// Setup
//===----------------------------------------------------------------------===//

void AudioCore::autodetect()
{
    Logger::writeToLog("AudioCore::autodetect");

    AudioIODeviceType *device_type =
        this->deviceManager.getCurrentDeviceTypeObject();

    AudioIODevice *device =
        this->deviceManager.getCurrentAudioDevice();

    if (!device_type || !device)
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

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AudioCore::serialize() const
{
    Logger::writeToLog("AudioCore::serialize");

    // сериализовать настройки
    // сериализовать все инструменты вместе с их графами
    // мета-граф не трогаем

    ValueTree tree(Serialization::Core::audioCore);

    {
        ValueTree orchestra(Serialization::Core::orchestra);

        for (int i = 0; i < this->instruments.size(); ++i)
        {
            Instrument *instrument = this->instruments.getUnchecked(i);
            orchestra.appendChild(instrument->serialize());
        }

        tree.appendChild(orchestra);
    }

    {
        ValueTree settings(Serialization::Core::audioSettings);
        ScopedPointer<XmlElement> deviceStateXml(this->deviceManager.createStateXml());
        // FIXME do not rely upon JUCE's xml serialization here:
        const String deviceStateString = deviceStateXml->createDocument();
        settings.setProperty(Serialization::Core::audioDevice, deviceStateString);
        tree.appendChild(settings);
    }

    return tree;
}

void AudioCore::deserialize(const ValueTree &tree)
{
    Logger::writeToLog("AudioCore::deserialize");

    // при десериализации пересоздаем мета-граф заново

    this->reset();

    const auto root = tree.hasType(Serialization::Core::audioCore) ?
    tree : tree.getChildWithName(Serialization::Core::audioCore);

    if (!root.isValid()) { return; }


    const auto orchestra = root.getChildWithName(Serialization::Core::orchestra);
    if (orchestra.isValid())
    {
        for (const auto &instrumentNode : orchestra)
        {
            //Logger::writeToLog("--- instrument ---");
            //Logger::writeToLog(instrumentNode->createDocument(""));
            Instrument *instrument = new Instrument(this->formatManager, "");
            this->addInstrumentToDevice(instrument);
            instrument->deserialize(instrumentNode);
            this->instruments.add(instrument);
        }
    }


    const auto audioSettings = root.getChildWithName(Serialization::Core::audioSettings);
    const String deviceState = audioSettings.getProperty(Serialization::Core::audioDevice);
    if (audioSettings.isValid())
    {
        Logger::writeToLog("--- setup ---");
        Logger::writeToLog(deviceState);
        Logger::writeToLog("--- setup ---");
        AudioDeviceManager &device = this->getDevice();
        const ScopedPointer<XmlElement> deviceStateXml(XmlDocument::parse(deviceState));
        device.initialise(0, 2, audioSettings.getChild(0), true);
        return;
    }

    this->autodetect();
}

void AudioCore::reset()
{
    while (this->instruments.size() > 0)
    {
        this->removeInstrument(this->instruments[0]);
    }
}
