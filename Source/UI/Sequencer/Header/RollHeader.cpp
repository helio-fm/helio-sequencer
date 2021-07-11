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

#include "Common.h"
#include "RollHeader.h"
#include "RollBase.h"
#include "Transport.h"
#include "SelectionComponent.h"
#include "SoundProbeIndicator.h"
#include "TimeDistanceIndicator.h"
#include "HeaderSelectionIndicator.h"
#include "ClipRangeIndicator.h"
#include "HelioCallout.h"
#include "TimelineMenu.h"
#include "ColourIDs.h"

class PlaybackLoopMarker final : public Component
{
public:

    enum class Type { LoopStart, LoopEnd };

    PlaybackLoopMarker(Transport &transport, RollBase &roll, Type type) :
        transport(transport),
        roll(roll),
        type(type)
    {
        //this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(true, false);
        this->setMouseCursor(MouseCursor::PointingHandCursor);
        this->setSize(16, 16);
    }

    void setBeat(float targetBeat)
    {
        this->beat = targetBeat;
        this->updatePosition();
    }

    void updatePosition()
    {
        const auto x = this->roll.getXPositionByBeat(this->beat);
        const auto w = this->getWidth();
        switch (this->type)
        {
        case Type::LoopStart:
            this->setBounds(x, 0, w, this->getParentHeight());
            break;
        case Type::LoopEnd:
            this->setBounds(x - w + 1, 0, w, this->getParentHeight());
            break;
        }
    }

    void paint(Graphics &g) override
    {
        const auto p1 = roundf(float(this->getHeight()) * 0.33f - 1.f);
        const auto p2 = roundf(float(this->getHeight()) * 0.66f - 4.f);

        // painting context here reuses the color set by parent (red or regular)

        switch (this->type)
        {
        case Type::LoopStart:
            
            g.fillRect(0, 1, 3, this->getHeight() - 2);
            g.fillRect(5, 1, 1, this->getHeight() - 2);

            g.fillRect(float(9), p1, 2.f, 4.f);
            g.fillRect(float(8), p1 + 1, 4.f, 2.f);

            g.fillRect(float(9), p2, 2.f, 4.f);
            g.fillRect(float(8), p2 + 1, 4.f, 2.f);

            // some fancy shadows
            g.setColour(this->shadowColour);

            g.fillRect(3, 1, 1, this->getHeight() - 2);
            g.fillRect(6, 1, 1, this->getHeight() - 2);
            break;
        case Type::LoopEnd:

            const int x = this->getWidth();

            g.fillRect(x - 3, 1, 3, this->getHeight() - 2);
            g.fillRect(x - 6, 1, 1, this->getHeight() - 2);

            g.fillRect(float(x - 11), p1, 2.f, 4.f);
            g.fillRect(float(x - 12), p1 + 1, 4.f, 2.f);

            g.fillRect(float(x - 11), p2, 2.f, 4.f);
            g.fillRect(float(x - 12), p2 + 1, 4.f, 2.f);

            // some fancy shadows
            g.setColour(this->shadowColour);

            g.fillRect(x, 1, 1, this->getHeight() - 2);
            g.fillRect(x - 5, 1, 1, this->getHeight() - 2);
            break;
        }
    }

    void mouseUp(const MouseEvent &e) override
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);

        if (e.getOffsetFromDragStart().isOrigin())
        {
            if (!this->transport.isPlayingAndRecording())
            {
                this->transport.stopPlayback();
                this->transport.seekToBeat(this->beat);
            }
        }
    }

    void mouseDrag(const MouseEvent &e) override
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);

        const auto dragAtHeader = e.getEventRelativeTo(this->getParentComponent());
        const auto beat = this->roll.getRoundBeatSnapByXPosition(dragAtHeader.x);

        switch (this->type)
        {
        case Type::LoopStart:
            this->transport.setPlaybackLoop(beat, this->transport.getPlaybackLoopEnd());
            break;
        case Type::LoopEnd:
            this->transport.setPlaybackLoop(this->transport.getPlaybackLoopStart(), beat);
            break;
        }
    }
    
private:

    const Type type;

    Transport &transport;
    RollBase &roll;

    float beat = 0.f;

    const Colour shadowColour = findDefaultColour(ColourIDs::Common::borderLineDark);

};

RollHeader::RollHeader(Transport &transport, RollBase &roll, Viewport &viewport) :
    transport(transport),
    roll(roll),
    viewport(viewport)
{
    this->setOpaque(true);
    this->setAlwaysOnTop(true);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->updateColours();

    // these two are added first, so when they repaint, they can use a color
    // set by the parent(this header), which is red when rendering or just regular
    this->loopMarkerStart = make<PlaybackLoopMarker>(transport,
        roll, PlaybackLoopMarker::Type::LoopStart);
    this->addChildComponent(this->loopMarkerStart.get());

    this->loopMarkerEnd = make<PlaybackLoopMarker>(transport,
        roll, PlaybackLoopMarker::Type::LoopEnd);
    this->addChildComponent(this->loopMarkerEnd.get());

    this->selectionIndicator = make<HeaderSelectionIndicator>();
    this->addChildComponent(this->selectionIndicator.get());

    this->setSize(this->getParentWidth(), Globals::UI::rollHeaderHeight);
    this->selectionIndicator->setTopLeftPosition(0,
        this->getHeight() - this->selectionIndicator->getHeight());
}

RollHeader::~RollHeader() = default;

void RollHeader::updateColours()
{
    // Painting is the very bottleneck of this app,
    // so make sure we have no lookups inside paint method
    this->backColour = findDefaultColour(ColourIDs::Roll::headerFill);
    this->barShadeColour = this->backColour.darker(0.1f);
    this->recordingColour = findDefaultColour(ColourIDs::Roll::headerRecording);
    this->barColour = this->recordingMode.get() ?
        findDefaultColour(ColourIDs::Roll::headerRecording) :
        findDefaultColour(ColourIDs::Roll::headerSnaps);
    this->beatColour = this->barColour.withMultipliedAlpha(0.8f);
    this->snapColour = this->barColour.withMultipliedAlpha(0.6f);
    this->bevelDarkColour = findDefaultColour(ColourIDs::Common::borderLineDark);
    this->bevelLightColour = findDefaultColour(ColourIDs::Common::borderLineLight)
        .withMultipliedAlpha(0.5f);
}

void RollHeader::showRecordingMode(bool showRecordingMarker)
{
    this->recordingMode = showRecordingMarker;
    this->updateColours();
    this->repaint();
}

void RollHeader::showLoopMode(bool hasLoop, float startBeat, float endBeat)
{
    this->loopMarkerStart->setBeat(startBeat);
    this->loopMarkerEnd->setBeat(endBeat);

    this->loopMarkerStart->setVisible(hasLoop);
    this->loopMarkerEnd->setVisible(hasLoop);

    this->resized(); // updates their positions
}

void RollHeader::setSoundProbeMode(bool shouldPreviewOnClick)
{
    if (this->soundProbeMode.get() == shouldPreviewOnClick)
    {
        return;
    }
    
    this->soundProbeMode = shouldPreviewOnClick;
    
    if (this->soundProbeMode.get())
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);
    }
    else
    {
        this->pointingIndicator = nullptr;
        this->timeDistanceIndicator = nullptr;
        this->setMouseCursor(MouseCursor::NormalCursor);
    }
}

void RollHeader::updateSubrangeIndicator(const Colour &colour, float firstBeat, float lastBeat)
{
    if (this->clipRangeIndicator == nullptr)
    {
        this->clipRangeIndicator = make<ClipRangeIndicator>();
        this->addAndMakeVisible(this->clipRangeIndicator.get());
    }

    if (this->clipRangeIndicator->updateWith(colour, firstBeat, lastBeat))
    {
        this->updateClipRangeIndicator();
    }
}

void RollHeader::updateIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e)
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const float roundBeat = this->roll.getBeatByXPosition(float(parentEvent.x));
    const int roundX = this->roll.getXPositionByBeat(roundBeat);
    const auto anchor = double(roundX) / double(this->roll.getWidth());
    indicator->setAnchoredAt(anchor);
}

double RollHeader::getUnalignedAnchorForEvent(const MouseEvent &e) const
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const double absX = double(parentEvent.getPosition().getX()) / double(this->roll.getWidth());
    return absX;
}

void RollHeader::updateTimeDistanceIndicator()
{
    if (this->pointingIndicator == nullptr ||
        this->probeIndicator == nullptr ||
        this->timeDistanceIndicator == nullptr)
    {
        return;
    }
    
    const double anchor1 = this->pointingIndicator->getAnchor();
    const double anchor2 = this->probeIndicator->getAnchor();
    
    const auto seek1 = this->roll.getBeatByXPosition(float(this->pointingIndicator->getX()));
    const auto seek2 = this->roll.getBeatByXPosition(float(this->probeIndicator->getX()));

    this->timeDistanceIndicator->setAnchoredBetween(anchor1, anchor2);

    const auto timeMs1 = this->transport.findTimeAt(seek1);
    const auto timeMs2 = this->transport.findTimeAt(seek2);
    
    const double timeDelta = fabs(timeMs2 - timeMs1);
    const auto timeDeltaText = Transport::getTimeString(timeDelta);
    this->timeDistanceIndicator->getTimeLabel()->setText(timeDeltaText, dontSendNotification);
}

void RollHeader::updateClipRangeIndicator()
{
    jassert(this->clipRangeIndicator != nullptr);
    const int x1 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getFirstBeat());
    const int x2 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getLastBeat());
    this->clipRangeIndicator->setBounds(x1, 0, x2 - x1 + 1, 1);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void RollHeader::mouseDown(const MouseEvent &e)
{
    if (this->soundProbeMode.get())
    {
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x);
        this->transport.probeSoundAtBeat(roundBeat, nullptr);
        
        this->probeIndicator = make<SoundProbeIndicator>();
        this->roll.addAndMakeVisible(this->probeIndicator.get());
        this->updateIndicatorPosition(this->probeIndicator.get(), e);
    }
    else
    {
        const auto parentEvent = e.getEventRelativeTo(&this->roll);
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
       
        const bool shouldStartSelection = (e.mods.isAltDown() ||
                                           e.mods.isCommandDown() ||
                                           e.mods.isCtrlDown() ||
                                           e.mods.isShiftDown() ||
                                           this->roll.isInSelectionMode());
        
        if (shouldStartSelection)
        {
            const float newX = parentEvent.position.x;
            this->roll.getSelectionComponent()->beginLasso({ newX, 0.f }, &this->roll);
            this->selectionIndicator->fadeIn();
            this->selectionIndicator->setStartAnchor(this->getUnalignedAnchorForEvent(e));
        }
        else if (!this->transport.isPlayingAndRecording())
        {
            this->transport.stopPlayback();
            this->roll.cancelPendingUpdate(); // why is it here?

            // two presses on mobile will emit the timeline menu,
            // on the desktop it is available via right click
#if PLATFORM_MOBILE
            if (this->transport.getSeekBeat() == roundBeat)
            {
                this->showPopupMenu();
            }
            else
            {
                this->transport.seekToBeat(roundBeat);
            }
#elif PLATFORM_DESKTOP
            this->transport.seekToBeat(roundBeat);
#endif
        }
    }
}

void RollHeader::mouseDrag(const MouseEvent &e)
{
    if (this->soundProbeMode.get())
    {
        if (this->pointingIndicator != nullptr)
        {
            this->updateIndicatorPosition(this->pointingIndicator.get(), e);

            if (this->probeIndicator != nullptr)
            {
                const int distance = abs(this->pointingIndicator->getX() - this->probeIndicator->getX());

                if (this->timeDistanceIndicator == nullptr)
                {
                    if (distance > RollHeader::minTimeDistanceIndicatorSize)
                    {
                        this->transport.stopPlaybackAndRecording();
                        this->transport.allNotesControllersAndSoundOff();

                        this->timeDistanceIndicator = make<TimeDistanceIndicator>();
                        this->roll.addAndMakeVisible(this->timeDistanceIndicator.get());
                        this->timeDistanceIndicator->setBounds(0, this->getBottom() + 4,
                            0, this->timeDistanceIndicator->getHeight());
                        this->updateTimeDistanceIndicator();
                    }
                }
                else
                {
                    if (distance <= RollHeader::minTimeDistanceIndicatorSize)
                    {
                        this->timeDistanceIndicator = nullptr;
                    }
                    else
                    {
                        this->updateTimeDistanceIndicator();
                    }
                }
            }
        }
    }
    else
    {
        if (this->roll.getSelectionComponent()->isDragging())
        {
            const auto parentEvent = e.getEventRelativeTo(&this->roll);
            const auto parentGlobalSelection = parentEvent.withNewPosition(Point<int>(parentEvent.x, this->roll.getHeight()));
            this->roll.getSelectionComponent()->dragLasso(parentGlobalSelection);
            this->selectionIndicator->setEndAnchor(this->getUnalignedAnchorForEvent(e));
        }
        else if (!this->transport.isPlayingAndRecording())
        {
            const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
            this->transport.stopPlayback();
            this->roll.cancelPendingUpdate();
            this->transport.seekToBeat(roundBeat);
        }
    }
}

void RollHeader::mouseUp(const MouseEvent &e)
{
    this->probeIndicator = nullptr;
    this->timeDistanceIndicator = nullptr;

    this->selectionIndicator->fadeOut();
    
    if (this->soundProbeMode.get())
    {
        this->transport.allNotesControllersAndSoundOff();
        return;
    }
    
    if (this->roll.getSelectionComponent()->isDragging())
    {
        this->roll.getSelectionComponent()->endLasso();
    }
    else if (!this->transport.isPlayingAndRecording())
    {
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
        this->transport.stopPlayback();
        this->transport.seekToBeat(roundBeat);
        
        if (e.mods.isRightButtonDown())
        {
            this->showPopupMenu();
        }
        else if (e.mods.isMiddleButtonDown())
        {
            this->transport.startPlayback();
        }
    }
}

void RollHeader::mouseMove(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->updateIndicatorPosition(this->pointingIndicator.get(), e);
    }
    else if (this->soundProbeMode.get())
    {
        this->pointingIndicator = make<SoundProbeIndicator>();
        this->roll.addAndMakeVisible(this->pointingIndicator.get());
        this->updateIndicatorPosition(this->pointingIndicator.get(), e);
    }
}

void RollHeader::mouseExit(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->pointingIndicator = nullptr;
    }
    
    if (this->timeDistanceIndicator != nullptr)
    {
        this->timeDistanceIndicator = nullptr;
    }
}

void RollHeader::mouseDoubleClick(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    if (this->soundProbeMode.get())
    {
        return;
    }

    const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
    this->transport.stopPlaybackAndRecording();
    this->transport.seekToBeat(roundBeat);
    this->transport.startPlayback();
#endif
}

void RollHeader::paint(Graphics &g)
{
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = this->viewport.getViewPositionX() + this->viewport.getViewWidth();

    g.setColour(this->backColour);
    g.fillRect(paintStartX, 0, paintEndX - paintStartX, Globals::UI::rollHeaderHeight);

    g.setColour(this->barColour);
    for (const auto f : this->roll.getVisibleBars())
    {
        g.fillRect(floorf(f), float(this->getHeight() - 13), 1.f, 12.f);
    }

    g.setColour(this->barShadeColour);
    for (const auto f : this->roll.getVisibleBars())
    {
        g.fillRect(floorf(f + 1), float(this->getHeight() - 12), 1.f, 11.f);
    }

    g.setColour(this->beatColour);
    for (const auto f : this->roll.getVisibleBeats())
    {
        g.fillRect(floorf(f), float(this->getHeight() - 8), 1.f, 7.f);
    }

    g.setColour(this->snapColour);
    for (const auto f : this->roll.getVisibleSnaps())
    {
        g.fillRect(floorf(f), float(this->getHeight() - 4), 1.f, 3.f);
    }

    g.setColour(this->bevelLightColour);
    g.fillRect(0, this->getHeight() - 2, this->getWidth(), 1);

    g.setColour(this->bevelDarkColour);
    g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);

    if (this->recordingMode.get())
    {
        g.setColour(this->recordingColour);
        g.fillRect(0, this->getHeight() - 4, this->getWidth(), 3);
    }
    else
    {
        g.setColour(this->barColour);
    }
}

void RollHeader::resized()
{
    if (this->clipRangeIndicator != nullptr)
    {
        this->updateClipRangeIndicator();
    }

    this->loopMarkerStart->updatePosition();
    this->loopMarkerEnd->updatePosition();
}

void RollHeader::showPopupMenu()
{
    HelioCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);
}
