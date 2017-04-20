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

class SpectrumComponent : public Component, private Thread, private AsyncUpdater
{
public:

    explicit SpectrumComponent(WeakReference<AudioMonitor> monitor);
    ~SpectrumComponent() override;
    
    void setTargetAnalyzer(WeakReference<AudioMonitor> monitor);
    
    inline float iecLevel(const float dB) const;
    inline int iecScale(const float dB) const;
    
    void setPeakFalloff(const int peakFalloff);
    int getPeakFalloff() const;
    
    void resized() override;
    void paint(Graphics &g) override;
    void mouseUp(const MouseEvent& event) override;

private:
    
    class SpectrumBand
    {
    public:
        
        explicit SpectrumBand(SpectrumComponent *parent);
        
        void setDashedLineMode(bool shouldDrawDashedLine);
        void setValue(float value);
        void reset();
        
        inline void drawBand(Graphics &g, float xx, float yy, float w, float h);
        
    private:
        
        SpectrumComponent *meter;
        
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
    
    bool isCompactMode() const;
    
    WeakReference<AudioMonitor> audioMonitor;
    OwnedArray<SpectrumBand> bands;
    const float *spectrumFrequencies;
    
    int peakFalloff;
    int bandCount;
    int skewTime;
    
    bool altMode;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumComponent);
};
