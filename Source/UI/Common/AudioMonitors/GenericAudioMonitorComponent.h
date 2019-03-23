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

#include "AudioMonitor.h"

#define GENERIC_METER_NUM_BANDS 11

class GenericAudioMonitorComponent : public Component, private Thread, private AsyncUpdater
{
public:

    explicit GenericAudioMonitorComponent(WeakReference<AudioMonitor> monitor);
    ~GenericAudioMonitorComponent() override;
    
    void setTargetAnalyzer(WeakReference<AudioMonitor> monitor);
        
    void resized() override;
    void paint(Graphics &g) override;

private:
    
    class SpectrumBand final
    {
    public:
        
        SpectrumBand();
        void reset();
        inline void processSignal(float v, float h, uint32 timeNow);
        
        float value;
        float valueDecay;
        uint32 valueDecayStart;

        float peak;
        float peakDecay;
        float peakDecayColour;
        uint32 peakDecayStart;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumBand);
    };
    
private:
    
    void run() override;
    void handleAsyncUpdate() override;
    
    const Colour colour;

    WeakReference<AudioMonitor> audioMonitor;
    OwnedArray<SpectrumBand> bands;

    ScopedPointer<SpectrumBand> lPeakBand;
    ScopedPointer<SpectrumBand> rPeakBand;

    Atomic<float> values[GENERIC_METER_NUM_BANDS];
    Atomic<float> lPeak;
    Atomic<float> rPeak;

    int skewTime;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericAudioMonitorComponent);
};
