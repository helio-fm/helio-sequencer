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
#include "RollBase.h"
#include "RollHeader.h"

#include "RollExpandMark.h"
#include "MidiEvent.h"
#include "MidiEventComponent.h"
#include "SelectionComponent.h"
#include "AnnotationLargeComponent.h"

#include "UndoStack.h"
#include "UndoActionIDs.h"
#include "NoteActions.h"

#include "ShadowDownwards.h"
#include "TimelineWarningMarker.h"

#include "LongTapController.h"
#include "SmoothPanController.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"

#include "Transport.h"
#include "IconComponent.h"
#include "PlayerThread.h"

#include "ProjectMetadata.h"
#include "ProjectTimeline.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesSequence.h"
#include "SequencerOperations.h"
#include "RollListener.h"
#include "VersionControlNode.h"
#include "MidiTrackNode.h"
#include "Pattern.h"

#include "AnnotationDialog.h"
#include "TimeSignatureDialog.h"
#include "KeySignatureDialog.h"
#include "TempoDialog.h"

#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "AudioMonitor.h"
#include "Config.h"
#include "ColourIDs.h"

#if PLATFORM_DESKTOP
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 1
#else
#   define ROLL_VIEW_FOLLOWS_PLAYHEAD 0
#endif

RollBase::RollBase(ProjectNode &parentProject, Viewport &viewportRef,
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
    this->setFocusContainerType(Component::FocusContainerType::none);

    this->temperament = this->project.getProjectInfo()->getTemperament();

    this->header = make<RollHeader>(this->project.getTransport(), *this, this->viewport);
    this->headerShadow = make<ShadowDownwards>(ShadowType::Normal);

    if (hasAnnotationsTrack)
    {
        this->annotationsMap = make<AnnotationsProjectMap>(this->project, *this, AnnotationsProjectMap::Type::Large);
    }

    if (hasTimeSignaturesTrack)
    {
        this->timeSignaturesMap = make<TimeSignaturesProjectMap>(this->project, *this, TimeSignaturesProjectMap::Type::Large);
    }

    if (hasKeySignaturesTrack)
    {
        this->keySignaturesMap = make<KeySignaturesProjectMap>(this->project, *this, KeySignaturesProjectMap::Type::Large);
    }

    this->playhead = make<Playhead>(*this, this->project.getTransport(), this);

    this->lassoComponent = make<SelectionComponent>();
    this->lassoComponent->setWantsKeyboardFocus(false);
    this->lassoComponent->setFocusContainerType(Component::FocusContainerType::none);

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

#if ROLL_LISTENS_LONG_TAP
    this->longTapController = make<LongTapController>(*this);
    this->addMouseListener(this->longTapController.get(), true); // true = listens child events as well
#endif

    this->multiTouchController = make<MultiTouchController>(*this);
    this->addMouseListener(this->multiTouchController.get(), false); // false = listens only this one

    this->smoothPanController = make<SmoothPanController>(*this);
    this->smoothZoomController = make<SmoothZoomController>(*this);

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->project.addListener(this);
    this->project.getEditMode().addChangeListener(this);
    this->project.getTransport().addTransportListener(this);
    this->project.getTimeline()->getTimeSignaturesAggregator()->addListener(this);

    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->addClippingListener(this);
    }

    auto *uiFlags = App::Config().getUiFlags();
    this->onUiAnimationsFlagChanged(uiFlags->areUiAnimationsEnabled());
    this->onMouseWheelFlagsChanged(uiFlags->getMouseWheelFlags());
    uiFlags->addListener(this);
}

RollBase::~RollBase()
{
    App::Config().getUiFlags()->removeListener(this);

    if (this->clippingDetector != nullptr)
    {
        this->clippingDetector->removeClippingListener(this);
    }

    this->removeAllRollListeners();

    this->project.getTimeline()->getTimeSignaturesAggregator()->removeListener(this);
    this->project.getTransport().removeTransportListener(this);
    this->project.getEditMode().removeChangeListener(this);
    this->project.removeListener(this);

    this->removeMouseListener(this->multiTouchController.get());

#if ROLL_LISTENS_LONG_TAP
    this->removeMouseListener(this->longTapController.get());
#endif
}

Viewport &RollBase::getViewport() const noexcept
{
    return this->viewport;
}

Transport &RollBase::getTransport() const noexcept
{
    return this->project.getTransport();
}

ProjectNode &RollBase::getProject() const noexcept
{
    return this->project;
}

//===----------------------------------------------------------------------===//
// Timeline events
//===----------------------------------------------------------------------===//

float RollBase::getPositionForNewTimelineEvent() const
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

void RollBase::insertAnnotationWithinScreen(const String &annotation)
{
    if (auto *annotationsLayer = dynamic_cast<AnnotationsSequence *>(this->project.getTimeline()->getAnnotations()))
    {
        annotationsLayer->checkpoint();
        const float targetBeat = this->getPositionForNewTimelineEvent();
        AnnotationEvent event(annotationsLayer, targetBeat, annotation, Colours::transparentWhite);
        annotationsLayer->insert(event, true);
    }
}

void RollBase::insertTimeSignatureWithinScreen(int numerator, int denominator)
{
    jassert(denominator == 2 || denominator == 4 ||
        denominator == 8 || denominator == 16 || denominator == 32);

    if (auto *tsSequence = dynamic_cast<TimeSignaturesSequence *>(
        this->project.getTimeline()->getTimeSignatures()->getSequence()))
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

void RollBase::addOwnedMap(Component *newTrackMap)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);
    newTrackMap->toFront(false);
    this->playhead->toFront(false);
    this->resized();
}

void RollBase::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);
        this->resized();
    }
}

//===----------------------------------------------------------------------===//
// Modes
//===----------------------------------------------------------------------===//

RollEditMode &RollBase::getEditMode() noexcept
{
    return this->project.getEditMode();
}

bool RollBase::isInSelectionMode() const
{
    return (this->project.getEditMode().isMode(RollEditMode::selectionMode));
}

bool RollBase::isInDragMode() const
{
    return (this->project.getEditMode().isMode(RollEditMode::dragMode));
}

//===----------------------------------------------------------------------===//
// Temperament info
//===----------------------------------------------------------------------===//

int RollBase::getNumKeys() const noexcept
{
    return this->temperament->getNumKeys();
}

int RollBase::getPeriodSize() const noexcept
{
    return this->temperament->getPeriodSize();
}

Note::Key RollBase::getMiddleC() const noexcept
{
    return this->temperament->getMiddleC();
}

Temperament::Ptr RollBase::getTemperament() const noexcept
{
    return this->temperament;
}

//===----------------------------------------------------------------------===//
// RollBase listeners management
//===----------------------------------------------------------------------===//

void RollBase::addRollListener(RollListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.add(listener);
}

void RollBase::removeRollListener(RollListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.remove(listener);
}

void RollBase::removeAllRollListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.clear();
}

void RollBase::broadcastRollMoved()
{
    this->listeners.call(&RollListener::onMidiRollMoved, this);
}

void RollBase::broadcastRollResized()
{
    this->listeners.call(&RollListener::onMidiRollResized, this);
}

//===----------------------------------------------------------------------===//
// Input Listeners
//===----------------------------------------------------------------------===//

void RollBase::longTapEvent(const Point<float> &position,
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

    if (target == this)
    {
        if (!this->project.getEditMode().forbidsSelectionMode({}))
        {
            this->lassoComponent->beginLasso(position, this);
            return;
        }
    }
}

void RollBase::multiTouchZoomEvent(const Point<float> &origin, const Point<float> &zoom)
{
    this->smoothPanController->cancelPan();
    this->smoothZoomController->zoomRelative(origin, zoom);
}

void RollBase::multiTouchPanEvent(const Point<float> &offset)
{
    //this->smoothZoomController->cancelZoom();
    const auto absOffset = this->viewport.getViewPosition() + offset.toInt();
    this->panByOffset(absOffset.x, absOffset.y);
}

void RollBase::multiTouchCancelZoom()
{
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    this->smoothZoomController->cancelZoom();
}

void RollBase::multiTouchCancelPan()
{
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
    this->smoothPanController->cancelPan();
}

Point<float> RollBase::getMultiTouchOrigin(const Point<float> &from)
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

bool RollBase::panByOffset(int offsetX, int offsetY)
{
    this->stopFollowingPlayhead();

    const bool stretchRight = (offsetX >= (this->getWidth() - this->viewport.getViewWidth()));
    const bool stretchLeft = (offsetX <= 0);

    if (stretchRight)
    {
        const float numBeatsToExpand = getNumBeatsToExpand(this->beatWidth);
        this->project.broadcastChangeViewBeatRange(this->firstBeat, this->lastBeat + numBeatsToExpand);
        this->viewport.setViewPosition(offsetX, offsetY); // after setLastBeat
        const float beatCloseToTheRight = this->lastBeat - numBeatsToExpand;
        this->header->addAndMakeVisible(new RollExpandMark(*this, beatCloseToTheRight, numBeatsToExpand));
    }
    else if (stretchLeft)
    {
        const float numBeatsToExpand = getNumBeatsToExpand(this->beatWidth);
        const float deltaW = float(this->beatWidth * numBeatsToExpand);
        this->clickAnchor.addXY(deltaW, 0); // an ugly hack
        this->project.broadcastChangeViewBeatRange(this->firstBeat - numBeatsToExpand, this->lastBeat);
        this->viewport.setViewPosition(offsetX + int(deltaW), offsetY); // after setFirstBeat
        this->header->addAndMakeVisible(new RollExpandMark(*this, this->firstBeat, numBeatsToExpand));
    }
    else
    {
        this->viewport.setViewPosition(offsetX, offsetY);
    }

    this->updateChildrenPositions();

    return stretchRight || stretchLeft;
}

void RollBase::panProportionally(float absX, float absY)
{
    this->stopFollowingPlayhead();
    this->viewport.setViewPositionProportionately(absX, absY);
    this->updateChildrenPositions();
}

Point<int> RollBase::getPanOffset() const
{
    return this->viewport.getViewPosition();
}

//===----------------------------------------------------------------------===//
// SmoothZoomListener
//===----------------------------------------------------------------------===//

void RollBase::startSmoothZoom(const Point<float> &origin, const Point<float> &factor)
{
    this->smoothZoomController->zoomRelative(origin, factor);
}

void RollBase::zoomInImpulse(float factor)
{
    const auto origin = this->viewport.getLocalBounds().getCentre();
    const Point<float> f(0.15f * factor, 0.15f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void RollBase::zoomOutImpulse(float factor)
{
    const auto origin = this->viewport.getLocalBounds().getCentre();
    const Point<float> f(-0.15f * factor, -0.15f * factor);
    this->startSmoothZoom(origin.toFloat(), f);
}

void RollBase::zoomToArea(float minBeat, float maxBeat, float marginBeats)
{
    jassert(maxBeat > minBeat);
    jassert(minBeat >= this->getFirstBeat());
    jassert(maxBeat <= this->getLastBeat());

    this->stopFollowingPlayhead();

    const float widthToFit = float(this->viewport.getViewWidth());
    const float numBeatsToFit = maxBeat - minBeat + (marginBeats * 2.f);
    this->setBeatWidth(widthToFit / numBeatsToFit);

    const int minBeatX = this->getXPositionByBeat(minBeat - marginBeats);
    this->viewport.setViewPosition(minBeatX, this->viewport.getViewPositionY());

    this->updateChildrenPositions();
}

void RollBase::zoomRelative(const Point<float> &origin,
    const Point<float> &factor, bool isInertialZoom)
{
    this->stopFollowingPlayhead();

    const auto oldViewPosition = this->viewport.getViewPosition().toFloat();
    const auto absoluteOrigin = oldViewPosition + origin;
    const float oldWidth = float(this->getWidth());

    float newBeatWidth = this->beatWidth + (factor.getX() * this->beatWidth);
    const float estimatedNewWidth = newBeatWidth * this->getNumBeats();

    const auto hitsMinZoomThreshold = estimatedNewWidth < float(this->viewport.getViewWidth());
    const auto shouldAutoFitViewRange = hitsMinZoomThreshold && !isInertialZoom;
    if (hitsMinZoomThreshold)
    {
        newBeatWidth = float(this->viewport.getWidth() + 1) / this->getNumBeats();
    }

    this->setBeatWidth(newBeatWidth); // will updateBounds() -> setSize() -> resized() -> updateChildrenBounds()

    const float newWidth = float(this->getWidth());
    const float newViewPositionX = (absoluteOrigin.getX() * newWidth / oldWidth) - origin.getX();
    this->viewport.setViewPosition(int(newViewPositionX + 0.5f), int(oldViewPosition.getY()));

    this->resetDraggingAnchors();
    this->updateChildrenPositions();

    if (shouldAutoFitViewRange)
    {
        constexpr auto margin = Globals::beatsPerBar * 2;
        const auto projectRange = this->project.getProjectBeatRange();
        const auto newFirstBeat = projectRange.getStart() - margin;
        const auto newLastBeat = projectRange.getEnd() + margin;
        this->project.broadcastChangeViewBeatRange(newFirstBeat, newLastBeat);

        const auto markWidthInBeats = 4.f / this->beatWidth;
        this->viewport.addAndMakeVisible(new RollExpandMark(*this, newFirstBeat, markWidthInBeats, false));
        this->viewport.addAndMakeVisible(new RollExpandMark(*this, newLastBeat - markWidthInBeats, markWidthInBeats, false));
    }
}

void RollBase::zoomAbsolute(const Rectangle<float> &proportion)
{
    jassert(!proportion.isEmpty());
    jassert(proportion.isFinite());

    const float widthToFit = float(this->viewport.getViewWidth());
    const auto rollLengthInBeats = this->getLastBeat() - this->getFirstBeat();
    const float numBeatsToFit = jmax(1.f, rollLengthInBeats * proportion.getWidth());
    this->setBeatWidth(widthToFit / numBeatsToFit);

    const auto minBeat = this->getFirstBeat() + rollLengthInBeats * proportion.getX();
    const int minBeatX = this->getXPositionByBeat(minBeat);
    this->viewport.setViewPosition(minBeatX, this->viewport.getViewPositionY());

    this->updateChildrenPositions();
}

float RollBase::getZoomFactorX() const noexcept
{
    const float viewWidth = float(this->viewport.getViewWidth());
    const float beatsOnScreen = (viewWidth / this->beatWidth);
    return beatsOnScreen / this->getNumBeats();
}

float RollBase::getZoomFactorY() const noexcept
{
    return 1.f;
}

//===----------------------------------------------------------------------===//
// TimeSignaturesAggregator::Listener
//===----------------------------------------------------------------------===//

void RollBase::onTimeSignaturesUpdated()
{
    this->repaint();
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int RollBase::getPlayheadPositionByBeat(double targetBeat, double parentWidth) const
{
    const double widthRatio = parentWidth / double(this->getWidth());
    return int((targetBeat - this->firstBeat) * this->beatWidth * widthRatio);
}

int RollBase::getXPositionByBeat(float targetBeat) const noexcept
{
    return int((targetBeat - this->firstBeat) * this->beatWidth);
}

float RollBase::getFloorBeatSnapByXPosition(int x) const noexcept
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

float RollBase::getRoundBeatSnapByXPosition(int x) const
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

void RollBase::setBeatRange(float first, float last)
{
    if (this->lastBeat == last && this->firstBeat == first)
    {
        return;
    }

    this->lastBeat = last;
    this->firstBeat = first;
    this->updateBounds();
}

void RollBase::setBeatWidth(float newBeatWidth)
{
    if (this->beatWidth == newBeatWidth)
    {
        return;
    }

    this->beatWidth = jlimit(0.1f, 360.f, newBeatWidth);
    this->updateBounds();
}

float RollBase::getMinVisibleBeatForCurrentZoomLevel() const
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

void RollBase::computeAllSnapLines()
{
    static constexpr auto minBarWidth = 14;
    static constexpr auto minBeatWidth = 8;

    constexpr auto beatsPerBar = float(Globals::beatsPerBar);

    this->visibleBars.clearQuick();
    this->visibleBeats.clearQuick();
    this->visibleSnaps.clearQuick();
    this->allSnaps.clearQuick();

    auto *timeSignatureAggregator = this->project.getTimeline()->getTimeSignaturesAggregator();
    const auto *orderedTimeSignatures = timeSignatureAggregator->getSequence();

    const float paintStartX = float(this->viewport.getViewPositionX());
    const float paintEndX = float(paintStartX + this->viewport.getViewWidth());

    const float defaultBarWidth = float(this->beatWidth * beatsPerBar);
    const float firstBar = this->firstBeat / beatsPerBar;
    const float paintStartBar = floorf(paintStartX / defaultBarWidth + firstBar);
    const float paintEndBar = ceilf(paintEndX / defaultBarWidth + firstBar);

    // get the number of snaps to display for default bar width,
    // e.g. 2 for 64, 4 for 128, 8 for 256, etc:
    const float nearestPowTwo = ceilf(log(defaultBarWidth) / log(2.f));
    const float numSnaps = powf(2, jlimit(1.f, 6.f, nearestPowTwo - 5.f)); // use -4.f for twice as dense grid
    const float snapWidth = defaultBarWidth / numSnaps;

    // in the absence of time signatures we still need defaults for the grid:
    int numerator = Globals::Defaults::timeSignatureNumerator;
    int denominator = Globals::Defaults::timeSignatureDenominator;
    // in the absence of time signatures we still need defaults for the grid:
    float defaultMeterStartBeat = this->projectFirstBeat;
    timeSignatureAggregator->updateGridDefaultsIfNeeded(numerator, denominator, defaultMeterStartBeat);

    int nextTsIndex = 0;
    // try to find the nearest time signature beyond the left side of visible area,
    // or just pick the very first time signature, if available
    for (; nextTsIndex < orderedTimeSignatures->size(); ++nextTsIndex)
    {
        const auto *signature = static_cast<const TimeSignatureEvent *>
            (orderedTimeSignatures->getUnchecked(nextTsIndex));

        numerator = signature->getNumerator();
        denominator = signature->getDenominator();
        defaultMeterStartBeat = signature->getBeat();

        if ((signature->getBeat() / beatsPerBar) >= paintStartBar)
        {
            break;
        }
    }

    const float defaultMeterStartBar = defaultMeterStartBeat / beatsPerBar;
    const float defaultMeterBarLength = float(numerator) / float(denominator);

    // iterate backwards from the anchor, if needed, using the single default meter
    float barWidthSum = 0.f;
    bool canDrawBarLine = false;
    auto barIterator = defaultMeterStartBar;

    while (barIterator >= (paintStartBar - defaultMeterBarLength))
    {
        const float beatStep = 1.f / float(denominator);
        const float barStep = beatStep * float(numerator);
        const float barStartX = defaultBarWidth * (barIterator - firstBar);
        const float barWidth = defaultBarWidth * barStep;

        // when in the drawing area:
        if (barIterator <= (paintEndBar + barStep))
        {
            if (canDrawBarLine)
            {
                this->visibleBars.add(barStartX);
                this->allSnaps.add(barStartX);
            }

            // the beat lines
            for (float j = 0.f; j < barStep; j += beatStep)
            {
                const float beatStartX = barStartX + defaultBarWidth * j;
                float nextBeatStartX = barStartX + defaultBarWidth * (j + beatStep);

                // snap lines and beat lines
                for (float k = beatStartX + snapWidth; k < (nextBeatStartX - 1); k += snapWidth)
                {
                    this->visibleSnaps.add(k);
                    this->allSnaps.add(k);
                }

                if (j >= beatStep && // don't draw the first one as it is a bar line
                    (nextBeatStartX - beatStartX) > minBeatWidth)
                {
                    this->visibleBeats.add(beatStartX);
                    this->allSnaps.add(beatStartX);
                }
            }
        }

        barIterator -= barStep;

        barWidthSum += barWidth;
        canDrawBarLine = barWidthSum > minBarWidth;
        barWidthSum = canDrawBarLine ? 0.f : barWidthSum;
    }

    // now iterate forwards from the anchor,
    // picking all the following time signatures

    barWidthSum = minBarWidth;
    barIterator = defaultMeterStartBar;

    while (barIterator <= paintEndBar)
    {
        // don't do anything here until we reach the left side of visible area

        const float beatStep = 1.f / float(denominator);
        float barStep = beatStep * float(numerator);
        const float barStartX = defaultBarWidth * (barIterator - firstBar);
        const float barWidth = defaultBarWidth * barStep;

        barWidthSum += barWidth;
        canDrawBarLine = barWidthSum > minBarWidth;
        barWidthSum = canDrawBarLine ? 0.f : barWidthSum;

        // when in the drawing area:
        if (barIterator >= (paintStartBar - barStep))
        {
            if (canDrawBarLine)
            {
                this->visibleBars.add(barStartX);
                this->allSnaps.add(barStartX);
            }

            // the beat lines
            bool lastFrame = false;
            for (float j = 0.f; j < barStep && !lastFrame; j += beatStep)
            {
                const float beatStartX = barStartX + defaultBarWidth * j;
                float nextBeatStartX = barStartX + defaultBarWidth * (j + beatStep);

                // check if we have more time signatures to come
                if (nextTsIndex < orderedTimeSignatures->size())
                {
                    const auto *nextSignature = static_cast<const TimeSignatureEvent *>(orderedTimeSignatures->getUnchecked(nextTsIndex));
                    const float tsBar = nextSignature->getBeat() / beatsPerBar;
                    if (tsBar <= (barIterator + j + beatStep))
                    {
                        numerator = nextSignature->getNumerator();
                        denominator = nextSignature->getDenominator();
                        barStep = (tsBar - barIterator); // i.e. incomplete bar
                        nextBeatStartX = barStartX + defaultBarWidth * barStep;
                        nextTsIndex++;
                        barWidthSum = minBarWidth; // forces to draw the next bar line
                        lastFrame = true;
                    }
                }

                // snap lines and beat lines
                for (float k = beatStartX + snapWidth; k < (nextBeatStartX - 1); k += snapWidth)
                {
                    this->visibleSnaps.add(k);
                    this->allSnaps.add(k);
                }

                if (j >= beatStep && // don't draw the first one as it is a bar line
                    (nextBeatStartX - beatStartX) > minBeatWidth)
                {
                    this->visibleBeats.add(beatStartX);
                    this->allSnaps.add(beatStartX);
                }
            }
        }

        barIterator += barStep;
    }

    // adding the project start beat to snaps helps a lot
    // on higher zoom levels when the project starts off-beat
    const auto projectStartBeatX = (this->projectFirstBeat - this->firstBeat) * this->beatWidth;
    if (projectStartBeatX > paintStartX && projectStartBeatX < paintEndX)
    {
        this->allSnaps.add(projectStartBeatX);
    }
}

//===----------------------------------------------------------------------===//
// Alternative key-down modes (space for drag, etc.)
//===----------------------------------------------------------------------===//

void RollBase::setSpaceDraggingMode(bool dragMode)
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
        this->project.getEditMode().setMode(RollEditMode::dragMode, true);
    }
    else
    {
        this->project.getEditMode().unsetLastMode();
    }
}

bool RollBase::isUsingSpaceDraggingMode() const
{
    return this->spaceDragMode;
}

//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

Lasso &RollBase::getLassoSelection()
{
    return this->selection;
}

void RollBase::selectEvent(SelectableComponent *event, bool shouldClearAllOthers)
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

void RollBase::deselectEvent(SelectableComponent *event)
{
    this->selection.deselect(event);
}

void RollBase::deselectAll()
{
    this->selection.deselectAll();
}

SelectionComponent *RollBase::getSelectionComponent() const noexcept
{
    return this->lassoComponent.get();
}

RollHeader *RollBase::getHeaderComponent() const noexcept
{
    return this->header.get();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void RollBase::onChangeMidiEvent(const MidiEvent &event, const MidiEvent &newEvent)
{
    // Time signatures have changed, need to repaint
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }

    if (this->isEnabled())
    {
        this->selection.onSelectableItemChanged(); // sends fake "selection changed" message
    }
}

void RollBase::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void RollBase::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        this->updateChildrenBounds();
        this->repaint();
    }
}

void RollBase::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->isEnabled())
    {
        this->selection.onSelectableItemChanged(); // sends fake "selection changed" message
    }
}

void RollBase::onChangeProjectBeatRange(float newFirstBeat, float newLastBeat)
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

void RollBase::onChangeViewBeatRange(float newFirstBeat, float newLastBeat)
{
    jassert(newFirstBeat < newLastBeat);
    const auto viewPos = this->viewport.getViewPosition();
    const auto viewStartBeat = this->getBeatByXPosition(float(viewPos.x));

    this->setBeatRange(newFirstBeat, newLastBeat);

    // If the beat range goes down (e.g. after midi import):
    const auto viewWidth = float(this->viewport.getWidth());
    const auto minBeatWidth = viewWidth / (newLastBeat - newFirstBeat);
    if (this->beatWidth < minBeatWidth)
    {
        this->setBeatWidth(minBeatWidth);
    }

    if (this->isVisible())
    {
        this->repaint(); // just in case setBeatRange/setBeatWidth didn't do it
    }
    else
    {
        // It's often the case that I expand visible range in a pattern editor,
        // then switch back to piano roll and find the viewport focus messed up;
        // let's try to detect that and preserve offset, when the roll is inactive:
        const auto newViewX = this->getXPositionByBeat(viewStartBeat);
        this->viewport.setViewPosition(newViewX, viewPos.y);
    }
}

void RollBase::onChangeProjectInfo(const ProjectMetadata *info)
{
    if (this->temperament != info->getTemperament())
    {
        this->temperament = info->getTemperament();
    }
}

void RollBase::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->firstBeat = 0.f;
    this->lastBeat = Globals::Defaults::projectLength;
    this->temperament = meta->getTemperament();
}

void RollBase::onBeforeReloadProjectContent()
{
    this->selection.deselectAll();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void RollBase::mouseDown(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        this->contextMenuController->cancelIfPending();
        this->lassoComponent->endLasso();
        return;
    }

    if (e.mods.isRightButtonDown())
    {
        if (this->getEditMode().isMode(RollEditMode::drawMode))
        {
            this->getEditMode().setMode(RollEditMode::eraseMode);
        }
        else if (this->getEditMode().isMode(RollEditMode::knifeMode))
        {
            this->getEditMode().setMode(RollEditMode::mergeMode);
        }
        else
        {
            this->contextMenuController->showMenu(e, 350);
        }
    }

    if (this->isErasingEvent(e))
    {
        this->startErasingEvents(e.position);
    }
    else if (this->isMergeToolEvent(e))
    {
        this->startMergingEvents(e.position);
    }
    else if (this->isLassoEvent(e))
    {
        this->lassoComponent->beginLasso(e.position, this);
    }
    else if (this->isViewportDragEvent(e))
    {
        this->resetDraggingAnchors();
    }
    else if (this->isViewportZoomEvent(e))
    {
        this->startZooming();
    }
}

void RollBase::mouseDrag(const MouseEvent &e)
{
    this->contextMenuController->cancelIfPending();

    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->isErasingEvent(e))
    {
        this->continueErasingEvents(e.position);
    }
    else if (this->isMergeToolEvent(e))
    {
        this->continueMergingEvents(e.position);
    }
    else if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->dragLasso(e); // if any. will do the check itself
    }
    else if (this->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->continueDragging(e);
    }
    else if (this->isViewportZoomEvent(e))
    {
        this->continueZooming(e);
    }
}

void RollBase::mouseUp(const MouseEvent &e)
{
    // I quite disliked it:
    //if (this->contextMenuController->isPending() &&
    //    e.getOffsetFromDragStart().isOrigin())
    //{
    //    this->contextMenuController->showMenu(e);
    //    return;
    //}
    // so instead:
    this->contextMenuController->cancelIfPending();

    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->isErasingEvent(e))
    {
        this->endErasingEvents();
        // the only way we can switch to erasing mode is by holding the rmb
        // in the draw mode, and on mouse up we're switching back:
        jassert(this->project.getEditMode().isMode(RollEditMode::eraseMode));
        this->project.getEditMode().setMode(RollEditMode::drawMode);
    }

    if (this->isMergeToolEvent(e))
    {
        this->endMergingEvents();
        // the only way we can switch to merging mode is by holding the rmb
        // in the knife mode, and on mouse up we're switching back:
        jassert(this->project.getEditMode().isMode(RollEditMode::mergeMode));
        this->project.getEditMode().setMode(RollEditMode::knifeMode);
    }

    if (this->isViewportZoomEvent(e))
    {
        this->endZooming();
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->endLasso();
    }

#if PLATFORM_DESKTOP
    constexpr auto minPanDistance = 10;
#elif PLATFORM_MOBILE
    constexpr auto minPanDistance = 20;
#endif

    if (e.mods.isLeftButtonDown() &&
        (e.getDistanceFromDragStart() < minPanDistance) &&
        !e.mods.isAltDown())
    {
        this->deselectAll();
    }

    this->setMouseCursor(this->project.getEditMode().getCursor());
}

void RollBase::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    //  This is a rewritten section of code for managing zoom / pan behavior. As it rests
    //  currently, there are two main ways of viewing this user interaction system:
    //
    //      1. For those that would prefer a naked scroll to zoom by default.
    //          - scrolling zooms according to your selected default axis
    //          - if alt is down, the zoom axis changes to the opposite of the default
    //          - if ctrl is down, the viewport will pan horizontally
    //          - if shift if down, the viewport will pan vertically
    //
    //      2. For those that would prefer a naked scroll to pan by default.
    //          - scrolling pans according to your selected default axis
    //          - if alt is down, the pan axis changes to the opposite of the default
    //          - if ctrl is down, the viewport will zoom horizontally
    //          - if shift if down, the viewport will zoom vertically
    //
    //  This is an implementation of that system. More lines than neccecary, but easy
    //  to follow along with. This code is expressed very clearly so as to make potential
    //  changes easier to implement before slimming down.

    bool panningModeDefault = this->mouseWheelFlags.usePanningByDefault;
    bool zoomingModeDefualt = !panningModeDefault;

    bool verticalPanByDefault = this->mouseWheelFlags.useVerticalPanningByDefault;
    bool horizontalPanByDefault = !verticalPanByDefault;

    bool verticalZoomByDefault = this->mouseWheelFlags.useVerticalZoomingByDefault;
    bool horizontalZoomByDefault = !verticalZoomByDefault;

    bool pan = false;
    bool zoom = false;

    bool panVertical = false;
    bool panHorizontal = false;

    bool zoomVertical = false;
    bool zoomHorizontal = false;

    if (panningModeDefault) //if we are in the "pan by default" paradigm
    {
        pan = true;

        if (event.mods.isCtrlDown() || event.mods.isCommandDown() || event.mods.isShiftDown())
        {
            pan = false;
            zoom = true;
        }

        if (pan)
        {
            panVertical = verticalPanByDefault;
            panHorizontal = !panVertical;

            if (event.mods.isAltDown()) //alt reverses everything
            {
                panVertical = !panVertical;
                panHorizontal = !panHorizontal;
            }
        }
        if (zoom)
        {
            zoomVertical = verticalZoomByDefault;
            zoomHorizontal = !zoomVertical;

            if (event.mods.isAltDown()) //alt reverses everything
            {
                zoomVertical = !zoomVertical;
                zoomHorizontal = !zoomHorizontal;
            }

            if (event.mods.isCtrlDown() || event.mods.isCommandDown())
            {
                zoomHorizontal = true;
                zoomVertical = false;
            }
            if (event.mods.isShiftDown())
            {
                zoomHorizontal = false;
                zoomVertical = true;
            }
            if (event.mods.isShiftDown() && (event.mods.isCtrlDown() || event.mods.isCommandDown()))
            {
                zoomHorizontal = true;
                zoomVertical = true;
            }
        }
    }
    if (zoomingModeDefualt) //if we are in the "zoom by default" paradigm
    {
        zoom = true;

        if (event.mods.isCtrlDown() || event.mods.isCommandDown() || event.mods.isShiftDown())
        {
            pan = true;
            zoom = false;
        }

        if (pan)
        {
            panVertical = verticalPanByDefault;
            panHorizontal = !panVertical;

            if (event.mods.isAltDown()) //alt reverses everything
            {
                panVertical = !panVertical;
                panHorizontal = !panHorizontal;
            }

            if (event.mods.isCtrlDown() || event.mods.isCommandDown())
            {
                panHorizontal = true;
                panVertical = false;
            }
            if (event.mods.isShiftDown())
            {
                panHorizontal = false;
                panVertical = true;
            }
        }
        if (zoom)
        {
            zoomVertical = verticalZoomByDefault;
            zoomHorizontal = !zoomVertical;

            if (event.mods.isAltDown()) //alt reverses everything
            {
                zoomVertical = !zoomVertical;
                zoomHorizontal = !zoomHorizontal;
            }
        }
    }

    if (pan)
    {
        const auto initialSpeed = this->smoothPanController->getInitialSpeed();

        // let's try to make the panning speed feel consistent, regardless
        // of the zoom level - slower when zoomed out, faster when zoomed in;
        const auto viewHeight = float(this->viewport.getViewHeight());
        const float panSpeedVertical = jmin(initialSpeed * 2.5f,
            initialSpeed * float(this->getHeight()) / viewHeight);
        // but also not too fast, with the half screen width as the upper speed limit:
        const auto viewWidth = float(this->viewport.getViewWidth());
        const float panSpeedHorizontal = jmin(viewWidth / 2.f,
            initialSpeed * float(this->getWidth()) / viewWidth);

        // the pattern roll almost always doesn't have that much rows to be able
        // to pan vertically, so for convenience we'll fallback to horizontal panning:
        const bool canPanVertically = this->getHeight() > this->viewport.getViewHeight();

        if (panVertical)
        {
            const float panDeltaY = wheel.deltaY * panSpeedVertical;
            const float panDeltaX = wheel.deltaX * panSpeedHorizontal;
            this->smoothPanController->panByOffset({ -panDeltaX, -panDeltaY });
        }

        if (panHorizontal)
        {
            const float panDeltaY = wheel.deltaY * panSpeedHorizontal;
            const float panDeltaX = wheel.deltaX * panSpeedVertical;
            this->smoothPanController->panByOffset({ -panDeltaY, -panDeltaX });
        }
    }
    if (zoom)
    {
        this->smoothPanController->cancelPan();

        //determines whether vertical zooming is enabled without needing to hold any modifier keys
        bool verticalZooming = this->mouseWheelFlags.useVerticalZoomingByDefault;
        bool horizontalZooming = !(this->mouseWheelFlags.useVerticalZoomingByDefault);

        const float zoomSpeed = this->smoothZoomController->getInitialSpeed();
        const float zoomDeltaY = wheel.deltaY * (wheel.isReversed ? -zoomSpeed : zoomSpeed);
        const float zoomDeltaX = wheel.deltaX * (wheel.isReversed ? -zoomSpeed : zoomSpeed);
        const auto mouseOffset = event.position - this->viewport.getViewPosition().toFloat();

        if (zoomVertical && zoomHorizontal) //zoom globally if both are enabled
        {
            this->startSmoothZoom(mouseOffset, { zoomDeltaY, zoomDeltaY });
        }
        else if (zoomVertical)
        {
            this->startSmoothZoom(mouseOffset, { zoomDeltaX, zoomDeltaY });
        }
        else if (zoomHorizontal)
        {
            this->startSmoothZoom(mouseOffset, { zoomDeltaY, zoomDeltaX });
        }
    }
}

void RollBase::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::EditModeDefault:
        this->project.getEditMode().setMode(RollEditMode::defaultMode);
        break;
    case CommandIDs::EditModeDraw:
        this->project.getEditMode().setMode(RollEditMode::drawMode);
        break;
    case CommandIDs::EditModePan:
        this->project.getEditMode().setMode(RollEditMode::dragMode);
        break;
    case CommandIDs::EditModeSelect:
        this->project.getEditMode().setMode(RollEditMode::selectionMode);
        break;
    case CommandIDs::EditModeKnife:
        this->project.getEditMode().setMode(RollEditMode::knifeMode);
        break;
    case CommandIDs::Undo:
        if (this->project.getUndoStack()->canUndo())
        {
            // when new notes are added, they will be automatically added to selection,
            // so if we're about to undo note removal, or redo note addition (notes are added in both cases),
            // we need to drop the selection first:
            const bool shouldDropSelection =
                this->project.getUndoStack()->undoHas<NoteRemoveAction>() ||
                this->project.getUndoStack()->undoHas<NotesGroupRemoveAction>();

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
        if (this->getTransport().isPlaying())
        {
            this->getTransport().stopPlaybackAndRecording();
            this->getTransport().stopSound();
        }
        {
            this->stopFollowingPlayhead();
            const auto nextJumpSeek = this->findNextAnchorBeat(this->getTransport().getSeekBeat());
            const auto nextJumpSafe = jlimit(this->projectFirstBeat, this->projectLastBeat, nextJumpSeek);
            this->getTransport().seekToBeat(nextJumpSafe);
            this->scrollToPlayheadPositionIfNeeded();
        }
        break;
    case CommandIDs::TimelineJumpPrevious:
        if (this->getTransport().isPlaying())
        {
            this->getTransport().stopPlaybackAndRecording();
            this->getTransport().stopSound();
        }
        {
            this->stopFollowingPlayhead();
            const auto prevJumpSeek = this->findPreviousAnchorBeat(this->getTransport().getSeekBeat());
            const auto prevJumpSafe = jlimit(this->projectFirstBeat, this->projectLastBeat, prevJumpSeek);
            this->getTransport().seekToBeat(prevJumpSafe);
            this->scrollToPlayheadPositionIfNeeded();
        }
        break;
    case CommandIDs::TimelineJumpHome:
        if (this->getTransport().isPlaying())
        {
            this->getTransport().stopPlaybackAndRecording();
            this->getTransport().stopSound();
        }

        this->stopFollowingPlayhead();
        this->getTransport().seekToBeat(this->projectFirstBeat);
        this->scrollToPlayheadPositionIfNeeded();
        break;
    case CommandIDs::TimelineJumpEnd:
        if (this->getTransport().isPlaying())
        {
            this->getTransport().stopPlaybackAndRecording();
            this->getTransport().stopSound();
        }

        this->stopFollowingPlayhead();
        this->getTransport().seekToBeat(this->projectLastBeat);
        this->scrollToPlayheadPositionIfNeeded();
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
            const bool notTooMuchTimeSpent =
                (Time::getCurrentTime() - this->timeEnteredDragMode).inMilliseconds() < 300;
            if (noDraggingWasDone && notTooMuchTimeSpent)
            {
                this->getTransport().toggleStartStopPlayback();
            }
            this->setSpaceDraggingMode(false);
        }
        break;
    case CommandIDs::ToggleBottomMiniMap:
        App::Config().getUiFlags()->toggleFullProjectMapVisibility();
        break;
    case CommandIDs::ToggleMetronome:
        App::Config().getUiFlags()->toggleMetronome();
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
                this->getTransport().disablePlaybackLoop();
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
            App::showModalComponent(TimeSignatureDialog::addingDialog(*this, this->project, sequence, targetBeat));
        }
        break;
    case CommandIDs::AddKeySignature:
        if (auto *sequence = dynamic_cast<KeySignaturesSequence *>
            (this->project.getTimeline()->getKeySignatures()->getSequence()))
        {
            const float targetBeat = this->getPositionForNewTimelineEvent();
            App::showModalComponent(KeySignatureDialog::addingDialog(this->project,
                sequence, targetBeat));
        }
        break;
    case CommandIDs::ProjectSetOneTempo:
    {
        auto dialog = make<TempoDialog>(Globals::Defaults::tempoBpm);
        dialog->onOk = [this](int newBpmValue)
        {
            SequencerOperations::setOneTempoForProject(this->getProject(), newBpmValue);
        };

        App::showModalComponent(move(dialog));
        break;
    }
    default:
        break;
    }
}

void RollBase::resized()
{
    this->updateChildrenBounds();
}

void RollBase::paint(Graphics &g)
{
    this->computeAllSnapLines();

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

void RollBase::onPlayheadMoved(int playheadX)
{
    if (this->playheadFollowMode == PlayheadFollowMode::Always)
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

void RollBase::onClippingWarning()
{
    if (!this->getTransport().isPlaying())
    {
        return;
    }

    const float clippingBeat = this->lastPlayheadBeat.get();

    if (!this->clippingIndicators.isEmpty())
    {
        const float lastMarkerEndBeat = this->clippingIndicators.getLast()->getEndBeat();
        const bool justNeedToUpdateLastMarker =
            (clippingBeat - lastMarkerEndBeat) < TimelineWarningMarker::minGapInBeats;

        if (justNeedToUpdateLastMarker)
        {
            this->clippingIndicators.getLast()->setEndBeat(clippingBeat);
            return;
        }
    }

    auto newMarker = make<TimelineWarningMarker>(TimelineWarningMarker::WarningLevel::Red, *this, clippingBeat);
    this->addAndMakeVisible(newMarker.get());
    this->clippingIndicators.add(newMarker.release());
}

void RollBase::resetAllClippingIndicators()
{
    this->clippingIndicators.clear();
}

void RollBase::onOversaturationWarning()
{
    if (!this->getTransport().isPlaying())
    {
        return;
    }

    const float warningBeat = this->lastPlayheadBeat.get();

    if (!this->oversaturationIndicators.isEmpty())
    {
        const float lastMarkerEndBeat = this->oversaturationIndicators.getLast()->getEndBeat();
        const bool justNeedToUpdateLastMarker =
            (warningBeat - lastMarkerEndBeat) < TimelineWarningMarker::minGapInBeats;

        if (justNeedToUpdateLastMarker)
        {
            this->oversaturationIndicators.getLast()->setEndBeat(warningBeat);
            return;
        }
    }

    auto newMarker = make<TimelineWarningMarker>(TimelineWarningMarker::WarningLevel::Yellow, *this, warningBeat);
    this->addAndMakeVisible(newMarker.get());
    this->oversaturationIndicators.add(newMarker.release());
}

void RollBase::resetAllOversaturationIndicators()
{
    this->oversaturationIndicators.clear();
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void RollBase::onUiAnimationsFlagChanged(bool enabled)
{
#if PLATFORM_DESKTOP
    this->smoothPanController->setAnimationsEnabled(enabled);
    this->smoothZoomController->setAnimationsEnabled(enabled);
    this->scrollToPlayheadTimerMs = enabled ? 6 : 1;
#elif PLATFORM_MOBILE
    this->smoothPanController->setAnimationsEnabled(false);
    this->smoothZoomController->setAnimationsEnabled(false);
    this->scrollToPlayheadTimerMs = 1;
#endif
}

void RollBase::onMouseWheelFlagsChanged(UserInterfaceFlags::MouseWheelFlags flags)
{
    this->mouseWheelFlags = flags;
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void RollBase::onSeek(float beatPosition, double currentTimeMs, double totalTimeMs)
{
    this->lastPlayheadBeat = beatPosition;
}

void RollBase::onLoopModeChanged(bool hasLoop, float start, float end)
{
    this->header->showLoopMode(hasLoop, start, end);
}

void RollBase::onPlay()
{
    this->resetAllClippingIndicators();
    this->resetAllOversaturationIndicators();
}

void RollBase::onRecord()
{
    this->header->showRecordingMode(true);
}

void RollBase::onStop()
{
    this->header->showRecordingMode(false);

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->stopFollowingPlayhead();
#endif
}

bool RollBase::scrollToPlayheadPositionIfNeeded()
{
    // first, check if the playhead is already on the screen and not too close to the edges
    const auto reducedViewArea = this->viewport.getViewArea().reduced(50, 0);
    const int playheadX = this->getXPositionByBeat(this->lastPlayheadBeat.get());
    if (playheadX > reducedViewArea.getX() && playheadX < reducedViewArea.getRight())
    {
        return false;
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD

    this->playheadFollowMode = PlayheadFollowMode::Once;
    this->startTimer(this->scrollToPlayheadTimerMs);
    return true;

#else

    this->viewport.setViewPosition(playheadX - (this->viewport.getViewWidth() / 3),
        this->viewport.getViewPositionY());

    this->updateChildrenBounds();
    return true;

#endif
}

void RollBase::startFollowingPlayhead()
{
#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->playheadFollowMode = PlayheadFollowMode::Always;
#endif
}

void RollBase::stopFollowingPlayhead()
{
    if (!this->isTimerRunning() &&
        this->playheadFollowMode == PlayheadFollowMode::None)
    {
        return;
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD
    this->stopTimer();
    this->playheadFollowMode = PlayheadFollowMode::None;
#endif
}


//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void RollBase::handleAsyncUpdate()
{
    // batch repaint & resize stuff
    if (this->batchRepaintList.size() > 0)
    {
        const auto &editMode = this->project.getEditMode();
        const bool childrenInteractionEnabled = editMode.shouldInteractWithChildren();
        const auto childCursor = childrenInteractionEnabled ?
            MouseCursor::NormalCursor : editMode.getCursor();

        ROLL_BATCH_REPAINT_START

        for (int i = 0; i < this->batchRepaintList.size(); ++i)
        {
            // There are still many cases when a component
            // scheduled for update is deleted at this time:
            if (FloatBoundsComponent *component = this->batchRepaintList.getUnchecked(i))
            {
                component->setInterceptsMouseClicks(childrenInteractionEnabled, childrenInteractionEnabled);
                component->setMouseCursor(childCursor);
                component->setFloatBounds(this->getEventBounds(component));
                component->repaint();
            }
        }

        ROLL_BATCH_REPAINT_END

        this->batchRepaintList.clearQuick();
    }

#if ROLL_VIEW_FOLLOWS_PLAYHEAD

    if (this->isTimerRunning())
    {
        const auto playheadOffset = this->findPlayheadOffsetFromViewCentre();
        const int playheadX = this->getPlayheadPositionByBeat(this->lastPlayheadBeat.get(), double(this->getWidth()));
        const int newX = playheadX - int(playheadOffset * 0.9) - (this->viewport.getViewWidth() / 2);

        const bool stuckFollowingPlayhead = newX == this->viewport.getViewPositionX() ||
            newX < 0 || newX > (this->getWidth() - this->viewport.getViewWidth());
        const bool doneFollowingPlayhead = fabs(playheadOffset) < 1.f &&
            this->playheadFollowMode == PlayheadFollowMode::Once;

        this->viewport.setViewPosition(newX, this->viewport.getViewPositionY());
        this->updateChildrenPositions();

        if (stuckFollowingPlayhead || doneFollowingPlayhead)
        {
            this->stopFollowingPlayhead();
        }
    }

#endif
}

double RollBase::findPlayheadOffsetFromViewCentre() const
{
    const int playheadX = this->getPlayheadPositionByBeat(this->lastPlayheadBeat.get(), double(this->getWidth()));
    const int viewportCentreX = this->viewport.getViewPositionX() + this->viewport.getViewWidth() / 2;
    return double(playheadX) - double(viewportCentreX);
}

void RollBase::triggerBatchRepaintFor(FloatBoundsComponent *target)
{
    this->batchRepaintList.add(target);
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void RollBase::hiResTimerCallback()
{
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Events check
//===----------------------------------------------------------------------===//

bool RollBase::isViewportZoomEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsViewportZooming(e.mods)) { return false; }
    if (this->project.getEditMode().forcesViewportZooming(e.mods)) { return true; }
    return false;
}

bool RollBase::isViewportDragEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsViewportDragging(e.mods)) { return false; }
    if (this->project.getEditMode().forcesViewportDragging(e.mods)) { return true; }
    if (e.source.isTouch()) { return e.mods.isLeftButtonDown(); }
    return (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown());
}

bool RollBase::isAddEvent(const MouseEvent &e) const
{
    if (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown()) { return false; }
    if (this->project.getEditMode().forbidsAddingEvents(e.mods)) { return false; }
    if (this->project.getEditMode().forcesAddingEvents(e.mods)) { return true; }
    return false;
}

bool RollBase::isLassoEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsSelectionMode(e.mods)) { return false; }
    if (this->project.getEditMode().forcesSelectionMode(e.mods)) { return true; }
    if (e.source.isTouch()) { return false; }
    return e.mods.isLeftButtonDown();
}

bool RollBase::isKnifeToolEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsCuttingEvents(e.mods)) { return false; }
    if (this->project.getEditMode().forcesCuttingEvents(e.mods)) { return true; }
    return false;
}

bool RollBase::isMergeToolEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsMergingEvents(e.mods)) { return false; }
    return this->project.getEditMode().forcesMergingEvents(e.mods);
}

bool RollBase::isErasingEvent(const MouseEvent &e) const
{
    if (this->project.getEditMode().forbidsErasingEvents(e.mods)) { return false; }
    return this->project.getEditMode().forcesErasingEvents(e.mods);
}

void RollBase::resetDraggingAnchors()
{
    this->viewportAnchor = this->viewport.getViewPosition();
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();
}

void RollBase::continueDragging(const MouseEvent &e)
{
    this->draggedDistance = e.getDistanceFromDragStart();
    this->smoothZoomController->cancelZoom();
    const auto offset = this->getMouseOffset(e.source.getScreenPosition()).toInt();
    this->panByOffset(offset.x, offset.y);
}

//===----------------------------------------------------------------------===//
// Zooming
//===----------------------------------------------------------------------===//

void RollBase::startZooming()
{
    this->smoothPanController->cancelPan();
    this->clickAnchor = Desktop::getInstance().getMainMouseSource().getScreenPosition();

    this->zoomAnchor.setXY(0, 0);

    this->zoomMarker = make<IconComponent>(Icons::zoomIn);
    this->zoomMarker->setAlwaysOnTop(true);

    const auto mouseDownPosition = this->clickAnchor.toInt() - this->viewport.getScreenPosition();

    this->zoomMarker->setSize(24, 24);
    this->zoomMarker->setCentrePosition(mouseDownPosition.getX(), mouseDownPosition.getY());
    this->viewport.addAndMakeVisible(this->zoomMarker.get());

    Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(true, false);
}

void RollBase::continueZooming(const MouseEvent &e)
{
    Desktop::getInstance().getMainMouseSource().setScreenPosition(e.getMouseDownScreenPosition().toFloat());

    const auto clickOffset = e.position - this->viewport.getViewPosition().toFloat();
    const float dragOffsetX = float(e.getPosition().getX() - e.getMouseDownPosition().getX());
    const float dragOffsetY = -float(e.getPosition().getY() - e.getMouseDownPosition().getY());
    const auto zoomOffset = (Point<float>(dragOffsetX, dragOffsetY) - this->zoomAnchor).toFloat() * 0.005f;

    this->smoothZoomController->zoomRelative(clickOffset, zoomOffset);
}

void RollBase::endZooming()
{
    Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(false, false);

    this->zoomAnchor.setXY(0, 0);
    this->zoomMarker = nullptr;
}

//===----------------------------------------------------------------------===//
// Misc
//===----------------------------------------------------------------------===//

Point<float> RollBase::getMouseOffset(Point<float> mouseScreenPosition) const
{
    const int w = this->getWidth() - this->viewport.getWidth();

    const auto distanceFromDragStart = mouseScreenPosition - this->clickAnchor;
    float x = this->viewportAnchor.getX() - distanceFromDragStart.getX();

    x = (x < 0) ? 0 : x;
    x = (x > w) ? w : x;

    const int h = this->getHeight() - this->viewport.getHeight();
    float y = this->viewportAnchor.getY() - distanceFromDragStart.getY();

    y = (y < 0) ? 0 : y;
    y = (y > h) ? h : y;

    return Point<float>(x, y);
}

Point<int> RollBase::getDefaultPositionForPopup() const
{
    // a point where pop-ups will appear when keypress is hit or toolbar button is clicked
    // on desktop, if mouse position is within a roll, use it, instead, use main layout centre
#if PLATFORM_DESKTOP
    const auto mousePositionWithinApp =
        Desktop::getInstance().getMainMouseSource().getScreenPosition().toInt() -
        App::Layout().getScreenBounds().getPosition();

    if (App::Layout().getBoundsForPopups().contains(mousePositionWithinApp))
    {
        return mousePositionWithinApp;
    }
#endif

    return App::Layout().getBoundsForPopups().getCentre();
}

void RollBase::updateBounds()
{
    const int newWidth = int(this->getNumBeats() * this->beatWidth);

    if (this->getWidth() != newWidth)
    {
        this->setSize(newWidth, this->getHeight());
    }
}

void RollBase::updateChildrenBounds()
{
    ROLL_BATCH_REPAINT_START

    const int &viewHeight = this->viewport.getViewHeight();
    const int &viewWidth = this->viewport.getViewWidth();
    const int &viewX = this->viewport.getViewPositionX();
    const int &viewY = this->viewport.getViewPositionY();

    this->header->setBounds(0, viewY, this->getWidth(), Globals::UI::rollHeaderHeight);
    this->headerShadow->setBounds(viewX, viewY + Globals::UI::rollHeaderHeight,
        viewWidth, Globals::UI::rollHeaderShadowSize);

    if (this->annotationsMap != nullptr)
    {
        this->annotationsMap->setBounds(0, viewY + Globals::UI::rollHeaderHeight,
            this->getWidth(), AnnotationLargeComponent::annotationHeight);
    }

    if (this->keySignaturesMap != nullptr)
    {
        this->keySignaturesMap->setBounds(0, viewY + Globals::UI::rollHeaderHeight,
            this->getWidth(), Globals::UI::rollHeaderHeight);
    }

    if (this->timeSignaturesMap != nullptr)
    {
        this->timeSignaturesMap->setBounds(0, viewY,
            this->getWidth(), Globals::UI::rollHeaderHeight);
    }

    for (auto *trackMap : this->trackMaps)
    {
        trackMap->setBounds(0,
            viewY + viewHeight - trackMap->getHeight(),
            this->getWidth(),
            trackMap->getHeight());
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->updateBounds();
    }

    this->broadcastRollResized();

    ROLL_BATCH_REPAINT_END
}

void RollBase::updateChildrenPositions()
{
    ROLL_BATCH_REPAINT_START

    const int &viewHeight = this->viewport.getViewHeight();
    const int &viewX = this->viewport.getViewPositionX();
    const int &viewY = this->viewport.getViewPositionY();

    this->header->setTopLeftPosition(0, viewY);
    this->headerShadow->setTopLeftPosition(viewX,
        viewY + Globals::UI::rollHeaderHeight);

    if (this->annotationsMap != nullptr)
    {
        this->annotationsMap->setTopLeftPosition(0,
            viewY + Globals::UI::rollHeaderHeight);
    }

    if (this->keySignaturesMap != nullptr)
    {
        this->keySignaturesMap->setTopLeftPosition(0,
            viewY + Globals::UI::rollHeaderHeight);
    }

    if (this->timeSignaturesMap != nullptr)
    {
        this->timeSignaturesMap->setTopLeftPosition(0, viewY);
    }

    for (auto *trackMap : this->trackMaps)
    {
        trackMap->setTopLeftPosition(0, viewY + viewHeight - trackMap->getHeight());
    }

    this->broadcastRollMoved();

    ROLL_BATCH_REPAINT_END
}

//===----------------------------------------------------------------------===//
// ChangeListener: edit mode changed
//===----------------------------------------------------------------------===//

void RollBase::changeListenerCallback(ChangeBroadcaster *source)
{
    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->endLasso();
    }

    this->applyEditModeUpdates();
}

void RollBase::applyEditModeUpdates()
{
    const auto &editMode = this->project.getEditMode();
    if (this->isUsingSpaceDraggingMode() &&
        !(editMode.isMode(RollEditMode::dragMode)))
    {
        this->setSpaceDraggingMode(false);
    }

    const auto cursor = editMode.getCursor();
    this->setMouseCursor(cursor);

    const bool interactsWithChildren = editMode.shouldInteractWithChildren();
    const auto childCursor = interactsWithChildren ? MouseCursor::NormalCursor : cursor;
    this->setChildrenInteraction(interactsWithChildren, childCursor);
}
