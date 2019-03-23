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

class AudioCore :
    public Serializable,
    public ChangeBroadcaster,
    public OrchestraPit
{
public:

    static void initAudioFormats(AudioPluginFormatManager &formatManager);

    AudioCore();
    ~AudioCore() override;
    
    void mute();
    void unmute();

    //===------------------------------------------------------------------===//
    // Instruments
    //===------------------------------------------------------------------===//

    void removeInstrument(Instrument *instrument);
    void addInstrument(const PluginDescription &pluginDescription,
        const String &name, Instrument::InitializationCallback callback);

    //===------------------------------------------------------------------===//
    // OrchestraPit
    //===------------------------------------------------------------------===//

    Array<Instrument *> getInstruments() const override;
    Instrument *findInstrumentById(const String &id) const override;
    void initDefaultInstrument();

    //===------------------------------------------------------------------===//
    // Setup
    //===------------------------------------------------------------------===//

    void autodetectDeviceSetup();

    AudioDeviceManager &getDevice() noexcept;
    AudioPluginFormatManager &getFormatManager() noexcept;
    AudioMonitor *getMonitor() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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

    inline static float iecLevel(float dB)
    {
        float fDef = 1.f;

        if (dB < -70.f) {
            fDef = 0.f;
        } else if (dB < -60.f) {
            fDef = (dB + 70.f) * 0.0025f;
        } else if (dB < -50.f) {
            fDef = (dB + 60.f) * 0.005f + 0.025f;
        } else if (dB < -40.f) {
            fDef = (dB + 50.f) * 0.0075f + 0.075f;
        } else if (dB < -30.f) {
            fDef = (dB + 40.f) * 0.015f + 0.15f;
        } else if (dB < -20.f) {
            fDef = (dB + 30.f) * 0.02f + 0.3f;
        } else { // if (dB < 0.f)
            fDef = (dB + 20.f) * 0.025f + 0.5f;
        }

        return fDef;
    }
    
private:

    void addInstrumentToDevice(Instrument *instrument);
    void removeInstrumentFromDevice(Instrument *instrument);

    ValueTree serializeDeviceManager() const;
    void deserializeDeviceManager(const ValueTree &tree);

    OwnedArray<Instrument> instruments;
    ScopedPointer<AudioMonitor> audioMonitor;

    AudioPluginFormatManager formatManager;
    AudioDeviceManager deviceManager;

    StringArray customMidiInputs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore)
    JUCE_DECLARE_WEAK_REFERENCEABLE(AudioCore)
};
