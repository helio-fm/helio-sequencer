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

const Colour &AutomationStepsClipComponent::getColour(const AutomationEvent &event) const
{
    return this->getEventColour();
}

Rectangle<float> AutomationStepsClipComponent::getEventBounds(const AutomationEvent &event, const Clip &clip) const
{
    const auto *seqence = event.getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = event.getBeat() - seqence->getFirstBeat();
    return this->getEventBounds(beat, sequenceLength, event.getControllerValue());
}

float AutomationStepsClipComponent::getBeatByPosition(int x, const Clip &clip) const
{
    const int xRoll = this->getX() + x;
    return this->roll.getRoundBeatSnapByXPosition(xRoll) - clip.getBeat();
}

void AutomationStepsClipComponent::getBeatValueByPosition(int x, int y,
    const Clip &clip, float &value, float &beat) const
{
    assert(false); // all child components just use getBeatByPosition
    beat = this->getBeatByPosition(x, clip);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::mouseDown(const MouseEvent &e)
{
    if (!this->project.getEditMode().forcesAddingEvents(e.mods))
    {
        ClipComponent::mouseDown(e);
        return;
    }

    const bool shouldAddTriggeredEvent = !e.mods.isLeftButtonDown();
    this->insertNewEventAt(e, shouldAddTriggeredEvent);
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

Rectangle<float> AutomationStepsClipComponent::getEventBounds(float beat,
    float sequenceLength, bool isPedalDown) const
{
    const float minWidth = 2.f;
    const float w = jmax(minWidth, float(this->getWidth()) *
        (AutomationStepEventComponent::minLengthInBeats / sequenceLength));

    const float x = (float(this->getWidth()) * (beat / sequenceLength));
    return { x - w + AutomationStepEventComponent::pointOffset, 0.f, w, float(this->getHeight()) };
}

//===----------------------------------------------------------------------===//
// Event Helpers
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::insertNewEventAt(const MouseEvent &e, bool shouldAddTriggeredEvent)
{
    const float sequenceLength = this->sequence->getLengthInBeats();
    const float w = float(this->getWidth()) *
        (AutomationStepEventComponent::minLengthInBeats / sequenceLength);
 
    const float draggingBeat = this->getBeatByPosition(int(e.x + w / 2), this->clip);
    
    if (auto *autoSequence = dynamic_cast<AutomationSequence *>(this->sequence.get()))
    {
        const auto *firstEvent = static_cast<AutomationEvent *>(autoSequence->getUnchecked(0));
        float prevEventCV = firstEvent->getControllerValue();
        float prevBeat = -FLT_MAX;
        float nextBeat = FLT_MAX;
        
        for (int i = 0; i < autoSequence->size(); ++i)
        {
            const auto *event = static_cast<AutomationEvent *>(autoSequence->getUnchecked(i));
            prevEventCV = event->getControllerValue();
            prevBeat = event->getBeat();
            
            if (i < (autoSequence->size() - 1))
            {
                const auto *nextEvent = static_cast<AutomationEvent *>(autoSequence->getUnchecked(i + 1));
                nextBeat = nextEvent->getBeat();
                
                if (event->getBeat() < draggingBeat && nextEvent->getBeat() > draggingBeat)
                {
                    break;
                }
            }
            else
            {
                nextBeat = FLT_MAX;
            }
        }
        
        const float invertedCV = 1.f - prevEventCV;
        const float alignedBeat = jmin((nextBeat - AutomationStepEventComponent::minLengthInBeats),
            jmax((prevBeat + AutomationStepEventComponent::minLengthInBeats), draggingBeat));
        
        autoSequence->checkpoint();
        AutomationEvent event(autoSequence, alignedBeat, invertedCV);
        autoSequence->insert(event, true);
        
        if (shouldAddTriggeredEvent)
        {
            AutomationEvent triggerEvent(autoSequence, alignedBeat + 0.75f, (1.f - invertedCV));
            autoSequence->insert(triggerEvent, true);
        }
    }
}

AutomationEditorBase::EventComponentBase *AutomationStepsClipComponent::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;
    return isPositiveAndBelow(indexOfPrevious, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfPrevious) : nullptr;
}

AutomationEditorBase::EventComponentBase *AutomationStepsClipComponent::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;
    return isPositiveAndBelow(indexOfNext, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfNext) : nullptr;
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() == this->sequence)
    {
        const auto &autoEvent = static_cast<const AutomationEvent &>(oldEvent);
        const auto &newAutoEvent = static_cast<const AutomationEvent &>(newEvent);
        
        if (auto *component = this->eventsHash[autoEvent])
        {
            // update links and connectors
            this->eventComponents.sort(*component);
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
            
            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);
            component->setFloatBounds(this->getEventBounds(newAutoEvent, this->clip));
            component->updateChildrenBounds();
            component->repaint();
            
            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
                
                auto *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
                previousEventComponent->setPreviousNeighbour(oneMorePrevious);
                
                if (oneMorePrevious)
                {
                    oneMorePrevious->setNextNeighbour(previousEventComponent);
                }
            }
            
            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(component);

                auto *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
                nextEventComponent->setNextNeighbour(oneMoreNext);
                
                if (oneMoreNext)
                {
                    oneMoreNext->setPreviousNeighbour(nextEventComponent);
                }
            }
            
            this->eventsHash.erase(autoEvent);
            this->eventsHash[newAutoEvent] = component;
            
            this->roll.triggerBatchRepaintFor(this);
        }
    }
}

void AutomationStepsClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const auto &autoEvent = static_cast<const AutomationEvent &>(event);
        
        auto *component = new AutomationStepEventComponent(*this, autoEvent, this->clip);
        this->addAndMakeVisible(component);
        
        // update links and connectors
        const int indexOfSorted = this->eventComponents.addSorted(*component, component);
        auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
        auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

        component->setNextNeighbour(nextEventComponent);
        component->setPreviousNeighbour(previousEventComponent);
        component->setFloatBounds(this->getEventBounds(autoEvent, this->clip));
        component->updateChildrenBounds();
        component->toFront(false);
        
        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(component);
        }

        if (nextEventComponent)
        {
            nextEventComponent->setPreviousNeighbour(component);
        }

        this->eventsHash[autoEvent] = component;
        
        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationStepsClipComponent::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const auto &autoEvent = static_cast<const AutomationEvent &>(event);
        
        if (auto *component = this->eventsHash[autoEvent])
        {
            //this->eventAnimator.fadeOut(component, Globals::UI::fadeOutShort);
            this->removeChildComponent(component);
            this->eventsHash.erase(autoEvent);
            
            // update links and connectors for neighbors
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

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
}

void AutomationStepsClipComponent::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->updateColours();
        this->repaint();
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
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->removeChildComponent(this->eventComponents.getUnchecked(i));
    }
    
    this->eventComponents.clear();
    this->eventsHash.clear();
    
    this->setVisible(false);
    
    for (int j = 0; j < this->sequence->size(); ++j)
    {
        auto *event = this->sequence->getUnchecked(j);
        
        if (auto *autoEvent = dynamic_cast<AutomationEvent *>(event))
        {
            auto *component = new AutomationStepEventComponent(*this, *autoEvent, this->clip);
            this->addAndMakeVisible(component);
            
            // update links and connectors
            const int indexOfSorted = this->eventComponents.addSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
            
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

            this->eventsHash[*autoEvent] = component;
        }
    }

    this->resized(); // Re-calculates children bounds
    this->roll.triggerBatchRepaintFor(this);
    this->setVisible(true);
}
