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
#include "AutomationCurveClipComponent.h"
#include "AutomationCurveEventComponent.h"
#include "AutomationCurveEventsConnector.h"
#include "AutomationCurveHelper.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "MidiTrack.h"

#if HELIO_DESKTOP
#   define TRACKMAP_TEMPO_EVENT_DIAMETER (12.f)
#   define TRACKMAP_TEMPO_HELPER_DIAMETER (8.f)
#elif HELIO_MOBILE
#   define TRACKMAP_TEMPO_EVENT_DIAMETER (24.f)
#   define TRACKMAP_TEMPO_HELPER_DIAMETER (20.f)
#endif

AutomationCurveClipComponent::AutomationCurveClipComponent(ProjectNode &project,
    MidiSequence *sequence, HybridRoll &roll, const Clip &clip) :
    ClipComponent(roll, clip),
    project(project),
    sequence(sequence),
    draggingEvent(nullptr),
    addNewEventMode(false)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);
       
    this->reloadTrack();
    
    this->project.addListener(this);
}

AutomationCurveClipComponent::~AutomationCurveClipComponent()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::mouseDown(const MouseEvent &e)
{
    if (!this->project.getEditMode().forcesAddingEvents())
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
    if (!this->project.getEditMode().forcesAddingEvents())
    {
        ClipComponent::mouseDrag(e);
        return;
    }

    if (this->draggingEvent)
    {
        if (this->draggingEvent->isDragging())
        {
            this->draggingEvent->mouseDrag(e.getEventRelativeTo(this->draggingEvent));
        }
        else
        {
            this->draggingEvent->mouseDown(e.getEventRelativeTo(this->draggingEvent));
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
        }
    }
}

void AutomationCurveClipComponent::mouseUp(const MouseEvent &e)
{
    if (!this->project.getEditMode().forcesAddingEvents())
    {
        ClipComponent::mouseUp(e);
        return;
    }

    if (this->draggingEvent != nullptr)
    {
        this->draggingEvent->mouseUp(e.getEventRelativeTo(this->draggingEvent));
        this->setMouseCursor(MouseCursor::CopyingCursor);
        this->draggingEvent = nullptr;
    }
}

void AutomationCurveClipComponent::resized()
{
    this->setVisible(false);
    
    // во избежание глюков - сначала обновляем позиции
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        AutomationCurveEventComponent *const c = this->eventComponents.getUnchecked(i);
        c->setBounds(this->getEventBounds(c));
    }
    
    // затем - зависимые элементы
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        auto *c = this->eventComponents.getUnchecked(i);
        c->updateConnector();
        c->updateHelper();
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
    float draggingValue = 0;
    float draggingBeat = 0.f;
    this->getRowsColsByMousePosition(e.x - int(this->getEventDiameter() / 2),
                                     e.y - int(this->getEventDiameter() / 2),
                                     draggingValue, draggingBeat);
    
    auto *autoSequence = static_cast<AutomationSequence *>(this->sequence.get());
    this->addNewEventMode = true;
    autoSequence->checkpoint();
    AutomationEvent event(autoSequence, draggingBeat, draggingValue);
    autoSequence->insert(event, true);
}

void AutomationCurveClipComponent::removeEventIfPossible(const AutomationEvent &e)
{
    auto *autoSequence = static_cast<AutomationSequence *>(e.getSequence());
    
    if (autoSequence->size() > 1)
    {
        autoSequence->checkpoint();
        autoSequence->remove(e, true);
    }
}

float AutomationCurveClipComponent::getEventDiameter() const
{
    return TRACKMAP_TEMPO_EVENT_DIAMETER;
}

float AutomationCurveClipComponent::getHelperDiameter() const
{
    return TRACKMAP_TEMPO_HELPER_DIAMETER;
}

int AutomationCurveClipComponent::getAvailableHeight() const
{
    return this->getHeight();
}

Rectangle<int> AutomationCurveClipComponent::getEventBounds(AutomationCurveEventComponent *event) const
{
    const auto *seqence = event->getEvent().getSequence();
    const float sequenceLength = seqence->getLengthInBeats();
    const float beat = event->getBeat() - seqence->getFirstBeat();
    return this->getEventBounds(beat, sequenceLength, event->getControllerValue());
}

Rectangle<int> AutomationCurveClipComponent::getEventBounds(float beat,
    float sequenceLength, double controllerValue) const
{
    const int diameter = int(this->getEventDiameter());
    const int x = int(float(this->getWidth()) * (beat / sequenceLength));
    const int fullh = this->getAvailableHeight();
    int y = int((1.0 - controllerValue) * fullh); // upside down flip
    return { x - (diameter / 2), y - (diameter / 2), diameter, diameter };
}

void AutomationCurveClipComponent::getRowsColsByMousePosition(int x, int y, float &targetValue, float &targetBeat) const
{
    const int radius = int(this->getEventDiameter() / 2.f);
    const int xRoll = this->getX() + x + radius;

    targetBeat = this->roll.getRoundBeatByXPosition(xRoll) - this->clip.getBeat();
    targetValue = float(this->getAvailableHeight() - y - radius) / float(this->getAvailableHeight()); // flip upside down
    targetValue = jlimit(0.f, 1.f, targetValue);
}

AutomationCurveEventComponent *AutomationCurveClipComponent::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;
    return isPositiveAndBelow(indexOfPrevious, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfPrevious) : nullptr;
}

AutomationCurveEventComponent *AutomationCurveClipComponent::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;
    return isPositiveAndBelow(indexOfNext, this->eventComponents.size()) ?
        this->eventComponents.getUnchecked(indexOfNext) : nullptr;
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AutomationCurveClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
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
            //component->setVisible(false);
            this->updateCurveComponent(component);
            //component->setVisible(true);
            
            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
                
                if (auto *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1))
                {
                    oneMorePrevious->setNextNeighbour(previousEventComponent);
                }
            }
            
            if (nextEventComponent)
            {
                nextEventComponent->setNextNeighbour(this->getNextEventComponent(indexOfSorted + 1));
            }
            
            this->eventsHash.erase(autoEvent);
            this->eventsHash[newAutoEvent] = component;
            
            this->roll.triggerBatchRepaintFor(this);
        }
    }
}

void AutomationCurveClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() == this->sequence)
    {
        const AutomationEvent &autoEvent = static_cast<const AutomationEvent &>(event);
        
        auto *component = new AutomationCurveEventComponent(*this, autoEvent);
        this->addAndMakeVisible(component);

        // update links and connectors
        const int indexOfSorted = this->eventComponents.addSorted(*component, component);
        auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
        auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

        component->setNextNeighbour(nextEventComponent);
        this->updateCurveComponent(component);
        component->toFront(false);
        //this->eventAnimator.fadeIn(component, 150);
        
        if (previousEventComponent)
        {
            previousEventComponent->setNextNeighbour(component);
        }

        this->eventsHash[autoEvent] = component;
                
        if (this->addNewEventMode)
        {
            this->draggingEvent = component;
            this->addNewEventMode = false;
        }

        this->roll.triggerBatchRepaintFor(this);
    }
}

void AutomationCurveClipComponent::onRemoveMidiEvent(const MidiEvent &event)
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
            {
                previousEventComponent->setNextNeighbour(nextEventComponent);
            }
            
            this->eventComponents.removeObject(component, true);
            
            this->roll.triggerBatchRepaintFor(this);
        }
    }
}

void AutomationCurveClipComponent::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->sequence != nullptr && track->getSequence() == this->sequence)
    {
        this->repaint();
    }
}

void AutomationCurveClipComponent::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    if (this->sequence != nullptr)
    {
        this->reloadTrack();
    }
}

void AutomationCurveClipComponent::onAddTrack(MidiTrack *const track)
{
    // TODO remove?
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

void AutomationCurveClipComponent::updateCurveComponent(AutomationCurveEventComponent *component)
{
    component->setBounds(this->getEventBounds(component));
    component->updateConnector();
    component->updateHelper();
}

void AutomationCurveClipComponent::reloadTrack()
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
        MidiEvent *event = this->sequence->getUnchecked(j);
        
        if (auto *autoEvent = dynamic_cast<AutomationEvent *>(event))
        {
            auto *component = new AutomationCurveEventComponent(*this, *autoEvent);
            this->addAndMakeVisible(component);
            
            // update links and connectors
            const int indexOfSorted = this->eventComponents.addSorted(*component, component);
            auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
            
            component->setNextNeighbour(nextEventComponent);
            //this->updateCurveComponent(component);
            component->toFront(false);
            
            if (previousEventComponent)
            {
                previousEventComponent->setNextNeighbour(component);
            }
            
            this->eventsHash[*autoEvent] = component;
        }
    }

    this->resized(); // Re-calculates children bounds
    this->roll.triggerBatchRepaintFor(this);
    this->setVisible(true);
}
