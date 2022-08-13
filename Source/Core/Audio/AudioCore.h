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

class AudioMonitor;

#include "Instrument.h"
#include "OrchestraPit.h"
#include "Scale.h"

class SleepTimer : private Timer
{
public:

    void setCanSleepAfter(int timeoutMs = 0)
    {
        if (timeoutMs < 0)
        {
            this->stopTimer();
            this->awakeNow();
        }
        else if (timeoutMs == 0)
        {
            this->stopTimer();
            this->sleepNow();
        }
        else
        {
            this->startTimer(timeoutMs);
        }
    }

    void setAwake()
    {
        this->stopTimer();
        this->awakeNow();
    }

protected:

    virtual bool canSleepNow() = 0;
    virtual void sleepNow() = 0;
    virtual void awakeNow() = 0;

private:

    void timerCallback() override
    {
        if (this->canSleepNow())
        {
            this->stopTimer();
            this->sleepNow();
        }
    }
};

class AudioCore final :
    public Serializable,
    public ChangeBroadcaster,
    public OrchestraPit,
    public SleepTimer
{
public:

    static void initAudioFormats(AudioPluginFormatManager &formatManager);

    AudioCore();
    ~AudioCore() override;

    void resetActiveMidiPlayer();
    void setActiveMidiPlayer(const String &instrumentId,
        int periodSize, Scale::Ptr chromaticMapping, bool forceReconnect);

    //===------------------------------------------------------------------===//
    // OrchestraPit
    //===------------------------------------------------------------------===//

    void removeInstrument(Instrument *instrument) override;
    void addInstrument(const PluginDescription &pluginDescription,
        const String &name, Instrument::InitializationCallback callback) override;

    Array<Instrument *> getInstruments() const override;

    // returns all instruments which can be used in the project;
    // at the moment the only exception is the internal metronome,
    // which is visible in the orchestra pit, but not in the menus:
    Array<Instrument *> getInstrumentsExceptInternal() const;

    Instrument *findInstrumentById(const String &id) const override;
    Instrument *getDefaultInstrument() const noexcept override;
    Instrument *getMetronomeInstrument() const noexcept override;
    String getMetronomeInstrumentId() const noexcept;
    void initBuiltInInstrumentsIfNeeded();

    //===------------------------------------------------------------------===//
    // Setup
    //===------------------------------------------------------------------===//

    bool autodetectAudioDeviceSetup();
    bool autodetectMidiDeviceSetup();

    AudioDeviceManager &getDevice() noexcept;
    AudioPluginFormatManager &getFormatManager() noexcept;
    AudioMonitor *getMonitor() const noexcept;

    //===------------------------------------------------------------------===//
    // MIDI input filtering
    //===------------------------------------------------------------------===//

    void addFilteredMidiInputCallback(MidiInputCallback *callback,
        int periodSize, Scale::Ptr chromaticMapping);
    void removeFilteredMidiInputCallback(MidiInputCallback *callback);
    bool isFilteringMidiInput() const noexcept;
    void setFilteringMidiInput(bool isOn) noexcept;

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
        auto result = 1.f;
        if (db < -70.f) { result = 0.f; }
        else if (db < -60.f) { result = (db + 70.f) * 0.0025f; }
        else if (db < -50.f) { result = (db + 60.f) * 0.005f + 0.025f; }
        else if (db < -40.f) { result = (db + 50.f) * 0.0075f + 0.075f; }
        else if (db < -30.f) { result = (db + 40.f) * 0.015f + 0.15f; }
        else if (db < -20.f) { result = (db + 30.f) * 0.02f + 0.3f; }
        else /* if (dB < 0.f) */ { result = (db + 20.f) * 0.025f + 0.5f; }
        return result;
    }
    
private:

    bool canSleepNow() noexcept override;
    void sleepNow() override;
    void awakeNow() override;
    void disconnectAllAudioCallbacks();
    void reconnectAllAudioCallbacks();

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
            MidiInputCallback *targetCallback,
            int periodSize, Scale::Ptr chromaticMapping) :
            audioCore(audioCore),
            targetInstrumentCallback(targetCallback),
            periodSize(periodSize),
            chromaticMapping(chromaticMapping) {}

        void update(int newPeriodSize, Scale::Ptr newChromaticMapping)
        {
            this->periodSize = newPeriodSize;
            this->chromaticMapping = newChromaticMapping;
        }

        int getMappedKey(int key) const noexcept
        {
            if (!this->audioCore.isReadjustingMidiInput.get())
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
                mappedMessage = MidiMessage::noteOn(message.getChannel(),
                    this->getMappedKey(message.getNoteNumber()), message.getVelocity());
            }
            else if (message.isNoteOff())
            {
                mappedMessage = MidiMessage::noteOff(message.getChannel(),
                    this->getMappedKey(message.getNoteNumber()), message.getVelocity());
            }

            this->targetInstrumentCallback->handleIncomingMidiMessage(source, mappedMessage);
            jassert(this->targetInstrumentCallback != nullptr);
        }

    private:

        const AudioCore &audioCore;
        MidiInputCallback *targetInstrumentCallback = nullptr;
        int periodSize = Globals::twelveTonePeriodSize;
        Scale::Ptr chromaticMapping;
    };

    struct FilteredMidiCallback final
    {
        UniquePointer<MidiRecordingKeyMapper> filter;
        MidiInputCallback *targetCallback;
    };

    Array<FilteredMidiCallback> filteredMidiCallbacks;
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
