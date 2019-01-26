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

#define HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS 1
#define HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS 0
#define MIN_TIME_DISTANCE_INDICATOR_SIZE (40)

HybridRollHeader::HybridRollHeader(Transport &transportRef, HybridRoll &rollRef, Viewport &viewportRef) :
    transport(transportRef),
    roll(rollRef),
    viewport(viewportRef),
    soundProbeMode(false)
{
    this->setAlwaysOnTop(true);
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);

    // Painting is the very bottleneck of this app,
    // so make sure we no lookups/computations inside paint method
    this->backColour = findDefaultColour(ColourIDs::Roll::headerFill);
    this->barColour = findDefaultColour(ColourIDs::Roll::headerSnaps);
    this->barShadeColour = this->backColour.darker(0.1f);
    this->beatColour = this->barColour.withMultipliedAlpha(0.8f);
    this->snapColour = this->barColour.withMultipliedAlpha(0.6f);
    this->bevelLightColour = findDefaultColour(ColourIDs::Common::borderLineLight).withMultipliedAlpha(0.35f);
    this->bevelDarkColour = findDefaultColour(ColourIDs::Common::borderLineDark);

    this->setMouseClickGrabsKeyboardFocus(false);
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);
    this->setSize(this->getParentWidth(), HYBRID_ROLL_HEADER_HEIGHT);
}

void HybridRollHeader::setSoundProbeMode(bool shouldPlayOnClick)
{
    if (this->soundProbeMode == shouldPlayOnClick)
    {
        return;
    }
    
    this->soundProbeMode = shouldPlayOnClick;
    
    if (this->soundProbeMode)
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
        this->clipRangeIndicator = new ClipRangeIndicator();
        this->addAndMakeVisible(this->clipRangeIndicator);
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
    const MouseEvent parentEvent = e.getEventRelativeTo(&this->roll);
    const double absX = double(parentEvent.getPosition().getX()) / double(this->roll.getWidth());
    return absX;
}

double HybridRollHeader::getAlignedAnchorForEvent(const MouseEvent &e) const
{
    const MouseEvent parentEvent = e.getEventRelativeTo(&this->roll);
    
#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
    const float roundBeat = this->roll.getRoundBeatByXPosition(parentEvent.x);
    const int roundX = this->roll.getXPositionByBeat(roundBeat);
    const double absX = double(roundX) / double(this->roll.getWidth());
#else
    const double absX = this->getUnalignedAnchorForEvent(e);
#endif
    
    return absX;
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
    
    const double seek1 = this->roll.getTransportPositionByXPosition(this->pointingIndicator->getX(), this->getWidth());
    const double seek2 = this->roll.getTransportPositionByXPosition(this->playingIndicator->getX(), this->getWidth());

    this->timeDistanceIndicator->setAnchoredBetween(anchor1, anchor2);
    
    double outTimeMs1 = 0.0;
    double outTempo1 = 0.0;
    double outTimeMs2 = 0.0;
    double outTempo2 = 0.0;
    
    // todo don't rebuild sequences here
    this->transport.calcTimeAndTempoAt(seek1, outTimeMs1, outTempo1);
    this->transport.calcTimeAndTempoAt(seek2, outTimeMs2, outTempo2);
    
    const double timeDelta = fabs(outTimeMs2 - outTimeMs1);
    const String timeDeltaText = Transport::getTimeString(timeDelta);
    this->timeDistanceIndicator->getTimeLabel()->setText(timeDeltaText, dontSendNotification);
}

void HybridRollHeader::updateClipRangeIndicator()
{
    jassert(this->clipRangeIndicator != nullptr);
    const int x1 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getFirstBeat());
    const int x2 = this->roll.getXPositionByBeat(this->clipRangeIndicator->getLastBeat());
    this->clipRangeIndicator->setBounds(x1, 0, x2 - x1, 1);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void HybridRollHeader::mouseDown(const MouseEvent &e)
{
    if (this->soundProbeMode)
    {
        // todo if playing, dont probe anything?
        
#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
        const float roundBeat = this->roll.getRoundBeatByXPosition(e.x);
        const double transportPosition = this->roll.getTransportPositionByBeat(roundBeat);
#else
        const double transportPosition = this->roll.getTransportPositionByXPosition(e.x, float(this->getWidth()));
#endif
        
        this->transport.probeSoundAt(transportPosition, nullptr);
        
        this->playingIndicator = new SoundProbeIndicator();
        this->roll.addAndMakeVisible(this->playingIndicator);
        this->updateIndicatorPosition(this->playingIndicator, e);
    }
    else
    {
#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
        const MouseEvent parentEvent = e.getEventRelativeTo(&this->roll);
        const float roundBeat = this->roll.getRoundBeatByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
        const double transportPosition = this->roll.getTransportPositionByBeat(roundBeat);
#else
        const double transportPosition = this->roll.getTransportPositionByXPosition(e.x, float(this->getWidth()));
#endif
        
        const bool shouldStartSelection = (e.mods.isAltDown() ||
                                           e.mods.isCommandDown() ||
                                           e.mods.isCtrlDown() ||
                                           e.mods.isShiftDown() ||
                                           this->roll.isInSelectionMode());
        
        if (shouldStartSelection)
        {
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            const float roundBeat = this->roll.getRoundBeatByXPosition(parentEvent.x);
            const int roundX = this->roll.getXPositionByBeat(roundBeat);
            const float newX = float(roundX + 1);
#else
            const float newX = parentEvent.position.x;
#endif

            const MouseEvent e2(e.source,
                                Point<float>(newX, 0.f),
                                e.mods,
                                1.f,
                                0.0f, 0.0f, 0.0f, 0.0f,
                                e.originalComponent,
                                e.originalComponent,
                                Time::getCurrentTime(),
                                Point<float>(newX, 0.f),
                                Time::getCurrentTime(),
                                1,
                                false);
            
            this->roll.getSelectionComponent()->beginLasso(e2, &this->roll);
            
            this->selectionIndicator = new HeaderSelectionIndicator();
            this->addAndMakeVisible(this->selectionIndicator);
            this->selectionIndicator->setBounds(0, this->getHeight() - this->selectionIndicator->getHeight(),
                                                0, this->selectionIndicator->getHeight());
            
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            this->selectionIndicator->setStartAnchor(this->getAlignedAnchorForEvent(e));
#else
            this->selectionIndicator->setStartAnchor(this->getUnalignedAnchorForEvent(e));
#endif
        }
        else
        {
            this->transport.stopPlayback();
            this->roll.cancelPendingUpdate(); // why is it here?
            this->transport.seekToPosition(transportPosition);
        }
    }
}

void HybridRollHeader::mouseDrag(const MouseEvent &e)
{
    if (this->soundProbeMode)
    {
        if (this->pointingIndicator != nullptr)
        {
            this->updateIndicatorPosition(this->pointingIndicator, e);

            if (this->playingIndicator != nullptr)
            {
                const int distance = abs(this->pointingIndicator->getX() - this->playingIndicator->getX());

                if (this->timeDistanceIndicator == nullptr)
                {
                    // todo rebuild sequences if not playing, do nothing if playing
                    this->transport.stopPlayback();
                    
                    if (distance > MIN_TIME_DISTANCE_INDICATOR_SIZE)
                    {
                        this->timeDistanceIndicator = new TimeDistanceIndicator();
                        this->roll.addAndMakeVisible(this->timeDistanceIndicator);
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
            const MouseEvent parentEvent = e.getEventRelativeTo(&this->roll);
            
#if HYBRID_ROLL_HEADER_SELECTION_ALIGNS_TO_BEATS
            const float roundBeat = this->roll.getRoundBeatByXPosition(parentEvent.x);
            const int roundX = this->roll.getXPositionByBeat(roundBeat);
            const MouseEvent parentGlobalSelection = parentEvent.withNewPosition(Point<int>(roundX - 1, this->roll.getHeight()));
#else
            const MouseEvent parentGlobalSelection = parentEvent.withNewPosition(Point<int>(parentEvent.x, this->roll.getHeight()));
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
#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
                const float roundBeat = this->roll.getRoundBeatByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
                const double transportPosition = this->roll.getTransportPositionByBeat(roundBeat);
#else
                const double transportPosition = this->roll.getTransportPositionByXPosition(e.x, float(this->getWidth()));
#endif
                
                this->transport.stopPlayback();
                this->roll.cancelPendingUpdate();
                this->transport.seekToPosition(transportPosition);
            }
        }
    }
}

void HybridRollHeader::mouseUp(const MouseEvent &e)
{
    this->playingIndicator = nullptr;
    this->timeDistanceIndicator = nullptr;
    this->selectionIndicator = nullptr;
    
    if (this->soundProbeMode)
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
#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
        const float roundBeat = this->roll.getRoundBeatByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
        const double transportPosition = this->roll.getTransportPositionByBeat(roundBeat);
#else
        const double transportPosition = this->roll.getTransportPositionByXPosition(e.x, float(this->getWidth()));
#endif
        
        this->transport.stopPlayback();
        this->transport.seekToPosition(transportPosition);
        
        if (e.mods.isRightButtonDown())
        {
            HelioCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);
            //this->transport.startPlayback();
        }
    }
}

void HybridRollHeader::mouseEnter(const MouseEvent &e)
{
}

void HybridRollHeader::mouseMove(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->updateIndicatorPosition(this->pointingIndicator, e);
    }
    
    if (this->soundProbeMode)
    {
        if (this->pointingIndicator == nullptr)
        {
            this->pointingIndicator = new SoundProbeIndicator();
            this->roll.addAndMakeVisible(this->pointingIndicator);
            this->updateIndicatorPosition(this->pointingIndicator, e);
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
    // this->roll.postCommandMessage(CommandIDs::AddAnnotation);
    // HelioCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);

#if HYBRID_ROLL_HEADER_ALIGNS_TO_BEATS
    const float roundBeat = this->roll.getRoundBeatByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
    const double transportPosition = this->roll.getTransportPositionByBeat(roundBeat);
#else
    const double transportPosition = this->roll.getTransportPositionByXPosition(e.x, float(this->getWidth()));
#endif

    this->transport.stopPlayback();
    this->transport.seekToPosition(transportPosition);
    this->transport.startPlayback();
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
        g.drawVerticalLine(int(floorf(f)), float(this->getHeight() - 14), float(this->getHeight() - 1));
    }

    g.setColour(this->barShadeColour);
    for (const auto f : this->roll.getVisibleBars())
    {
        g.drawVerticalLine(int(floorf(f)) + 1, float(this->getHeight() - 13), float(this->getHeight() - 1));
    }

    g.setColour(this->beatColour);
    for (const auto f : this->roll.getVisibleBeats())
    {
        g.drawVerticalLine(int(floorf(f)), float(this->getHeight() - 8), float(this->getHeight() - 1));
    }

    g.setColour(this->snapColour);
    for (const auto f : this->roll.getVisibleSnaps())
    {
        g.drawVerticalLine(int(floorf(f)), float(this->getHeight() - 4), float(this->getHeight() - 1));
    }

    g.setColour(this->bevelLightColour);
    g.drawHorizontalLine(this->getHeight() - 2, 0.f, float(this->getWidth()));

    g.setColour(this->bevelDarkColour);
    g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
}

void HybridRollHeader::resized()
{
    if (this->clipRangeIndicator != nullptr)
    {
        this->updateClipRangeIndicator();
    }
}
