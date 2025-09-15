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

#include "ColourIDs.h"
#include "SequencerLayout.h"

class WaveformAudioMonitorComponent final :
    public Component,
    private Timer
{
public:

    explicit WaveformAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer);
    ~WaveformAudioMonitorComponent() override;

    void setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;

private:

    void timerCallback() override;

    const Colour peaksColour =
        findDefaultColour(ColourIDs::AudioMonitor::foreground)
            .withMultipliedAlpha(0.420f);

    const Colour rmsColour =
        findDefaultColour(ColourIDs::AudioMonitor::foreground)
            .withMultipliedAlpha(0.69f);

    WeakReference<AudioMonitor> audioMonitor;
    
    static constexpr auto bufferSize = Globals::UI::sidebarWidth / 2;

    float peakBufferLeft[bufferSize] = {};
    float peakBufferRight[bufferSize] = {};

    float rmsBufferLeft[bufferSize] = {};
    float rmsBufferRight[bufferSize] = {};

    int emptyFramesCounter = bufferSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformAudioMonitorComponent)

};
