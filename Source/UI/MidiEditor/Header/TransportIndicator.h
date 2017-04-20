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
class MidiRoll;
class MovementListener;

#include "TransportListener.h"

class TransportIndicator :
    public Component,
    public TransportListener,
    private AsyncUpdater,
    private Timer
{
public:

    class MovementListener
    {
    public:
        virtual ~MovementListener() {}
        virtual void onTransportIndicatorMoved(int indicatorX) = 0;
    };

    TransportIndicator(MidiRoll &parentRoll,
                       Transport &owner,
                       TransportIndicator::MovementListener *movementListener = nullptr,
                       int width = 2);

    ~TransportIndicator() override;


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

    MidiRoll &roll;

    Transport &transport;

    int indicatorWidth;

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

    MovementListener *listener;

};
