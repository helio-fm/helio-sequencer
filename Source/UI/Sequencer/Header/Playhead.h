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
    public Timer
{
public:

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void onMovePlayhead(int oldX, int newX) = 0;
    };

    Playhead(RollBase &parentRoll,
        Transport &owner,
        Playhead::Listener *movementListener = nullptr,
        float alpha = 1.f);

    ~Playhead() override;

    void updatePosition();
    void updatePosition(float position);

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(float beatPosition) override;
    void onCurrentTempoChanged(double msPerQuarter) override;
    void onTotalTimeChanged(double timeMs) override {}
    void onLoopModeChanged(bool hasLoop, float start, float end) override {}

    void onPlay() override;
    void onRecord() override;
    void onStop() override;

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void parentSizeChanged() override;
    void parentHierarchyChanged() override;

private:

    RollBase &roll;
    Transport &transport;

    Listener *listener = nullptr;

private:

    void parentChanged();

    // warning: spinlock is not reentrant, use carefully;
    // for now it synchronizes updates in TransportListener callbacks
    // coming from the background thread with the timer callback
    // on the main thread, so that position changes are smooth
    SpinLock playbackUpdatesLock;

    // it's meant to lock these 4 fields:
    float beatAnchor = 0.f;
    double timeAnchor = 0.0;
    double msPerQuarterNote = Globals::Defaults::msPerBeat;
    float lastCorrectBeat = 0.f;

    float lastEstimatedBeat = 0.f;
    float calculateEstimatedBeat() const noexcept;

private:

    Colour currentColour;

    const Colour shadeColour;
    const Colour playbackColour;
    const Colour recordingColour;

};
