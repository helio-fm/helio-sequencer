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

class Transport;
class RollBase;

#include "TransportListener.h"

class Playhead final :
    public Component,
    public TransportListener,
    private AsyncUpdater,
    private Timer
{
public:

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void onPlayheadMoved(int playheadX) = 0;
    };

    Playhead(RollBase &parentRoll,
        Transport &owner,
        Playhead::Listener *movementListener = nullptr,
        float alpha = 1.f);

    ~Playhead() override;

    void updatePosition();

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(float beatPosition, double currentTimeMs) override;
    void onCurrentTempoChanged(double msPerQuarter) override;
    void onTotalTimeChanged(double timeMs) override {}
    void onLoopModeChanged(bool hasLoop, float start, float end) override {}

    void onPlay() override;
    void onRecord() override;
    void onStop() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void parentSizeChanged() override;
    void parentHierarchyChanged() override;

protected:

    RollBase &roll;
    Transport &transport;

private:

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;
    void tick();

    void parentChanged();

    Atomic<float> beatAnchor = 0.f;
    Atomic<double> timeAnchor = 0.0;
    Atomic<double> msPerQuarterNote = Globals::Defaults::msPerBeat;

private:

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;
    void updatePosition(double position);

    Colour currentColour;

    const Colour shadeColour;
    const Colour playbackColour;
    const Colour recordingColour;

    Atomic<float> lastCorrectBeat = 0.f;

    Listener *listener = nullptr;

};
