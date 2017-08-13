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

class TrackMapNoteComponent : public Component
{
public:

    TrackMapNoteComponent(PianoTrackMap &parent, const Note &event) :
        note(event),
        map(parent),
        dx(0.f),
        dw(0.f)
    {
        this->setOpaque(false);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    inline int getKey() const noexcept           { return this->note.getKey(); }
    inline float getBeat() const noexcept        { return this->note.getBeat(); }
    inline float getLength() const noexcept      { return this->note.getLength(); }
    inline float getVelocity() const noexcept    { return this->note.getVelocity(); }

    void setRealBounds(float x, int y, float w, int h)
    {
#if HELIO_DESKTOP
        this->dx = x - float(int(x));
        this->dw = w - float(int(w));
#endif
        this->setBounds(int(x), y, int(w), h);
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(this->note.getLayer()->getColour().
                    interpolatedWith(Colours::white, .35f).
                    withAlpha(this->note.getVelocity() * .3f + .4f));

#if HELIO_DESKTOP
        g.drawHorizontalLine(0, this->dx, float(this->getWidth()) + this->dw + this->dx);
#elif HELIO_MOBILE
        g.drawHorizontalLine(0, 0, float(this->getWidth()));
#endif
    }

private:

    const Note &note;

    PianoTrackMap &map;
    
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
    this->setOpaque(false);
    this->setInterceptsMouseClicks(false, false);
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
    this->componentHeight = float(this->getHeight()) / 128.f;
    
    this->setVisible(false);

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->applyNoteBounds(this->eventComponents.getUnchecked(i));
    }

    this->setVisible(true);
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoTrackMap::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (!dynamic_cast<const Note *>(&oldEvent)) { return; }

    const Note &note = static_cast<const Note &>(oldEvent);
    const Note &newNote = static_cast<const Note &>(newEvent);

    if (TrackMapNoteComponent *component = this->componentsHashTable[note])
    {
        //component->setVisible(false);
        this->applyNoteBounds(component);
        //component->repaint(); // for volume changes
        //component->setVisible(true);

        this->componentsHashTable.set(newNote, component);
    }
}

void PianoTrackMap::onAddMidiEvent(const MidiEvent &event)
{
    if (!dynamic_cast<const Note *>(&event)) { return; }

    const Note &note = static_cast<const Note &>(event);

    auto component = new TrackMapNoteComponent(*this, note);
    this->addAndMakeVisible(component);
    this->applyNoteBounds(component);
    component->toFront(false);

    //this->eventAnimator.fadeIn(component, 150);
    this->eventComponents.add(component);

    this->componentsHashTable.set(note, component);
}

void PianoTrackMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (!dynamic_cast<const Note *>(&event)) { return; }

    const Note &note = static_cast<const Note &>(event);

    if (TrackMapNoteComponent *component = this->componentsHashTable[note])
    {
        //this->eventAnimator.fadeOut(component, 150);
        this->removeChildComponent(component);
        this->componentsHashTable.remove(note);
        this->eventComponents.removeObject(component, true);
    }
}

void PianoTrackMap::onChangeTrack(MidiSequence *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (!dynamic_cast<const PianoSequence *>(layer)) { return; }

    this->reloadTrackMap();
}

void PianoTrackMap::onAddTrack(MidiSequence *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (!dynamic_cast<const PianoSequence *>(layer)) { return; }

    if (layer->size() > 0)
    {
        this->reloadTrackMap();
    }
}

void PianoTrackMap::onRemoveTrack(MidiSequence *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (!dynamic_cast<const PianoSequence *>(layer)) { return; }

    for (int i = 0; i < layer->size(); ++i)
    {
        const Note &note = static_cast<const Note &>(*layer->getUnchecked(i));

        if (TrackMapNoteComponent *component = this->componentsHashTable[note])
        {
            this->removeChildComponent(component);
            this->componentsHashTable.remove(note);
            this->eventComponents.removeObject(component, true);
        }
    }
}

void PianoTrackMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;
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
    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->removeChildComponent(this->eventComponents.getUnchecked(i));
    }

    this->eventComponents.clear();
    this->componentsHashTable.clear();

    const Array<MidiSequence *> &layers = this->project.getLayersList();

    this->setVisible(false);

    for (auto layer : layers)
    {
        for (int j = 0; j < layer->size(); ++j)
        {
            MidiEvent *event = layer->getUnchecked(j);

            if (Note *note = dynamic_cast<Note *>(event))
            {
                auto noteComponent = new TrackMapNoteComponent(*this, *note);

                this->eventComponents.add(noteComponent);
                this->componentsHashTable.set(*note, noteComponent);

                //const bool &belongsToActiveLayer = noteComponent->belongsToLayer(this->activeLayer);
                //noteComponent->setActive(belongsToActiveLayer);
                this->addAndMakeVisible(noteComponent);
            }
        }
    }

    this->resized();
    this->setVisible(true);
}

void PianoTrackMap::applyNoteBounds(TrackMapNoteComponent *nc)
{
    const float rollLengthInBeats = (this->rollLastBeat - this->rollFirstBeat);
    const float projectLengthInBeats = (this->projectLastBeat - this->projectFirstBeat);

    const float beat = (nc->getBeat() - this->rollFirstBeat);
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = (mapWidth * (beat / projectLengthInBeats));
    const float w = (mapWidth * (nc->getLength() / projectLengthInBeats));
    const int y = this->getHeight() - (nc->getKey() * this->componentHeight);
    nc->setRealBounds(x, y, jmax(1.f, w), 1.f);
}
