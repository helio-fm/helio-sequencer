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
#include "HybridRoll.h"
#include "HybridRollHeader.h"
#include "SoundProbeIndicator.h"
#include "TimeDistanceIndicator.h"
#include "HeaderSelectionIndicator.h"
#include "ClipRangeIndicator.h"

#include "HybridRollExpandMark.h"
#include "MidiEvent.h"
#include "MidiEventComponent.h"
#include "MidiSequence.h"
#include "SelectionComponent.h"

#include "UndoStack.h"
#include "UndoActionIDs.h"
#include "NoteActions.h"
#include "PianoTrackActions.h"

#include "ShadowDownwards.h"
#include "ShadowUpwards.h"
#include "TimelineWarningMarker.h"

#include "LongTapController.h"
#include "SmoothPanController.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "Origami.h"

#include "Transport.h"
#include "IconComponent.h"
#include "PlayerThread.h"

#include "ProjectTimeline.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesSequence.h"
#include "SequencerOperations.h"
#include "HybridRollListener.h"
#include "VersionControlNode.h"
#include "MidiTrackNode.h"
#include "Pattern.h"

#include "AnnotationDialog.h"
#include "TimeSignatureDialog.h"
#include "KeySignatureDialog.h"
#include "TrackPropertiesDialog.h"

#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "AudioMonitor.h"
#include "Config.h"

#include "SerializationKeys.h"
#include "ColourIDs.h"

#include <limits.h>

#if HELIO_DESKTOP
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 1
#else
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 0
#endif

HybridRoll::HybridRoll(ProjectNode &parentProject, Viewport &viewportRef,
    WeakReference<AudioMonitor> audioMonitor,
    bool hasAnnotationsTrack,
    bool hasKeySignaturesTrack,
    bool hasTimeSignaturesTrack) :
    clippingDetector(audioMonitor),
    project(parentProject),
    viewport(viewportRef),
    barLineColour(findDefaultColour(ColourIDs::Roll::barLine)),
    barLineBevelColour(findDefaultColour(ColourIDs::Roll::barLineBevel)),
    beatLineColour(findDefaultColour(ColourIDs::Roll::beatLine)),
    snapLineColour(findDefaultColour(ColourIDs::Roll::snapLine))
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);

    this->setSize(this->viewport.getWidth(), this->viewport.getHeight());

    this->setMouseClickGrabsKeyboardFocus(false);
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);

    this->header = make<HybridRollHeader>(this->project.getTransport(), *this, this->viewport);
    this->headerShadow = make<ShadowDownwards>(Normal);

    if (hasAnnotationsTrack)
    {
        this->annotationsMap = make<AnnotationsProjectMap>(this->project, *this, AnnotationsProjectMap::Large);
    }

    if (hasTimeSignaturesTrack)
    {
        this->timeSignaturesMap = make<TimeSignaturesProjectMap>(this->project, *this, TimeSignaturesProjectMap::Large);
    }

    if (hasKeySignaturesTrack)
    {
        this->keySignaturesMap = make<KeySignaturesProjectMap>(this->project, *this, KeySignaturesProjectMap::Large);
    }

    this->playhead = make<Playhead>(*this, this->project.getTransport(), this);

    this->lassoComponent = make<SelectionComponent>();
    this->lassoComponent->setWantsKeyboardFocus(false);
    this->lassoComponent->setFocusContainer(false);

    this->addAndMakeVisible(this->header.get());
    this->addAndMakeVisible(this->headerShadow.get());

    if (this->annotationsMap)
    {
        this->addAndMakeVisible(this->annotationsMap.get());
    }

    if (this->timeSignaturesMap)
    {
        this->addAndMakeVisible(this->timeSignaturesMap.get());
    }

    if (this->keySignaturesMap)
    {
        this->addAndMakeVisible(this->keySignaturesMap.get());
    }

    this->addAndMakeVisible(this->playhead.get());

    this->addAndMakeVisible(this->lassoComponent.get());

#if HYBRID_ROLL_LISTENS_LONG_TAP
    this->longTapController = make<LongTapController>(*this);
    this->addMouseListener(this->longTapController.get(), true); // true = listens child events as well
#endif

    this->multiTouchController = make<MultiTouchController>(*this);
    this->addMouseListener(this->multiTouchController.get(), false); // false = listens only this one

    this->smoothPanController = make<SmoothPanController>(*this);
    this->smoothZoomController = make<SmoothZoomController>(*this);
    
    this->project.addListener(this);
    this->project.getEditMode().addChangeListener(this);
    this->project.getTransport().addTransportListener(this);
    
    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->addClippingListener(this);
    }

    App::Config().getUiFlags()->addListener(this);
}

HybridRoll::~HybridRoll()
{
    App::Config().getUiFlags()->removeListener(this);

    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->removeClippingListener(this);
    }
    
    this->removeAllRollListeners();

    this->project.getTransport().removeTransportListener(this);
    this->project.getEditMode().removeChangeListener(this);
    this->project.removeListener(this);

    this->removeMouseListener(this->multiTouchController.get());

#if HYBRID_ROLL_LISTENS_LONG_TAP
    this->removeMouseListener(this->longTapController.get());
#endif
}

Viewport &HybridRoll::getViewport() const noexcept
{
    return this->viewport;
}

Transport &HybridRoll::getTransport() const noexcept
{
    return this->project.getTransport();
}

ProjectNode &HybridRoll::getProject() const noexcept
{
    return this->project;
}

//===----------------------------------------------------------------------===//
// Timeline events
//===----------------------------------------------------------------------===//

float HybridRoll::getPositionForNewTimelineEvent() const
{
    const double playheadOffset = this->findPlayheadOffsetFromViewCentre();
    const bool playheadIsWithinScreen = fabs(playheadOffset) < (this->viewport.getViewWidth() / 2);

    // If playhead is visible, put new event on it's position, otherwise just align to the screen center
    if (playheadIsWithinScreen)
    {
        const int viewCentre = this->viewport.getViewPositionX() + (this->viewport.getViewWidth() / 2);
        const int playheadPosition = viewCentre + int(playheadOffset);
        return this->getRoundBeatSnapByXPosition(playheadPosition);
    }

    const int viewCentre = this->viewport.getViewPositionX() + (this->viewport.getViewWidth() / 2);
    return this->getRoundBeatSnapByXPosition(viewCentre);
}

void HybridRoll::insertAnnotationWithinScreen(const String &annotation)
{
    if (AnnotationsSequence *annotationsLayer = dynamic_cast<AnnotationsSequence *>(this->project.getTimeline()->getAnnotations()))
    {
        annotationsLayer->checkpoint();
        const float targetBeat = this->getPositionForNewTimelineEvent();
        AnnotationEvent event(annotationsLayer, targetBeat, annotation, Colours::transparentWhite);
        annotationsLayer->insert(event, true);
    }
}

void HybridRoll::insertTimeSignatureWithinScreen(int numerator, int denominator)
{
    jassert(denominator == 2 || 
        denominator == 4 || denominator == 8 ||
        denominator == 16 || denominator == 32);

    if (TimeSignaturesSequence *tsSequence = 
        dynamic_cast<TimeSignaturesSequence *>(this->project.
            getTimeline()->getTimeSignatures()->getSequence()))
    {
        tsSequence->checkpoint();
        const float targetBeat = this->getPositionForNewTimelineEvent();
        TimeSignatureEvent event(tsSequence, targetBeat, numerator, denominator);
        tsSequence->insert(event, true);
    }
}

//===----------------------------------------------------------------------===//
// Custom maps
//===----------------------------------------------------------------------===//

void HybridRoll::addOwnedMap(Component *newTrackMap)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);

    // fade-in if not the first child
    if (this->trackMaps.size() > 1)
    {
        newTrackMap->setVisible(false);
        this->fader.fadeIn(newTrackMap, 200);
    }

    newTrackMap->toFront(false);
    this->playhead->toFront(false);
    this->resized();
}

void HybridRoll::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        this->fader.fadeOut(existingTrackMap, 150);
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);
        this->resized();
    }
}

//===----------------------------------------------------------------------===//
// Modes
//===----------------------------------------------------------------------===//

HybridRollEditMode &HybridRoll::getEditMode() noexcept
{
    return this->project.getEditMode();
}

bool HybridRoll::isInSelectionMode() const
{
    return (this->project.getEditMode().isMode(HybridRollEditMode::selectionMode));
}

bool HybridRoll::isInDragMode() const
{
    return (this->project.getEditMode().isMode(HybridRollEditMode::dragMode));
}

//===----------------------------------------------------------------------===//
// HybridRoll listeners management
//===----------------------------------------------------------------------===//

void HybridRoll::addRollListener(HybridRollListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.add(listener);
}

void HybridRoll::removeRollListener(HybridRollListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.remove(listener);
}

void HybridRoll::removeAllRollListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.clear();
}

void HybridRoll::broadcastRollMoved()
{
    this->listeners.call(&HybridRollListener::onMidiRollMoved, this);
}

void HybridRoll::broadcastRollResized()
{
    this->listeners.call(&HybridRollListener::onMidiRollResized, this);
}

//===----------------------------------------------------------------------===//
// Input Listeners
//===----------------------------------------------------------------------===//

void HybridRoll::longTapEvent(const Point<float> &position,
    const WeakReference<Component> &target)
{
    if (this->multiTouchController->hasMultitouch())
    {
        return;
    }

    if (target == this->header.get())
    {
        this->header->showPopupMenu();
        return;
    }

    if (target == this &&
        !this->project.getEditMode().forbidsSelectionMode())
    {
        this->lassoComponent->beginLasso(position, this);
        return;
    }
}

void HybridRoll::multiTouchZoomEvent(const Point<float> &origin, const Point<float> &zoom)
{
    this->smoothPanController->cancelPan();
    this->smoothZoomController->zoomRelative(origin, zoom);
}

void HybridRoll::multiTouchPanEvent(const Point<float> &offset)
{
    //this->smoothZoomController->cancelZoom();
    this->smoothPanController->panByOffset(this->viewport.getViewPosition() + offset.toInt());
}

void HybridRoll::multiTouchCancelZoom()
{
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    this->smoothZoomController->cancelZoom();
}

void HybridRoll::multiTouchCancelPan()
{
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    this->smoothPanController->cancelPan();
}

Point<float> HybridRoll::getMultiTouchOrigin(const Point<float> &from)
{
    return (from - this->viewport.getViewPosition().toFloat());
}

//===----------------------------------------------------------------------===//
// SmoothPanListener
//===----------------------------------------------------------------------===//

inline float getNumBeatsToExpand(float beatWidth)
{
    // find the number of beats to expand, depending on a beat width, like this:
    // 1  2  4  8 16 32 64 128 256
    // |  |  |  | \/ |  |  |   |
    // |  |  |  \____/  |  |   |
    // |  |  \__________/  |   |
    // |  \________________/   |
    // \_______________________/

    const float nearestPowTwo = ceilf(log(beatWidth) / log(2.f));
    return powf(2.f, jlimit(0.f, 8.f, 8.f - nearestPowTwo));

    //DBG(this->beatWidth);
    //DBG(numBeatsToExpand);
}

void HybridRoll::panByOffset(int offsetX, int offsetY)
{
    this->stopFollowingPlayhead();

    const bool needsToStretchRight = (offsetX >= (this->getWidth() - this->viewport.getViewWidth()));
    const bool needsToStretchLeft = (offsetX <= 0);

    if (needsToStretchRight)
    {
        const float numBeatsToExpand = getNumBeatsToExpand(this->beatWidth);
        this->project.broadcastChangeViewBeatRange(this->firstBeat, this->lastBeat + numBeatsToExpand);
        this->viewport.setViewPosition(offsetX, offsetY); // after setLastBeat
        const float beatCloseToTheRight = this->lastBeat - numBeatsToExpand;
        this->header->addAndMakeVisible(new HybridRollExpandMark(*this, beatCloseToTheRight, int(numBeatsToExpand)));
    }
    else if (needsToStretchLeft)
    {
        const float numBeatsToExpand = getNumBeatsToExpand(this->beatWidth);
        const float deltaW = float(this->beatWidth * numBeatsToExpand);
        this->clickAnchor.addXY(deltaW, 0); // an ugly hack
        this->project.broadcastChangeViewBeatRange(this->firstBeat - numBeatsToExpand, this->lastBeat);
        this->viewport.setViewPosition(offsetX + int(deltaW), offsetY); // after setFirstBeat
        this->header->addAndMakeVisible(new HybridRollExpandMark(*this, this->firstBeat, int(numBeatsToExpand)));
    }
    else
    {
        this->viewport.setViewPosition(offsetX, offsetY);
    }

    this->updateChildrenPositions();
}

void HybridRoll::panProportionally(float absX, float absY)
{
    this->stopFollowingPlayhead();
    this->viewport.setViewPositionProportionately(absX, absY);
    this->updateChildrenPositions();
}

Point<int> HybridRoll::getPanOffset() const
{
    return this->viewport.getViewPosition();
}

//===----------------------------------------------------------------------===//
// SmoothZoomListener
//===----------------------------------------------------------------------===//

void HybridRoll::startSmoothZoom(const Point<float> &origin, const Point<float> &factor)
{
    this->smoothZoomController->zoomRelative(origin, factor);
}

void HybridRoll::zoomInImpulse(float factor)
{
    const auto origin = this->getViewport().getLocalBounds().getCentre();
    const Point<float> f(0.15f * factor, 0.15f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void HybridRoll::zoomOutImpulse(float factor)
{
    const auto origin = this->getViewport().getLocalBounds().getCentre();
    const Point<float> f(-0.15f * factor, -0.15f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void HybridRoll::zoomToArea(float minBeat, float maxBeat)
{
    jassert(maxBeat > minBeat);
    jassert(minBeat >= this->getFirstBeat());
    jassert(maxBeat <= this->getLastBeat());

    this->stopFollowingPlayhead();

    constexpr auto margin = Globals::beatsPerBar * 2;
    const float widthToFit = float(this->viewport.getViewWidth());
    const float numBeatsToFit = maxBeat - minBeat + (margin * 2.f);
    this->setBeatWidth(widthToFit / numBeatsToFit);

    const int minBeatX = this->getXPositionByBeat(minBeat - margin);
    this->viewport.setViewPosition(minBeatX, this->viewport.getViewPositionY());

    this->updateChildrenPositions();
}

void HybridRoll::zoomAbsolute(const Point<float> &zoom)
{
    this->stopFollowingPlayhead();

    const float newWidth = this->getNumBeats() * HYBRID_ROLL_MAX_BEAT_WIDTH * zoom.getX();
    const float beatsOnNewScreen = float(newWidth / HYBRID_ROLL_MAX_BEAT_WIDTH);
    const float viewWidth = float(this->viewport.getViewWidth());
    const float newBeatWidth = floorf(viewWidth / beatsOnNewScreen + .5f);

    this->setBeatWidth(newBeatWidth);
}

void HybridRoll::zoomRelative(const Point<float> &origin, const Point<float> &factor)
{
    this->stopFollowingPlayhead();

    const auto oldViewPosition = this->viewport.getViewPosition().toFloat();
    const auto absoluteOrigin = oldViewPosition + origin;
    const float oldWidth = float(this->getWidth());

    float newBeatWidth = this->beatWidth + (factor.getX() * this->beatWidth);
    const float estimatedNewWidth = newBeatWidth * this->getNumBeats();

    if (estimatedNewWidth < float(this->viewport.getViewWidth()))
    {
        newBeatWidth = (float(this->viewport.getWidth() + 1) / this->getNumBeats());
    } // a hack

    this->setBeatWidth(newBeatWidth); // will updateBounds() -> setSize() -> resized() -> updateChildrenBounds()

    const float newWidth = float(this->getWidth());
    const float mouseOffsetX = float(absoluteOrigin.getX() - oldViewPosition.getX());
    const float newViewPositionX = float((absoluteOrigin.getX() * newWidth) / oldWidth) - mouseOffsetX;
    this->viewport.setViewPosition(int(newViewPositionX + 0.5f), int(oldViewPosition.getY()));

    this->resetDraggingAnchors();
    this->updateChildrenPositions();
}

float HybridRoll::getZoomFactorX() const noexcept
{
    const float viewWidth = float(this->viewport.getViewWidth());
    const float beatsOnScreen = (viewWidth / this->beatWidth);
    return beatsOnScreen / this->getNumBeats();
}

float HybridRoll::getZoomFactorY() const noexcept
{
    return 1.f;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int HybridRoll::getPlayheadPositionByBeat(double targetBeat, double parentWidth) const
{
    const double widthRatio = parentWidth / double(this->getWidth());
    return int((targetBeat - this->firstBeat) * this->beatWidth * widthRatio);
}

int HybridRoll::getXPositionByBeat(float targetBeat) const noexcept
{
    return int((targetBeat - this->firstBeat) * this->beatWidth);
}

float HybridRoll::getFloorBeatSnapByXPosition(int x) const
{
    float d = FLT_MAX;
    float targetX = float(x);
    for (const float &snapX : this->allSnaps)
    {
        const float distance = fabs(x - snapX);
        if (distance < d && snapX < x)
        {
            d = distance;
            targetX = snapX;
        }
    }

    return this->getBeatByXPosition(targetX);
}

float HybridRoll::getRoundBeatSnapByXPosition(int x) const
{
    float d = FLT_MAX;
    float targetX = float(x);
    for (const float &snapX : this->allSnaps)
    {
        const float distance = fabs(x - snapX);
        if (distance < d)
        {
            d = distance;
            // get lowest beat possible for target x position:
            targetX = snapX;
        }
    }

    return this->getBeatByXPosition(targetX);
}

void HybridRoll::setBeatRange(float first, float last)
{
    if (this->lastBeat == last && this->firstBeat == first)
    {
        return;
    }

    this->lastBeat = last;
    this->firstBeat = first;
    this->updateBounds();
}

void HybridRoll::setBeatWidth(const float newBeatWidth)
{
    if (this->beatWidth == newBeatWidth ||
        newBeatWidth > 360 || newBeatWidth <= 0)
    {
        return;
    }

    this->beatWidth = newBeatWidth;
    this->updateBounds();
}

float HybridRoll::getMinVisibleBeatForCurrentZoomLevel() const
{
    // min visible beat should start from 1/16, which is the max roll resolution
    // pow-of-2     beat width   min note length
    // 4            16           ...
    // 5            32           1
    // 6            64           1/2
    // 7            128          1/4
    // 8            256          1/8
    // 9            512          1/16
    // 10           1024         ...

    // note that computeVisibleBeatLines also uses (nearestPowTwo - 5.f) to set density,
    // so that the minimum visible beat is now consistent with visible snaps;
    // probably these calculations should be refactored and put in one place:
    const float nearestPowOfTwo = ceilf(log(this->beatWidth) / log(2.f));
    const float minBeat = 1.f / powf(2.f, jlimit(0.f, 4.f, nearestPowOfTwo - 5.f));

    jassert(minBeat <= 1.f);
    jassert(minBeat >= 1.f / Globals::ticksPerBeat);

    return minBeat;
}

#define MIN_BAR_WIDTH 14
#define MIN_BEAT_WIDTH 8

void HybridRoll::computeVisibleBeatLines()
{
    this->visibleBars.clearQuick();
    this->visibleBeats.clearQuick();
    this->visibleSnaps.clearQuick();
    this->allSnaps.clearQuick();

    const auto *tsSequence =
        this->project.getTimeline()->getTimeSignatures()->getSequence();
    
    const float zeroCanvasOffset = this->firstBeat * this->beatWidth; // usually a negative value
    const float viewPosX = float(this->viewport.getViewPositionX());
    const float paintStartX = viewPosX + zeroCanvasOffset;
    const float paintEndX = float(viewPosX + this->viewport.getViewWidth()) + zeroCanvasOffset;
    
    const float barWidth = float(this->beatWidth * Globals::beatsPerBar);
    const float firstBar = this->firstBeat / float(Globals::beatsPerBar);

    const float paintStartBar = floorf(paintStartX / barWidth);
    const float paintEndBar = ceilf(paintEndX / barWidth);

    // Get the number of snaps depending on a bar width,
    // 2 for 64, 4 for 128, 8 for 256, etc:
    const float nearestPowTwo = ceilf(log(barWidth) / log(2.f));
    const float numSnaps = powf(2, jlimit(1.f, 6.f, nearestPowTwo - 5.f)); // use -4.f for twice as dense grid
    const float snapWidth = barWidth / numSnaps;

    int numerator = TIME_SIGNATURE_DEFAULT_NUMERATOR;
    int denominator = TIME_SIGNATURE_DEFAULT_DENOMINATOR;
    float barIterator = firstBar;
    int nextTsIdx = 0;
    bool firstEvent = true;

    // Find a time signature to start from (or use default values):
    // find a first time signature after a paint start and take a previous one, if any
    for (; nextTsIdx < tsSequence->size(); ++nextTsIdx)
    {
        const auto *signature = static_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(nextTsIdx));
        const float signatureBar = (signature->getBeat() / Globals::beatsPerBar);

        // The very first event defines what's before it (both time signature and offset)
        if (firstEvent)
        {
            numerator = signature->getNumerator();
            denominator = signature->getDenominator();
            const float beatStep = 1.f / float(denominator);
            const float barStep = beatStep * float(numerator);
            barIterator += (fmodf(signatureBar - firstBar, barStep) - barStep);
            firstEvent = false;
        }

        if (signatureBar >= paintStartBar)
        {
            break;
        }

        numerator = signature->getNumerator();
        denominator = signature->getDenominator();
        barIterator = signatureBar;
    }

    // At this point we have barIterator pointing at the anchor,
    // that is nearest to the left side of visible screen area
    // (it could be either TimeSignatureEvent, or just the very first bar)

    float barWidthSum = 0.f;
    bool canDrawBarLine = false;

    while (barIterator <= paintEndBar)
    {
        // We don't do anything else unless we have reached the left side of visible area
        // Since we have already found the nearest time signature,
        // we assume that time signature does not change in between,
        // and we only need to count barWidthSum:

        float beatStep = 1.f / float(denominator);
        float barStep = beatStep * float(numerator);
        const float barStartX = barWidth * barIterator - zeroCanvasOffset;
        const float stepWidth = barWidth * barStep;

        barWidthSum += stepWidth;
        canDrawBarLine = barWidthSum > MIN_BAR_WIDTH;
        barWidthSum = canDrawBarLine ? 0.f : barWidthSum;

        // When in the drawing area:
        if (barIterator >= (paintStartBar - barStep))
        {
            if (canDrawBarLine)
            {
                this->visibleBars.add(barStartX);
                this->allSnaps.add(barStartX);
            }

            // Check if we have more time signatures to come
            const auto *nextSignature = (nextTsIdx >= tsSequence->size()) ? nullptr :
                static_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(nextTsIdx));

            // Now for the beat lines
            bool lastFrame = false;
            for (float j = 0.f; j < barStep && !lastFrame; j += beatStep)
            {
                const float beatStartX = barStartX + barWidth * j;
                float nextBeatStartX = barStartX + barWidth * (j + beatStep);

                // Check for time signature change at this point
                if (nextSignature != nullptr)
                {
                    const float tsBar = nextSignature->getBeat() / Globals::beatsPerBar;
                    if (tsBar <= (barIterator + j + beatStep))
                    {
                        numerator = nextSignature->getNumerator();
                        denominator = nextSignature->getDenominator();
                        barStep = (tsBar - barIterator); // i.e. incomplete bar
                        nextBeatStartX = barStartX + barWidth * barStep;
                        nextTsIdx++;
                        barWidthSum = MIN_BAR_WIDTH; // forces to draw the next bar line
                        lastFrame = true;
                    }
                }

                // Get snap lines and beat lines
                for (float k = beatStartX + snapWidth;
                    k < (nextBeatStartX - 1);
                    k += snapWidth)
                {
                    if (k >= viewPosX)
                    {
                        this->visibleSnaps.add(k);
                        this->allSnaps.add(k);
                    }
                }

                if (beatStartX >= viewPosX &&
                    j >= beatStep && // don't draw the first one as it is a bar line
                    (nextBeatStartX - beatStartX) > MIN_BEAT_WIDTH)
                {
                    this->visibleBeats.add(beatStartX);
                    this->allSnaps.add(beatStartX);
                }
            }
        }

        barIterator += barStep;
    }
}

//===----------------------------------------------------------------------===//
// Alternative keydown modes (space for drag, etc.)
//===----------------------------------------------------------------------===//

void HybridRoll::setSpaceDraggingMode(bool dragMode)
{
    if (this->spaceDragMode == dragMode)
    {
        return;
    }

    this->spaceDragMode = dragMode;
    this->resetDraggingAnchors();
    this->draggedDistance = 0;
    this->timeEnteredDragMode = Time::getCurrentTime();

    if (dragMode)
    {
        this->project.getEditMode().setMode(HybridRollEditMode::dragMode, true);
    }
    else
    {
        this->project.getEditMode().unsetLastMode();
    }
}

bool HybridRoll::isUsingSpaceDraggingMode() const
{
    return this->spaceDragMode;
}

void HybridRoll::setAltDrawingMode(bool drawMode)
{
    if (this->altDrawMode == drawMode)
    {
        return;
    }

    this->altDrawMode = drawMode;

    if (drawMode)
    {
        this->project.getEditMode().setMode(HybridRollEditMode::drawMode, true);
    }
    else
    {
        this->project.getEditMode().unsetLastMode();
    }
}

bool HybridRoll::isUsingAltDrawingMode() const
{
    return this->altDrawMode;
}

bool HybridRoll::isUsingAnyAltMode() const
{
    return this->isUsingAltDrawingMode() || this->isUsingSpaceDraggingMode();
}

//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

Lasso &HybridRoll::getLassoSelection()
{
    return this->selection;
}

void HybridRoll::selectEvent(SelectableComponent *event, bool shouldClearAllOthers)
{
    if (shouldClearAllOthers)
    {
        this->selection.deselectAll();
    }

    if (event != nullptr)
    {
        this->selection.addToSelection(event);
    }
}

void HybridRoll::deselectEvent(SelectableComponent *event)
{
    this->selection.deselect(event);
}

void HybridRoll::deselectAll()
{
    this->selection.deselectAll();
}

SelectionComponent *HybridRoll::getSelectionComponent() const noexcept
{
    return this->lassoComponent.get();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void HybridRoll::onChangeMidiEvent(const MidiEvent &event, const MidiEvent &newEvent)
{
    // Time signatures have changed, need to repaint
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onChangeProjectBeatRange(float newFirstBeat, float newLastBeat)
{
    jassert(this->projectFirstBeat != newFirstBeat || this->projectLastBeat != newLastBeat);

    if (this->projectFirstBeat == newFirstBeat && this->projectLastBeat == newLastBeat)
    {
        return;
    }

    this->projectFirstBeat = newFirstBeat;
    this->projectLastBeat = newLastBeat;

    const float rollFirstBeat = jmin(this->firstBeat, newFirstBeat);
    const float rollLastBeat = jmax(this->lastBeat, newLastBeat);

    this->setBeatRange(rollFirstBeat, rollLastBeat);
}

void HybridRoll::onChangeViewBeatRange(float newFirstBeat, float newLastBeat)
{
    const auto viewPos = this->viewport.getViewPosition();
    const auto viewStartBeat = this->getBeatByXPosition(float(viewPos.x));

    this->setBeatRange(newFirstBeat, newLastBeat);

    // It's often the case that I expand visible range in a pattern editor,
    // then switch back to piano roll and find the viewport focus fucked up;
    // let's try to detect that and preserve offset, when the roll is inactive:
    if (!this->isVisible())
    {
        const auto newViewX = this->getXPositionByBeat(viewStartBeat);
        this->viewport.setViewPosition(newViewX, viewPos.y);
    }
}

void HybridRoll::onBeforeReloadProjectContent()
{
    this->selection.deselectAll();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void HybridRoll::mouseDown(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        this->lassoComponent->endLasso();
        return;
    }

    if (this->isLassoEvent(e))
    {
        this->lassoComponent->beginLasso(e.position, this);
    }
    else if (this->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->resetDraggingAnchors();
    }
    else if (this->isViewportZoomEvent(e))
    {
        this->startZooming();
    }
}

void HybridRoll::mouseDrag(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->dragLasso(e); // if any. will do the check itself
    }
    else if (this->isViewportDragEvent(e))
    {
        this->continueDragging(e);
    }
    else if (this->isViewportZoomEvent(e))
    {
        this->continueZooming(e);
    }
}

void HybridRoll::mouseUp(const MouseEvent &e)
{
    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }

    this->setMouseCursor(this->project.getEditMode().getCursor());

    if (this->isViewportZoomEvent(e))
    {
        this->endZooming();
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->endLasso();
    }

#if HELIO_DESKTOP
#   define MIN_PAN_DISTANCE 10
#elif HELIO_MOBILE
#   define MIN_PAN_DISTANCE 20
#endif

    if (e.mods.isLeftButtonDown() && (e.getDistanceFromDragStart() < MIN_PAN_DISTANCE))
    {
        this->deselectAll();
    }
}

void HybridRoll::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    // TODO check if any operation is in progress (lasso drag, knife tool drag, etc)
    const float inititalSpeed = this->smoothZoomController->getInitialZoomSpeed();
    const float forwardWheel = wheel.deltaY * (wheel.isReversed ? -inititalSpeed : inititalSpeed);
    const auto mouseOffset = (event.position - this->viewport.getViewPosition().toFloat());
    if (event.mods.isAnyModifierKeyDown())
    {
        this->startSmoothZoom(mouseOffset, Point<float>(0.f, forwardWheel));
    }
    else
    {
        this->startSmoothZoom(mouseOffset, Point<float>(forwardWheel, 0.f));
    }
}

void HybridRoll::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::EditModeDefault:
        this->project.getEditMode().setMode(HybridRollEditMode::defaultMode);
        break;
    case CommandIDs::EditModeDraw:
        this->project.getEditMode().setMode(HybridRollEditMode::drawMode);
        break;
    case CommandIDs::EditModePan:
        this->project.getEditMode().setMode(HybridRollEditMode::dragMode);
        break;
    case CommandIDs::EditModeSelect:
        this->project.getEditMode().setMode(HybridRollEditMode::selectionMode);
        break;
    case CommandIDs::EditModeKnife:
        this->project.getEditMode().setMode(HybridRollEditMode::knifeMode);
        break;
    case CommandIDs::EditModeEraser:
        this->project.getEditMode().setMode(HybridRollEditMode::eraserMode);
        break;
    case CommandIDs::Undo:
        if (this->project.getUndoStack()->canUndo())
        {
            // when new notes are added, they will be automatically added to selection,
            // so if we're about to undo note removal, or redo note addition (both result in added notes),
            // we need to drop the selection first:
            const bool shouldDropSelection =
                this->project.getUndoStack()->undoHas<NoteRemoveAction>() ||
                this->project.getUndoStack()->undoHas<NotesGroupRemoveAction>();

            // when there are only note changes or note removals,
            // it's more convenient to leave the selection as it is:
            if (shouldDropSelection)
            {
                this->deselectAll();
            }

            this->project.undo();
        }
        break;
    case CommandIDs::Redo:
        if (this->project.getUndoStack()->canRedo())
        {
            const bool shouldDropSelection =
                this->project.getUndoStack()->redoHas<NoteInsertAction>() ||
                this->project.getUndoStack()->redoHas<NotesGroupInsertAction>();

            if (shouldDropSelection)
            {
                this->deselectAll();
            }

            this->project.redo();
        }
        break;
    case CommandIDs::ZoomIn:
        this->zoomInImpulse(0.75f);
        break;
    case CommandIDs::ZoomOut:
        this->zoomOutImpulse(0.75f);
        break;
    case CommandIDs::TimelineJumpNext:
        if (!this->getTransport().isPlaying())
        {
            this->stopFollowingPlayhead();
            const auto newSeek = this->findNextAnchorBeat(this->getTransport().getSeekBeat());
            const auto newSeekSafe = jlimit(this->projectFirstBeat, this->projectLastBeat, newSeek);
            this->getTransport().seekToBeat(newSeekSafe);
            this->scrollToSeekPosition();
        }
        break;
    case CommandIDs::TimelineJumpPrevious:
        if (!this->getTransport().isPlaying())
        {
            this->stopFollowingPlayhead();
            const auto newSeek = this->findPreviousAnchorBeat(this->getTransport().getSeekBeat());
            const auto newSeekSafe = jlimit(this->projectFirstBeat, this->projectLastBeat, newSeek);
            this->getTransport().seekToBeat(newSeekSafe);
            this->scrollToSeekPosition();
        }
        break;
    case CommandIDs::StartDragViewport:
        this->header->setSoundProbeMode(true);
        this->setSpaceDraggingMode(true);
        break;
    case CommandIDs::EndDragViewport:
        this->header->setSoundProbeMode(false);
        if (this->isUsingSpaceDraggingMode())
        {
            const bool noDraggingWasDone = (this->draggedDistance < 3);
            const bool notTooMuchTimeSpent = (Time::getCurrentTime() - this->timeEnteredDragMode).inMilliseconds() < 300;
            if (noDraggingWasDone && notTooMuchTimeSpent)
            {
                this->getTransport().toggleStartStopPlayback();
            }
            this->setSpaceDraggingMode(false);
        }
        break;
    case CommandIDs::TransportRecordingAwait:
        if (this->getTransport().isRecording())
        {
            this->getTransport().stopRecording();
        }
        else
        {
            this->getTransport().startRecording();
        }
        break;
    case CommandIDs::TransportRecordingStart:
        this->stopFollowingPlayhead();
        this->getTransport().startRecording();
        if (!this->getTransport().isPlaying())
        {
            this->getTransport().startPlayback();
        }
        this->startFollowingPlayhead();
        break;
    case CommandIDs::TransportPlaybackStart:
        this->stopFollowingPlayhead();
        if (!this->getTransport().isPlaying())
        {
            this->getTransport().startPlayback();
        }
        this->startFollowingPlayhead();
        break;
    case CommandIDs::TransportStop:
        if (!this->getTransport().isPlaying())
        {
            // escape keypress when not playing also resets some stuff:
            this->resetAllClippingIndicators();
            this->resetAllOversaturationIndicators();

            if (!this->getTransport().isRecording())
            {
                this->getTransport().disableLoopPlayback();
            }
        }

        this->getTransport().stopPlaybackAndRecording();
        this->getTransport().stopSound();
        break;
    case CommandIDs::VersionControlToggleQuickStash:
        if (auto *vcs = this->project.findChildOfType<VersionControlNode>())
        {
            this->deselectAll(); // a couple of hacks, instead will need to improve event system
            this->getTransport().stopPlaybackAndRecording(); // with a pre-reset callback or so
            vcs->toggleQuickStash();
        }
        break;
    case CommandIDs::AddAnnotation:
        if (auto *sequence = dynamic_cast<AnnotationsSequence *>
            (this->project.getTimeline()->getAnnotations()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            App::showModalComponent(AnnotationDialog::addingDialog(*this, sequence, targetBeat));
        }
        break;
    case CommandIDs::AddTimeSignature:
        if (auto *sequence = dynamic_cast<TimeSignaturesSequence *>
            (this->project.getTimeline()->getTimeSignatures()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            App::showModalComponent(TimeSignatureDialog::addingDialog(*this, sequence, targetBeat));
        }
        break;
    case CommandIDs::AddKeySignature:
        if (auto *sequence = dynamic_cast<KeySignaturesSequence *>
            (this->project.getTimeline()->getKeySignatures()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            App::showModalComponent(KeySignatureDialog::addingDialog(*this,
                this->getTransport(), sequence, targetBeat));
        }
        break;
    default:
        break;
    }
}

void HybridRoll::resized()
{
    this->updateChildrenBounds();
}

void HybridRoll::paint(Graphics &g)
{
    this->computeVisibleBeatLines();

    const float y = float(this->viewport.getViewPositionY());
    const float h = float(this->viewport.getViewHeight());

    g.setColour(this->barLineColour);
    for (const auto &f : this->visibleBars)
    {
        g.fillRect(floorf(f), y, 1.f, h);
    }

    g.setColour(this->barLineBevelColour);
    for (const auto &f : this->visibleBars)
    {
        g.fillRect(floorf(f + 1.f), y, 1.f, h);
    }

    g.setColour(this->beatLineColour);
    for (const auto &f : this->visibleBeats)
    {
        g.fillRect(floorf(f), y, 1.f, h);
    }
    
    g.setColour(this->snapLineColour);
    for (const auto &f : this->visibleSnaps)
    {
        g.fillRect(floorf(f), y, 1.f, h);
    }
}

//===----------------------------------------------------------------------===//
// Playhead::Listener
//===----------------------------------------------------------------------===//

void HybridRoll::onPlayheadMoved(int playheadX)
{
    if (this->shouldFollowPlayhead)
    {
        const int viewHalfWidth = this->viewport.getViewWidth() / 2;
        const int viewportCentreX = this->viewport.getViewPositionX() + viewHalfWidth;
        const double offset = double(playheadX) - double(viewportCentreX);
        // Smoothness should depend on zoom level (TODO it smarter):
        const double smoothCoefficient = (this->beatWidth > 75) ? 0.915 : 0.975;
        const double smoothThreshold = (this->beatWidth > 75) ? 128.0 : 5.0;
        const double newOffset = (abs(offset) < smoothThreshold) ? 0.0 : offset * smoothCoefficient;
        const int newViewPosX = playheadX - viewHalfWidth - int(round(newOffset));
        this->viewport.setViewPosition(newViewPosX, this->viewport.getViewPositionY());
        this->updateChildrenPositions();
    }
}

//===----------------------------------------------------------------------===//
// VolumeCallback::ClippingListener
//===----------------------------------------------------------------------===//

void HybridRoll::onClippingWarning()
{
    if (! this->getTransport().isPlaying())
    {
        return;
    }
    
    const float clippingBeat = this->lastTransportBeat.get();
    
    if (this->clippingIndicators.size() > 0)
    {
        const float lastMarkerEndBeat = this->clippingIndicators.getLast()->getEndBeat();
        const bool justNeedToUpdateLastMarker = ((clippingBeat - lastMarkerEndBeat) < CLIPPING_MARKER_MAX_GAP_IN_BEATS);
        
        if (justNeedToUpdateLastMarker)
        {
            this->clippingIndicators.getLast()->setEndBeat(clippingBeat);
            return;
        }
    }
    
    auto *newMarker = new TimelineWarningMarker(TimelineWarningMarker::Red, *this, clippingBeat);
    this->clippingIndicators.add(newMarker);
    this->addAndMakeVisible(newMarker);
}

void HybridRoll::resetAllClippingIndicators()
{
    this->clippingIndicators.clear();
}

void HybridRoll::onOversaturationWarning()
{
    if (! this->getTransport().isPlaying())
    {
        return;
    }
    
    const float warningBeat = this->lastTransportBeat.get();

    if (this->oversaturationIndicators.size() > 0)
    {
        const float lastMarkerEndBeat = this->oversaturationIndicators.getLast()->getEndBeat();
        const bool justNeedToUpdateLastMarker = ((warningBeat - lastMarkerEndBeat) < CLIPPING_MARKER_MAX_GAP_IN_BEATS);
        
        if (justNeedToUpdateLastMarker)
        {
            this->oversaturationIndicators.getLast()->setEndBeat(warningBeat);
            return;
        }
    }
    
    auto *newMarker = new TimelineWarningMarker(TimelineWarningMarker::Yellow, *this, warningBeat);
    this->oversaturationIndicators.add(newMarker);
    this->addAndMakeVisible(newMarker);
}

void HybridRoll::resetAllOversaturationIndicators()
{
    this->oversaturationIndicators.clear();
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void HybridRoll::onSeek(float beatPosition, double currentTimeMs, double totalTimeMs)
{
    this->lastTransportBeat = beatPosition;
}

void HybridRoll::onLoopModeChanged(bool hasLoop, float start, float end)
{
    this->header->showLoopMode(hasLoop, start, end);
}

void HybridRoll::onPlay()
{
    this->resetAllClippingIndicators();
    this->resetAllOversaturationIndicators();
}

void HybridRoll::onRecord()
{
    this->header->showRecordingMode(true);
}

void HybridRoll::onStop()
{
    this->header->showRecordingMode(false);

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    // todo sync screen back to playhead?
    this->stopFollowingPlayhead();
#endif
}

void HybridRoll::startFollowingPlayhead()
{
#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->playheadOffset = this->findPlayheadOffsetFromViewCentre();
    this->shouldFollowPlayhead = true;
#endif
}

void HybridRoll::stopFollowingPlayhead()
{
    if (! this->shouldFollowPlayhead)
    {
        return;
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->stopTimer();
    this->shouldFollowPlayhead = false;
#endif
}

void HybridRoll::scrollToSeekPosition()
{
#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->startFollowingPlayhead();
    this->startTimer(7);
#else
    const int playheadX = this->getXPositionByBeat(this->lastTransportBeat.get());
    this->viewport.setViewPosition(playheadX - (this->viewport.getViewWidth() / 3), this->viewport.getViewPositionY());
    this->updateChildrenBounds();
#endif
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void HybridRoll::handleAsyncUpdate()
{
    // batch repaint & resize stuff
    if (this->batchRepaintList.size() > 0)
    {
        HYBRID_ROLL_BULK_REPAINT_START

        for (int i = 0; i < this->batchRepaintList.size(); ++i)
        {
            // There are still many cases when a scheduled component is deleted at this time:
            if (FloatBoundsComponent *component = this->batchRepaintList.getUnchecked(i))
            {
                const Rectangle<float> nb(this->getEventBounds(component));
                component->setFloatBounds(nb);
                component->repaint();
            }
        }

        HYBRID_ROLL_BULK_REPAINT_END

        this->batchRepaintList.clearQuick();
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    if (this->isTimerRunning())
    {
        const int playheadX = this->getPlayheadPositionByBeat(this->lastTransportBeat.get(), double(this->getWidth()));
        const int newX = playheadX - int(this->playheadOffset.get() * 0.75) -
            (this->viewport.getViewWidth() / 2);

        this->viewport.setViewPosition(newX, this->viewport.getViewPositionY());
        this->playheadOffset = this->findPlayheadOffsetFromViewCentre();
        this->updateChildrenPositions();
    }
#endif
}

double HybridRoll::findPlayheadOffsetFromViewCentre() const
{
    const int playheadX = this->getPlayheadPositionByBeat(this->lastTransportBeat.get(), double(this->getWidth()));
    const int viewportCentreX = this->viewport.getViewPositionX() + this->viewport.getViewWidth() / 2;
    return double(playheadX) - double(viewportCentreX);
}

void HybridRoll::triggerBatchRepaintFor(FloatBoundsComponent *target)
{
    this->batchRepaintList.add(target);
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void HybridRoll::hiResTimerCallback()
{
    if (fabs(this->playheadOffset.get()) < 0.1)
    {
        this->stopTimer();
    }

    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Events check
//===----------------------------------------------------------------------===//

bool HybridRoll::isViewportZoomEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsViewportZooming())   { return false; }
    if (this->project.getEditMode().forcesViewportZooming())    { return true; }
    return false;
}

bool HybridRoll::isViewportDragEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsViewportDragging())  { return false; }
    if (this->project.getEditMode().forcesViewportDragging())   { return true; }
    if (e.source.isTouch())                                     { return e.mods.isLeftButtonDown(); }
    return (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown());
}

bool HybridRoll::isAddEvent(const MouseEvent &e) const
{
    if (e.mods.isRightButtonDown())                         { return false; }
    if (this->project.getEditMode().forbidsAddingEvents())  { return false; }
    if (this->project.getEditMode().forcesAddingEvents())   { return true; }
    return false;
}

bool HybridRoll::isLassoEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsSelectionMode()) { return false; }
    if (this->project.getEditMode().forcesSelectionMode())  { return true; }
    if (e.source.isTouch())                                 { return false; }
    return e.mods.isLeftButtonDown();
}

bool HybridRoll::isKnifeToolEvent(const MouseEvent &e) const
{
    if (e.mods.isRightButtonDown())                         { return false; }
    if (this->project.getEditMode().forbidsCuttingEvents()) { return false; }
    if (this->project.getEditMode().forcesCuttingEvents())  { return true; }
    return false;
}

void HybridRoll::resetDraggingAnchors()
{
    this->viewportAnchor = this->viewport.getViewPosition();
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
}

void HybridRoll::continueDragging(const MouseEvent &e)
{
    this->draggedDistance = e.getDistanceFromDragStart();
    this->smoothZoomController->cancelZoom();
    this->smoothPanController->panByOffset(this->getMouseOffset(e.source.getScreenPosition()).toInt());
}

//===----------------------------------------------------------------------===//
// Zooming
//===----------------------------------------------------------------------===//

void HybridRoll::startZooming()
{
    this->smoothPanController->cancelPan();
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();

    this->zoomAnchor.setXY(0, 0);

    this->zoomMarker.reset(new IconComponent(Icons::zoomIn));
    this->zoomMarker->setAlwaysOnTop(true);

    const Point<int> vScreenPosition(this->viewport.getScreenPosition());
    const Point<int> sMouseDownPosition(this->clickAnchor.toInt());
    const Point<int> vMouseDownPosition(sMouseDownPosition - vScreenPosition);
    this->zoomMarker->setSize(24, 24);
    this->zoomMarker->setCentrePosition(vMouseDownPosition.getX(), vMouseDownPosition.getY());
    this->viewport.addAndMakeVisible(this->zoomMarker.get());

    Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(true, false);
}

void HybridRoll::continueZooming(const MouseEvent &e)
{
    Desktop::getInstance().getMainMouseSource().setScreenPosition(e.getMouseDownScreenPosition().toFloat());

    const Point<float> clickOffset = e.position - this->viewport.getViewPosition().toFloat();
    const float dragOffsetX = float(e.getPosition().getX() - e.getMouseDownPosition().getX());
    const float dragOffsetY = -float(e.getPosition().getY() - e.getMouseDownPosition().getY());
    const Point<float> zoomOffset = (Point<float>(dragOffsetX, dragOffsetY) - this->zoomAnchor).toFloat() * 0.005f;

    this->smoothZoomController->zoomRelative(clickOffset, zoomOffset);
    //    this->zoomAnchor.setXY(dragOffsetX, dragOffsetY);

    //    const Point<int> markerCentre(this->zoomMarker->getBounds().getCentre());
    //    this->zoomMarker->setSize(jmax<int>(32, abs(dragOffsetX * 2)), jmax<int>(32, abs(dragOffsetY * 2)));
    //    this->zoomMarker->setCentrePosition(markerCentre.getX(), markerCentre.getY());
}

void HybridRoll::endZooming()
{
    Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(false, false);

    this->zoomAnchor.setXY(0, 0);
    this->zoomMarker = nullptr;
}


Point<float> HybridRoll::getMouseOffset(Point<float> mouseScreenPosition) const
{
    const int w = this->getWidth() - this->viewport.getWidth();

    const Point<float> distanceFromDragStart = mouseScreenPosition - this->clickAnchor;
    float x = this->viewportAnchor.getX() - distanceFromDragStart.getX();

    x = (x < 0) ? 0 : x;
    x = (x > w) ? w : x;

    const int h = this->getHeight() - this->viewport.getHeight();
    float y = this->viewportAnchor.getY() - distanceFromDragStart.getY();

    y = (y < 0) ? 0 : y;
    y = (y > h) ? h : y;

    return Point<float>(x, y);
}

Point<int> HybridRoll::getDefaultPositionForPopup() const
{
    // a point where pop-ups will appear when keypress is hit or toolbar button is clicked
    // on desktop, if mouse position is within a roll, use it, instead, use main layout centre
#if HELIO_DESKTOP
    const auto mousePositionWithinApp =
        Desktop::getInstance().getMainMouseSource().getScreenPosition().toInt() -
        App::Layout().getScreenBounds().getPosition();

    if (App::Layout().getPageBounds().contains(mousePositionWithinApp))
    {
        return mousePositionWithinApp;
    }
#endif

    return App::Layout().getPageBounds().getCentre();
}

void HybridRoll::updateBounds()
{
    const int newWidth = int(this->getNumBeats() * this->beatWidth);

    if (this->getWidth() != newWidth)
    {
        this->setSize(newWidth, this->getHeight());
    }
}

void HybridRoll::updateChildrenBounds()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const int &viewHeight = this->viewport.getViewHeight();
    const int &viewWidth = this->viewport.getViewWidth();
    const int &viewX = this->viewport.getViewPositionX();
    const int &viewY = this->viewport.getViewPositionY();

    this->header->setBounds(0, viewY, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    this->headerShadow->setBounds(viewX, viewY + HYBRID_ROLL_HEADER_HEIGHT, viewWidth, HYBRID_ROLL_HEADER_SHADOW_SIZE);

    if (this->annotationsMap != nullptr)
    {
        this->annotationsMap->setBounds(0, viewY + HYBRID_ROLL_HEADER_HEIGHT, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->keySignaturesMap != nullptr)
    {
        this->keySignaturesMap->setBounds(0, viewY + HYBRID_ROLL_HEADER_HEIGHT, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->timeSignaturesMap != nullptr)
    {
        this->timeSignaturesMap->setBounds(0, viewY, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT - 1);
    }

    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        Component *const trackMap = this->trackMaps.getUnchecked(i);
        trackMap->setBounds(0, viewY + viewHeight - trackMap->getHeight(),
                            this->getWidth(), trackMap->getHeight());
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->updateBounds();
    }

    this->broadcastRollResized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void HybridRoll::updateChildrenPositions()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const int &viewHeight = this->viewport.getViewHeight();
    const int &viewX = this->viewport.getViewPositionX();
    const int &viewY = this->viewport.getViewPositionY();

    this->header->setTopLeftPosition(0, viewY);
    this->headerShadow->setTopLeftPosition(viewX, viewY + HYBRID_ROLL_HEADER_HEIGHT);

    if (this->annotationsMap != nullptr)
    {
        this->annotationsMap->setTopLeftPosition(0, viewY + HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->keySignaturesMap != nullptr)
    {
        this->keySignaturesMap->setTopLeftPosition(0, viewY + HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->timeSignaturesMap != nullptr)
    {
        this->timeSignaturesMap->setTopLeftPosition(0, viewY);
    }

    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        Component *const trackMap = this->trackMaps.getUnchecked(i);
        trackMap->setTopLeftPosition(0, viewY + viewHeight - trackMap->getHeight());
    }

    this->broadcastRollMoved();

    HYBRID_ROLL_BULK_REPAINT_END
}

float HybridRoll::findNextAnchorBeat(float beat) const
{
    // to be overridden in subclasses
    return this->project.getTimeline()->findNextAnchorBeat(beat);
}

float HybridRoll::findPreviousAnchorBeat(float beat) const
{
    return this->project.getTimeline()->findPreviousAnchorBeat(beat);
}

void HybridRoll::addTrackInteractively(MidiTrackNode *trackPreset,
    UndoActionId checkpoint, bool switchToNewTrack, const String &defaultTrackName,
    const String &dialogTitle, const String &dialogConfirmation)
{
    if (trackPreset == nullptr ||
        trackPreset->getSequence() == nullptr ||
        trackPreset->getPattern() == nullptr ||
        trackPreset->getPattern()->getClips().isEmpty())
    {
        jassertfalse;
        return;
    }

    const auto trackId = trackPreset->getTrackId();
    const auto trackTemplate = trackPreset->serialize();
    const auto newName = SequencerOperations::generateNextNameForNewTrack(defaultTrackName,
        this->project.getAllTrackNames());

    this->project.getUndoStack()->perform(new PianoTrackInsertAction(this->project,
        &this->project, trackTemplate, newName));

    auto *newlyAddedTrack = this->project.findTrackById<MidiTrackNode>(trackId);

    if (switchToNewTrack)
    {
        auto *tracksSingleClip = newlyAddedTrack->getPattern()->getUnchecked(0);
        this->project.setEditableScope(newlyAddedTrack, *tracksSingleClip, false);
    }

    auto dialog = make<TrackPropertiesDialog>(this->project,
        newlyAddedTrack, dialogTitle, dialogConfirmation);

    dialog->onCancel = [this]()
    {
        // make it rather undoUpTo(checkpoint) ?
        this->project.getUndoStack()->undo();
    };

    dialog->onOk = [this, checkpoint]()
    {
        if (checkpoint != UndoActionIDs::None)
        {
            this->project.getUndoStack()->mergeTransactionsUpTo(checkpoint);
        }
    };

    App::showModalComponent(move(dialog));
}

//===----------------------------------------------------------------------===//
// ChangeListener: edit mode changed
//===----------------------------------------------------------------------===//

void HybridRoll::changeListenerCallback(ChangeBroadcaster *source)
{
    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->endLasso();
    }

    this->applyEditModeUpdates();
}

void HybridRoll::applyEditModeUpdates()
{
    if (this->isUsingSpaceDraggingMode() &&
        !(this->project.getEditMode().isMode(HybridRollEditMode::dragMode)))
    {
        this->setSpaceDraggingMode(false);
    }

    if (this->isUsingAltDrawingMode() &&
        !(this->project.getEditMode().isMode(HybridRollEditMode::drawMode)))
    {
        this->setAltDrawingMode(false);
    }

    const MouseCursor cursor(this->project.getEditMode().getCursor());
    this->setMouseCursor(cursor);

    const bool interactsWithChildren = this->project.getEditMode().shouldInteractWithChildren();
    this->setChildrenInteraction(interactsWithChildren,
        interactsWithChildren ? MouseCursor::NormalCursor : cursor);
}
