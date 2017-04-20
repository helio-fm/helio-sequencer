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

class Instrument;
class AudioMonitor;

#include "Serializable.h"
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

    Instrument *addInstrument(const PluginDescription &pluginDescription, const String &name);
    void removeInstrument(Instrument *instrument);

    //===------------------------------------------------------------------===//
    // OrchestraPit
    //===------------------------------------------------------------------===//

    Array<Instrument *> getInstruments() const override;
    Instrument *findInstrumentById(const String &id) const override;
    void initDefaultInstrument();

    //===------------------------------------------------------------------===//
    // Setup
    //===------------------------------------------------------------------===//

    void autodetect();
    AudioDeviceManager &getDevice() noexcept;
    AudioPluginFormatManager &getFormatManager() noexcept;
    AudioMonitor *getMonitor() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
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
    
private:

    void addInstrumentToDevice(Instrument *instrument);
    void removeInstrumentFromDevice(Instrument *instrument);

    OwnedArray<Instrument> instruments;
    ScopedPointer<AudioMonitor> audioMonitor;

    AudioPluginFormatManager formatManager;
    AudioDeviceManager deviceManager;
    
    WeakReference<AudioCore>::Master masterReference;
    friend class WeakReference<AudioCore>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore);

};
