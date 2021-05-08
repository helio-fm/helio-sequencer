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
#include "ProjectTimeline.h"
#include "PlayerThread.h"
#include "RollBase.h"
#include "AnnotationDialog.h"
#include "AnnotationLargeComponent.h"
#include "AnnotationSmallComponent.h"

AnnotationsProjectMap::AnnotationsProjectMap(ProjectNode &parentProject, RollBase &parentRoll, Type type) :
    project(parentProject),
    roll(parentRoll),
    type(type)
{
    this->setAlwaysOnTop(true);
    this->setPaintingIsUnclipped(false);
    this->setInterceptsMouseClicks(false, true);

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
    AnnotationComponent *previous = nullptr;

    for (int i = 0; i < this->annotationComponents.size(); ++i)
    {
        auto *current = this->annotationComponents.getUnchecked(i);

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
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void AnnotationsProjectMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Type::Annotation))
    {
        const auto &annotation = static_cast<const AnnotationEvent &>(oldEvent);
        const auto &newAnnotation = static_cast<const AnnotationEvent &>(newEvent);

        if (auto *component = this->annotationsHash[annotation])
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
    
    if (auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted))
    {
        this->applyAnnotationBounds(previousEventComponent, component);
        
        if (auto *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1))
        {
            this->applyAnnotationBounds(oneMorePrevious, previousEventComponent);
        }
    }
    
    auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
    if (nextEventComponent != nullptr)
    {
        auto *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
        this->applyAnnotationBounds(nextEventComponent, oneMoreNext);
    }
    
    component->updateContent();
    this->applyAnnotationBounds(component, nextEventComponent);
}

void AnnotationsProjectMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Annotation))
    {
        const auto &annotation = static_cast<const AnnotationEvent &>(event);

        auto *component = this->createComponent(annotation);
        this->addChildComponent(component);

        const int indexOfSorted = this->annotationComponents.addSorted(*component, component);
        auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

        component->updateContent();
        this->applyAnnotationBounds(component, nextEventComponent);
        component->toFront(false);

        if (auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted))
        {
            this->applyAnnotationBounds(previousEventComponent, component);
        }

        this->annotationsHash[annotation] = component;

        component->setAlpha(0.f);
        const Rectangle<int> bounds(component->getBounds());
        this->animator.animateComponent(component, bounds, 1.f,
            Globals::UI::fadeInLong, false, 0.0, 0.0);
    }
}

void AnnotationsProjectMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Annotation))
    {
        const auto &annotation = static_cast<const AnnotationEvent &>(event);

        if (auto *component = this->annotationsHash[annotation])
        {
            this->animator.animateComponent(component,
                component->getBounds(),0.f, Globals::UI::fadeOutLong, true, 0.0, 0.0);

            this->removeChildComponent(component);
            this->annotationsHash.erase(annotation);

            const int indexOfSorted = this->annotationComponents.indexOfSorted(*component, component);
            auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

            if (auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted))
            {
                this->applyAnnotationBounds(previousEventComponent, nextEventComponent);
            }

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
            const auto &annotation = 
                static_cast<const AnnotationEvent &>(*track->getSequence()->getUnchecked(i));

            if (auto *component = this->annotationsHash[annotation])
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

    if (this->rollFirstBeat > firstBeat || this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = jmin(firstBeat, this->rollFirstBeat);
        this->rollLastBeat = jmax(lastBeat, this->rollLastBeat);
        this->resized();
    }
}

void AnnotationsProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
        this->resized();
    }
}

void AnnotationsProjectMap::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
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
    const auto *timeline = this->project.getTimeline();
    const auto annotations = timeline->getAnnotations()->getSequence();
    const auto seekBeat = this->project.getTransport().getSeekBeat();

    for (int i = 0; i < annotations->size(); ++i)
    {
        if (auto *annotation = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i)))
        {
            if (fabs(annotation->getBeat() - seekBeat) < 0.001f)
            {
                annotationUnderSeekCursor = annotation;
                break;
            }
        }
    }

    const auto newSeekBeat = nc->getBeat();
    const bool wasPlaying = this->project.getTransport().isPlaying();

    this->project.getTransport().stopPlaybackAndRecording();
    this->project.getTransport().seekToBeat(newSeekBeat);

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
        App::showModalComponent(AnnotationDialog::editingDialog(*this, nc->getEvent()));
    }
}

void AnnotationsProjectMap::alternateActionFor(AnnotationComponent *nc)
{
    // Selects everything within the range of this annotation
    this->annotationComponents.sort(*nc);
    const int indexOfSorted = this->annotationComponents.indexOfSorted(*nc, nc);
    auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
    
    const float startBeat = nc->getBeat();
    const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool shouldClearSelection = !isShiftPressed;
    
    this->roll.selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

float AnnotationsProjectMap::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll.getWidth()));
    const float targetBeat = this->roll.getRoundBeatSnapByXPosition(xRoll);
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

    auto *sequence = this->project.getTimeline()->getAnnotations()->getSequence();

    for (int j = 0; j < sequence->size(); ++j)
    {
        MidiEvent *event = sequence->getUnchecked(j);

        if (auto *annotation = dynamic_cast<AnnotationEvent *>(event))
        {
            auto *component = this->createComponent(*annotation);
            this->addAndMakeVisible(component);
            component->updateContent();

            this->annotationComponents.addSorted(*component, component);
            this->annotationsHash[*annotation] = component;
        }
    }

    this->resized();
}

void AnnotationsProjectMap::applyAnnotationBounds(AnnotationComponent *ac, AnnotationComponent *nextOne)
{
    constexpr float minWidth = 10.f;
    constexpr float widthMargin = 16.f;
    constexpr float componentsPadding = 10.f;

    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (ac->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));
    const float length = ac->hasLength() ?
        (mapWidth * ((beat + ac->getLength()) / projectLengthInBeats)) - x :
        ac->getTextWidth() + widthMargin;

    const float nextBeat = ((nextOne != nullptr ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat);
    const float nextX = mapWidth * (nextBeat / projectLengthInBeats);

    const float maxWidth = nextX - x;
    const float w = jmax(minWidth, jmin((maxWidth - componentsPadding), length));

    ac->setRealBounds({ x, 0.f, w, float(this->getHeight()) });
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
    return isPositiveAndBelow(indexOfNext, this->annotationComponents.size()) ?
        this->annotationComponents.getUnchecked(indexOfNext) :
        nullptr;
}

AnnotationComponent *AnnotationsProjectMap::createComponent(const AnnotationEvent &event)
{
    switch (this->type)
    {
    case Type::Large:
        return new AnnotationLargeComponent(*this, event);
    case Type::Small:
        return new AnnotationSmallComponent(*this, event);
    default:
        return nullptr;
    }
}
