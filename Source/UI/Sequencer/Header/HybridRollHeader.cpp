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
#include "HybridRollHeader.h"
#include "HybridRoll.h"
#include "MidiSequence.h"
#include "Transport.h"
#include "AnnotationsProjectMap.h"
#include "Origami.h"
#include "SelectionComponent.h"
#include "SoundProbeIndicator.h"
#include "TimeDistanceIndicator.h"
#include "HeaderSelectionIndicator.h"
#include "ClipRangeIndicator.h"
#include "HelioCallout.h"
#include "TimelineMenu.h"
#include "CommandIDs.h"
#include "ColourIDs.h"

#define HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS 0
#define MIN_TIME_DISTANCE_INDICATOR_SIZE (40)

HybridRollHeader::HybridRollHeader(Transport &transportRef, HybridRoll &rollRef, Viewport &viewportRef) :
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
    this->setSize(this->getParentWidth(), HYBRID_ROLL_HEADER_HEIGHT);
}

void HybridRollHeader::updateColours()
{
    // Painting is the very bottleneck of this app,
    // so make sure we no lookups/computations inside paint method
    this->backColour = findDefaultColour(ColourIDs::Roll::headerFill);
    this->barShadeColour = this->backColour.darker(0.1f);
    this->recordingColour = findDefaultColour(ColourIDs::Roll::headerRecording);
    this->barColour = this->recordingMode.get() ?
        findDefaultColour(ColourIDs::Roll::headerRecording) :
        findDefaultColour(ColourIDs::Roll::headerSnaps);
    this->beatColour = this->barColour.withMultipliedAlpha(0.8f);
    this->snapColour = this->barColour.withMultipliedAlpha(0.6f);
    this->bevelLightColour = findDefaultColour(ColourIDs::Common::borderLineLight).withMultipliedAlpha(0.5f);
    this->bevelDarkColour = findDefaultColour(ColourIDs::Common::borderLineDark);
}

void HybridRollHeader::showRecordingMode(bool showRecordingMarker)
{
    this->recordingMode = showRecordingMarker;
    this->updateColours();
    this->repaint();
}

void HybridRollHeader::showLoopMode(bool hasLoop, float startBeat, float endBeat)
{
    this->loopMode = hasLoop;
    this->loopStartBeat = startBeat;
    this->loopEndBeat = endBeat;
    this->repaint();
}

void HybridRollHeader::setSoundProbeMode(bool shouldPlayOnClick)
{
    if (this->soundProbeMode.get() == shouldPlayOnClick)
    {
        return;
    }
    
    this->soundProbeMode = shouldPlayOnClick;
    
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

void HybridRollHeader::updateSubrangeIndicator(const Colour &colour, float firstBeat, float lastBeat)
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

void HybridRollHeader::updateIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e)
{
    indicator->setAnchoredAt(this->getAlignedAnchorForEvent(e));
}

double HybridRollHeader::getUnalignedAnchorForEvent(const MouseEvent &e) const
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const double absX = double(parentEvent.getPosition().getX()) / double(this->roll.getWidth());
    return absX;
}

double HybridRollHeader::getAlignedAnchorForEvent(const MouseEvent &e) const
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const float roundBeat = this->roll.getRoundBeatSnapByXPosition(parentEvent.x);
    const int roundX = this->roll.getXPositionByBeat(roundBeat);
    return double(roundX) / double(this->roll.getWidth());
}

void HybridRollHeader::updateTimeDistanceIndicator()
{
    if (this->pointingIndicator == nullptr ||
        this->playingIndicator == nullptr ||
        this->timeDistanceIndicator == nullptr)
    {
        return;
    }
    
    const double anchor1 = this->pointingIndicator->getAnchor();
    const double anchor2 = this->playingIndicator->getAnchor();
    
    const auto seek1 = this->roll.getBeatByXPosition(float(this->pointingIndicator->getX()));
    const auto seek2 = this->roll.getBeatByXPosition(float(this->playingIndicator->getX()));

    this->timeDistanceIndicator->setAnchoredBetween(anchor1, anchor2);
    
    double outTimeMs1 = 0.0;
    double outTempo1 = 0.0;
    double outTimeMs2 = 0.0;
    double outTempo2 = 0.0;
    
    // todo don't rebuild sequences here
    this->transport.findTimeAndTempoAt(seek1, outTimeMs1, outTempo1);
    this->transport.findTimeAndTempoAt(seek2, outTimeMs2, outTempo2);
    
    const double timeDelta = fabs(outTimeMs2 - outTimeMs1);
    const auto timeDeltaText = Transport::getTimeString(timeDelta);
    this->timeDistanceIndicator->getTimeLabel()->setText(timeDeltaText, dontSendNotification);
}

void HybridRollHeader::updateClipRangeIndicator()
{
    jassert(this->clipRangeIndicator != nullptr);
    const int x1 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getFirstBeat());
    const int x2 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getLastBeat());
    this->clipRangeIndicator->setBounds(x1, 0, x2 - x1 + 1, 1);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void HybridRollHeader::mouseDown(const MouseEvent &e)
{
    if (this->soundProbeMode.get())
    {
        // todo if playing, dont probe anything?
        
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x);
        this->transport.probeSoundAtBeat(roundBeat, nullptr);
        
        this->playingIndicator = make<SoundProbeIndicator>();
        this->roll.addAndMakeVisible(this->playingIndicator.get());
        this->updateIndicatorPosition(this->playingIndicator.get(), e);
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
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            const float roundBeat = this->roll.getRoundBeatSnapByXPosition(parentEvent.x);
            const int roundX = this->roll.getXPositionByBeat(roundBeat);
            const float newX = float(roundX + 1);
#else
            const float newX = parentEvent.position.x;
#endif
            
            this->roll.getSelectionComponent()->beginLasso({ newX, 0.f }, &this->roll);
            
            this->selectionIndicator = make<HeaderSelectionIndicator>();
            this->addAndMakeVisible(this->selectionIndicator.get());
            this->selectionIndicator->setBounds(0,
                this->getHeight() - this->selectionIndicator->getHeight(),
                0, this->selectionIndicator->getHeight());
            
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            this->selectionIndicator->setStartAnchor(this->getAlignedAnchorForEvent(e));
#else
            this->selectionIndicator->setStartAnchor(this->getUnalignedAnchorForEvent(e));
#endif
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
#if HELIO_MOBILE
            if (this->transport.getSeekBeat() == roundBeat)
            {
                this->showPopupMenu();
            }
            else
            {
                this->transport.seekToBeat(roundBeat);
            }
#elif HELIO_DESKTOP
            this->transport.seekToBeat(roundBeat);
#endif
        }
    }
}

void HybridRollHeader::mouseDrag(const MouseEvent &e)
{
    if (this->soundProbeMode.get())
    {
        if (this->pointingIndicator != nullptr)
        {
            this->updateIndicatorPosition(this->pointingIndicator.get(), e);

            if (this->playingIndicator != nullptr)
            {
                const int distance = abs(this->pointingIndicator->getX() - this->playingIndicator->getX());

                if (this->timeDistanceIndicator == nullptr)
                {
                    // todo rebuild sequences if not playing, do nothing if playing
                    this->transport.stopPlaybackAndRecording();
                    
                    if (distance > MIN_TIME_DISTANCE_INDICATOR_SIZE)
                    {
                        this->timeDistanceIndicator = make<TimeDistanceIndicator>();
                        this->roll.addAndMakeVisible(this->timeDistanceIndicator.get());
                        this->timeDistanceIndicator->setBounds(0, this->getBottom() + 4,
                                                               0, this->timeDistanceIndicator->getHeight());
                        this->updateTimeDistanceIndicator();
                    }
                }
                else
                {
                    if (distance <= MIN_TIME_DISTANCE_INDICATOR_SIZE)
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
            
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            const float roundBeat = this->roll.getRoundBeatSnapByXPosition(parentEvent.x);
            const int roundX = this->roll.getXPositionByBeat(roundBeat);
            const auto parentGlobalSelection = parentEvent.withNewPosition(Point<int>(roundX - 1, this->roll.getHeight()));
#else
            const auto parentGlobalSelection = parentEvent.withNewPosition(Point<int>(parentEvent.x, this->roll.getHeight()));
#endif
            
            this->roll.getSelectionComponent()->dragLasso(parentGlobalSelection);
            
            if (this->selectionIndicator != nullptr)
            {
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
                this->selectionIndicator->setEndAnchor(this->getAlignedAnchorForEvent(e));
#else
                this->selectionIndicator->setEndAnchor(this->getUnalignedAnchorForEvent(e));
#endif
            }
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

void HybridRollHeader::mouseUp(const MouseEvent &e)
{
    this->playingIndicator = nullptr;
    this->timeDistanceIndicator = nullptr;
    this->selectionIndicator = nullptr;
    
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

void HybridRollHeader::mouseMove(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->updateIndicatorPosition(this->pointingIndicator.get(), e);
    }
    
    if (this->soundProbeMode.get())
    {
        if (this->pointingIndicator == nullptr)
        {
            this->pointingIndicator = make<SoundProbeIndicator>();
            this->roll.addAndMakeVisible(this->pointingIndicator.get());
            this->updateIndicatorPosition(this->pointingIndicator.get(), e);
        }
    }
}

void HybridRollHeader::mouseExit(const MouseEvent &e)
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

void HybridRollHeader::mouseDoubleClick(const MouseEvent &e)
{
#if HELIO_DESKTOP
    const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
    this->transport.stopPlaybackAndRecording();
    this->transport.seekToBeat(roundBeat);
    this->transport.startPlayback();
#endif
}

void HybridRollHeader::paint(Graphics &g)
{
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = this->viewport.getViewPositionX() + this->viewport.getViewWidth();

    g.setColour(this->backColour);
    g.fillRect(paintStartX, 0, paintEndX - paintStartX, HYBRID_ROLL_HEADER_HEIGHT);

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

    if (this->loopMode.get())
    {
        const int startX = this->roll.getXPositionByBeat(this->loopStartBeat.get());
        const int endX = this->roll.getXPositionByBeat(this->loopEndBeat.get());

        g.fillRect(float(startX), 1.f, 3.f, float(this->getHeight() - 2));
        g.fillRect(float(startX + 5), 1.f, 1.f, float(this->getHeight() - 2));

        g.fillRect(float(endX - 2), 1.f, 3.f, float(this->getHeight() - 2));
        g.fillRect(float(endX - 5), 1.f, 1.f, float(this->getHeight() - 2));

        // todo draw ellipses instead?
        const auto p1 = roundf(float(this->getHeight()) * 0.33f + 1.f);
        const auto p2 = roundf(float(this->getHeight()) * 0.66f - 3.f);

        g.fillRect(float(startX + 8), p1, 3.f, 3.f);
        g.fillRect(float(startX + 8), p2, 3.f, 3.f);

        g.fillRect(float(endX - 10), p1, 3.f, 3.f);
        g.fillRect(float(endX - 10), p2, 3.f, 3.f);

        // some fancy shadows
        g.setColour(this->bevelDarkColour);

        g.fillRect(float(startX + 3), 1.f, 1.f, float(this->getHeight() - 2));
        g.fillRect(float(startX + 6), 1.f, 1.f, float(this->getHeight() - 2));

        g.fillRect(float(endX + 1), 1.f, 1.f, float(this->getHeight() - 2));
        g.fillRect(float(endX - 4), 1.f, 1.f, float(this->getHeight() - 2));

        g.fillRect(float(startX + 11), p1, 1.f, 3.f);
        g.fillRect(float(startX + 11), p2, 1.f, 3.f);
        g.fillRect(float(endX - 7), p1, 1.f, 3.f);
        g.fillRect(float(endX - 7), p2, 1.f, 3.f);
    }
}

void HybridRollHeader::resized()
{
    if (this->clipRangeIndicator != nullptr)
    {
        this->updateClipRangeIndicator();
    }
}

void HybridRollHeader::showPopupMenu()
{
    HelioCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);
}
