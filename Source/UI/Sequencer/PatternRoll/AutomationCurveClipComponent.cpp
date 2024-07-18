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
#include "AutomationCurveClipComponent.h"
#include "AutomationCurveEventsConnector.h"
#include "AutomationCurveEventComponent.h"
#include "AutomationCurveHelper.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "PatternRoll.h"
#include "MidiTrack.h"
#include "TempoDialog.h"
#include "Icons.h"

AutomationCurveClipComponent::AutomationCurveClipComponent(ProjectNode &project,
    MidiSequence *sequence, RollBase &roll, const Clip &clip) :
    ClipComponent(roll, clip),
    project(project),
    sequence(sequence)
{
    this->setPaintingIsUnclipped(true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);

    this->reloadTrack();

    this->project.addListener(this);
}

AutomationCurveClipComponent::~AutomationCurveClipComponent()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// AutomationEditorBase
//===----------------------------------------------------------------------===//

Colour AutomationCurveClipComponent::getColour(const AutomationEvent &) const
{
    return this->getEventColour();
}

float AutomationCurveClipComponent::getBeatByPosition(int x, const Clip &clip) const
{
    return this->roll.getRoundBeatSnapByXPosition(this->getX() + x) - clip.getBeat();
}

void AutomationCurveClipComponent::getBeatValueByPosition(int x, int y,
    const Clip &clip, float &targetValue, float &targetBeat) const
{
    targetBeat = this->getBeatByPosition(x, clip);
    targetValue = float(this->getHeight() - y) / float(this->getHeight()); // flipped upside down
    targetValue = jlimit(0.f, 1.f, targetValue);
}

Rectangle<float> AutomationCurveClipComponent::getEventBounds(const AutomationEvent &event, const Clip &clip) const
{
    const auto *seqence = event.getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = event.getBeat() - seqence->getFirstBeat();
    return this->getEventBounds(beat, sequenceLength, event.getControllerValue());
}

Rectangle<float> AutomationCurveClipComponent::getEventBounds(float beat,
    float sequenceLength, double controllerValue) const
{
    constexpr auto diameter = AutomationCurveClipComponent::eventComponentDiameter;
    const float x = roundf(float(this->getWidth()) * (beat / jmax(1.f, sequenceLength)));
    const float y = roundf(float(1.0 - controllerValue) * float(this->getHeight())); // flipped upside down
    return { x - (diameter / 2.f), y - (diameter / 2.f), diameter, diameter };
}

bool AutomationCurveClipComponent::hasEditMode(RollEditMode::Mode mode) const noexcept
{
    return this->project.getEditMode().isMode(mode);
}

bool AutomationCurveClipComponent::isMultiTouchEvent(const MouseEvent &e) const noexcept
{
    return this->roll.isMultiTouchEvent(e);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::mouseDoubleClick(const MouseEvent &e)
{
    if (this->sequence->getTrack()->isTempoTrack())
    {
        this->getRoll().postCommandMessage(CommandIDs::TrackSetOneTempo);
    }
}

void AutomationCurveClipComponent::mouseDown(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (!this->project.getEditMode().forcesAddingEvents(e.mods))
    {
        ClipComponent::mouseDown(e);
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        this->insertNewEventAt(e);
    }
}

void AutomationCurveClipComponent::mouseDrag(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (!this->project.getEditMode().forcesAddingEvents(e.mods))
    {
        ClipComponent::mouseDrag(e);
        return;
    }

    if (this->draggingEvent != nullptr)
    {
        if (this->draggingEvent->isInEditMode())
        {
            this->draggingEvent->mouseDrag(e.getEventRelativeTo(this->draggingEvent));
        }
        else
        {
            const auto relativeEvent = e.getEventRelativeTo(this->draggingEvent);
            const auto cursor = this->draggingEvent->startEditingNewEvent(relativeEvent);
            this->setMouseCursor(cursor);
        }
    }
}

void AutomationCurveClipComponent::mouseUp(const MouseEvent &e)
{
    if (!this->project.getEditMode().forcesAddingEvents(e.mods))
    {
        ClipComponent::mouseUp(e);
        return;
    }

    if (this->draggingEvent != nullptr)
    {
        if (this->draggingEvent->isInEditMode())
        {
            this->draggingEvent->mouseUp(e.getEventRelativeTo(this->draggingEvent));
            this->setMouseCursor(Icons::getCopyingCursor());
        }

        this->draggingEvent = nullptr;
    }
}

void AutomationCurveClipComponent::resized()
{
    this->setVisible(false);
    
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        auto *c = this->eventComponents.getUnchecked(i);
        c->setFloatBounds(this->getEventBounds(c->getEvent(), this->clip));
    }
    
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        auto *c = this->eventComponents.getUnchecked(i);
        c->updateChildrenBounds();
    }
    
    this->setVisible(true);
}

void AutomationCurveClipComponent::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
}

//===----------------------------------------------------------------------===//
// Event Helpers
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::insertNewEventAt(const MouseEvent &e)
{
    float draggingValue = 0.f;
    float draggingBeat = 0.f;
    constexpr auto cursorOffset = 3;
    this->getBeatValueByPosition(e.x + cursorOffset, e.y, this->clip, draggingValue, draggingBeat);
    
    const auto shouldShowEditingDialog =
        sequence->getTrack()->isTempoTrack() && e.mods.isAnyModifierKeyDown();
    this->dragNewEventMode = !shouldShowEditingDialog;

    auto *sequence = static_cast<AutomationSequence *>(this->sequence.get());
    sequence->checkpoint();

    // try to snap to the closest event's value, if the click isn't too far away
    float snappedValue = draggingValue;
    {
        float minDistance = FLT_MAX;
        AutomationEditorBase::EventComponentBase *closestComponent = nullptr;

        for (int i = 0; i < this->eventComponents.size(); ++i)
        {
            auto *component = this->eventComponents.getUnchecked(i);
            // ignore the clip's beat, it is already substracted from draggingBeat
            const auto beatDistance = fabs(component->getEvent().getBeat() - draggingBeat);
            if (minDistance > beatDistance)
            {
                closestComponent = component;
                minDistance = beatDistance;
            }
        }

        if (closestComponent != nullptr &&
            fabs(closestComponent->getEvent().getControllerValue() - snappedValue) < 0.1f)
        {
            snappedValue = closestComponent->getEvent().getControllerValue();
        }
    }

    const AutomationEvent event(sequence, draggingBeat, snappedValue);
    sequence->insert(event, true);

    if (shouldShowEditingDialog)
    {
        auto dialog = make<TempoDialog>(event.getControllerValueAsBPM());
        dialog->onOk = [event](int newBpmValue)
        {
            auto *sequence = static_cast<AutomationSequence *>(event.getSequence());
            sequence->change(event, event.withTempoBpm(newBpmValue), true);
        };

        App::showModalComponent(move(dialog));
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(oldEvent);
    const auto &newAutoEvent = static_cast<const AutomationEvent &>(newEvent);

    if (auto *component = this->eventsMap[autoEvent])
    {
        this->eventComponents.sort(*component);
        const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
        auto *previousEventComponent = this->eventComponents[indexOfSorted - 1];
        auto *nextEventComponent = this->eventComponents[indexOfSorted + 1];
            
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
        component->setFloatBounds(this->getEventBounds(newAutoEvent, this->clip));
        component->updateChildrenBounds();
            
        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(component);
        }
            
        if (nextEventComponent)
        {
            nextEventComponent->setPreviousNeighbour(component);
        }
            
        this->eventsMap.erase(autoEvent);
        this->eventsMap[newAutoEvent] = component;
            
        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationCurveClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(event);

    auto *component = new AutomationCurveEventComponent(*this, autoEvent, this->clip);
    this->addAndMakeVisible(component);

    // update links and connectors
    const int indexOfSorted = this->eventComponents.addSorted(*component, component);
    auto *previousEventComponent = this->eventComponents[indexOfSorted - 1];
    auto *nextEventComponent = this->eventComponents[indexOfSorted + 1];

    component->setNextNeighbour(nextEventComponent);
    component->setPreviousNeighbour(previousEventComponent);
    component->setFloatBounds(this->getEventBounds(autoEvent, this->clip));
    component->updateChildrenBounds();
        
    if (previousEventComponent)
    {
        previousEventComponent->setNextNeighbour(component);
    }

    if (nextEventComponent)
    {
        nextEventComponent->setPreviousNeighbour(component);
    }

    this->eventsMap[autoEvent] = component;

    if (this->dragNewEventMode)
    {
        this->draggingEvent = component;
        this->dragNewEventMode = false;
    }

    this->roll.triggerBatchRepaintFor(this);
}

void AutomationCurveClipComponent::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(event);
        
    if (auto *component = this->eventsMap[autoEvent])
    {
        //this->eventAnimator.fadeOut(component, Globals::UI::fadeOutShort);
        this->removeChildComponent(component);
        this->eventsMap.erase(autoEvent);
            
        // update links and connectors for neighbors
        const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
        auto *previousEventComponent = this->eventComponents[indexOfSorted - 1];
        auto *nextEventComponent = this->eventComponents[indexOfSorted + 1];
            
        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(nextEventComponent);
        }

        if (nextEventComponent)
        {
            nextEventComponent->setPreviousNeighbour(previousEventComponent);
        }

        this->eventComponents.removeObject(component, true);
            
        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationCurveClipComponent::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->updateColours();

        for (auto *component : this->eventComponents)
        {
            component->updateColour();
        }

        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationCurveClipComponent::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    if (this->sequence != nullptr)
    {
        this->reloadTrack();
    }
}

void AutomationCurveClipComponent::onAddTrack(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

void AutomationCurveClipComponent::onRemoveTrack(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::reloadTrack()
{
    for (auto *component : this->eventComponents)
    {
        this->removeChildComponent(component);
    }
    
    this->eventComponents.clear();
    this->eventsMap.clear();
    
    this->setVisible(false);
    
    for (int i = 0; i < this->sequence->size(); ++i)
    {
        jassert(this->sequence->getUnchecked(i)->isTypeOf(MidiEvent::Type::Auto));
        if (this->sequence->getUnchecked(i)->isTypeOf(MidiEvent::Type::Auto))
        {
            auto *autoEvent = static_cast<AutomationEvent *>(this->sequence->getUnchecked(i));
            auto *component = new AutomationCurveEventComponent(*this, *autoEvent, this->clip);

            this->addAndMakeVisible(component);

            this->eventComponents.addSorted(*component, component);
            this->eventsMap[*autoEvent] = component;
        }
    }

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        auto *component = this->eventComponents.getUnchecked(i);
        auto *previousEventComponent = this->eventComponents[i - 1];
        auto *nextEventComponent = this->eventComponents[i + 1];

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

    this->resized(); // Re-calculates children bounds
    this->roll.triggerBatchRepaintFor(this);
    this->setVisible(true);
}
