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
#include "AutomationStepsSequenceMap.h"
#include "ProjectTreeItem.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AutomationStepEventComponent.h"
#include "AutomationStepEventsConnector.h"
#include "MidiTrack.h"

#define DEFAULT_TRACKMAP_HEIGHT 16

AutomationStepsSequenceMap::AutomationStepsSequenceMap(ProjectTreeItem &parentProject, HybridRoll &parentRoll, WeakReference<MidiSequence> sequence) :
    project(parentProject),
    roll(parentRoll),
    sequence(sequence),
    projectFirstBeat(0.f),
    projectLastBeat(16.f),
    rollFirstBeat(0.f),
    rollLastBeat(16.f)
{
    this->setAlwaysOnTop(true);
    this->setPaintingIsUnclipped(true);

    this->leadingConnector = new AutomationStepEventsConnector(nullptr, nullptr, DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
    this->addAndMakeVisible(this->leadingConnector);
    
    this->setMouseCursor(MouseCursor::CopyingCursor);
    
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->reloadTrack();
    
    this->project.addListener(this);

    this->setSize(1, DEFAULT_TRACKMAP_HEIGHT);
}

AutomationStepsSequenceMap::~AutomationStepsSequenceMap()
{
    this->project.removeListener(this);
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepsSequenceMap::mouseDown(const MouseEvent &e)
{
    const bool shouldAddTriggeredEvent = (! e.mods.isLeftButtonDown());
    this->insertNewEventAt(e, shouldAddTriggeredEvent);
}

void AutomationStepsSequenceMap::resized()
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

void AutomationStepsSequenceMap::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
}


Rectangle<float> AutomationStepsSequenceMap::getEventBounds(AutomationStepEventComponent *c) const
{
    return this->getEventBounds(c->getBeat(),
                                c->isPedalDownEvent(),
                                c->getAnchor());
}

static const float kComponentLengthInBeats = 0.5f;

Rectangle<float> AutomationStepsSequenceMap::getEventBounds(float targetBeat, bool isPedalDown, float anchor) const
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);
    
    const float beat = (targetBeat - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);
    
    const float x = (mapWidth * (beat / projectLengthInBeats));

    const float minWidth = 2.f;
    const float w = jmax(minWidth, (mapWidth * (kComponentLengthInBeats / projectLengthInBeats)));
    
    return Rectangle<float>(x - (w * anchor), 0.f, w, float(this->getHeight()));
}


//===----------------------------------------------------------------------===//
// Event Helpers
//===----------------------------------------------------------------------===//

void AutomationStepsSequenceMap::insertNewEventAt(const MouseEvent &e, bool shouldAddTriggeredEvent)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);
    const float w = mapWidth * (kComponentLengthInBeats / projectLengthInBeats);
    const float draggingBeat = this->getBeatByXPosition(int(e.x + w / 2));
    
    if (AutomationSequence *activeAutoLayer =
        dynamic_cast<AutomationSequence *>(this->sequence.get()))
    {
        const AutomationEvent *firstEvent = static_cast<AutomationEvent *>(activeAutoLayer->getUnchecked(0));
        float prevEventCV = firstEvent->getControllerValue();
        float prevBeat = -FLT_MAX;
        float nextBeat = FLT_MAX;
        
        for (int i = 0; i < activeAutoLayer->size(); ++i)
        {
            const AutomationEvent *event = static_cast<AutomationEvent *>(activeAutoLayer->getUnchecked(i));
            prevEventCV = event->getControllerValue();
            prevBeat = event->getBeat();
            
            if (i < (activeAutoLayer->size() - 1))
            {
                const AutomationEvent *nextEvent = static_cast<AutomationEvent *>(activeAutoLayer->getUnchecked(i + 1));
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
        
        activeAutoLayer->checkpoint();
        AutomationEvent event(activeAutoLayer, alignedBeat, invertedCV);
        activeAutoLayer->insert(event, true);
        
        if (shouldAddTriggeredEvent)
        {
            AutomationEvent triggerEvent(activeAutoLayer, alignedBeat + 0.75f, (1.f - invertedCV));
            activeAutoLayer->insert(triggerEvent, true);
        }
    }
}

void AutomationStepsSequenceMap::removeEventIfPossible(const AutomationEvent &e)
{
    AutomationSequence *autoLayer = static_cast<AutomationSequence *>(e.getSequence());
    
    if (autoLayer->size() > 1)
    {
        autoLayer->checkpoint();
        autoLayer->remove(e, true);
    }
}

AutomationStepEventComponent *AutomationStepsSequenceMap::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;
    
    return
        isPositiveAndBelow(indexOfPrevious, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfPrevious) :
        nullptr;
}

AutomationStepEventComponent *AutomationStepsSequenceMap::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;
    
    return
        isPositiveAndBelow(indexOfNext, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfNext) :
        nullptr;
}

float AutomationStepsSequenceMap::getBeatByXPosition(int x) const
{
    const int xRoll = int(roundf(float(x) / float(this->getWidth()) * float(this->roll.getWidth())));
    const float targetBeat = this->roll.getRoundBeatByXPosition(xRoll);
    return jmin(jmax(targetBeat, this->rollFirstBeat), this->rollLastBeat);
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationStepsSequenceMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(oldEvent);
        const AutomationEvent &newAutoEvent = static_cast<const AutomationEvent &>(newEvent);
        
        if (AutomationStepEventComponent *component = this->eventsHash[autoEvent])
        {
            // update links and connectors
            this->eventComponents.sort(*component);
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            AutomationStepEventComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            AutomationStepEventComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
            
            component->setNextNeighbour(nextEventComponent);
            component->setPreviousNeighbour(previousEventComponent);
            
            this->updateEventComponent(component);
            component->repaint();
            
            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
                
                AutomationStepEventComponent *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
                previousEventComponent->setPreviousNeighbour(oneMorePrevious);
                
                if (oneMorePrevious)
                { oneMorePrevious->setNextNeighbour(previousEventComponent); }
            }
            
            if (nextEventComponent)
            {
                nextEventComponent->setPreviousNeighbour(component);

                AutomationStepEventComponent *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
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
        }
    }
}

void AutomationStepsSequenceMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(event);
        
        auto component = new AutomationStepEventComponent(*this, autoEvent);
        this->addAndMakeVisible(component);
        
        // update links and connectors
        const int indexOfSorted = this->eventComponents.addSorted(*component, component);
        AutomationStepEventComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
        AutomationStepEventComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
        
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
    }
}

void AutomationStepsSequenceMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(event);
        
        if (AutomationStepEventComponent *component = this->eventsHash[autoEvent])
        {
            //this->eventAnimator.fadeOut(component, 150);
            this->removeChildComponent(component);
            this->eventsHash.erase(autoEvent);
            
            // update links and connectors for neighbors
            const int indexOfSorted = this->eventComponents.indexOfSorted(*component, component);
            AutomationStepEventComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            AutomationStepEventComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
            
            if (previousEventComponent)
            { previousEventComponent->setNextNeighbour(nextEventComponent); }
            
            if (nextEventComponent)
            { nextEventComponent->setPreviousNeighbour(previousEventComponent); }
            
            this->eventComponents.removeObject(component, true);
            
            if (this->eventComponents.size() > 0)
            {
                this->leadingConnector->retargetAndUpdate(nullptr, this->eventComponents[0], DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE);
            }
        }
    }
}

void AutomationStepsSequenceMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->repaint();
    }
}

void AutomationStepsSequenceMap::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrack();
}

void AutomationStepsSequenceMap::onAddTrack(MidiTrack *const track)
{
    // TODO remove?
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

void AutomationStepsSequenceMap::onRemoveTrack(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->reloadTrack();
    }
}

void AutomationStepsSequenceMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat ||
        this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
        this->resized();
    }
}

void AutomationStepsSequenceMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    this->resized();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void AutomationStepsSequenceMap::updateEventComponent(AutomationStepEventComponent *component)
{
    component->setRealBounds(this->getEventBounds(component));
    component->updateConnector();
}

void AutomationStepsSequenceMap::reloadTrack()
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
            AutomationStepEventComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            AutomationStepEventComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
            
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
    
    this->resized();
    this->setVisible(true);
}
