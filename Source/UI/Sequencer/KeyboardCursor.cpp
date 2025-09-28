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

#include "Common.h"
#include "KeyboardCursor.h"
#include "Transport.h"
#include "UserInterfaceFlags.h"

class KeyboardCursor::BlinkAnimator final : private Timer
{
public:

    explicit KeyboardCursor::BlinkAnimator(WeakReference<KeyboardCursor> cursor) :
        cursor(cursor) {}

    void fadeIn()
    {
        if (this->cursor->blinkAlpha == 1.f)
        {
            return;
        }

        this->targetAlpha = 1.f;
        this->startTimerHz(60);
    }

    void fadeOut()
    {
        if (this->cursor->blinkAlpha == 0.f)
        {
            return;
        }

        this->targetAlpha = 0.f;
        this->startTimerHz(60);
    }

    void cancelAllAnimations()
    {
        this->stopTimer();
    }

private:

    void timerCallback() override
    {
        const auto alphaDelta = this->targetAlpha - this->cursor->blinkAlpha;
        if (fabs(alphaDelta) > 0.01f)
        {
            this->cursor->blinkAlpha += (alphaDelta * 0.5f);
            this->cursor->repaint();
        }
        else
        {
            this->cursor->blinkAlpha = this->targetAlpha;
            this->cursor->repaint();
            this->stopTimer();
        }
    }

    float targetAlpha = 1.f;

    WeakReference<KeyboardCursor> cursor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardCursor::BlinkAnimator)
};

KeyboardCursor::KeyboardCursor(Transport &transport) :
    transport(transport)
{
    this->blinkAnimator = make<BlinkAnimator>(this);
    this->transport.addTransportListener(this);
}

KeyboardCursor::~KeyboardCursor()
{
    this->transport.removeTransportListener(this);
}

void KeyboardCursor::parentHierarchyChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->setAlwaysOnTop(true);
    }
}

void KeyboardCursor::timerCallback()
{
    if (this->transportIsPlayingOrRecording.get())
    {
        return;
    }

    if (!App::Config().getUiFlags()->areUiAnimationsEnabled())
    {
        this->stopTimer();
        this->blinkAnimator->cancelAllAnimations();
        this->blinkAlpha = 1.f;
        this->setVisible(true);
        this->repaint();
        return;
    }

    if (this->blinkAlpha > 0.f)
    {
        this->blinkAnimator->fadeOut();
    }
    else
    {
        this->blinkAnimator->fadeIn();
    }
}

void KeyboardCursor::showAndRestartBlinking()
{
    this->blinkAnimator->cancelAllAnimations();
    this->blinkAlpha = 1.f;

    this->setVisible(true);
    this->toFront(false);
    this->repaint();

    if (App::Config().getUiFlags()->areUiAnimationsEnabled())
    {
        this->startTimer(Globals::UI::cursorBlinkDelayMs);
    }
}

#define SELECTION_MARKER_SIZE (5.f)

static const auto makeTopLeftMarker = [](const Rectangle<float> &b) {
    Path p;
    const auto minSide = jmin(b.getWidth(), b.getHeight());
    const auto markerSize = jlimit(1.f, SELECTION_MARKER_SIZE, minSide / 2);
    p.addTriangle(b.getTopLeft(),
        b.getTopLeft().translated(markerSize, 0.f),
        b.getTopLeft().translated(0.f, markerSize));
    return p;
};

static const auto makeTopRightMarker = [](const Rectangle<float> &b) {
    Path p;
    const auto minSide = jmin(b.getWidth(), b.getHeight());
    const auto markerSize = jlimit(1.f, SELECTION_MARKER_SIZE, minSide / 2);
    p.addTriangle(b.getTopRight(),
        b.getTopRight().translated(0.f, markerSize),
        b.getTopRight().translated(-markerSize, 0.f));
    return p;
};

static const auto makeBottomLeftMarker = [](const Rectangle<float> &b) {
    Path p;
    const auto minSide = jmin(b.getWidth(), b.getHeight());
    const auto markerSize = jlimit(1.f, SELECTION_MARKER_SIZE, minSide / 2);
    p.addTriangle(b.getBottomLeft(),
        b.getBottomLeft().translated(0.f, -markerSize),
        b.getBottomLeft().translated(markerSize, 0.f));
    return p;
};

static const auto makeBottomRightMarker = [](const Rectangle<float> &b) {
    Path p;
    const auto minSide = jmin(b.getWidth(), b.getHeight());
    const auto markerSize = jlimit(1.f, SELECTION_MARKER_SIZE, minSide / 2);
    p.addTriangle(b.getBottomRight(),
        b.getBottomRight().translated(-markerSize, 0.f),
        b.getBottomRight().translated(0.f, -markerSize));
    return p;
};

void KeyboardCursor::paintSelectionMarkers(Graphics &g, const Rectangle<float> &b, bool axis)
{
    const Path p1 = axis ? makeTopLeftMarker(b) : makeTopRightMarker(b);
    const Path p2 = axis ? makeBottomRightMarker(b) : makeBottomLeftMarker(b);

    g.setColour(this->fillColour.withMultipliedAlpha(this->blinkAlpha));
    g.fillPath(p1);
    g.fillPath(p2);
}

void KeyboardCursor::handleAsyncUpdate()
{
    if (this->isVisible())
    {
        this->updateBounds();
    }
}

void KeyboardCursor::changeListenerCallback(ChangeBroadcaster *source)
{
    if (this->isVisible())
    {
        this->updateBounds();
    }
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void KeyboardCursor::onSeek(float beatPosition)
{
    if (!this->transportIsPlayingOrRecording.get())
    {
        //JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
        this->triggerAsyncUpdate(); // instead of this->updateBounds();
    }
}

void KeyboardCursor::onPlay()
{
    this->transportIsPlayingOrRecording = true;
    this->blinkAnimator->cancelAllAnimations();
    this->blinkAlpha = 0.f;

    if (this->isVisible())
    {
        this->repaint();
    }
}

void KeyboardCursor::onRecord()
{
    this->transportIsPlayingOrRecording = true;
    this->blinkAnimator->cancelAllAnimations();
    this->blinkAlpha = 0.f;

    if (this->isVisible())
    {
        this->repaint();
    }
}

void KeyboardCursor::onStop()
{
    this->transportIsPlayingOrRecording = false;
    this->blinkAlpha = 1.f;

    if (this->isVisible())
    {
        this->showAndRestartBlinking();
    }
}
