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
#include "PianoTrackMap.h"
#include "ProjectTreeItem.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "MidiTrack.h"

PianoTrackMap::PianoTrackMap(ProjectTreeItem &parentProject, HybridRoll &parentRoll) :
    project(parentProject),
    roll(parentRoll),
    projectFirstBeat(0.f),
    projectLastBeat(0.f),
    rollFirstBeat(0.f),
    rollLastBeat(0.f),
    componentHeight(1.f)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->reloadTrackMap();
    this->project.addListener(this);
}

PianoTrackMap::~PianoTrackMap()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoTrackMap::resized()
{
    this->componentHeight = float(this->getHeight()) / 128.f; // TODO remove hard-coded value
    
    this->setVisible(false);

    for (const auto &e : this->componentsMap)
    {
        this->applyNoteBounds(e.second.get());
    }

    this->setVisible(true);
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoTrackMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(oldEvent);
        const Note &newNote = static_cast<const Note &>(newEvent);
        if (const auto component = this->componentsMap[note].release())
        {
            this->componentsMap.erase(note);
            this->componentsMap[newNote] = UniquePointer<PianoTrackMapNoteComponent>(component);
            this->applyNoteBounds(component);
        }
    }
}

void PianoTrackMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);

        auto component = new PianoTrackMapNoteComponent(note);
        this->componentsMap[note] = UniquePointer<PianoTrackMapNoteComponent>(component);

        this->addAndMakeVisible(component);
        this->applyNoteBounds(component);
        component->toFront(false);
    }
}

void PianoTrackMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        if (const auto deletedComponent = this->componentsMap[note].get())
        {
            this->componentsMap.erase(note);
        }
    }
}

void PianoTrackMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    this->setVisible(false);

    for (const auto &e : this->componentsMap)
    {
        e.second->updateColour();
    }

    this->setVisible(true);
    this->repaint();
}

void PianoTrackMap::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrackMap();
}

void PianoTrackMap::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    if (track->getSequence()->size() > 0)
    {
        this->reloadTrackMap();
    }
}

void PianoTrackMap::onRemoveTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const Note &note = static_cast<const Note &>(*track->getSequence()->getUnchecked(i));
        if (const auto deletedComponent = this->componentsMap[note].get())
        {
            this->componentsMap.erase(note);
        }
    }
}

void PianoTrackMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
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

void PianoTrackMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    this->resized();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void PianoTrackMap::reloadTrackMap()
{
    this->componentsMap.clear();

    this->setVisible(false);

    const auto &tracks = this->project.getTracks();
    for (auto track : tracks)
    {
        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            MidiEvent *event = track->getSequence()->getUnchecked(j);

            if (Note *note = dynamic_cast<Note *>(event))
            {
                auto noteComponent = new PianoTrackMapNoteComponent(*note);
                this->componentsMap[*note] = UniquePointer<PianoTrackMapNoteComponent>(noteComponent);
                this->addAndMakeVisible(noteComponent);
            }
        }
    }

    this->resized();
    this->setVisible(true);
}

void PianoTrackMap::applyNoteBounds(PianoTrackMapNoteComponent *nc)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));
    const float w = (mapWidth * (nc->getLength() / projectLengthInBeats));
    const int y = this->getHeight() - int(nc->getKey() * this->componentHeight);
    nc->setRealBounds(x, y, jmax(1.f, w), 1);
}
