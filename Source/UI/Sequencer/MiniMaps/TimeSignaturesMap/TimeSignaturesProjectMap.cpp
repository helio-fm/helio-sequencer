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
#include "TimeSignaturesProjectMap.h"
#include "ProjectNode.h"
#include "Transport.h"
#include "MidiSequence.h"
#include "ProjectTimeline.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AnnotationMenu.h"
#include "TimeSignatureMenu.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "TimeSignatureDialog.h"
#include "MainLayout.h"

template<typename T>
TimeSignaturesProjectMap<T>::TimeSignaturesProjectMap(ProjectNode &parentProject, HybridRoll &parentRoll) :
    project(parentProject),
    roll(parentRoll),
    projectFirstBeat(0.f),
    projectLastBeat(16.f), // non zero!
    rollFirstBeat(0.f),
    rollLastBeat(16.f)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->trackStartIndicator = new TrackStartIndicator();
    this->addAndMakeVisible(this->trackStartIndicator);
    
    this->trackEndIndicator = new TrackEndIndicator();
    this->addAndMakeVisible(this->trackEndIndicator);
    
    this->updateTrackRangeIndicatorsAnchors();
    
    this->reloadTrackMap();
    
    this->project.addListener(this);
}

template<typename T>
TimeSignaturesProjectMap<T>::~TimeSignaturesProjectMap()
{
    this->project.removeListener(this);
}

template<typename T>
void TimeSignaturesProjectMap<T>::updateTrackRangeIndicatorsAnchors()
{
    const double rollLengthInBeats = double(this->rollLastBeat - this->rollFirstBeat);
    const double absStart = double(this->projectFirstBeat - this->rollFirstBeat) / rollLengthInBeats;
    const double absEnd = double(this->projectLastBeat - this->rollFirstBeat) / rollLengthInBeats;
    this->trackStartIndicator->setAnchoredAt(absStart);
    this->trackEndIndicator->setAnchoredAt(absEnd);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

template<typename T>
void TimeSignaturesProjectMap<T>::resized()
{
    this->setVisible(false);

    T *previous = nullptr;

    for (int i = 0; i < this->timeSignatureComponents.size(); ++i)
    {
        T *current = this->timeSignatureComponents.getUnchecked(i);

        if (previous != nullptr)
        {
            this->applyTimeSignatureBounds(previous, current);
        }

        if (i == (this->timeSignatureComponents.size() - 1))
        {
            this->applyTimeSignatureBounds(current, nullptr);
        }

        previous = current;
    }
    
    this->updateTrackRangeIndicatorsAnchors();
    
    this->setVisible(true);
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

template<typename T>
void TimeSignaturesProjectMap<T>::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() ==
        this->project.getTimeline()->getTimeSignatures()->getSequence())
    {
        const TimeSignatureEvent &timeSignature = static_cast<const TimeSignatureEvent &>(oldEvent);
        const TimeSignatureEvent &newTimeSignature = static_cast<const TimeSignatureEvent &>(newEvent);

        if (T *component = this->timeSignaturesHash[timeSignature])
        {
            this->alignTimeSignatureComponent(component);

            this->timeSignaturesHash.erase(timeSignature);
            this->timeSignaturesHash[newTimeSignature] = component;
        }
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::alignTimeSignatureComponent(T *component)
{
    this->timeSignatureComponents.sort(*component);
    const int indexOfSorted = this->timeSignatureComponents.indexOfSorted(*component, component);
    T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
    T *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    if (previousEventComponent)
    {
        this->applyTimeSignatureBounds(previousEventComponent, component);
        
        T *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
        
        if (oneMorePrevious)
        { this->applyTimeSignatureBounds(oneMorePrevious, previousEventComponent); }
    }
    
    if (nextEventComponent)
    {
        T *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
        this->applyTimeSignatureBounds(nextEventComponent, oneMoreNext);
    }
    
    component->updateContent();
    this->applyTimeSignatureBounds(component, nextEventComponent);
}

template<typename T>
void TimeSignaturesProjectMap<T>::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() ==
        this->project.getTimeline()->getTimeSignatures()->getSequence())
    {
        const TimeSignatureEvent &timeSignature = static_cast<const TimeSignatureEvent &>(event);

        auto component = new T(*this, timeSignature);
        this->addChildComponent(component);

        const int indexOfSorted = this->timeSignatureComponents.addSorted(*component, component);
        T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
        T *nextEventComponent(this->getNextEventComponent(indexOfSorted));

        component->updateContent();
        this->applyTimeSignatureBounds(component, nextEventComponent);
        component->toFront(false);

        if (previousEventComponent)
        { this->applyTimeSignatureBounds(previousEventComponent, component); }

        this->timeSignaturesHash[timeSignature] = component;

        component->setAlpha(0.f);
        const Rectangle<int> bounds(component->getBounds());
        component->setBounds(bounds.translated(0, -component->getHeight()));
        this->animator.animateComponent(component, bounds, 1.f, 250, false, 0.0, 0.0);
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() ==
        this->project.getTimeline()->getTimeSignatures()->getSequence())
    {
        const TimeSignatureEvent &timeSignature = static_cast<const TimeSignatureEvent &>(event);

        if (T *component = this->timeSignaturesHash[timeSignature])
        {
            this->animator.animateComponent(component,
                                            component->getBounds().translated(0, -component->getHeight()),
                                            0.f, 250, true, 0.0, 0.0);

            this->removeChildComponent(component);
            this->timeSignaturesHash.erase(timeSignature);

            const int indexOfSorted = this->timeSignatureComponents.indexOfSorted(*component, component);
            T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            T *nextEventComponent(this->getNextEventComponent(indexOfSorted));

            if (previousEventComponent)
            { this->applyTimeSignatureBounds(previousEventComponent, nextEventComponent); }

            this->timeSignatureComponents.removeObject(component, true);
        }
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getTimeSignatures())
    {
        this->repaint();
    }
}


template< typename T >
void TimeSignaturesProjectMap<T>::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrackMap();
}

template<typename T>
void TimeSignaturesProjectMap<T>::onAddTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getTimeSignatures())
    {
        if (track->getSequence()->size() > 0)
        {
            this->reloadTrackMap();
        }
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::onRemoveTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getTimeSignatures())
    {
        for (int i = 0; i < track->getSequence()->size(); ++i)
        {
            const TimeSignatureEvent &timeSignature =
                static_cast<const TimeSignatureEvent &>(*track->getSequence()->getUnchecked(i));

            if (T *component = this->timeSignaturesHash[timeSignature])
            {
                this->removeChildComponent(component);
                this->timeSignaturesHash.erase(timeSignature);
                this->timeSignatureComponents.removeObject(component, true);
            }
        }
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::onChangeProjectBeatRange(float firstBeat, float lastBeat)
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
    else
    {
        this->updateTrackRangeIndicatorsAnchors();
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    this->resized();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

template<typename T>
void TimeSignaturesProjectMap<T>::onTimeSignatureMoved(T *nc)
{
}

template<typename T>
void TimeSignaturesProjectMap<T>::onTimeSignatureTapped(T *nc)
{
    const TimeSignatureEvent *timeSignatureUnderSeekCursor = nullptr;
    const ProjectTimeline *timeline = this->project.getTimeline();
    const auto timeSignatures = timeline->getTimeSignatures()->getSequence();
    const double seekPosition = this->project.getTransport().getSeekPosition();

    for (int i = 0; i < timeSignatures->size(); ++i)
    {
        if (TimeSignatureEvent *timeSignature =
            dynamic_cast<TimeSignatureEvent *>(timeSignatures->getUnchecked(i)))
        {
            const float seekBeat = this->roll.getBeatByTransportPosition(seekPosition);
            
            if (fabs(timeSignature->getBeat() - seekBeat) < 0.1)
            {
                timeSignatureUnderSeekCursor = timeSignature;
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

    if (timeSignatureUnderSeekCursor == &nc->getEvent() && !wasPlaying)
    {
        this->showContextMenuFor(nc);
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::showContextMenuFor(T *nc)
{
    if (! this->project.getTransport().isPlaying())
    {
        Component *dialog =
            TimeSignatureDialog::createEditingDialog(*this, nc->getEvent());
        App::Layout().showModalComponentUnowned(dialog);
    }
}

template<typename T>
void TimeSignaturesProjectMap<T>::alternateActionFor(T *nc)
{
    // Selects everything within the range of this timeSignature
    this->timeSignatureComponents.sort(*nc);
    const int indexOfSorted = this->timeSignatureComponents.indexOfSorted(*nc, nc);
    T *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    const float startBeat = nc->getBeat();
    const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool shouldClearSelection = !isShiftPressed;
    
    this->roll.selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

template<typename T>
float TimeSignaturesProjectMap<T>::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll.getWidth()));
    const float targetBeat = this->roll.getRoundBeatByXPosition(xRoll);
    return jlimit(this->rollFirstBeat, this->rollLastBeat, targetBeat);
}

template<typename T>
void TimeSignaturesProjectMap<T>::reloadTrackMap()
{
    if (this->project.getTimeline() == nullptr)
    {
        return;
    }

    this->timeSignatureComponents.clear();
    this->timeSignaturesHash.clear();

    this->setVisible(false);

    MidiSequence *sequence = this->project.getTimeline()->getTimeSignatures()->getSequence();

    for (int j = 0; j < sequence->size(); ++j)
    {
        MidiEvent *event = sequence->getUnchecked(j);

        if (TimeSignatureEvent *timeSignature = dynamic_cast<TimeSignatureEvent *>(event))
        {
            auto component = new T(*this, *timeSignature);
            this->addAndMakeVisible(component);
            component->updateContent();

            this->timeSignatureComponents.addSorted(*component, component);
            this->timeSignaturesHash[*timeSignature] = component;
        }
    }

    this->resized();
    this->setVisible(true);
}

template<typename T>
void TimeSignaturesProjectMap<T>::applyTimeSignatureBounds(T *nc, T *nextOne)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));

    const float nextBeat = ((nextOne ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat);
    const float nextX = mapWidth * (nextBeat / projectLengthInBeats);

    const float minWidth = 10.f;
    const float widthMargin = 26.f;
    const float componentsPadding = 10.f;
    const float maxWidth = nextX - x;
    const float w = jmax(minWidth, jmin((maxWidth - componentsPadding), widthMargin));

    nc->setRealBounds(Rectangle<float>(x, 0.f, w, float(this->getHeight())));
}

template<typename T>
T *TimeSignaturesProjectMap<T>::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;

    return
        isPositiveAndBelow(indexOfPrevious, this->timeSignatureComponents.size()) ?
        this->timeSignatureComponents.getUnchecked(indexOfPrevious) :
        nullptr;
}

template<typename T>
T *TimeSignaturesProjectMap<T>::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;

    return
        isPositiveAndBelow(indexOfNext, this->timeSignatureComponents.size()) ?
        this->timeSignatureComponents.getUnchecked(indexOfNext) :
        nullptr;
}
