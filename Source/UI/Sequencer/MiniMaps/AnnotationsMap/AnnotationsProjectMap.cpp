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
#include "AnnotationsProjectMap.h"
#include "ProjectNode.h"
#include "Transport.h"
#include "MidiSequence.h"
#include "ProjectTimeline.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AnnotationDialog.h"
#include "MainLayout.h"
#include "AnnotationLargeComponent.h"
#include "AnnotationSmallComponent.h"

AnnotationsProjectMap::AnnotationsProjectMap(ProjectNode &parentProject, HybridRoll &parentRoll, Type type) :
    project(parentProject),
    roll(parentRoll),
    projectFirstBeat(0.f),
    projectLastBeat(16.f), // non zero!
    rollFirstBeat(0.f),
    rollLastBeat(16.f),
    type(type)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(false);

    this->reloadTrackMap();
    
    this->project.addListener(this);
}

AnnotationsProjectMap::~AnnotationsProjectMap()
{
    this->project.removeListener(this);
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AnnotationsProjectMap::resized()
{
    this->setVisible(false);

    AnnotationComponent *previous = nullptr;

    for (int i = 0; i < this->annotationComponents.size(); ++i)
    {
        AnnotationComponent *current = this->annotationComponents.getUnchecked(i);

        if (previous != nullptr)
        {
            this->applyAnnotationBounds(previous, current);
        }

        if (i == (this->annotationComponents.size() - 1))
        {
            this->applyAnnotationBounds(current, nullptr);
        }

        previous = current;
    }

    this->setVisible(true);
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AnnotationsProjectMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Annotation))
    {
        const AnnotationEvent &annotation = static_cast<const AnnotationEvent &>(oldEvent);
        const AnnotationEvent &newAnnotation = static_cast<const AnnotationEvent &>(newEvent);

        if (AnnotationComponent *component = this->annotationsHash[annotation])
        {
            this->alignAnnotationComponent(component);

            this->annotationsHash.erase(annotation);
            this->annotationsHash[newAnnotation] = component;
        }
    }
}

void AnnotationsProjectMap::alignAnnotationComponent(AnnotationComponent *component)
{
    this->annotationComponents.sort(*component);
    const int indexOfSorted = this->annotationComponents.indexOfSorted(*component, component);
    AnnotationComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
    AnnotationComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    if (previousEventComponent)
    {
        this->applyAnnotationBounds(previousEventComponent, component);
        
        AnnotationComponent *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
        
        if (oneMorePrevious)
        { this->applyAnnotationBounds(oneMorePrevious, previousEventComponent); }
    }
    
    if (nextEventComponent)
    {
        AnnotationComponent *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
        this->applyAnnotationBounds(nextEventComponent, oneMoreNext);
    }
    
    component->updateContent();
    this->applyAnnotationBounds(component, nextEventComponent);
}

void AnnotationsProjectMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Annotation))
    {
        const AnnotationEvent &annotation = static_cast<const AnnotationEvent &>(event);

        auto *component = this->createComponent(annotation);
        this->addChildComponent(component);

        const int indexOfSorted = this->annotationComponents.addSorted(*component, component);
        AnnotationComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
        AnnotationComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));

        component->updateContent();
        this->applyAnnotationBounds(component, nextEventComponent);
        component->toFront(false);

        if (previousEventComponent)
        { this->applyAnnotationBounds(previousEventComponent, component); }

        this->annotationsHash[annotation] = component;

        component->setAlpha(0.f);
        const Rectangle<int> bounds(component->getBounds());
        component->setBounds(bounds.translated(0, -component->getHeight()));
        this->animator.animateComponent(component, bounds, 1.f, 250, false, 0.0, 0.0);
    }
}

void AnnotationsProjectMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Annotation))
    {
        const AnnotationEvent &annotation = static_cast<const AnnotationEvent &>(event);

        if (AnnotationComponent *component = this->annotationsHash[annotation])
        {
            this->animator.animateComponent(component,
                                            component->getBounds().translated(0, -component->getHeight()),
                                            0.f, 250, true, 0.0, 0.0);

            this->removeChildComponent(component);
            this->annotationsHash.erase(annotation);

            const int indexOfSorted = this->annotationComponents.indexOfSorted(*component, component);
            AnnotationComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            AnnotationComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));

            if (previousEventComponent)
            { this->applyAnnotationBounds(previousEventComponent, nextEventComponent); }

            this->annotationComponents.removeObject(component, true);
        }
    }
}

void AnnotationsProjectMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getAnnotations())
    {
        this->repaint();
    }
}

void AnnotationsProjectMap::onAddTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getAnnotations())
    {
        if (track->getSequence()->size() > 0)
        {
            this->reloadTrackMap();
        }
    }
}

void AnnotationsProjectMap::onRemoveTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getAnnotations())
    {
        for (int i = 0; i < track->getSequence()->size(); ++i)
        {
            const AnnotationEvent &annotation = 
                static_cast<const AnnotationEvent &>(*track->getSequence()->getUnchecked(i));

            if (AnnotationComponent *component = this->annotationsHash[annotation])
            {
                this->removeChildComponent(component);
                this->annotationsHash.erase(annotation);
                this->annotationComponents.removeObject(component, true);
            }
        }
    }
}

void AnnotationsProjectMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
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

void AnnotationsProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    this->resized();
}

void AnnotationsProjectMap::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrackMap();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void AnnotationsProjectMap::onAnnotationMoved(AnnotationComponent *nc) {}

void AnnotationsProjectMap::onAnnotationTapped(AnnotationComponent *nc)
{
    const AnnotationEvent *annotationUnderSeekCursor = nullptr;
    const ProjectTimeline *timeline = this->project.getTimeline();
    const auto annotations = timeline->getAnnotations()->getSequence();
    const double seekPosition = this->project.getTransport().getSeekPosition();

    for (int i = 0; i < annotations->size(); ++i)
    {
        if (AnnotationEvent *annotation = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i)))
        {
            const float seekBeat = this->roll.getBeatByTransportPosition(seekPosition);
            
            if (fabs(annotation->getBeat() - seekBeat) < 0.1)
            {
                annotationUnderSeekCursor = annotation;
                break;
            }
        }
    }

    const double newSeekPosition = this->roll.getTransportPositionByBeat(nc->getBeat());
    const bool wasPlaying = this->project.getTransport().isPlaying();

    if (wasPlaying)
    {
        this->project.getTransport().stopPlayback();
    }

    this->project.getTransport().seekToPosition(newSeekPosition);

    //if (wasPlaying)
    //{
    //    this->project.getTransport().startPlayback();
    //}

    // если аннотация уже выбрана - покажем ее меню
    if (annotationUnderSeekCursor == &nc->getEvent() && !wasPlaying)
    {
        this->showContextMenuFor(nc);
    }
}

void AnnotationsProjectMap::showContextMenuFor(AnnotationComponent *nc)
{
    if (! this->project.getTransport().isPlaying())
    {
        Component *dialog =
            AnnotationDialog::createEditingDialog(*this, nc->getEvent());
        App::Layout().showModalComponentUnowned(dialog);
    }
}

void AnnotationsProjectMap::alternateActionFor(AnnotationComponent *nc)
{
    // Selects everything within the range of this annotation
    this->annotationComponents.sort(*nc);
    const int indexOfSorted = this->annotationComponents.indexOfSorted(*nc, nc);
    AnnotationComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    const float startBeat = nc->getBeat();
    const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool shouldClearSelection = !isShiftPressed;
    
    this->roll.selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

float AnnotationsProjectMap::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll.getWidth()));
    const float targetBeat = this->roll.getRoundBeatByXPosition(xRoll);
    return jlimit(this->rollFirstBeat, this->rollLastBeat, targetBeat);
}

void AnnotationsProjectMap::reloadTrackMap()
{
    if (this->project.getTimeline() == nullptr)
    {
        return;
    }

    for (int i = 0; i < this->annotationComponents.size(); ++i)
    {
        this->removeChildComponent(this->annotationComponents.getUnchecked(i));
    }

    this->annotationComponents.clear();
    this->annotationsHash.clear();

    this->setVisible(false);

    MidiSequence *sequence =
        this->project.getTimeline()->getAnnotations()->getSequence();

    for (int j = 0; j < sequence->size(); ++j)
    {
        MidiEvent *event = sequence->getUnchecked(j);

        if (AnnotationEvent *annotation = dynamic_cast<AnnotationEvent *>(event))
        {
            auto *component = this->createComponent(*annotation);
            this->addAndMakeVisible(component);
            component->updateContent();

            this->annotationComponents.addSorted(*component, component);
            this->annotationsHash[*annotation] = component;
        }
    }

    this->resized();
    this->setVisible(true);
}

void AnnotationsProjectMap::applyAnnotationBounds(AnnotationComponent *nc, AnnotationComponent *nextOne)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));

    const float nextBeat = ((nextOne ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat);
    const float nextX = mapWidth * (nextBeat / projectLengthInBeats);

    const float minWidth = 10.f;
    const float widthMargin = 16.f;
    const float componentsPadding = 10.f;
    const float maxWidth = nextX - x;
    const float w = jmax(minWidth, jmin((maxWidth - componentsPadding), (nc->getTextWidth() + widthMargin)));

    nc->setRealBounds(Rectangle<float>(x, 0.f, w, float(nc->getHeight())));
}

AnnotationComponent *AnnotationsProjectMap::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;

    return
        isPositiveAndBelow(indexOfPrevious, this->annotationComponents.size()) ?
        this->annotationComponents.getUnchecked(indexOfPrevious) :
        nullptr;
}

AnnotationComponent *AnnotationsProjectMap::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;

    return
        isPositiveAndBelow(indexOfNext, this->annotationComponents.size()) ?
        this->annotationComponents.getUnchecked(indexOfNext) :
        nullptr;
}

AnnotationComponent *AnnotationsProjectMap::createComponent(const AnnotationEvent &event)
{
    switch (this->type)
    {
    case Large:
        return new AnnotationLargeComponent(*this, event);
    case Small:
        return new AnnotationSmallComponent(*this, event);
    default:
        return nullptr;
    }
}
