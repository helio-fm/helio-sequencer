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

#include "ColourIDs.h"
#include "TransportListener.h"
#include "RollEditMode.h"

class KeyboardCursor :
    public Component,
    public TransportListener,
    public ChangeListener, // subscribes on lasso component's changes
    public Timer,
    private AsyncUpdater
{
public:

    explicit KeyboardCursor(Transport &transport);
    ~KeyboardCursor() override;

    void parentHierarchyChanged() override;

    virtual void moveLeft() = 0;
    virtual void moveRight() = 0;
    virtual void moveUp() = 0;
    virtual void moveDown() = 0;

    virtual void selectLeft() = 0;
    virtual void selectRight() = 0;
    virtual void selectUp() = 0;
    virtual void selectDown() = 0;

    virtual void updateBounds() = 0;

    // e.g. { x + 1, y + centreY }
    virtual Point<int> getTargetPoint() const = 0;

    virtual void interact(RollEditMode editMode) = 0;

    float blinkAlpha = 1.f;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(float beatPosition) override;
    void onCurrentTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override {}
    void onLoopModeChanged(bool hasLoop, float start, float end) override {}

    void onPlay() override;
    void onRecord() override;
    void onStop() override;

protected:

    Transport &transport;

    Atomic<bool> transportIsPlayingOrRecording = false;

    class BlinkAnimator;
    UniquePointer<BlinkAnimator> blinkAnimator;

    void timerCallback() override;

    void handleAsyncUpdate() override;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void showAndRestartBlinking();

    void paintSelectionMarkers(Graphics &g,
        const Rectangle<float> &b, bool axis); // axis for tl+br or tr+bl

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::cursorFill);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::cursorShade);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardCursor)
    JUCE_DECLARE_WEAK_REFERENCEABLE(KeyboardCursor)
};
