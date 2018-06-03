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
#include "ProjectTreeItem.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AutomationStepEventComponent.h"
#include "AutomationStepEventsConnector.h"
#include "MidiTrack.h"

AutomationStepsClipComponent::AutomationStepsClipComponent(ProjectTreeItem &project,
    MidiSequence *sequence, HybridRoll &roll, const Clip &clip) :
    ClipComponent(roll, clip),
    project(project),
    roll(roll),
    sequence(sequence)
{
    this->setAlwaysOnTop(true);
    this->setPaintingIsUnclipped(true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);
    this->setMouseCursor(MouseCursor::CopyingCursor);

    this->leadingConnector = new AutomationStepEventsConnector(nullptr, nullptr, DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
    this->addAndMakeVisible(this->leadingConnector);

    this->reloadTrack();
    
    this->project.addListener(this);
}

AutomationStepsClipComponent::~AutomationStepsClipComponent()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::mouseDown(const MouseEvent &e)
{
    const bool shouldAddTriggeredEvent = (! e.mods.isLeftButtonDown());
    this->insertNewEventAt(e, shouldAddTriggeredEvent);
}

void AutomationStepsClipComponent::resized()
{
    this->setVisible(false);
    
    // вместо одного updateSustainPedalComponent(с) -
    // во избежание глюков - скачала обновляем позиции
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        AutomationStepEventComponent *const c = this->eventComponents.getUnchecked(i);
        c->setRealBounds(this->getEventBounds(c));
    }
    
    // затем - зависимые элементы
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        AutomationStepEventComponent *const c = this->eventComponents.getUnchecked(i);
        c->updateConnector();
    }
    
    this->leadingConnector->resizeToFit(DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
    
    this->setVisible(true);
}

void AutomationStepsClipComponent::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
}

Rectangle<float> AutomationStepsClipComponent::getEventBounds(AutomationStepEventComponent *c) const
{
    const auto *seqence = c->getEvent().getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = c->getBeat() - seqence->getFirstBeat();
    return this->getEventBounds(beat, sequenceLength, c->isPedalDownEvent(), c->getAnchor());
}

static const float kComponentLengthInBeats = 0.5f;

Rectangle<float> AutomationStepsClipComponent::getEventBounds(float beat,
    float sequenceLength, bool isPedalDown, float anchor) const
{
    const float minWidth = 2.f;
    const float w = jmax(minWidth, float(this->getWidth()) * (kComponentLengthInBeats / sequenceLength));
    const float x = (float(this->getWidth()) * (beat / sequenceLength));
    return { x - (w * anchor), 0.f, w, float(this->getHeight()) };
}

//===----------------------------------------------------------------------===//
// Event Helpers
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::insertNewEventAt(const MouseEvent &e, bool shouldAddTriggeredEvent)
{
    const float sequenceLength = this->sequence->getLengthInBeats();
    const float w = float(this->getWidth()) * (kComponentLengthInBeats / sequenceLength);
    const float draggingBeat = this->getBeatByXPosition(int(e.x + w / 2));
    
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
                
                if (event->getBeat() < draggingBeat &&
                    nextEvent->getBeat() > draggingBeat)
                {
                    break;
                }
            }
            else
            {
                nextBeat = FLT_MAX;
            }
        }
        
        const float invertedCV = (1.f - prevEventCV);
        const float alignedBeat = jmin((nextBeat - kComponentLengthInBeats), jmax((prevBeat + kComponentLengthInBeats), draggingBeat));
        
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

void AutomationStepsClipComponent::removeEventIfPossible(const AutomationEvent &e)
{
    auto *autoSequence = static_cast<AutomationSequence *>(e.getSequence());
    if (autoSequence->size() > 1)
    {
        autoSequence->checkpoint();
        autoSequence->remove(e, true);
    }
}

AutomationStepEventComponent *AutomationStepsClipComponent::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;
    return isPositiveAndBelow(indexOfPrevious, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfPrevious) : nullptr;
}

AutomationStepEventComponent *AutomationStepsClipComponent::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;
    return isPositiveAndBelow(indexOfNext, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfNext) : nullptr;
}

float AutomationStepsClipComponent::getBeatByXPosition(int x) const
{
    const int xRoll = int(roundf(float(x) / float(this->getWidth()) * float(this->roll.getWidth())));
    return this->roll.getRoundBeatByXPosition(xRoll);
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationStepsClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(oldEvent);
        const AutomationEvent &newAutoEvent = static_cast<const AutomationEvent &>(newEvent);
        
        if (auto *component = this->eventsHash[autoEvent])
        {
            // update links and connectors
            this->eventComponents.sort(*component);
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
            
            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);
            
            this->updateEventComponent(component);
            component->repaint();
            
            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
                
                auto *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
                previousEventComponent->setPreviousNeighbour(oneMorePrevious);
                
                if (oneMorePrevious)
                { oneMorePrevious->setNextNeighbour(previousEventComponent); }
            }
            
            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(component);

                auto *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
                nextEventComponent->setNextNeighbour(oneMoreNext);
                
                if (oneMoreNext)
                { oneMoreNext->setPreviousNeighbour(nextEventComponent); }
            }
            
            this->eventsHash.erase(autoEvent);
            this->eventsHash[newAutoEvent] = component;
            
            if (indexOfSorted == 0 || indexOfSorted == 1)
            {
                this->leadingConnector->retargetAndUpdate(nullptr, this->eventComponents[0], DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
                // false - потому, что по умолчанию, с начала трека педаль не нажата
            }

            this->roll.triggerBatchRepaintFor(this);
        }
    }
}

void AutomationStepsClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(event);
        
        auto *component = new AutomationStepEventComponent(*this, autoEvent);
        this->addAndMakeVisible(component);
        
        // update links and connectors
        const int indexOfSorted = this->eventComponents.addSorted(*component, component);
        auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
        auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

        component->setNextNeighbour(nextEventComponent);
        component->setPreviousNeighbour(previousEventComponent);

        this->updateEventComponent(component);
        component->toFront(false);
        
        if (previousEventComponent)
        { previousEventComponent->setNextNeighbour(component); }

        if (nextEventComponent)
        { nextEventComponent->setPreviousNeighbour(component); }

        this->eventsHash[autoEvent] = component;
        
        if (indexOfSorted == 0)
        {
            this->leadingConnector->retargetAndUpdate(nullptr, this->eventComponents[0], DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
        }

        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationStepsClipComponent::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(event);
        
        if (auto *component = this->eventsHash[autoEvent])
        {
            //this->eventAnimator.fadeOut(component, 150);
            this->removeChildComponent(component);
            this->eventsHash.erase(autoEvent);
            
            // update links and connectors for neighbors
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

            if (previousEventComponent)
            { previousEventComponent->setNextNeighbour(nextEventComponent); }
            
            if (nextEventComponent)
            { nextEventComponent->setPreviousNeighbour(previousEventComponent); }
            
            this->eventComponents.removeObject(component, true);
            
            if (this->eventComponents.size() > 0)
            {
                this->leadingConnector->retargetAndUpdate(nullptr, this->eventComponents[0], DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
            }

            this->roll.triggerBatchRepaintFor(this);
        }
    }
}

void AutomationStepsClipComponent::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->repaint();
    }
}

void AutomationStepsClipComponent::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrack();
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

void AutomationStepsClipComponent::updateEventComponent(AutomationStepEventComponent *component) const
{
    component->setRealBounds(this->getEventBounds(component));
    component->updateConnector();
}

void AutomationStepsClipComponent::reloadTrack()
{
    //Logger::writeToLog("TriggersTrackMap::reloadSustainPedalTrack");
    
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->removeChildComponent(this->eventComponents.getUnchecked(i));
    }
    
    this->eventComponents.clear();
    this->eventsHash.clear();
    
    this->setVisible(false);
    
    for (int j = 0; j < this->sequence->size(); ++j)
    {
        MidiEvent *event = this->sequence->getUnchecked(j);
        
        if (AutomationEvent *autoEvent = dynamic_cast<AutomationEvent *>(event))
        {
            auto component = new AutomationStepEventComponent(*this, *autoEvent);
            this->addAndMakeVisible(component);
            
            // update links and connectors
            const int indexOfSorted = this->eventComponents.addSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
            
            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);

            //this->updateSustainPedalComponent(component); // double call? see resized() later
            
            if (previousEventComponent)
            { previousEventComponent->setNextNeighbour(component); }

            if (nextEventComponent)
            { nextEventComponent->setPreviousNeighbour(component); }

            this->eventsHash[*autoEvent] = component;
        }
    }
    
    if (this->eventComponents.size() > 0)
    {
        this->leadingConnector->retargetAndUpdate(nullptr, this->eventComponents[0], DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
    }
    
    this->roll.triggerBatchRepaintFor(this);
    this->setVisible(true);
}
