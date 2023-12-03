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
#include "AutomationEditor.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "ProjectNode.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "AnnotationEvent.h"
#include "RollBase.h"
#include "AutomationCurveEventComponent.h"
#include "AutomationStepEventComponent.h"
#include "MultiTouchController.h"
#include "Lasso.h"
#include "ColourIDs.h"
#include "FineTuningValueIndicator.h"

//===----------------------------------------------------------------------===//
// Hand-drawing helper
//===----------------------------------------------------------------------===//

// hand-drawing helper with home-cooked curve fitting algorithm
// that works most of the time: for now, automation tracks use
// curves with one control point for adjusting ease-in/ease-out
// level for each curve point, so they are not nearly as flexible
// as Bezier curves, and the fitting algorithm is less-than-ideal,
// but I like how they are much simpler from the UI/UX perspective;
// in future it may be worth supporting Bezier curves as well

class AutomationHandDrawingHelper final : public Component
{
public:

    AutomationHandDrawingHelper(AutomationEditor &editor) : editor(editor)
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }
    
    template <typename T>
    struct LineWithEasing final : public Line<T>
    {
        LineWithEasing() = default;
        LineWithEasing(Point<T> startPoint, Point<T> endPoint, float easing) noexcept :
            Line(startPoint, endPoint),
            easing(easing) {}

        float easing = 0.f;
    };

    const Array<LineWithEasing<float>> &getSimplifiedCurve() const noexcept
    {
        return this->curveInBeats;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        g.fillPath(this->curveInPixels);

        for (const auto &pivotPoint : pivotsInPixels)
        {
            g.fillRect({ pivotPoint.translated(-2, -1), pivotPoint.translated(2, 1) });
            g.fillRect({ pivotPoint.translated(-1, -2), pivotPoint.translated(1, 2) });
        }
    }

    void setStartMousePosition(const Point<float> &mousePos)
    {
        this->rawPositions.clearQuick();
        this->simplifiedPositionsL1.clearQuick();
        this->simplifiedPositionsL2.clearQuick();

        const auto newPosition = mousePos.toDouble() / this->getSize();

        this->rawPositions.add(newPosition);
        this->simplifiedPositionsL1.add(newPosition);
        this->simplifiedPositionsL2.add({ newPosition, 0.f });
    }

    void addMousePosition(const Point<float> &mousePos, const float viewWidth)
    {
        jassert(!this->rawPositions.isEmpty());
        jassert(!this->simplifiedPositionsL1.isEmpty());
        jassert(!this->simplifiedPositionsL2.isEmpty());

        const auto mySize = this->getSize();
        const auto newPosition = mousePos.toDouble() / mySize;

        constexpr auto prefilterThreshold = 1;
        const auto lastPositionInPixels = this->rawPositions.getLast() * mySize;
        const auto d = lastPositionInPixels.getDistanceFrom(mousePos.toDouble());
        if (d > prefilterThreshold)
        {
            this->rawPositions.add(newPosition);
        }

        // picks the epsilon depending on zoom level
        const auto epsilon1 = jmax(0.000001f, viewWidth / float(mySize.getX()) * 0.001f);
        const auto epsilon2 = epsilon1 * 50.f;

        // ~50% reduction for filtering out the noise for painting
        this->simplifiedPositionsL1 =
            PointReduction<double>::simplify(this->rawPositions, epsilon1);

        // ~1-2 orders of magnitude reduction for actual curve fitting
        this->simplifiedPositionsL2 =
            PointReduction<double>::simplifyExtended(this->rawPositions, epsilon2);
        
        if (this->simplifiedPositionsL1.size() >= 2)
        {
            this->simplifiedPositionsL1[this->simplifiedPositionsL1.size() - 1] = newPosition;
        }

        if (this->simplifiedPositionsL2.size() >= 2)
        {
            this->simplifiedPositionsL2[this->simplifiedPositionsL2.size() - 1].point = newPosition;
        }
    }

    void updateCurves()
    {
        const auto mySize = this->getSize();

        this->curveInBeats.clearQuick();
        this->pivotsInPixels.clearQuick();
        this->curveInPixels.clear();

        {
            jassert(!this->simplifiedPositionsL1.isEmpty());
            auto previousPointPx = (this->simplifiedPositionsL1.getFirst() * mySize).toFloat();
            this->curveInPixels.startNewSubPath(previousPointPx);

            for (int i = 1; i < this->simplifiedPositionsL1.size(); ++i)
            {
                const auto nextPointPx = (this->simplifiedPositionsL1.getUnchecked(i) * mySize).toFloat();
                this->curveInPixels.lineTo(nextPointPx);
                previousPointPx = nextPointPx;
            }
        }

        {
            jassert(!this->simplifiedPositionsL2.isEmpty());
            auto previousPointPx = (this->simplifiedPositionsL2.getFirst().point * mySize).toFloat();
            auto previousEasing = this->simplifiedPositionsL2.getFirst().easingLevel;
            this->pivotsInPixels.add(previousPointPx.roundToInt());

            for (int i = 1; i < this->simplifiedPositionsL2.size(); ++i)
            {
                const auto nextPointPx = (this->simplifiedPositionsL2.getUnchecked(i).point * mySize).toFloat();
                const auto nextEasing = this->simplifiedPositionsL2.getUnchecked(i).easingLevel;

                this->pivotsInPixels.add(nextPointPx.roundToInt());
                this->curveInBeats.add({
                    { this->editor.getBeatByXPosition(previousPointPx.x), previousPointPx.y },
                    { this->editor.getBeatByXPosition(nextPointPx.x), nextPointPx.y },
                    (previousPointPx.x < nextPointPx.x) ? previousEasing : 1.f - previousEasing
                });

                previousPointPx = nextPointPx;
                previousEasing = nextEasing;
            }

            // fallback to a single point; on mouseUp it will just insert a single event
            if (this->curveInBeats.isEmpty())
            {
                this->curveInBeats.add({
                    { this->editor.getBeatByXPosition(previousPointPx.x), previousPointPx.y },
                    { this->editor.getBeatByXPosition(previousPointPx.x), previousPointPx.y },
                    previousEasing
                });
            }
        }

        static const float lineThickness = 1.f;
        static Array<float> dashes(4.f, 3.f);
        PathStrokeType(1.f).createDashedStroke(this->curveInPixels, this->curveInPixels,
            dashes.getRawDataPointer(), dashes.size());
    }

private:

    AutomationEditor &editor;

    // all 3 in absolute values, 0..1
    Array<Point<double>> rawPositions;
    // ~50% reduction for painting:
    Array<Point<double>> simplifiedPositionsL1;
    // ~1-2 orders of magnitude reduction for curve fitting:
    Array<PointReduction<double>::PointWithEasing> simplifiedPositionsL2;

    // simplifiedPositionsL1 for painting
    Path curveInPixels;
    // simplifiedPositionsL2 for painting
    Array<Point<int>> pivotsInPixels;
    // simplifiedPositionsL2 in beats
    Array<LineWithEasing<float>> curveInBeats;

    const Colour colour = findDefaultColour(Label::textColourId);

    inline const Point<double> getSize() const noexcept
    {
        return this->getLocalBounds().getBottomRight().toDouble();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationHandDrawingHelper)
};

//===----------------------------------------------------------------------===//
// AutomationEditor
//===----------------------------------------------------------------------===//

#define AUTO_EDITOR_BATCH_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define AUTO_EDITOR_BATCH_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

AutomationEditor::AutomationEditor(ProjectNode &project, SafePointer<RollBase> roll) :
    project(project),
    roll(roll)
{
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);

    this->multiTouchController = make<MultiTouchController>(*this);
    this->addMouseListener(this->multiTouchController.get(), true);

    this->reloadTrackMap();

    this->project.addListener(this);
    this->project.getEditMode().addListener(this);
}

AutomationEditor::~AutomationEditor()
{
    this->project.getEditMode().removeListener(this);
    this->project.removeListener(this);
    this->removeMouseListener(this->multiTouchController.get());
}

//===----------------------------------------------------------------------===//
// AutomationEditorBase
//===----------------------------------------------------------------------===//

Colour AutomationEditor::getColour(const AutomationEvent &event) const
{
    return event.getTrackColour()
        .withMultipliedSaturation(0.75f)
        .withMultipliedBrightness(1.5f);
}

float AutomationEditor::getBeatByPosition(int x, const Clip &clip) const
{
    return this->roll->getRoundBeatSnapByXPosition(x) - clip.getBeat();
}

void AutomationEditor::getBeatValueByPosition(int x, int y,
    const Clip &clip, float &targetValue, float &targetBeat) const
{
    targetBeat = this->getBeatByPosition(x, clip);
    targetValue = float(this->getHeight() - y) / float(this->getHeight()); // flipped upside down
    targetValue = jlimit(0.f, 1.f, targetValue);
}

Rectangle<float> AutomationEditor::getEventBounds(const AutomationEvent &event, const Clip &clip) const
{
    const auto *seqence = event.getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = event.getBeat() + clip.getBeat();
    return seqence->getTrack()->isOnOffAutomationTrack() ?
        this->getOnOffEventBounds(beat, sequenceLength, event.isPedalDownEvent()) :
        this->getCurveEventBounds(beat, sequenceLength, event.getControllerValue());
}

Rectangle<float> AutomationEditor::getCurveEventBounds(float beat,
    float sequenceLength, double controllerValue) const
{
    constexpr auto diameter = AutomationEditor::curveEventComponentDiameter;
    const float x = float(this->roll->getXPositionByBeat(beat, double(this->getWidth())));
    const float y = roundf(float(1.0 - controllerValue) * float(this->getHeight())); // flipped upside down
    return { x - (diameter / 2.f), y - (diameter / 2.f), diameter, diameter };
}

Rectangle<float> AutomationEditor::getOnOffEventBounds(float beat,
    float sequenceLength, bool isPedalDown) const
{
    const float minWidth = 2.f;
    const float w = jmax(minWidth,
        float(this->roll->getBeatWidth()) * AutomationStepEventComponent::minLengthInBeats);

    const float x = float(this->roll->getXPositionByBeat(beat, double(this->getWidth())));
    return { x - w + AutomationStepEventComponent::pointOffset, 0.f, w, float(this->getHeight()) };
}

bool AutomationEditor::hasEditMode(RollEditMode::Mode mode) const noexcept
{
    return this->getEditMode().isMode(mode);
}

//===----------------------------------------------------------------------===//
// Hand-drawing shapes
//===----------------------------------------------------------------------===//

constexpr float getControllerValueByY(float y)
{
    return 1.f - (y / float(Globals::UI::editorPanelHeight));
}

void AutomationEditor::applyHandDrawnCurve()
{
    if (!this->activeClip.hasValue())
    {
        jassertfalse;
        return;
    }

    const auto activeClip = *this->activeClip;
    if (!this->patternMap.contains(activeClip))
    {
        jassertfalse;
        return;
    }

    jassert(!activeClip.getPattern()->getTrack()->isOnOffAutomationTrack());

    const auto *activeMap = this->patternMap.at(activeClip).get();
    auto *sequence = static_cast<AutomationSequence *>(activeClip.getPattern()->getTrack()->getSequence());

    const auto &simplifiedCurve = this->handDrawingHelper->getSimplifiedCurve();
    if (simplifiedCurve.isEmpty())
    {
        jassertfalse;
        return;
    }
    
    bool didCheckpoint = false;

    {
        // find all events within the range of the shape and delete them
        Array<AutomationEvent> eventsToDelete;

        constexpr auto threshold = 2.f;
        for (const auto &i : activeMap->eventsMap)
        {
            const auto eventFullBeat =
                i.second->getEvent().getBeat() + i.second->getClip().getBeat();

            for (const auto &line : simplifiedCurve)
            {
                const auto startBeat = jmin(line.getStartX(), line.getEndX());
                const auto endBeat = jmax(line.getStartX(), line.getEndX());

                if (eventFullBeat >= (startBeat - threshold) &&
                    eventFullBeat <= (endBeat + threshold))
                {
                    eventsToDelete.add(i.second->getEvent());
                    break; // not adding it twice or more
                }
            }
        }

        if (!eventsToDelete.isEmpty())
        {
            if (!didCheckpoint)
            {
                didCheckpoint = true;
                this->project.checkpoint();
            }

            sequence->removeGroup(eventsToDelete, true);
        }
    }

    {
        // insert new events
        Array<AutomationEvent> newEvents;

        const auto clipBeatOffset = activeClip.getBeat();

        // if the shape is shorter that a beat,
        // just insert one point at the end of the curve
        if (fabs(simplifiedCurve.getLast().getEndX() - simplifiedCurve.getFirst().getStartX()) > 1.f)
        {
            for (const auto &line : simplifiedCurve)
            {
                newEvents.add(AutomationEvent(sequence,
                    line.getStartX() - clipBeatOffset,
                    getControllerValueByY(line.getStartY()))
                        .withCurvature(simplifiedCurve.getLast().easing / 2.f + 0.5f));
            }
        }

        // the last point
        newEvents.add(AutomationEvent(sequence,
            simplifiedCurve.getLast().getEndX() - clipBeatOffset,
            getControllerValueByY(simplifiedCurve.getLast().getEndY()))
                .withCurvature(simplifiedCurve.getLast().easing / 2.f + 0.5f));

        if (!newEvents.isEmpty())
        {
            if (!didCheckpoint)
            {
                didCheckpoint = true;
                this->project.checkpoint();
            }

            sequence->insertGroup(newEvents, true);
        }
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationEditor::resized()
{
    AUTO_EDITOR_BATCH_REPAINT_START

    for (const auto &map : this->patternMap)
    {
        this->applyEventsBounds(map.second.get());
    }

    if (this->handDrawingHelper != nullptr)
    {
        this->handDrawingHelper->setBounds(this->getLocalBounds());
        this->handDrawingHelper->updateCurves();
    }

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::mouseDown(const MouseEvent &e)
{
    if (this->hasMultiTouch(e))
    {
        return;
    }

    this->panningStart = e.getPosition();

    if (this->isDrawingEvent(e) &&
        this->activeClip.hasValue() &&
        !this->activeClip->getPattern()->getTrack()->isOnOffAutomationTrack())
    {
        this->handDrawingHelper = make<AutomationHandDrawingHelper>(*this);
        this->addAndMakeVisible(this->handDrawingHelper.get());
        this->handDrawingHelper->setBounds(this->getLocalBounds());
        this->handDrawingHelper->setStartMousePosition(e.position);
        this->handDrawingHelper->updateCurves();
        this->handDrawingHelper->repaint();
    }
    else if (this->isDraggingEvent(e))
    {
        this->roll->mouseDown(e.getEventRelativeTo(this->roll));
    }
}

void AutomationEditor::mouseDrag(const MouseEvent &e)
{
    if (this->hasMultiTouch(e))
    {
        return;
    }

    if (this->handDrawingHelper != nullptr)
    {
        this->handDrawingHelper->addMousePosition(e.position,
            float(this->roll->getViewport().getViewWidth()));
        this->handDrawingHelper->updateCurves();
        this->handDrawingHelper->repaint();
    }

    if (this->isDraggingEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll->mouseDrag(e
            .withNewPosition(Point<int>(e.getPosition().getX(), this->panningStart.getY()))
            .getEventRelativeTo(this->roll));
    }
}

void AutomationEditor::mouseUp(const MouseEvent &e)
{
    if (this->handDrawingHelper != nullptr)
    {
        this->applyHandDrawnCurve();
        this->handDrawingHelper = nullptr;
    }

    if (this->hasMultiTouch(e))
    {
        return;
    }

    if (this->isDraggingEvent(e))
    {
        this->roll->mouseUp(e
            .withNewPosition(Point<int>(e.getPosition().getX(), this->panningStart.getY()))
            .getEventRelativeTo(this->roll));
    }

    this->setMouseCursor(this->getEditMode().getCursor());
}

void AutomationEditor::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    jassert(this->roll != nullptr);
    this->roll->mouseWheelMove(e.getEventRelativeTo(this->roll), wheel);
}

//===----------------------------------------------------------------------===//
// EditorPanelBase
//===----------------------------------------------------------------------===//

void AutomationEditor::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
}

float AutomationEditor::getBeatByXPosition(float x) const noexcept
{
    jassert(this->getWidth() == this->roll->getWidth());
    return this->roll->getBeatByXPosition(x);
}

void AutomationEditor::setEditableClip(Optional<Clip> clip)
{
    if (this->activeClip == clip)
    {
        return;
    }

    this->activeClip = clip;

    AUTO_EDITOR_BATCH_REPAINT_START

    for (const auto &map : this->patternMap)
    {
        const bool isEditable = map.first == clip;
        for (auto *component : map.second->sortedComponents)
        {
            component->setEditable(isEditable);
        }
    }

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::setEditableClip(const Clip &selectedClip, const EventFilter &filter)
{
    const auto *selectedSequence = selectedClip.getPattern()->getTrack()->getSequence();
    const auto selectedRange = Range<float>(
        selectedSequence->getFirstBeat() + selectedClip.getBeat(),
        selectedSequence->getLastBeat() + selectedClip.getBeat());

    Clip matchingClip;
    float maxIntersectionLength = -1.f;
    for (const auto &map : this->patternMap)
    {
        const auto *track = map.first.getPattern()->getTrack();
        const auto *matchingSequence = track->getSequence();
        if (track->getTrackControllerNumber() != filter.id)
        {
            continue;
        }

        const auto matchingRange = Range<float>(
            matchingSequence->getFirstBeat() + map.first.getBeat(),
            matchingSequence->getLastBeat() + map.first.getBeat());

        const auto intersectionLength = selectedRange
            .getIntersectionWith(matchingRange).getLength();

        if (intersectionLength > maxIntersectionLength)
        {
            maxIntersectionLength = intersectionLength;
            matchingClip = map.first;
        }
    }

    jassert(matchingClip.isValid());
    if (matchingClip.isValid())
    {
        this->setEditableClip(matchingClip);
    }
}

void AutomationEditor::setEditableSelection(WeakReference<Lasso> selection)
{
    // automation events can't be selected, so just keep the entire clip editable
}

bool AutomationEditor::canEditSequence(WeakReference<MidiSequence> sequence) const
{
    return dynamic_cast<AutomationSequence *>(sequence.get()) != nullptr;
}

Array<AutomationEditor::EventFilter> AutomationEditor::getAllEventFilters() const
{
    FlatHashMap<int, String> trackGrouping;
    for (const auto &map : this->patternMap)
    {
        const auto *track = map.first.getPattern()->getTrack();
        trackGrouping[track->getTrackControllerNumber()] =
            track->isTempoTrack() ? TRANS(I18n::Defaults::tempoTrackName) :
            track->getTrackName();
    }

    Array<AutomationEditor::EventFilter> result;
    for (const auto &it : trackGrouping)
    {
        result.add({it.first, TRANS(it.second)});
    }

    static AutomationEditor::EventFilter orderById;
    result.sort(orderById);

    return result;
}

//===----------------------------------------------------------------------===//
// MultiTouchListener
//===----------------------------------------------------------------------===//

// because the roll is the upstream of move-resize events for all bottom panels,
// this will simply ensure that zooming and panning events are horizontal-only
// and pass them to the currently active roll; the same logic can be found in VelocityEditor

void AutomationEditor::multiTouchStartZooming()
{
    this->roll->multiTouchStartZooming();
}

void AutomationEditor::multiTouchContinueZooming(
        const Rectangle<float> &relativePositions,
        const Rectangle<float> &relativeAnchor,
        const Rectangle<float> &absoluteAnchor)
{
    this->roll->multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);
}

void AutomationEditor::multiTouchEndZooming(const MouseEvent &anchorEvent)
{
    this->panningStart = anchorEvent.getPosition();
    this->roll->multiTouchEndZooming(anchorEvent.getEventRelativeTo(this->roll));
}

Point<float> AutomationEditor::getMultiTouchRelativeAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchRelativeAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), this->panningStart.getY()))
        .getEventRelativeTo(this->roll));
}

Point<float> AutomationEditor::getMultiTouchAbsoluteAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchAbsoluteAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), this->panningStart.getY()))
        .getEventRelativeTo(this->roll));
}

bool AutomationEditor::hasMultiTouch(const MouseEvent &e) const
{
    return this->roll->hasMultiTouch(e) || this->multiTouchController->hasMultiTouch(e);
}

//===----------------------------------------------------------------------===//
// RollEditMode::Listener & edit mode helpers
//===----------------------------------------------------------------------===//

// for now, automation editor only supports three modes: dragging, drawing
// and the default mode, i.e. no selection mode and no knife/merge tool;
// this hack maps all existing edit modes to supported ones, so that
// selection mode becomes the default mode and knife mode becomes drawing mode
// (see the same logic in VelocityEditor)

RollEditMode AutomationEditor::getSupportedEditMode(const RollEditMode &rollMode) const noexcept
{
    if (rollMode.isMode(RollEditMode::dragMode))
    {
        return RollEditMode::dragMode;
    }
    else if (rollMode.isMode(RollEditMode::drawMode) || rollMode.isMode(RollEditMode::knifeMode))
    {
        return RollEditMode::drawMode;
    }

    return RollEditMode::defaultMode;
}

RollEditMode AutomationEditor::getEditMode() const noexcept
{
    return this->getSupportedEditMode(this->project.getEditMode());
}

void AutomationEditor::onChangeEditMode(const RollEditMode &mode)
{
    this->setMouseCursor(this->getSupportedEditMode(mode).getCursor());
    // todo fix children interaction someday as well
    // (not straighforward because it shouldn't break setEditable stuff)
}

bool AutomationEditor::isDraggingEvent(const MouseEvent &e) const
{
    if (this->getEditMode().forbidsViewportDragging(e.mods)) { return false; }
    if (this->getEditMode().forcesViewportDragging(e.mods)) { return true; }
    return e.source.isTouch() || e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown();
}

bool AutomationEditor::isDrawingEvent(const MouseEvent &e) const
{
    if (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown()) { return false; }
    if (this->getEditMode().forbidsAddingEvents(e.mods)) { return false; }
    if (this->getEditMode().forcesAddingEvents(e.mods)) { return true; }
    return false;
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

#define forEachSequenceMapOfGivenTrack(map, child, track) \
    for (const auto &child : map) \
        if (child.first.getPattern()->getTrack() == track)

void AutomationEditor::onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2)
{
    if (!e1.isTypeOf(MidiEvent::Type::Auto))
    {
        return;
    }

    const auto &event = static_cast<const AutomationEvent &>(e1);
    const auto &newEvent = static_cast<const AutomationEvent &>(e2);
    const auto *track = newEvent.getSequence()->getTrack();

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        auto *sequenceMap = c.second.get();
        if (auto *component = sequenceMap->eventsMap[event])
        {
            sequenceMap->sortedComponents.sort(*component);

            const int indexOfSorted = sequenceMap->sortedComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = sequenceMap->sortedComponents[indexOfSorted - 1];
            auto *nextEventComponent = sequenceMap->sortedComponents[indexOfSorted + 1];

            // if the neighbourhood has changed,
            // connect the most recent neighbours to each other:
            if (nextEventComponent != component->getNextNeighbour() ||
                previousEventComponent != component->getPreviousNeighbour())
            {
                if (component->getPreviousNeighbour())
                {
                    component->getPreviousNeighbour()->setNextNeighbour(component->getNextNeighbour());
                }

                if (component->getNextNeighbour())
                {
                    component->getNextNeighbour()->setPreviousNeighbour(component->getPreviousNeighbour());
                }
            }

            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);
            this->applyEventBounds(component);

            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
            }

            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(component);
            }

            sequenceMap->eventsMap.erase(event);
            sequenceMap->eventsMap[newEvent] = component;
        }
    }
}

void AutomationEditor::onAddMidiEvent(const MidiEvent &event)
{
    if (!event.isTypeOf(MidiEvent::Type::Auto))
    {
        return;
    }
    
    const auto &autoEvent = static_cast<const AutomationEvent &>(event);
    const auto *track = autoEvent.getSequence()->getTrack();
    const bool isOnOffTrack = track->isOnOffAutomationTrack();

    AUTO_EDITOR_BATCH_REPAINT_START

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        auto *sequenceMap = c.second.get();
        const auto *targetClipParams = &c.first;
        const int i = track->getPattern()->indexOfSorted(targetClipParams);
        jassert(i >= 0);

        const auto *clip = track->getPattern()->getUnchecked(i);
        const bool isEditable = this->activeClip == *clip;

        auto *component = isOnOffTrack ?
            this->createOnOffEventComponent(autoEvent, *clip) :
            this->createCurveEventComponent(autoEvent, *clip);

        component->setEditable(isEditable);
        this->addAndMakeVisible(component);

        // update links and connectors
        const int indexOfSorted = sequenceMap->sortedComponents.addSorted(*component, component);
        auto *previousEventComponent = sequenceMap->sortedComponents[indexOfSorted - 1];
        auto *nextEventComponent = sequenceMap->sortedComponents[indexOfSorted + 1];

        component->setNextNeighbour(nextEventComponent);
        component->setPreviousNeighbour(previousEventComponent);
        this->applyEventBounds(component);

        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(component);
        }

        if (nextEventComponent)
        {
            nextEventComponent->setPreviousNeighbour(component);
        }

        sequenceMap->eventsMap[autoEvent] = component;
    }

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::onRemoveMidiEvent(const MidiEvent &event)
{
    if (!event.isTypeOf(MidiEvent::Type::Auto))
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(event);
    const auto *track = autoEvent.getSequence()->getTrack();

    AUTO_EDITOR_BATCH_REPAINT_START

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        auto *sequenceMap = c.second.get();

        if (auto *component = sequenceMap->eventsMap[autoEvent])
        {
            //this->eventAnimator.fadeOut(component, Globals::UI::fadeOutShort);
            this->removeChildComponent(component);
            sequenceMap->eventsMap.erase(autoEvent);

            // update links and connectors for neighbors
            const int indexOfSorted = sequenceMap->sortedComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = sequenceMap->sortedComponents[indexOfSorted - 1];
            auto *nextEventComponent = sequenceMap->sortedComponents[indexOfSorted + 1];

            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(nextEventComponent);
            }

            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(previousEventComponent);
            }

            sequenceMap->sortedComponents.remove(indexOfSorted, true);
        }
    }

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::onAddClip(const Clip &clip)
{
    SequenceMap *referenceMap = nullptr;
    const auto *track = clip.getPattern()->getTrack();
    if (!dynamic_cast<const AutomationSequence *>(track->getSequence())) { return; }

    const bool isOnOffTrack = track->isOnOffAutomationTrack();

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        // a sequence map for the same track:
        referenceMap = c.second.get();
        break;
    }

    if (referenceMap == nullptr)
    {
        jassertfalse;
        return;
    }

    auto *sequenceMap = new SequenceMap();
    this->patternMap[clip] = UniquePointer<SequenceMap>(sequenceMap);

    AUTO_EDITOR_BATCH_REPAINT_START

    for (const auto *referenceComponent : referenceMap->sortedComponents)
    {
        const auto &referenceEvent = referenceComponent->getEvent();
        const bool editable = this->activeClip == clip;

        auto *component = isOnOffTrack ?
            this->createOnOffEventComponent(referenceEvent, clip) :
            this->createCurveEventComponent(referenceEvent, clip);

        component->setEditable(this->activeClip == clip);
        this->addAndMakeVisible(component);

        const int indexOfSorted = sequenceMap->sortedComponents.addSorted(*component, component);
        auto *previousEventComponent = sequenceMap->sortedComponents[indexOfSorted - 1];
        auto *nextEventComponent = sequenceMap->sortedComponents[indexOfSorted + 1];

        component->setNextNeighbour(nextEventComponent);
        component->setPreviousNeighbour(previousEventComponent);

        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(component);
        }

        if (nextEventComponent)
        {
            nextEventComponent->setPreviousNeighbour(component);
        }

        sequenceMap->eventsMap[referenceEvent] = component;
    }

    this->applyEventsBounds(sequenceMap);

    AUTO_EDITOR_BATCH_REPAINT_END

    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->activeClip == clip) // same id
    {
        this->activeClip = newClip; // new parameters
    }

    if (this->patternMap.contains(clip))
    {
        AUTO_EDITOR_BATCH_REPAINT_START

        // set new key for the existing sequence map
        auto *sequenceMap = this->patternMap[clip].release();
        this->patternMap.erase(clip);
        this->patternMap[newClip] = UniquePointer<SequenceMap>(sequenceMap);
        this->applyEventsBounds(sequenceMap);

        AUTO_EDITOR_BATCH_REPAINT_END
    }
}

void AutomationEditor::onRemoveClip(const Clip &clip)
{
    AUTO_EDITOR_BATCH_REPAINT_START

    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
    }

    AUTO_EDITOR_BATCH_REPAINT_END

    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onChangeTrackProperties(MidiTrack *const track)
{
    if (!dynamic_cast<const AutomationSequence *>(track->getSequence())) { return; }

    AUTO_EDITOR_BATCH_REPAINT_START

    for (const auto &map : this->patternMap)
    {
        for (auto *component : map.second->sortedComponents)
        {
            component->updateColour();
        }
    }

    AUTO_EDITOR_BATCH_REPAINT_END

    this->repaint();

    // track name might have changed
    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->reloadTrackMap();
    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const AutomationSequence *>(track->getSequence())) { return; }

    AUTO_EDITOR_BATCH_REPAINT_START
    this->loadTrack(track);
    AUTO_EDITOR_BATCH_REPAINT_END

    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onRemoveTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const AutomationSequence *>(track->getSequence())) { return; }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto &clip = *track->getPattern()->getUnchecked(i);
        if (this->patternMap.contains(clip))
        {
            this->patternMap.erase(clip);
        }
    }

    this->listeners.call(&EditorPanelBase::Listener::onUpdateEventFilters);
}

void AutomationEditor::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat || this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = jmin(firstBeat, this->rollFirstBeat);
        this->rollLastBeat = jmax(lastBeat, this->rollLastBeat);
    }
}

void AutomationEditor::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
    }
}

// not reacting on onChangeViewEditableScope here: will update the selection
// in setEditableScope callbacks called by parent EditorPanelsScroller

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void AutomationEditor::reloadTrackMap()
{
    this->patternMap.clear();

    AUTO_EDITOR_BATCH_REPAINT_START

    const auto &tracks = this->project.getTracks();
    for (const auto *track : tracks)
    {
        if (nullptr != dynamic_cast<const AutomationSequence *>(track->getSequence()))
        {
            this->loadTrack(track);
        }
    }

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::loadTrack(const MidiTrack *const track)
{
    if (track->getPattern() == nullptr)
    {
        return;
    }

    const bool isOnOffTrack = track->isOnOffAutomationTrack();

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto *clip = track->getPattern()->getUnchecked(i);
        const bool editable = this->activeClip == *clip;

        auto *sequenceMap = new SequenceMap();
        this->patternMap[*clip] = UniquePointer<SequenceMap>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const auto *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Auto))
            {
                const auto *autoEvent = static_cast<const AutomationEvent *>(event);

                auto *component = isOnOffTrack ?
                    this->createOnOffEventComponent(*autoEvent, *clip) :
                    this->createCurveEventComponent(*autoEvent, *clip);

                component->setEditable(editable);
                this->addAndMakeVisible(component);

                sequenceMap->sortedComponents.addSorted(*component, component);
                sequenceMap->eventsMap[*autoEvent] = component;
            }
        }

        for (int j = 0; j < sequenceMap->sortedComponents.size(); ++j)
        {
            auto *component = sequenceMap->sortedComponents.getUnchecked(j);
            auto *previousEventComponent = sequenceMap->sortedComponents[j - 1];
            auto *nextEventComponent = sequenceMap->sortedComponents[j + 1];

            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);

            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
            }

            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(component);
            }
        }

        this->applyEventsBounds(sequenceMap);
    }
}

void AutomationEditor::applyEventBounds(EventComponentBase *c)
{
    c->setFloatBounds(this->getEventBounds(c->getEvent(), c->getClip()));
    c->updateChildrenBounds();
}

void AutomationEditor::applyEventsBounds(SequenceMap *map)
{
    for (auto *component : map->sortedComponents)
    {
        component->setFloatBounds(this->getEventBounds(component->getEvent(), component->getClip()));
    }

    // helpers and connectors depend on event component positions,
    // so update them after repositioning all event components
    for (auto *component : map->sortedComponents)
    {
        component->updateChildrenBounds();
    }
}

AutomationEditor::EventComponentBase *AutomationEditor::createCurveEventComponent(const AutomationEvent &event, const Clip &clip)
{
    return new AutomationCurveEventComponent(*this, event, clip);
}

AutomationEditor::EventComponentBase *AutomationEditor::createOnOffEventComponent(const AutomationEvent &event, const Clip &clip)
{
    return new AutomationStepEventComponent(*this, event, clip);
}
