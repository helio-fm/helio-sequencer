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
#include "KeySignaturesProjectMap.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "ProjectTimeline.h"
#include "ProjectMetadata.h"
#include "PlayerThread.h"
#include "RollBase.h"
#include "KeySignatureDialog.h"
#include "KeySignatureLargeComponent.h"
#include "KeySignatureSmallComponent.h"
#include "RescalePreviewTool.h"
#include "ModalCallout.h"

KeySignaturesProjectMap::KeySignaturesProjectMap(ProjectNode &project,
    SafePointer<RollBase> roll, Type type) :
    ScrolledComponent(roll),
    project(project),
    type(type)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->reloadTrackMap();
    
    this->project.addListener(this);
}

KeySignaturesProjectMap::~KeySignaturesProjectMap()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void KeySignaturesProjectMap::resized()
{
    KeySignatureComponent *previous = nullptr;

    for (int i = 0; i < this->keySignatureComponents.size(); ++i)
    {
        auto *current = this->keySignatureComponents.getUnchecked(i);

        if (previous != nullptr)
        {
            this->applyKeySignatureBounds(previous, current);
        }

        if (i == (this->keySignatureComponents.size() - 1))
        {
            this->applyKeySignatureBounds(current, nullptr);
        }

        previous = current;
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void KeySignaturesProjectMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Type::KeySignature))
    {
        const auto &keySignature = static_cast<const KeySignatureEvent &>(oldEvent);
        const auto &newKeySignature = static_cast<const KeySignatureEvent &>(newEvent);

        if (auto *component = this->keySignaturesMap[keySignature])
        {
            this->alignKeySignatureComponent(component);
            this->keySignaturesMap.erase(keySignature);
            this->keySignaturesMap[newKeySignature] = component;
        }
    }
}

void KeySignaturesProjectMap::alignKeySignatureComponent(KeySignatureComponent *component)
{
    this->keySignatureComponents.sort(*component);
    const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*component, component);
    auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
    auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
    
    if (previousEventComponent)
    {
        this->applyKeySignatureBounds(previousEventComponent, component);
        
       auto *oneMorePrevious = this->getPreviousEventComponent(indexOfSorted - 1);
        
        if (oneMorePrevious)
        {
            this->applyKeySignatureBounds(oneMorePrevious, previousEventComponent);
        }
    }
    
    if (nextEventComponent)
    {
        auto *oneMoreNext = this->getNextEventComponent(indexOfSorted + 1);
        this->applyKeySignatureBounds(nextEventComponent, oneMoreNext);
    }
    
    component->updateContent(this->getProjectKeyNames());
    this->applyKeySignatureBounds(component, nextEventComponent);
}

void KeySignaturesProjectMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::KeySignature))
    {
        const KeySignatureEvent &keySignature = static_cast<const KeySignatureEvent &>(event);

        auto *component = this->createComponent(keySignature);
        this->addChildComponent(component);

        const int indexOfSorted = this->keySignatureComponents.addSorted(*component, component);
        auto *previousEventComponent = this->getPreviousEventComponent(indexOfSorted);
        auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);

        component->updateContent(this->getProjectKeyNames());
        this->applyKeySignatureBounds(component, nextEventComponent);
        component->toFront(false);

        if (previousEventComponent)
        {
            this->applyKeySignatureBounds(previousEventComponent, component);
        }

        this->keySignaturesMap[keySignature] = component;

        component->setAlpha(0.f);
        this->animator.animateComponent(component,
            component->getBounds(), 1.f, Globals::UI::fadeInLong, false, 0.0, 0.0);
    }
}

void KeySignaturesProjectMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::KeySignature))
    {
        const auto &keySignature = static_cast<const KeySignatureEvent &>(event);

        if (auto *component = this->keySignaturesMap[keySignature])
        {
            this->animator.animateComponent(component,
                component->getBounds(), 0.f, Globals::UI::fadeOutLong, true, 0.0, 0.0);

            this->removeChildComponent(component);
            this->keySignaturesMap.erase(keySignature);

            const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*component, component);
            KeySignatureComponent *previousEventComponent(this->getPreviousEventComponent(indexOfSorted));
            KeySignatureComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));

            if (previousEventComponent)
            { this->applyKeySignatureBounds(previousEventComponent, nextEventComponent); }

            this->keySignatureComponents.removeObject(component, true);
        }
    }
}

void KeySignaturesProjectMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        this->repaint();
    }
}

void KeySignaturesProjectMap::onAddTrack(MidiTrack *const track)
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

void KeySignaturesProjectMap::onRemoveTrack(MidiTrack *const track)
{
    if (this->project.getTimeline() != nullptr &&
        track == this->project.getTimeline()->getKeySignatures())
    {
        for (int i = 0; i < track->getSequence()->size(); ++i)
        {
            const auto &keySignature =
                static_cast<const KeySignatureEvent &>(*track->getSequence()->getUnchecked(i));

            if (auto *component = this->keySignaturesMap[keySignature])
            {
                this->removeChildComponent(component);
                this->keySignaturesMap.erase(keySignature);
                this->keySignatureComponents.removeObject(component, true);
            }
        }
    }
}

void KeySignaturesProjectMap::onChangeProjectInfo(const ProjectMetadata *info)
{
    // track temperament changes to update signature descriptions:
    if (this->keyboardSize != info->getKeyboardSize())
    {
        this->keyboardSize = info->getKeyboardSize();
        const auto &keyNames = this->getProjectKeyNames();
        for (auto *component : this->keySignatureComponents)
        {
            component->updateContent(keyNames);
        }
    }
}

void KeySignaturesProjectMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
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

void KeySignaturesProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
        this->resized();
    }
}

void KeySignaturesProjectMap::onBeforeReloadProjectContent()
{
    this->keySignatureComponents.clear();
}

void KeySignaturesProjectMap::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->keyboardSize = meta->getKeyboardSize();
    this->reloadTrackMap();
}

//===----------------------------------------------------------------------===//
// Stuff for children
//===----------------------------------------------------------------------===//

void KeySignaturesProjectMap::onKeySignatureSelected(KeySignatureComponent *nc)
{
    // Selects everything within the range of this keySignature
    this->keySignatureComponents.sort(*nc);
    const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*nc, nc);
    KeySignatureComponent *nextEventComponent(this->getNextEventComponent(indexOfSorted));

    const float startBeat = nc->getBeat();
    const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool shouldClearSelection = !isShiftPressed;

    this->roll->selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

void KeySignaturesProjectMap::onKeySignatureMainAction(KeySignatureComponent *ksc)
{
    this->keySignatureTapAction(ksc, false);
}

void KeySignaturesProjectMap::onKeySignatureAltAction(KeySignatureComponent *ksc)
{
    this->keySignatureTapAction(ksc, true);
}

float KeySignaturesProjectMap::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll->getWidth()));
    const float targetBeat = this->roll->getRoundBeatSnapByXPosition(xRoll);
    return jlimit(this->rollFirstBeat, this->rollLastBeat, targetBeat);
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void KeySignaturesProjectMap::keySignatureTapAction(KeySignatureComponent *ksc, bool altMode)
{
    if (altMode)
    {
        this->keySignatureComponents.sort(*ksc);
        const int indexOfSorted = this->keySignatureComponents.indexOfSorted(*ksc, ksc);
        const auto *nextEventComponent = this->getNextEventComponent(indexOfSorted);
        const float endBeat = (nextEventComponent != nullptr) ? nextEventComponent->getBeat() : FLT_MAX;
        ModalCallout::emit(new QuickRescaleMenu(this->project, ksc->getEvent(), endBeat), this, true);
        return;
    }

    const auto seekBeat = this->project.getTransport().getSeekBeat();
    const auto newSeekBeat = ksc->getBeat();
    const bool wasPlaying = this->project.getTransport().isPlaying();

    this->project.getTransport().stopPlaybackAndRecording();
    this->project.getTransport().seekToBeat(newSeekBeat);

    if (fabs(ksc->getBeat() - seekBeat) < 0.001f && !wasPlaying)
    {
        App::showModalComponent(KeySignatureDialog::editingDialog(this->project, ksc->getEvent()));
    }
}

void KeySignaturesProjectMap::reloadTrackMap()
{
    if (this->project.getTimeline() == nullptr)
    {
        return;
    }

    for (int i = 0; i < this->keySignatureComponents.size(); ++i)
    {
        this->removeChildComponent(this->keySignatureComponents.getUnchecked(i));
    }

    this->keySignatureComponents.clear();
    this->keySignaturesMap.clear();

    MidiSequence *sequence = this->project.getTimeline()->getKeySignatures()->getSequence();
    const auto &keyNames = this->getProjectKeyNames();

    for (int j = 0; j < sequence->size(); ++j)
    {
        MidiEvent *event = sequence->getUnchecked(j);

        KeySignatureEvent *keySignature = static_cast<KeySignatureEvent *>(event);
        auto *component = this->createComponent(*keySignature);
        this->addAndMakeVisible(component);
        component->updateContent(keyNames);

        this->keySignatureComponents.addSorted(*component, component);
        this->keySignaturesMap[*keySignature] = component;
    }

    this->resized();
}

void KeySignaturesProjectMap::applyKeySignatureBounds(KeySignatureComponent *nc, KeySignatureComponent *nextOne)
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

KeySignatureComponent *KeySignaturesProjectMap::getPreviousEventComponent(int indexOfSorted) const
{
    const int indexOfPrevious = indexOfSorted - 1;

    return
        isPositiveAndBelow(indexOfPrevious, this->keySignatureComponents.size()) ?
        this->keySignatureComponents.getUnchecked(indexOfPrevious) :
        nullptr;
}

KeySignatureComponent *KeySignaturesProjectMap::getNextEventComponent(int indexOfSorted) const
{
    const int indexOfNext = indexOfSorted + 1;

    return
        isPositiveAndBelow(indexOfNext, this->keySignatureComponents.size()) ?
        this->keySignatureComponents.getUnchecked(indexOfNext) :
        nullptr;
}

KeySignatureComponent *KeySignaturesProjectMap::createComponent(const KeySignatureEvent &keySignature)
{
    switch (this->type)
    {
    case Type::Large:
        return new KeySignatureLargeComponent(*this, keySignature);
    case Type::Small:
        return new KeySignatureSmallComponent(*this, keySignature);
    default:
        return nullptr;
    }
}

const StringArray &KeySignaturesProjectMap::getProjectKeyNames() const noexcept
{
    return this->project.getProjectInfo()->getTemperament()->getPeriod();
}
