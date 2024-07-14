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

#pragma once

class AudioMonitor;

#include "Instrument.h"
#include "OrchestraPit.h"
#include "KeyboardMapping.h"
#include "Scale.h"

class AudioCore final :
    public OrchestraPit,
    public Serializable,
    public ChangeBroadcaster
{
public:

    static void initAudioFormats(AudioPluginFormatManager &formatManager);

    AudioCore();
    ~AudioCore() override;

    void resetActiveMidiPlayer();
    void setActiveMidiPlayer(const String &instrumentId,
        int periodSize, Scale::Ptr chromaticMapping, bool forceReconnect);

    void disconnectAllAudioCallbacks();
    void reconnectAllAudioCallbacks();

    //===------------------------------------------------------------------===//
    // OrchestraPit
    //===------------------------------------------------------------------===//

    void removeInstrument(Instrument *instrument) override;
    void addInstrument(const PluginDescription &pluginDescription,
        const String &name, Instrument::InitializationCallback callback) override;
    Instrument *addBuiltInInstrument(const PluginDescription &pluginDescription,
        const String &name) override;

    Array<Instrument *> getInstruments() const override;

    // returns all instruments which can be used in the project;
    // at the moment the only exception is the internal metronome,
    // which is visible in the orchestra pit, but not in the menus:
    Array<Instrument *> getInstrumentsExceptInternal() const;

    Instrument *findInstrumentById(const String &id) const override;
    Instrument *getDefaultInstrument() const noexcept override;
    Instrument *getMidiOutputInstrument() const noexcept override;
    Instrument *getMetronomeInstrument() const noexcept override;
    String getMetronomeInstrumentId() const noexcept;

    void initRequiredInstruments();
    void initFirstLaunchInstruments();

    //===------------------------------------------------------------------===//
    // Setup
    //===------------------------------------------------------------------===//

    bool autodetectAudioDeviceSetup();
    bool autodetectMidiDeviceSetup();

    AudioDeviceManager &getDevice() noexcept;
    AudioPluginFormatManager &getFormatManager() noexcept;
    AudioMonitor *getMonitor() const noexcept;

    //===------------------------------------------------------------------===//
    // MIDI input/output
    //===------------------------------------------------------------------===//

    bool isFilteringMidiInput() const noexcept;
    void setFilteringMidiInput(bool isOn) noexcept;
    void addFilteredMidiInputCallback(Instrument *instrument,
        int periodSize, Scale::Ptr chromaticMapping);
    void removeFilteredMidiInputCallback(Instrument *instrument);

    void sendMessageToMidiOutputNow(const MidiMessage &message);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    inline static float fastLog2(float val)
    {
        int *const exp_ptr = reinterpret_cast<int *>(&val);
        int x = *exp_ptr;
        const int log_2 = ((x >> 23) & 255) - 128;
        x &= ~(255 << 23);
        x += 127 << 23;
        *exp_ptr = x;
        return (val + log_2);
    }
    
    inline static float fastLog10(float val)
    {
        return fastLog2(val) / fastLog2(10.f);
    }

    inline static float iecLevel(float db)
    {
        if (db < -70.f) { return 0.f; }
        else if (db < -60.f) { return (db + 70.f) * 0.0025f; }
        else if (db < -50.f) { return (db + 60.f) * 0.005f + 0.025f; }
        else if (db < -40.f) { return (db + 50.f) * 0.0075f + 0.075f; }
        else if (db < -30.f) { return (db + 40.f) * 0.015f + 0.15f; }
        else if (db < -20.f) { return (db + 30.f) * 0.02f + 0.3f; }
        /* else if (dB < 0.f) */
        return (db + 20.f) * 0.025f + 0.5f;
    }
    
private:

    Instrument *addMidiOutputInstrument(const String &name);

    void addInstrumentToMidiDevice(Instrument *instrument,
        int periodSize, Scale::Ptr chromaticMapping);
    void removeInstrumentFromMidiDevice(Instrument *instrument);

    void addInstrumentToAudioDevice(Instrument *instrument);
    void removeInstrumentFromAudioDevice(Instrument *instrument);

    SerializedData serializeDeviceManager() const;
    void deserializeDeviceManager(const SerializedData &tree);

    OwnedArray<Instrument> instruments;

    WeakReference<Instrument> defaultInstrument;
    WeakReference<Instrument> metronomeInstrument;
    WeakReference<Instrument> midiOutputInstrument;

    UniquePointer<AudioMonitor> audioMonitor;

    AudioPluginFormatManager formatManager;
    AudioDeviceManager deviceManager;

    Atomic<bool> isMuted = false;

private:

    // We want to make it possible to play/improvise with a standard 12-tone
    // physical keyboard and record MIDI regardless of the temperament used.
    // This filter uses current temperament's 'chromatic mapping' to readjust
    // the incoming MIDI data so that notes sound like their closest 12-tone equivalents.
    struct MidiRecordingKeyMapper final : public MidiInputCallback
    {
        MidiRecordingKeyMapper() = delete;
        MidiRecordingKeyMapper(const AudioCore &audioCore,
            WeakReference<Instrument> instrument,
            int periodSize, Scale::Ptr chromaticMapping) :
            audioCore(audioCore),
            instrument(instrument),
            periodSize(periodSize),
            chromaticMapping(chromaticMapping) {}

        void update(int newPeriodSize, Scale::Ptr newChromaticMapping)
        {
            this->periodSize = newPeriodSize;
            this->chromaticMapping = newChromaticMapping;
        }

        // note: this only fixes realtime playback,
        // MidiRecorder has to do the same thing on its own:
        int getMappedKey(int key) const noexcept
        {
            if (!this->audioCore.isFilteringMidiInput())
            {
                return key;
            }

            jassert(this->chromaticMapping->isValid());
            const auto periodNumber = key / Globals::twelveTonePeriodSize;
            const auto keyInPeriod = key % Globals::twelveTonePeriodSize;
            return this->chromaticMapping->getChromaticKey(keyInPeriod,
                this->periodSize * periodNumber, false);
        }

        void handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) override
        {
            MidiMessage mappedMessage(message);

            if (message.isNoteOn())
            {
                const auto keyInNewTemperament = this->getMappedKey(message.getNoteNumber());
                const auto mapped = this->instrument->getKeyboardMapping()->map(keyInNewTemperament, 1);
                mappedMessage = MidiMessage::noteOn(mapped.channel, mapped.key, message.getVelocity());
                mappedMessage.setTimeStamp(message.getTimeStamp());
            }
            else if (message.isNoteOff())
            {
                const auto keyInNewTemperament = this->getMappedKey(message.getNoteNumber());
                const auto mapped = this->instrument->getKeyboardMapping()->map(keyInNewTemperament, 1);
                mappedMessage = MidiMessage::noteOff(mapped.channel, mapped.key, message.getVelocity());
                mappedMessage.setTimeStamp(message.getTimeStamp());
            }

            this->instrument->getProcessorPlayer().
                getMidiMessageCollector().handleIncomingMidiMessage(source, mappedMessage);
        }

        const AudioCore &audioCore;
        WeakReference<Instrument> instrument;
        int periodSize = Globals::twelveTonePeriodSize;
        Scale::Ptr chromaticMapping;
    };

    Array<UniquePointer<MidiRecordingKeyMapper>> filteredMidiCallbacks;
    Atomic<bool> isReadjustingMidiInput = true;

    struct MidiPlayerInfo final
    {
        String instrumentId;
        int periodSize = Globals::twelveTonePeriodSize;
        Scale::Ptr chromaticMapping;
    };

    MidiPlayerInfo lastActiveMidiPlayer;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore)
    JUCE_DECLARE_WEAK_REFERENCEABLE(AudioCore)
};
