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
#include "AutomationStepsClipComponent.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "RollBase.h"
#include "AutomationStepEventComponent.h"
#include "AutomationStepEventsConnector.h"
#include "MidiTrack.h"

AutomationStepsClipComponent::AutomationStepsClipComponent(ProjectNode &project,
    MidiSequence *sequence, RollBase &roll, const Clip &clip) :
    ClipComponent(roll, clip),
    project(project),
    sequence(sequence)
{
    this->setPaintingIsUnclipped(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);

    this->reloadTrack();

    this->project.addListener(this);
}

AutomationStepsClipComponent::~AutomationStepsClipComponent()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// AutomationEditorBase
//===----------------------------------------------------------------------===//

Colour AutomationStepsClipComponent::getColour(const AutomationEvent &event) const
{
    return this->getEventColour();
}

float AutomationStepsClipComponent::getBeatByPosition(int x, const Clip &clip) const
{
    return this->roll.getRoundBeatSnapByXPosition(this->getX() + x) - clip.getBeat();
}

void AutomationStepsClipComponent::getBeatValueByPosition(int x, int y,
    const Clip &clip, float &value, float &beat) const
{
    jassertfalse; // all child components just use getBeatByPosition
    beat = this->getBeatByPosition(x, clip);
}

Rectangle<float> AutomationStepsClipComponent::getEventBounds(const AutomationEvent &event, const Clip &clip) const
{
    const auto *seqence = event.getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = event.getBeat() - seqence->getFirstBeat();
    return this->getEventBounds(beat, sequenceLength, event.getControllerValue());
}

Rectangle<float> AutomationStepsClipComponent::getEventBounds(float beat,
    float sequenceLength, bool isPedalDown) const
{
    const float w = AutomationStepEventComponent::pointRadius * 4.f;
    const auto safeLength = jmax(1.f, sequenceLength);
    const float x = float(this->getWidth()) * (beat / safeLength);
    return { x - w + AutomationStepEventComponent::pointRadius, 0.f, w, float(this->getHeight()) };
}

bool AutomationStepsClipComponent::hasEditMode(RollEditMode::Mode mode) const noexcept
{
    return this->project.getEditMode().isMode(mode);
}

bool AutomationStepsClipComponent::isMultiTouchEvent(const MouseEvent &e) const noexcept
{
    return this->roll.isMultiTouchEvent(e);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::mouseDown(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
        return;
    }

    if (!this->project.getEditMode().forcesAddingEvents(e.mods))
    {
        ClipComponent::mouseDown(e);
        return;
    }

    const bool shouldAddPairedEvents = !e.mods.isLeftButtonDown() || e.source.isTouch();
    this->insertNewEventAt(e, shouldAddPairedEvents);
}

void AutomationStepsClipComponent::resized()
{
    this->setVisible(false);
    
    // connectors depend on these bounds, so they are updated later
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

void AutomationStepsClipComponent::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
}

//===----------------------------------------------------------------------===//
// Event Helpers
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::insertNewEventAt(const MouseEvent &e, bool shouldAddPairedEvents)
{
    constexpr auto minLength = AutomationStepEventComponent::minLengthInBeats;
    const float draggingBeat = this->getBeatByPosition(e.x, this->clip);

    if (auto *sequence = dynamic_cast<AutomationSequence *>(this->sequence.get()))
    {
        float prevBeat = -FLT_MAX;
        float nextBeat = FLT_MAX;
        float prevCV = Globals::Defaults::onOffControllerState;

        for (int i = 0; i < sequence->size(); ++i)
        {
            const auto *nextEvent = static_cast<AutomationEvent *>(sequence->getUnchecked(i));
            nextBeat = nextEvent->getBeat();
            
            if (prevBeat < draggingBeat && nextBeat > draggingBeat)
            {
                break;
            }

            prevCV = nextEvent->getControllerValue();
            prevBeat = nextBeat;
        }
        
        const float invertedCV = 1.f - prevCV;
        const float alignedBeat = jmin((nextBeat - minLength), jmax((prevBeat + minLength), draggingBeat));
        
        sequence->checkpoint();
        sequence->insert(AutomationEvent(sequence, alignedBeat, invertedCV), true);
        
        if (shouldAddPairedEvents)
        {
            sequence->insert(AutomationEvent(sequence, alignedBeat + minLength, (1.f - invertedCV)), true);
        }
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(oldEvent);
    const auto &newAutoEvent = static_cast<const AutomationEvent &>(newEvent);
        
    if (auto *component = this->eventsMap[autoEvent])
    {
        // update links and connectors
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
        component->repaint();
            
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

void AutomationStepsClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(event);
        
    auto *component = new AutomationStepEventComponent(*this, autoEvent, this->clip);
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
        
    this->roll.triggerBatchRepaintFor(this);
}

void AutomationStepsClipComponent::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() != this->sequence)
    {
        return;
    }

    const auto &autoEvent = static_cast<const AutomationEvent &>(event);
        
    if (auto *component = this->eventsMap[autoEvent])
    {
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

void AutomationStepsClipComponent::onChangeTrackProperties(MidiTrack *const track)
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

void AutomationStepsClipComponent::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    if (this->sequence != nullptr)
    {
        this->reloadTrack();
    }
}

void AutomationStepsClipComponent::onAddTrack(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

void AutomationStepsClipComponent::onRemoveTrack(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::reloadTrack()
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
        if (this->sequence->getUnchecked(i)->isTypeOf(MidiEvent::Type::Auto))
        {
            auto *autoEvent = static_cast<AutomationEvent *>(this->sequence->getUnchecked(i));
            auto *component = new AutomationStepEventComponent(*this, *autoEvent, this->clip);

            this->addAndMakeVisible(component);

            this->eventComponents.addSorted(*component, component);
            this->eventsMap[*autoEvent] = component;
        }
    }

    // update links and connectors
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
