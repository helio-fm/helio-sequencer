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

class TrackMapNoteComponent : public Component
{
public:

    TrackMapNoteComponent(PianoTrackMap &parent, const Note &event) :
        note(event),
        map(parent),
        dx(0.f),
        dw(0.f)
    {
        this->updateColour();
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    inline int getKey() const noexcept           { return this->note.getKey(); }
    inline float getBeat() const noexcept        { return this->note.getBeat(); }
    inline float getLength() const noexcept      { return this->note.getLength(); }
    inline float getVelocity() const noexcept    { return this->note.getVelocity(); }
    inline void updateColour()
    {
        this->colour = this->note.getColour().
            interpolatedWith(Colours::white, .35f).
            withAlpha(.55f);
    }

    void setRealBounds(float x, int y, float w, int h)
    {
        this->dx = x - floorf(x);
        this->dw = ceilf(w) - w;
        this->setBounds(int(floorf(x)), y, int(ceilf(w)), h);
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        g.drawHorizontalLine(0, this->dx, float(this->getWidth()) - this->dw);
    }

private:

    const Note &note;
    PianoTrackMap &map;
    
    Colour colour;

    float dx;
    float dw;

};


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
    this->clearTrackMap();
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoTrackMap::resized()
{
    this->componentHeight = float(this->getHeight()) / 128.f;
    
    this->setVisible(false);

    for (const auto &e : this->componentsMap)
    {
        this->applyNoteBounds(e.second);
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
        if (TrackMapNoteComponent *component = this->componentsMap[note])
        {
            this->applyNoteBounds(component);
            this->componentsMap.erase(note);
            this->componentsMap[newNote] = component;
        }
    }
}

void PianoTrackMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);

        auto component = new TrackMapNoteComponent(*this, note);
        this->addAndMakeVisible(component);
        this->applyNoteBounds(component);
        component->toFront(false);

        this->componentsMap[note] = component;
    }
}

void PianoTrackMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        if (ScopedPointer<TrackMapNoteComponent> deleter = this->componentsMap[note])
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
        if (ScopedPointer<TrackMapNoteComponent> deletedComponent = this->componentsMap[note])
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
    this->clearTrackMap();

    this->setVisible(false);

    const auto &tracks = this->project.getTracks();
    for (auto track : tracks)
    {
        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            MidiEvent *event = track->getSequence()->getUnchecked(j);

            if (Note *note = dynamic_cast<Note *>(event))
            {
                auto noteComponent = new TrackMapNoteComponent(*this, *note);
                this->componentsMap[*note] = noteComponent;
                this->addAndMakeVisible(noteComponent);
            }
        }
    }

    this->resized();
    this->setVisible(true);
}

void PianoTrackMap::clearTrackMap()
{
    OwnedArray<TrackMapNoteComponent> deleters;
    for (const auto &e : this->componentsMap)
    {
        deleters.add(e.second);
    }

    this->componentsMap.clear();
}

void PianoTrackMap::applyNoteBounds(TrackMapNoteComponent *nc)
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
