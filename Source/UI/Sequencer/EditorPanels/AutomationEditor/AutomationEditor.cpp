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
#include "Lasso.h"
#include "ColourIDs.h"
#include "FineTuningValueIndicator.h"

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

    this->reloadTrackMap();

    this->project.addListener(this);
}

AutomationEditor::~AutomationEditor()
{
    this->project.removeListener(this);
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

    AUTO_EDITOR_BATCH_REPAINT_END
}

void AutomationEditor::mouseDown(const MouseEvent &e)
{
    if (this->roll->hasMultiTouch(e))
    {
        return;
    }

    // roll panning hack
    if (this->roll->isViewportDragEvent(e))
    {
        this->roll->mouseDown(e.getEventRelativeTo(this->roll));
        return;
    }

    //if (e.mods.isLeftButtonDown())
    //{
    //    this->dragHelper = make<VelocityLevelDraggingHelper>(*this);
    //    this->addAndMakeVisible(this->dragHelper.get());
    //    this->dragHelper->setStartPosition(e.position);
    //    this->dragHelper->setEndPosition(e.position);
    //}
}

void AutomationEditor::mouseDrag(const MouseEvent &e)
{
    if (this->roll->hasMultiTouch(e))
    {
        return;
    }

    if (this->roll->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll->mouseDrag(e.getEventRelativeTo(this->roll));
        return;
    }

    // todo: hand-drawing custom shapes
}

void AutomationEditor::mouseUp(const MouseEvent &e)
{
    // no multi-touch check here, need to exit the editing mode (if any) even in multi-touch
    //if (this->roll->hasMultiTouch(e)) { return; }

    if (this->roll->isViewportDragEvent(e))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll->mouseUp(e.getEventRelativeTo(this->roll));
        return;
    }

    // todo: hand-drawing custom shapes
}

void AutomationEditor::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    //if ...
    //{
    //   todo: hand-drawing custom shapes
    //}
    //else
    //{
    jassert(this->roll != nullptr);
    this->roll->mouseWheelMove(e.getEventRelativeTo(this->roll), wheel);
    //}
}

//===----------------------------------------------------------------------===//
// EditorPanelBase
//===----------------------------------------------------------------------===//

void AutomationEditor::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
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

        /*
        if (this->addNewEventMode)
        {
            this->draggingEvent = component;
            this->addNewEventMode = false;
        }
        */
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

    this->listeners.call(&Listener::onUpdateEventFilters);
}

void AutomationEditor::onChangeClip(const Clip &clip, const Clip &newClip)
{
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

    this->listeners.call(&Listener::onUpdateEventFilters);
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
    this->listeners.call(&Listener::onUpdateEventFilters);
}

void AutomationEditor::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->reloadTrackMap();
    this->listeners.call(&Listener::onUpdateEventFilters);
}

void AutomationEditor::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const AutomationSequence *>(track->getSequence())) { return; }

    AUTO_EDITOR_BATCH_REPAINT_START
    this->loadTrack(track);
    AUTO_EDITOR_BATCH_REPAINT_END

    this->listeners.call(&Listener::onUpdateEventFilters);
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

    this->listeners.call(&Listener::onUpdateEventFilters);
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
