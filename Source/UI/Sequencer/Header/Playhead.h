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

class Transport;
class HybridRoll;
class MovementListener;

#include "TransportListener.h"

class Playhead :
    public Component,
    public TransportListener,
    private AsyncUpdater,
    private Timer
{
public:

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void onPlayheadMoved(int indicatorX) = 0;
    };

    Playhead(HybridRoll &parentRoll,
        Transport &owner,
        Playhead::Listener *movementListener = nullptr,
        int width = 1);

    ~Playhead() override;


    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(double absolutePosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override;
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void parentSizeChanged() override;
    void parentHierarchyChanged() override;

protected:

    HybridRoll &roll;
    Transport &transport;

    int playheadWidth;

private:

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;
    void tick();

    void parentChanged();

    SpinLock anchorsLock;
    double timerStartTime;
    double timerStartPosition;
    double msPerQuarterNote;

private:

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;

    void updatePosition(double position);

    Colour mainColour;
    Colour shadeColour;

    SpinLock lastCorrectPositionLock;
    double lastCorrectPosition;

    Listener *listener;

};
