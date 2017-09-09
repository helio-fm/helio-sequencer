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

class VolumePeakMeter : public Component, private Thread, private AsyncUpdater
{
public:

    enum Orientation
    {
        Left,
        Right
    };
    
    VolumePeakMeter(WeakReference<AudioMonitor> targetAnalyzer,
                    int targetChannel,
                    Orientation bandOrientation);
    
    ~VolumePeakMeter() override;

    void setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;

private:

    class Band
    {
    public:

        explicit Band();

        void setValue(float value);
        void reset();

        inline void drawBand(Graphics &g, float left, float right, float height);

    private:

        float value;
        float valueHold;
        float valueDecay;
        float peak;
        float peakHold;
        float peakDecay;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Band);
    };

private:

    void run() override;
    void handleAsyncUpdate() override;
    
    WeakReference<AudioMonitor> volumeAnalyzer;
    
    Band peakBand;
    
    int channel;
    int skewTime;
    Orientation orientation;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumePeakMeter)

};
