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

class SpectrogramAudioMonitorComponent final :
    public Component, private Thread, private AsyncUpdater
{
public:

    explicit SpectrogramAudioMonitorComponent(WeakReference<AudioMonitor> monitor);
    ~SpectrogramAudioMonitorComponent() override;
    
    void setTargetAnalyzer(WeakReference<AudioMonitor> monitor);
        
    void resized() override;
    void paint(Graphics &g) override;

private:
    
    class SpectrumBand final
    {
    public:
        
        SpectrumBand() = default;
        void reset();
        inline void processSignal(float v, float h, uint32 timeNow);
        
        static constexpr auto maxAlpha = 0.35f;
        static constexpr auto bandFadeMs = 650;
        static constexpr auto peakFadeMs = 1300;

        float value = 0.f;
        float valueDecay = 1.f;
        uint32 valueDecayStart = 0;

        float peak = 0.f;
        float peakDecay = 1.f;
        float peakDecayColour = SpectrumBand::maxAlpha;
        uint32 peakDecayStart = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumBand);
    };
    
private:
    
    void run() override;
    void handleAsyncUpdate() override;
    
    const Colour colour;

    WeakReference<AudioMonitor> audioMonitor;
    OwnedArray<SpectrumBand> bands;

    UniquePointer<SpectrumBand> lPeakBand;
    UniquePointer<SpectrumBand> rPeakBand;

    static constexpr auto numBands = 11;
    Atomic<float> values[SpectrogramAudioMonitorComponent::numBands];

    Atomic<float> lPeak;
    Atomic<float> rPeak;

    int skewTime = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramAudioMonitorComponent)
};
