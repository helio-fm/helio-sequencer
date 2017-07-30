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
		int width = 2);

    ~Playhead() override;


    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(const double newPosition,
                        const double currentTimeMs,
                        const double totalTimeMs) override;

    void onTempoChanged(const double newTempo) override;

    void onTotalTimeChanged(const double timeMs) override;

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

    ReadWriteLock anchorsLock;
    double timerStartTime;
    double timerStartPosition;
    double tempo;

private:

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;


    void updatePosition(double position);

    ReadWriteLock lastCorrectPositionLock;
    double lastCorrectPosition;

    Listener *listener;

};
