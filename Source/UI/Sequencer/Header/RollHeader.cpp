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

RollHeader::RollHeader(Transport &transportRef, RollBase &rollRef, Viewport &viewportRef) :
    transport(transportRef),
    roll(rollRef),
    viewport(viewportRef)
{
    this->setOpaque(true);
    this->setAlwaysOnTop(true);
    this->setPaintingIsUnclipped(true);

    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->updateColours();
    this->setSize(this->getParentWidth(), Globals::UI::rollHeaderHeight);

    this->selectionIndicator = make<HeaderSelectionIndicator>();
    this->addChildComponent(this->selectionIndicator.get());
    this->selectionIndicator->setTopLeftPosition(0, this->getHeight() - this->selectionIndicator->getHeight());
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
    this->loopMode = hasLoop;
    this->loopStartBeat = startBeat;
    this->loopEndBeat = endBeat;
    this->repaint();
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
        else
        {
            if (!this->transport.isRecording())
            {
                this->transport.stopPlayback();
            }

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
        else
        {
            //if (! this->transport.isPlaying())
            {
                const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
                this->transport.stopPlaybackAndRecording();
                this->roll.cancelPendingUpdate();
                this->transport.seekToBeat(roundBeat);
            }
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
    else
    {
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
        
        if (!this->transport.isRecording())
        {
            this->transport.stopPlayback();
        }

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
    g.setFont(Globals::UI::Fonts::XS); //set font to extra small

    //Array<float> visibleBarNums = this->roll.getVisibleBarNums(); //get all visible bar numbers

    //int i = 0;
    for (const auto f : this->roll.getVisibleBars())
    {
        g.fillRect(floorf(f), float(this->getHeight() - 13), 2.f, 12.f);    //made 1px thicker - RPM
        //g.drawText(std::to_string(visibleBarNums[i]),int(f), this->getHeight() - 13, 10, 10, Justification::centred, true);

        //i++;
    }

    g.setColour(this->barShadeColour);
    for (const auto f : this->roll.getVisibleBars())
    {
        g.fillRect(floorf(f -1), float(this->getHeight() - 12), 1.f, 11.f); //draws beat bar tick shade
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

    if (this->loopMode.get())
    {
        const int startX = this->roll.getXPositionByBeat(this->loopStartBeat.get());
        const int endX = this->roll.getXPositionByBeat(this->loopEndBeat.get());

        g.fillRect(startX, 1, 3, this->getHeight() - 2);
        g.fillRect(startX + 5, 1, 1, this->getHeight() - 2);

        g.fillRect(endX - 2, 1, 3, this->getHeight() - 2);
        g.fillRect(endX - 5, 1, 1, this->getHeight() - 2);

        // todo draw ellipses instead?
        const auto p1 = roundf(float(this->getHeight()) * 0.33f + 1.f);
        const auto p2 = roundf(float(this->getHeight()) * 0.66f - 3.f);

        g.fillRect(float(startX + 8), p1, 3.f, 3.f);
        g.fillRect(float(startX + 8), p2, 3.f, 3.f);

        g.fillRect(float(endX - 10), p1, 3.f, 3.f);
        g.fillRect(float(endX - 10), p2, 3.f, 3.f);

        // some fancy shadows
        g.setColour(this->bevelDarkColour);

        g.fillRect(startX + 3, 1, 1, this->getHeight() - 2);
        g.fillRect(startX + 6, 1, 1, this->getHeight() - 2);

        g.fillRect(endX + 1, 1, 1, this->getHeight() - 2);
        g.fillRect(endX - 4, 1, 1, this->getHeight() - 2);

        g.fillRect(float(startX + 11), p1, 1.f, 3.f);
        g.fillRect(float(startX + 11), p2, 1.f, 3.f);
        g.fillRect(float(endX - 7), p1, 1.f, 3.f);
        g.fillRect(float(endX - 7), p2, 1.f, 3.f);
    }
}

void RollHeader::resized()
{
    if (this->clipRangeIndicator != nullptr)
    {
        this->updateClipRangeIndicator();
    }
}

void RollHeader::showPopupMenu()
{
    HelioCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);
}
