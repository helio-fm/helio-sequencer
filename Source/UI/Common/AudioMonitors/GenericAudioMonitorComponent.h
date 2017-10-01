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

class GenericAudioMonitorComponent : public Component, private Thread, private AsyncUpdater
{
public:

    explicit GenericAudioMonitorComponent(WeakReference<AudioMonitor> monitor);
    ~GenericAudioMonitorComponent() override;
    
    void setTargetAnalyzer(WeakReference<AudioMonitor> monitor);
        
    void resized() override;
    void paint(Graphics &g) override;

private:
    
    class SpectrumBand
    {
    public:
        
        explicit SpectrumBand();
        
        void setDashedLineMode(bool shouldDrawDashedLine);
        void setValue(float value);
        void reset();
        
        inline void drawBand(Graphics &g, float xx, float yy, float w, float h);
        
    private:
                
        float value;
        float valueHold;
        float valueDecay;
        float peak;
        float peakHold;
        float peakDecay;
        
        float maxPeak;
        float averagePeak;

        bool drawsDashedLine;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumBand);
    };
    
private:
    
    void run() override;
    void handleAsyncUpdate() override;
    
    WeakReference<AudioMonitor> audioMonitor;
    OwnedArray<SpectrumBand> bands;

    int skewTime;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericAudioMonitorComponent);
};
