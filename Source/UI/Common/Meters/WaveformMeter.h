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

// The same as component width (and the same as sidebar width):
#define WAVEFORM_METER_BUFFER_SIZE 72

class WaveformMeter : public Component, private Thread, private AsyncUpdater
{
public:

    WaveformMeter(WeakReference<AudioMonitor> targetAnalyzer);
    ~WaveformMeter() override;

    void setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;

private:

    void run() override;
    void handleAsyncUpdate() override;
    
    WeakReference<AudioMonitor> volumeAnalyzer;
    
    Atomic<float> lPeakBuffer[WAVEFORM_METER_BUFFER_SIZE];
    Atomic<float> rPeakBuffer[WAVEFORM_METER_BUFFER_SIZE];

    Atomic<float> lRmsBuffer[WAVEFORM_METER_BUFFER_SIZE];
    Atomic<float> rRmsBuffer[WAVEFORM_METER_BUFFER_SIZE];

    int skewTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformMeter)

};
