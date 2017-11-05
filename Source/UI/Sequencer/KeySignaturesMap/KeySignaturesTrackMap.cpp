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
#include "KeySignaturesTrackMap.h"
#include "ProjectTreeItem.h"
#include "Transport.h"
#include "MidiSequence.h"
#include "ProjectTimeline.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "HelioCallout.h"
#include "AnnotationCommandPanel.h"
//#include "KeySignatureCommandPanel.h"

#include "KeySignatureDialog.h"
#include "App.h"
#include "MainLayout.h"

template<typename T>
KeySignaturesTrackMap<T>::KeySignaturesTrackMap(ProjectTreeItem &parentProject, HybridRoll &parentRoll) :
    project(parentProject),
    roll(parentRoll),
    projectFirstBeat(0.f),
    projectLastBeat(16.f), // non zero!
    rollFirstBeat(0.f),
    rollLastBeat(16.f)
{
    this->setOpaque(false);
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, true);
    
    this->reloadTrackMap();
    
    this->project.addListener(this);
}

template<typename T>
KeySignaturesTrackMap<T>::~KeySignaturesTrackMap()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

template<typename T>
void KeySignaturesTrackMap<T>::resized()
{
    //Logger::writeToLog("KeySignaturesTrackMap<T>::resized");
    this->setVisible(false);

    for (int i = 0; i < this->keySignatureComponents.size(); ++i)
    {
        T *current = this->keySignatureComponents.getUnchecked(i);
        current->updateContent();
    }
    
    this->setVisible(true);
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

template<typename T>
void KeySignaturesTrackMap<T>::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.getSequence() ==
        this->project.getTimeline()->getKeySignatures()->getSequence())
    {
        const KeySignatureEvent &keySignature = static_cast<const KeySignatureEvent &>(oldEvent);
        const KeySignatureEvent &newKeySignature = static_cast<const KeySignatureEvent &>(newEvent);

        if (T *component = this->keySignaturesHash[keySignature])
        {
            this->alignKeySignatureComponent(component);

            this->keySignaturesHash.remove(keySignature);
            this->keySignaturesHash.set(newKeySignature, component);
        }
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::alignKeySignatureComponent(T *component)
{
    this->keySignatureComponents.sort(*component);
    const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*component, component);
    T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
    T *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    if (previousEventComponent)
    {
        this->applyKeySignatureBounds(previousEventComponent, component);
        
        T *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
        
        if (oneMorePrevious)
        { this->applyKeySignatureBounds(oneMorePrevious, previousEventComponent); }
    }
    
    if (nextEventComponent)
    {
        T *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
        this->applyKeySignatureBounds(nextEventComponent, oneMoreNext);
    }
    
    component->updateContent();
    this->applyKeySignatureBounds(component, nextEventComponent);
}

template<typename T>
void KeySignaturesTrackMap<T>::onAddMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() ==
        this->project.getTimeline()->getKeySignatures()->getSequence())
    {
        const KeySignatureEvent &keySignature = static_cast<const KeySignatureEvent &>(event);

        auto component = new T(*this, keySignature);
        this->addChildComponent(component);

        const int indexOfSorted = this->keySignatureComponents.addSorted(*component, component);
        T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
        T *nextEventComponent(this->getNextEventComponent(indexOfSorted));

        component->updateContent();
        this->applyKeySignatureBounds(component, nextEventComponent);
        component->toFront(false);

        if (previousEventComponent)
        { this->applyKeySignatureBounds(previousEventComponent, component); }

        this->keySignaturesHash.set(keySignature, component);

        component->setAlpha(0.f);
        const Rectangle<int> bounds(component->getBounds());
        component->setBounds(bounds.translated(0, -component->getHeight()));
        this->animator.animateComponent(component, bounds, 1.f, 250, false, 0.0, 0.0);
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.getSequence() ==
        this->project.getTimeline()->getKeySignatures()->getSequence())
    {
        const KeySignatureEvent &keySignature = static_cast<const KeySignatureEvent &>(event);

        if (T *component = this->keySignaturesHash[keySignature])
        {
            this->animator.animateComponent(component,
                                            component->getBounds().translated(0, -component->getHeight()),
                                            0.f, 250, true, 0.0, 0.0);

            this->removeChildComponent(component);
            this->keySignaturesHash.remove(keySignature);

            const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*component, component);
            T *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            T *nextEventComponent(this->getNextEventComponent(indexOfSorted));

            if (previousEventComponent)
            { this->applyKeySignatureBounds(previousEventComponent, nextEventComponent); }

            this->keySignatureComponents.removeObject(component, true);
        }
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        this->repaint();
    }
}


template< typename T >
void KeySignaturesTrackMap<T>::onResetTrackContent(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        this->reloadTrackMap();
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::onAddTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        if (track->getSequence()->size() > 0)
        {
            this->reloadTrackMap();
        }
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::onRemoveTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        for (int i = 0; i < track->getSequence()->size(); ++i)
        {
            const KeySignatureEvent &keySignature =
                static_cast<const KeySignatureEvent &>(*track->getSequence()->getUnchecked(i));

            if (T *component = this->keySignaturesHash[keySignature])
            {
                this->removeChildComponent(component);
                this->keySignaturesHash.removeValue(component);
                this->keySignatureComponents.removeObject(component, true);
            }
        }
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;
}

template<typename T>
void KeySignaturesTrackMap<T>::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    this->resized();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

template<typename T>
void KeySignaturesTrackMap<T>::onKeySignatureMoved(T *nc)
{
}

template<typename T>
void KeySignaturesTrackMap<T>::onKeySignatureTapped(T *nc)
{
    const KeySignatureEvent *keySignatureUnderSeekCursor = nullptr;
    const ProjectTimeline *timeline = this->project.getTimeline();
    const auto keySignatures = timeline->getKeySignatures()->getSequence();
    const double seekPosition = this->project.getTransport().getSeekPosition();

    for (int i = 0; i < keySignatures->size(); ++i)
    {
        if (KeySignatureEvent *keySignature =
            dynamic_cast<KeySignatureEvent *>(keySignatures->getUnchecked(i)))
        {
            const float seekBeat = this->roll.getBeatByTransportPosition(seekPosition);
            
            if (fabs(keySignature->getBeat() - seekBeat) < 0.1)
            {
                keySignatureUnderSeekCursor = keySignature;
                break;
            }
        }
    }

    const double newSeekPosition = this->roll.getTransportPositionByBeat(nc->getBeat() - 0.01f);
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

    if (keySignatureUnderSeekCursor == &nc->getEvent() && !wasPlaying)
    {
        this->showContextMenuFor(nc);
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::showContextMenuFor(T *nc)
{
    if (! this->project.getTransport().isPlaying())
    {
        Component *dialog =
            KeySignatureDialog::createEditingDialog(*this, nc->getEvent());
        App::Layout().showModalNonOwnedDialog(dialog);
    }
}

template<typename T>
void KeySignaturesTrackMap<T>::alternateActionFor(T *nc)
{
    // Selects everything within the range of this keySignature
    this->keySignatureComponents.sort(*nc);
    const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*nc, nc);
    T *nextEventComponent(this->getNextEventComponent(indexOfSorted));
    
    const float startBeat = nc->getBeat();
    const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool shouldClearSelection = !isShiftPressed;
    
    this->roll.selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

template<typename T>
float KeySignaturesTrackMap<T>::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll.getWidth()));
    const float targetBeat = this->roll.getRoundBeatByXPosition(xRoll);
    return jmin(jmax(targetBeat, this->rollFirstBeat), this->rollLastBeat);
}

template<typename T>
void KeySignaturesTrackMap<T>::reloadTrackMap()
{
    //Logger::writeToLog("KeySignaturesTrackMap<T>::reloadTrackMap");

    for (int i = 0; i < this->keySignatureComponents.size(); ++i)
    {
        this->removeChildComponent(this->keySignatureComponents.getUnchecked(i));
    }

    this->keySignatureComponents.clear();
    this->keySignaturesHash.clear();

    this->setVisible(false);

    MidiSequence *sequence = this->project.getTimeline()->getKeySignatures()->getSequence();

    for (int j = 0; j < sequence->size(); ++j)
    {
        MidiEvent *event = sequence->getUnchecked(j);

        if (KeySignatureEvent *keySignature = dynamic_cast<KeySignatureEvent *>(event))
        {
            auto component = new T(*this, *keySignature);
            this->addAndMakeVisible(component);

            this->keySignatureComponents.addSorted(*component, component);
            this->keySignaturesHash.set(*keySignature, component);
        }
    }

    this->resized();
    this->setVisible(true);
}

template<typename T>
void KeySignaturesTrackMap<T>::applyKeySignatureBounds(T *nc, T *nextOne)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));

    const float nextBeat = ((nextOne ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat);
    const float nextX = mapWidth * (nextBeat / projectLengthInBeats);

    const float minWidth = 10.f;
    const float widthMargin = 32.f;
    const float componentsPadding = 10.f;
    const float maxWidth = nextX - x;
    const float w = jmax(minWidth, jmin((maxWidth - componentsPadding), widthMargin));

    nc->setRealBounds(Rectangle<float>(x, 0.f, w, float(nc->getHeight())));
}

template<typename T>
T *KeySignaturesTrackMap<T>::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;

    return
        isPositiveAndBelow(indexOfPrevious, this->keySignatureComponents.size()) ?
        this->keySignatureComponents.getUnchecked(indexOfPrevious) :
        nullptr;
}

template<typename T>
T *KeySignaturesTrackMap<T>::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;

    return
        isPositiveAndBelow(indexOfNext, this->keySignatureComponents.size()) ?
        this->keySignatureComponents.getUnchecked(indexOfNext) :
        nullptr;
}
