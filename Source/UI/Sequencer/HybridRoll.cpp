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
#include "ProjectNode.h"

#include "ShadowDownwards.h"
#include "ShadowUpwards.h"

#include "TimelineWarningMarker.h"
#include "SerializationKeys.h"

#include "LongTapController.h"
#include "SmoothPanController.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "Origami.h"
#include "MainLayout.h"

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

#include "AnnotationDialog.h"
#include "TimeSignatureDialog.h"

#include "Workspace.h"
#include "AudioCore.h"
#include "AudioMonitor.h"

#include "ColourIDs.h"

#include <limits.h>

#if HELIO_DESKTOP
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 1
#else
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 0
#endif

// force compile template
#include "AnnotationsProjectMap.cpp"
template class AnnotationsProjectMap<AnnotationLargeComponent>;

// force compile template
#include "TimeSignaturesProjectMap.cpp"
template class TimeSignaturesProjectMap<TimeSignatureLargeComponent>;

// force compile template
#include "KeySignaturesProjectMap.cpp"
template class KeySignaturesProjectMap<KeySignatureLargeComponent>;


HybridRoll::HybridRoll(ProjectNode &parentProject, Viewport &viewportRef,
    WeakReference<AudioMonitor> audioMonitor,
    bool hasAnnotationsTrack,
    bool hasKeySignaturesTrack,
    bool hasTimeSignaturesTrack) :
    clippingDetector(audioMonitor),
    project(parentProject),
    viewport(viewportRef),
    viewportAnchor(0, 0),
    clickAnchor(0, 0),
    zoomAnchor(0, 0),
    barWidth(0),
    firstBar(FLT_MAX),
    lastBar(-FLT_MAX),
    projectFirstBeat(0.f),
    projectLastBeat(DEFAULT_NUM_BARS * BEATS_PER_BAR),
    header(nullptr),
    playhead(nullptr),
    spaceDragMode(false),
    altDrawMode(false),
    draggedDistance(0),
    timeEnteredDragMode(0),
    lastTransportPosition(0.0),
    playheadOffset(0.0),
    shouldFollowPlayhead(false),
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

    this->topShadow = new ShadowDownwards(Normal);
    this->bottomShadow = new ShadowUpwards(Normal);

    this->header = new HybridRollHeader(this->project.getTransport(), *this, this->viewport);

    if (hasAnnotationsTrack)
    {
        this->annotationsTrack = new AnnotationsLargeMap(this->project, *this);
    }

    if (hasTimeSignaturesTrack)
    {
        this->timeSignaturesTrack = new TimeSignaturesLargeMap(this->project, *this);
    }

    if (hasKeySignaturesTrack)
    {
        this->keySignaturesTrack = new KeySignaturesLargeMap(this->project, *this);
    }

    this->playhead = new Playhead(*this, this->project.getTransport(), this);

    this->lassoComponent = new SelectionComponent();
    this->lassoComponent->setWantsKeyboardFocus(false);
    this->lassoComponent->setFocusContainer(false);

    this->addAndMakeVisible(this->topShadow);
    this->addAndMakeVisible(this->bottomShadow);
    this->addAndMakeVisible(this->header);

    if (this->annotationsTrack)
    {
        this->addAndMakeVisible(this->annotationsTrack);
    }

    if (this->timeSignaturesTrack)
    {
        this->addAndMakeVisible(this->timeSignaturesTrack);
    }

    if (this->keySignaturesTrack)
    {
        this->addAndMakeVisible(this->keySignaturesTrack);
    }

    this->addAndMakeVisible(this->playhead);

    this->addAndMakeVisible(this->lassoComponent);

    this->longTapController = new LongTapController(*this);
    this->addMouseListener(this->longTapController, true); // on this and * children

    this->multiTouchController = new MultiTouchController(*this);
    this->addMouseListener(this->multiTouchController, false); // on this only

    this->smoothPanController = new SmoothPanController(*this);
    this->smoothZoomController = new SmoothZoomController(*this);
    
    this->project.addListener(this);
    this->project.getEditMode().addChangeListener(this);
    this->project.getTransport().addTransportListener(this);
    
    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->addClippingListener(this);
    }
}

HybridRoll::~HybridRoll()
{
    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->removeClippingListener(this);
    }
    
    this->removeAllRollListeners();

    this->project.getTransport().removeTransportListener(this);
    this->project.getEditMode().removeChangeListener(this);
    this->project.removeListener(this);

    this->removeMouseListener(this->multiTouchController);
    this->removeMouseListener(this->longTapController);
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
        return this->getRoundBeatByXPosition(playheadPosition);
    }

    const int viewCentre = this->viewport.getViewPositionX() + (this->viewport.getViewWidth() / 2);
    return this->getRoundBeatByXPosition(viewCentre);
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
// MultiTouchListener
//===----------------------------------------------------------------------===//

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

void HybridRoll::panByOffset(const int offsetX, const int offsetY)
{
    this->stopFollowingPlayhead();

    const bool needsToStretchRight = (offsetX >= (this->getWidth() - this->viewport.getViewWidth()));
    const bool needsToStretchLeft = (offsetX <= 0);

    float numBarsToExpand = 1.f;
    if (this->barWidth <= 1)
    {
        numBarsToExpand = 64.f;
    }
    else if (this->barWidth <= 16)
    {
        numBarsToExpand = 16.f;
    }
    else if (this->barWidth <= 64)
    {
        numBarsToExpand = 4.f;
    }
    
    if (needsToStretchRight)
    {
        this->project.broadcastChangeViewBeatRange(
            this->firstBar * float(BEATS_PER_BAR),
            (this->lastBar + numBarsToExpand) * float(BEATS_PER_BAR));
        this->viewport.setViewPosition(offsetX, offsetY); // after setLastBar
        const float barCloseToTheRight = this->lastBar - numBarsToExpand;
        this->header->addAndMakeVisible(new HybridRollExpandMark(*this, barCloseToTheRight, int(numBarsToExpand)));
    }
    else if (needsToStretchLeft)
    {
        const float deltaW = float(this->barWidth * numBarsToExpand);
        this->clickAnchor.addXY(deltaW, 0); // an ugly hack
        this->project.broadcastChangeViewBeatRange(
            (this->firstBar - numBarsToExpand) * float(BEATS_PER_BAR),
            this->lastBar * float(BEATS_PER_BAR));
        this->viewport.setViewPosition(offsetX + int(deltaW), offsetY); // after setFirstBar
        this->header->addAndMakeVisible(new HybridRollExpandMark(*this, this->firstBar, int(numBarsToExpand)));
    }
    else
    {
        this->viewport.setViewPosition(offsetX, offsetY);
    }

    this->updateChildrenPositions();
}

void HybridRoll::panProportionally(const float absX, const float absY)
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
    const Point<float> f(0.15f * factor, 0.05f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void HybridRoll::zoomOutImpulse(float factor)
{
    const auto origin = this->getViewport().getLocalBounds().getCentre();
    const Point<float> f(-0.15f * factor, -0.05f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void HybridRoll::zoomToArea(float minBeat, float maxBeat)
{
    jassert(maxBeat > minBeat);
    jassert(minBeat >= this->getFirstBeat());
    jassert(maxBeat <= this->getLastBeat());

    const float margin = 4.f;
    const float widthToFit = float(this->viewport.getViewWidth());
    const float numBarsToFit = (maxBeat - minBeat + margin) / BEATS_PER_BAR;
    this->setBarWidth(widthToFit / numBarsToFit);

    const int minBeatX = this->getXPositionByBeat(minBeat - (margin / 2.f));
    this->viewport.setViewPosition(minBeatX, this->viewport.getViewPositionY());

    this->playheadOffset = this->findPlayheadOffsetFromViewCentre();
}

void HybridRoll::zoomAbsolute(const Point<float> &zoom)
{
    const float newWidth = this->getNumBars() * HYBRID_ROLL_MAX_BAR_WIDTH * zoom.getX();
    const float barsOnNewScreen = float(newWidth / HYBRID_ROLL_MAX_BAR_WIDTH);
    const float viewWidth = float(this->viewport.getViewWidth());
    const float newBarWidth = floorf(viewWidth / barsOnNewScreen + .5f);
    this->setBarWidth(newBarWidth);
    this->playheadOffset = this->findPlayheadOffsetFromViewCentre();
}

void HybridRoll::zoomRelative(const Point<float> &origin, const Point<float> &factor)
{
    const auto oldViewPosition = this->viewport.getViewPosition().toFloat();
    const auto absoluteOrigin = oldViewPosition + origin;
    const float oldWidth = float(this->getWidth());

    float newBarWidth = this->getBarWidth() + (factor.getX() * this->getBarWidth());
    const float estimatedNewWidth = newBarWidth * this->getNumBars();

    if (estimatedNewWidth < float(this->viewport.getViewWidth()))
    { newBarWidth = (float(this->viewport.getWidth() + 1) / this->getNumBars()); } // a hack
    //{ newBarWidth = (float(this->viewport.getViewWidth()) / this->getNumBars()); }

    this->setBarWidth(newBarWidth);

    const float newWidth = float(this->getWidth());
    const float mouseOffsetX = float(absoluteOrigin.getX() - oldViewPosition.getX());
    const float newViewPositionX = float((absoluteOrigin.getX() * newWidth) / oldWidth) - mouseOffsetX;
    this->viewport.setViewPosition(int(newViewPositionX + 0.5f), int(oldViewPosition.getY()));

    this->playheadOffset = this->findPlayheadOffsetFromViewCentre();

    this->resetDraggingAnchors();
    this->updateChildrenPositions();
}

float HybridRoll::getZoomFactorX() const noexcept
{
    const float numBars = this->getNumBars();
    const float viewWidth = float(this->viewport.getViewWidth());
    const float barWidth = float(this->getBarWidth());
    const float barsOnScreen = (viewWidth / barWidth);
    return barsOnScreen / numBars;
}

float HybridRoll::getZoomFactorY() const noexcept
{
    return 1.f;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int HybridRoll::getXPositionByTransportPosition(double absPosition, double canvasWidth) const
{
    const double rollLengthInBeats = this->getLastBeat() - this->getFirstBeat();
    const double projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;
    const double firstBeatOffset = this->projectFirstBeat - this->getFirstBeat();

    const double trackWidth = canvasWidth * (projectLengthInBeats / rollLengthInBeats);
    const double trackStart = canvasWidth * (firstBeatOffset / rollLengthInBeats);

    return int(floor(trackStart + absPosition * trackWidth));
}

double HybridRoll::getTransportPositionByXPosition(int xPosition, double canvasWidth) const
{
    const double rollLengthInBeats = (this->getLastBeat() - this->getFirstBeat());
    const double projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);
    const double firstBeatOffset = (this->projectFirstBeat - this->getFirstBeat());

    const double trackWidth = canvasWidth * (projectLengthInBeats / rollLengthInBeats);
    const double trackStart = canvasWidth * (firstBeatOffset / rollLengthInBeats);

    return double((xPosition - trackStart) / trackWidth);
}

double HybridRoll::getTransportPositionByBeat(float targetBeat) const
{
    const auto targetBeatSafe = jlimit(this->projectFirstBeat, this->projectLastBeat, targetBeat);
    const double projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);
    return double(targetBeatSafe - this->projectFirstBeat) / projectLengthInBeats;
}

float HybridRoll::getBeatByTransportPosition(double absSeekPosition) const
{
    const double projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);
    return float(projectLengthInBeats * absSeekPosition) + this->projectFirstBeat;
}

float HybridRoll::getSeekBeat() const
{
    return this->getBeatByTransportPosition(this->getTransport().getSeekPosition());
}

float HybridRoll::getBarByXPosition(int xPosition) const
{
    const float zeroCanvasOffset = this->firstBar * this->getBarWidth();
    const float bar = (float(xPosition) + zeroCanvasOffset) / this->getBarWidth();
    return bar;
}

int HybridRoll::getXPositionByBar(float targetBar) const
{
    return int(roundf((targetBar - this->firstBar) * this->getBarWidth()));
}

int HybridRoll::getXPositionByBeat(float targetBeat) const
{
    return this->getXPositionByBar(targetBeat / float(BEATS_PER_BAR));
}

float HybridRoll::getFloorBeatByXPosition(int x) const
{
    Array<float> allSnaps;
    allSnaps.addArray(this->visibleBars);
    allSnaps.addArray(this->visibleBeats);
    allSnaps.addArray(this->visibleSnaps);

    float d = FLT_MAX;
    float targetX = float(x);
    for (float snapX : allSnaps)
    {
        const float distance = fabs(x - snapX);
        if (distance < d && snapX < x)
        {
            d = distance;
            targetX = snapX;
        }
    }

    const float beatNumber = roundBeat(targetX / this->barWidth * BEATS_PER_BAR + this->getFirstBeat());
    return jlimit(this->getFirstBeat(), this->getLastBeat(), beatNumber);
}

float HybridRoll::getRoundBeatByXPosition(int x) const
{
    Array<float> allSnaps;
    allSnaps.addArray(this->visibleBars);
    allSnaps.addArray(this->visibleBeats);
    allSnaps.addArray(this->visibleSnaps);

    float d = FLT_MAX;
    float targetX = float(x);
    for (float snapX : allSnaps)
    {
        const float distance = fabs(x - snapX);
        if (distance < d)
        {
            d = distance;
            // get lowest beat possible for target x position:
            targetX = snapX;
        }
    }

    const float beatNumber = roundBeat(targetX / this->barWidth * BEATS_PER_BAR + this->getFirstBeat());
    return jlimit(this->getFirstBeat(), this->getLastBeat(), beatNumber);
}

void HybridRoll::setBarRange(float first, float last)
{
    if (this->lastBar == last && this->firstBar == first)
    {
        return;
    }

    this->lastBar = last;
    this->firstBar = first;
    this->updateBounds();
}

void HybridRoll::setBarWidth(const float newBarWidth)
{
    if (newBarWidth > 1440 || newBarWidth <= 0) { return; }
    this->barWidth = newBarWidth;
    this->updateBounds();
}

#define MIN_BAR_WIDTH 14
#define MIN_BEAT_WIDTH 8

void HybridRoll::computeVisibleBeatLines()
{
    this->visibleBars.clearQuick();
    this->visibleBeats.clearQuick();
    this->visibleSnaps.clearQuick();

    const auto tsSequence =
        this->project.getTimeline()->getTimeSignatures()->getSequence();
    
    const float zeroCanvasOffset = this->firstBar * this->barWidth; // usually a negative value
    const float viewPosX = float(this->viewport.getViewPositionX());
    const float paintStartX = viewPosX + zeroCanvasOffset;
    const float paintEndX = float(viewPosX + this->viewport.getViewWidth()) + zeroCanvasOffset;
    
    const float paintStartBar = roundf(paintStartX / this->barWidth) - 2.f;
    const float paintEndBar = roundf(paintEndX / this->barWidth) + 1.f;
    
    // Get number of snaps depending on bar width, 
    // 2 for 64, 4 for 128, 8 for 256, etc:
    const float nearestPowTwo = ceilf(log(this->barWidth) / log(2.f));
    const float numSnaps = powf(2, jlimit(1.f, 6.f, nearestPowTwo - 5.f)); // use -4.f for twice as dense grid
    const float snapWidth = this->barWidth / numSnaps;

    int numerator = TIME_SIGNATURE_DEFAULT_NUMERATOR;
    int denominator = TIME_SIGNATURE_DEFAULT_DENOMINATOR;
    float barIterator = float(this->firstBar);
    int nextSignatureIdx = 0;
    bool firstEvent = true;

    // Find a time signature to start from (or use default values):
    // find a first time signature after a paint start and take a previous one, if any
    for (; nextSignatureIdx < tsSequence->size(); ++nextSignatureIdx)
    {
        const auto signature =
            static_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(nextSignatureIdx));

        // The very first event defines what's before it (both time signature and offset)
        if (firstEvent)
        {
            numerator = signature->getNumerator();
            denominator = signature->getDenominator();
            const float beatStep = 1.f / float(denominator);
            const float barStep = beatStep * float(numerator);
            barIterator += fmodf(signature->getBeat() / BEATS_PER_BAR - float(this->firstBar), barStep) - barStep;
            firstEvent = false;
        }

        if (signature->getBeat() >= (paintStartBar * BEATS_PER_BAR))
        {
            break;
        }

        numerator = signature->getNumerator();
        denominator = signature->getDenominator();
        barIterator = signature->getBeat() / BEATS_PER_BAR;
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
        const float barStartX = this->barWidth * barIterator - zeroCanvasOffset;
        const float stepWidth = this->barWidth * barStep;

        barWidthSum += stepWidth;
        canDrawBarLine = barWidthSum > MIN_BAR_WIDTH;
        barWidthSum = canDrawBarLine ? 0.f : barWidthSum;

        // When in the drawing area:
        if (barIterator > paintStartBar)
        {
            if (canDrawBarLine)
            {
                visibleBars.add(barStartX);
            }

            // Check if we have more time signatures to come
            TimeSignatureEvent *nextSignature = nullptr;
            if (nextSignatureIdx < tsSequence->size())
            {
                nextSignature = static_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(nextSignatureIdx));
            }

            // Now for the beat lines
            bool lastFrame = false;
            for (float j = 0.f; j < barStep && !lastFrame; j += beatStep)
            {
                const float beatStartX = barStartX + this->barWidth * j;
                float nextBeatStartX = barStartX + this->barWidth * (j + beatStep);

                // Check for time signature change at this point
                if (nextSignature != nullptr)
                {
                    const float tsBar = nextSignature->getBeat() / BEATS_PER_BAR;
                    if (tsBar <= (barIterator + j + beatStep))
                    {
                        numerator = nextSignature->getNumerator();
                        denominator = nextSignature->getDenominator();
                        barStep = (tsBar - barIterator); // i.e. incomplete bar
                        nextBeatStartX = barStartX + this->barWidth * barStep;
                        nextSignatureIdx++;
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
                        visibleSnaps.add(k);
                    }
                }

                if (beatStartX >= viewPosX &&
                    j >= beatStep && // don't draw the first one as it is a bar line
                    (nextBeatStartX - beatStartX) > MIN_BEAT_WIDTH)
                {
                    visibleBeats.add(beatStartX);
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

    if (event)
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
    return this->lassoComponent;
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void HybridRoll::onChangeMidiEvent(const MidiEvent &event, const MidiEvent &newEvent)
{
    // Time signatures have changed, need to repaint
    if (event.isTypeOf(MidiEvent::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void HybridRoll::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    if (this->projectFirstBeat == firstBeat &&
        this->projectLastBeat == lastBeat)
    {
        return;
    }

    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    const float projectFirstBar = firstBeat / float(BEATS_PER_BAR);
    const float projectLastBar = lastBeat / float(BEATS_PER_BAR);
    const float rollFirstBar = jmin(this->firstBar, projectFirstBar);
    const float rollLastBar = jmax(this->lastBar, projectLastBar);

    this->setBarRange(rollFirstBar, rollLastBar);
}

void HybridRoll::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    const float viewFirstBar = firstBeat / float(BEATS_PER_BAR);
    const float viewLastBar = lastBeat / float(BEATS_PER_BAR);
    this->setBarRange(viewFirstBar, viewLastBar);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void HybridRoll::longTapEvent(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() ||
        this->project.getEditMode().forbidsSelectionMode() ||
        e.eventComponent != this)
    {
        return;
    }

    this->lassoComponent->beginLasso(e, this);
}

void HybridRoll::mouseDown(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        this->lassoComponent->endLasso();
        return;
    }

    if (this->isLassoEvent(e))
    {
        this->lassoComponent->beginLasso(e, this);
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
    const float beatWidth = (this->barWidth / BEATS_PER_BAR);
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
        this->deselectAll(); // FIXME: don't drop selection if there are only event changes
        this->project.undo();
        break;
    case CommandIDs::Redo:
        this->deselectAll();
        this->project.redo();
        break;
    case CommandIDs::ZoomIn:
        this->zoomInImpulse();
        break;
    case CommandIDs::ZoomOut:
        this->zoomOutImpulse();
        break;
    case CommandIDs::TimelineJumpNext:
        if (!this->project.getTransport().isPlaying())
        {
            this->stopFollowingPlayhead();
            const auto beat = this->project.getTimeline()->findNextAnchorBeat(this->getSeekBeat());
            const auto newSeek = this->getTransportPositionByBeat(beat);
            this->getTransport().seekToPosition(newSeek);
            this->scrollToSeekPosition();
        }
        break;
    case CommandIDs::TimelineJumpPrevious:
        if (!this->project.getTransport().isPlaying())
        {
            this->stopFollowingPlayhead();
            const auto beat = this->project.getTimeline()->findPreviousAnchorBeat(this->getSeekBeat());
            const auto newSeek = this->getTransportPositionByBeat(beat);
            this->getTransport().seekToPosition(newSeek);
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
                this->project.getTransport().toggleStatStopPlayback();
            }
            this->setSpaceDraggingMode(false);
        }
        break;
    case CommandIDs::TransportStartPlayback:
        if (!this->project.getTransport().isPlaying())
        {
            this->stopFollowingPlayhead();
            this->project.getTransport().startPlayback();
        }
        this->startFollowingPlayhead();
        break;
    case CommandIDs::TransportPausePlayback:
        if (this->project.getTransport().isPlaying())
        {
            this->project.getTransport().stopPlayback();
        }
        else
        {
            this->resetAllClippingIndicators();
            this->resetAllOversaturationIndicators();
        }

        App::Workspace().getAudioCore().mute();
        App::Workspace().getAudioCore().unmute();
        break;
    case CommandIDs::VersionControlToggleQuickStash:
        if (auto *vcs = this->project.findChildOfType<VersionControlNode>())
        {
            this->deselectAll(); // a couple of hacks, instead will need to improve event system
            this->getTransport().stopPlayback(); // with a pre-reset callback or so
            vcs->toggleQuickStash();
        }
        break;
    case CommandIDs::AddAnnotation:
        if (auto *sequence = dynamic_cast<AnnotationsSequence *>
            (this->project.getTimeline()->getAnnotations()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            Component *dialog = AnnotationDialog::createAddingDialog(*this, sequence, targetBeat);
            App::Layout().showModalComponentUnowned(dialog);
        }
        break;
    case CommandIDs::AddTimeSignature:
        if (auto *sequence = dynamic_cast<TimeSignaturesSequence *>
            (this->project.getTimeline()->getTimeSignatures()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            Component *dialog = TimeSignatureDialog::createAddingDialog(*this, sequence, targetBeat);
            App::Layout().showModalComponentUnowned(dialog);
        }
        break;
    case CommandIDs::AddKeySignature:
        if (auto *sequence = dynamic_cast<KeySignaturesSequence *>
            (this->project.getTimeline()->getKeySignatures()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            Component *dialog = KeySignatureDialog::createAddingDialog(*this,
                this->getTransport(), sequence, targetBeat);
            App::Layout().showModalComponentUnowned(dialog);
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

    const float paintStartY = float(this->viewport.getViewPositionY());
    const float paintEndY = paintStartY + this->viewport.getViewHeight();

    g.setColour(this->barLineColour);
    for (const auto &f : this->visibleBars)
    {
        g.drawVerticalLine(int(floorf(f)), paintStartY, paintEndY);
    }

    g.setColour(this->barLineBevelColour);
    for (const auto &f : this->visibleBars)
    {
        g.drawVerticalLine(int(floorf(f)) + 1, paintStartY, paintEndY);
    }

    g.setColour(this->beatLineColour);
    for (const auto &f : this->visibleBeats)
    {
        g.drawVerticalLine(int(floorf(f)), paintStartY, paintEndY);
    }
    
    g.setColour(this->snapLineColour);
    for (const auto &f : this->visibleSnaps)
    {
        g.drawVerticalLine(int(floorf(f)), paintStartY, paintEndY);
    }
}

//===----------------------------------------------------------------------===//
// Playhead::Listener
//===----------------------------------------------------------------------===//

void HybridRoll::onPlayheadMoved(int playheadX)
{
    if (this->shouldFollowPlayhead &&
        !this->smoothZoomController->isZooming())
    {
        const int viewHalfWidth = this->viewport.getViewWidth() / 2;
        const int viewportCentreX = this->viewport.getViewPositionX() + viewHalfWidth;
        const double offset = double(playheadX) - double(viewportCentreX);
        // Smoothness should depend on zoom level (TODO it smarter):
        const double smoothCoefficient = (this->barWidth > 300) ? 0.915 : 0.975;
        const double smoothThreshold = (this->barWidth > 300) ? 128.0 : 5.0;
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
    if (! this->project.getTransport().isPlaying())
    {
        return;
    }
    
    const float clippingBeat = this->getBeatByTransportPosition(this->lastTransportPosition.get());
    
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
    
    auto newMarker = new TimelineWarningMarker(TimelineWarningMarker::Red, *this, clippingBeat);
    this->clippingIndicators.add(newMarker);
    this->addAndMakeVisible(newMarker);
}

void HybridRoll::resetAllClippingIndicators()
{
    this->clippingIndicators.clear();
}

void HybridRoll::onOversaturationWarning()
{
    if (! this->project.getTransport().isPlaying())
    {
        return;
    }
    
    const float warningBeat = this->getBeatByTransportPosition(this->lastTransportPosition.get());

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
    
    auto newMarker = new TimelineWarningMarker(TimelineWarningMarker::Yellow, *this, warningBeat);
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

void HybridRoll::onSeek(double absolutePosition, double, double)
{
    this->lastTransportPosition = absolutePosition;
}

void HybridRoll::onTempoChanged(double msPerQuarter) {}
void HybridRoll::onTotalTimeChanged(double timeMs) {}

void HybridRoll::onPlay()
{
    this->resetAllClippingIndicators();
    this->resetAllOversaturationIndicators();
}

void HybridRoll::onStop()
{
#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    // todo sync screen back to playhead?
    this->stopFollowingPlayhead();
#endif
}

bool HybridRoll::isFollowingPlayhead() const noexcept
{
    return this->shouldFollowPlayhead;
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
    if (! this->isFollowingPlayhead())
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
    const int playheadX = this->getXPositionByTransportPosition(this->lastTransportPosition.get(), float(this->getWidth()));
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

        this->batchRepaintList.clear();
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    if (this->shouldFollowPlayhead && !this->smoothZoomController->isZooming())
    {
        const int playheadX = this->getXPositionByTransportPosition(this->lastTransportPosition.get(), float(this->getWidth()));

        if (fabs(this->playheadOffset) > 1.0)
        {
            this->playheadOffset *= 0.75;
        }
        else
        {
            this->playheadOffset = 0.0;
        }

        this->viewport.setViewPosition(playheadX - int(this->playheadOffset) - (this->viewport.getViewWidth() / 2),
            this->viewport.getViewPositionY());

        this->updateChildrenPositions();
    }
#endif
}

double HybridRoll::findPlayheadOffsetFromViewCentre() const
{
    const int playheadX = this->getXPositionByTransportPosition(this->lastTransportPosition.get(), float(this->getWidth()));
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
    if (fabs(this->playheadOffset) < 0.1)
    {
        MessageManagerLock lock(Thread::getCurrentThread());
        if (lock.lockWasGained())
        {
            this->stopFollowingPlayhead();
        }
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

    this->zoomMarker = new IconComponent(Icons::zoomIn);
    this->zoomMarker->setAlwaysOnTop(true);

    const Point<int> vScreenPosition(this->viewport.getScreenPosition());
    const Point<int> sMouseDownPosition(this->clickAnchor.toInt());
    const Point<int> vMouseDownPosition(sMouseDownPosition - vScreenPosition);
    this->zoomMarker->setSize(24, 24);
    this->zoomMarker->setCentrePosition(vMouseDownPosition.getX(), vMouseDownPosition.getY());
    this->viewport.addAndMakeVisible(this->zoomMarker);

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
    const int newWidth = int(this->getNumBars() * this->barWidth);
    if (this->getWidth() != newWidth)
    {
        this->setSize(newWidth, this->getHeight());
    }
}

static const int shadowSize = 15;

void HybridRoll::updateChildrenBounds()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const int &viewHeight = this->viewport.getViewHeight();
    const int &viewWidth = this->viewport.getViewWidth();
    const int &viewX = this->viewport.getViewPositionX();
    const int &viewY = this->viewport.getViewPositionY();

    this->topShadow->setBounds(viewX, viewY + HYBRID_ROLL_HEADER_HEIGHT, viewWidth, shadowSize);
    this->bottomShadow->setBounds(viewX, viewY + viewHeight - shadowSize, viewWidth, shadowSize);
    this->header->setBounds(0, viewY, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    
    if (this->annotationsTrack)
    {
        this->annotationsTrack->setBounds(0, viewY + HYBRID_ROLL_HEADER_HEIGHT, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->keySignaturesTrack)
    {
        this->keySignaturesTrack->setBounds(0, viewY + HYBRID_ROLL_HEADER_HEIGHT, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->timeSignaturesTrack)
    {
        this->timeSignaturesTrack->setBounds(0, viewY, this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT - 1);
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

    this->topShadow->setTopLeftPosition(viewX, viewY + HYBRID_ROLL_HEADER_HEIGHT);
    this->bottomShadow->setTopLeftPosition(viewX, viewY + viewHeight - shadowSize);
    this->header->setTopLeftPosition(0, viewY);

    if (this->annotationsTrack)
    {
        this->annotationsTrack->setTopLeftPosition(0, viewY + HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->keySignaturesTrack)
    {
        this->keySignaturesTrack->setTopLeftPosition(0, viewY + HYBRID_ROLL_HEADER_HEIGHT);
    }

    if (this->timeSignaturesTrack)
    {
        this->timeSignaturesTrack->setTopLeftPosition(0, viewY);
    }

    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        Component *const trackMap = this->trackMaps.getUnchecked(i);
        trackMap->setTopLeftPosition(0, viewY + viewHeight - trackMap->getHeight());
    }

    this->broadcastRollMoved();

    HYBRID_ROLL_BULK_REPAINT_END
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
