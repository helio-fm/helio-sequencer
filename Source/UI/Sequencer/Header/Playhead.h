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

#include "ColourIDs.h"
#include "TransportListener.h"

class Playhead :
    public Component,
    public TransportListener,
    public Timer,
    private AsyncUpdater
{
public:

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void onMovePlayhead(int oldX, int newX) = 0;
    };

    Playhead(RollBase &parentRoll, Transport &owner,
        Playhead::Listener *movementListener = nullptr);

    ~Playhead() override;

    void updatePosition();
    virtual void updatePosition(float position);

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

protected:

    RollBase &roll;
    Transport &transport;

    Listener *listener = nullptr;

    void handleAsyncUpdate() override;

    // warning: spinlock is not reentrant, use carefully;
    // for now it synchronizes updates in TransportListener callbacks
    // coming from the background thread with the timer callback
    // on the main thread, so that position changes are smooth
    SpinLock playbackUpdatesLock;

    // it's meant to lock these 4 fields:
    float beatAnchor = 0.f;
    uint32 timeAnchor = 0;
    double msPerQuarterNote = Globals::Defaults::msPerBeat;
    float lastCorrectBeat = 0.f;

    float lastEstimatedBeat = 0.f;
    float calculateEstimatedBeat() const noexcept;

    Colour currentColour;

    const Colour shadeColour =
        findDefaultColour(ColourIDs::Roll::playheadShade);
    const Colour playbackColour =
        findDefaultColour(ColourIDs::Roll::playheadPlayback);
    const Colour recordingColour =
        findDefaultColour(ColourIDs::Roll::playheadRecording);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Playhead)
};

class PlayheadSmall final : public Playhead
{
public:

    PlayheadSmall(RollBase &parentRoll, Transport &owner);

    void parentSizeChanged() override;
    void paint(Graphics &g) override;

    void updatePosition(float position) override;
    void onStop() override;

protected:

    const Colour playbackColour =
        findDefaultColour(ColourIDs::Roll::playheadSmallPlayback);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayheadSmall)
};
