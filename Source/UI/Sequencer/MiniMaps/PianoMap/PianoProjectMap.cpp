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
#include "PianoProjectMap.h"
#include "ProjectNode.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "Pattern.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "MidiTrack.h"
#include "ColourIDs.h"

PianoProjectMap::PianoProjectMap(ProjectNode &parentProject, HybridRoll &parentRoll) :
    project(parentProject),
    roll(parentRoll)
{
    this->baseColour = findDefaultColour(ColourIDs::Roll::noteFill);

    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->reloadTrackMap();

    this->project.addListener(this);
}

PianoProjectMap::~PianoProjectMap()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoProjectMap::resized()
{
    this->componentHeight = float(this->getHeight()) / 128.f; // TODO remove hard-coded value
}

void PianoProjectMap::paint(Graphics &g)
{
    const float rollLengthInBeats = this->rollLastBeat - this->rollFirstBeat;
    const float projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    for (const auto &c : this->patternMap)
    {
        const auto sequenceMap = c.second.get();
        const bool isActiveClip = this->activeClip == c.first;

        g.setColour(c.first.getTrackColour().
            interpolatedWith(this->baseColour, .4f).
            withAlpha(isActiveClip ? .9f : .6f));

        for (const auto &n : *sequenceMap)
        {
            const auto key = jlimit(0, 128, n.getKey() + c.first.getKey());
            const auto beat = n.getBeat() + c.first.getBeat() - this->rollFirstBeat;
            const auto length = n.getLength();

            const float x = (mapWidth * (beat / projectLengthInBeats));
            const float w = (mapWidth * (length / projectLengthInBeats));
            const float y = roundf(this->getHeight() - (key * this->componentHeight));

            g.fillRect(x, y, jmax(0.25f, w), 1.0f);
        }
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

#define forEachSequenceMapOfGivenTrack(map, child, track) \
    for (const auto &child : map) \
        if (child.first.getPattern()->getTrack() == track)

void PianoProjectMap::onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2)
{
    if (e1.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(e1);
        const Note &newNote = static_cast<const Note &>(e2);
        const auto *track = newNote.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (sequenceMap.contains(note))
            {
                sequenceMap.erase(note);
                sequenceMap.insert(newNote);
            }
        }

        this->triggerAsyncUpdate();
    }
}

void PianoProjectMap::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            sequenceMap.insert(note);
        }

        this->triggerAsyncUpdate();
    }
}

void PianoProjectMap::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (sequenceMap.contains(note))
            {
                sequenceMap.erase(note);
            }
        }

        this->triggerAsyncUpdate();
    }
}

void PianoProjectMap::onAddClip(const Clip &clip)
{
    const SequenceSet *referenceMap = nullptr;
    const auto *track = clip.getPattern()->getTrack();
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        // Found a sequence map for the same track
        referenceMap = c.second.get();
        break;
    }

    if (referenceMap == nullptr)
    {
        jassertfalse;
        return;
    }

    auto sequenceMap = new SequenceSet();
    this->patternMap[clip] = UniquePointer<SequenceSet>(sequenceMap);
        
    for (const auto &note : *referenceMap)
    {
        sequenceMap->insert(note);
    }

    this->triggerAsyncUpdate();
}

void PianoProjectMap::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->patternMap.contains(clip))
    {
        // Set new key for existing sequence map
        auto *sequenceMap = this->patternMap[clip].release();
        this->patternMap.erase(clip);
        this->patternMap[newClip] = UniquePointer<SequenceSet>(sequenceMap);
        this->triggerAsyncUpdate();
    }
}

void PianoProjectMap::onRemoveClip(const Clip &clip)
{
    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
        this->triggerAsyncUpdate();
    }
}

void PianoProjectMap::onChangeTrackProperties(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }
    this->triggerAsyncUpdate();
}

void PianoProjectMap::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadTrackMap();
}

void PianoProjectMap::onAddTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }
    this->loadTrack(track);
    this->triggerAsyncUpdate();
}

void PianoProjectMap::onRemoveTrack(MidiTrack *const track)
{
    if (!dynamic_cast<const PianoSequence *>(track->getSequence())) { return; }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto &clip = *track->getPattern()->getUnchecked(i);
        if (this->patternMap.contains(clip))
        {
            this->patternMap.erase(clip);
        }
    }

    this->triggerAsyncUpdate();
}

void PianoProjectMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat ||
        this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
        //this->resized(); // seems to cause glitches sometimes?
    }
}

void PianoProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->rollFirstBeat = firstBeat;
    this->rollLastBeat = lastBeat;
    //this->resized(); // seems to cause glitches sometimes?
}

void PianoProjectMap::onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool)
{
    if (this->activeClip == clip)
    {
        return;
    }

    this->activeClip = clip;
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void PianoProjectMap::reloadTrackMap()
{
    this->patternMap.clear();

    const auto &tracks = this->project.getTracks();
    for (const auto *track : tracks)
    {
        if (dynamic_cast<const PianoSequence *>(track->getSequence()))
        {
            this->loadTrack(track);
        }
    }

    this->triggerAsyncUpdate();
}

void PianoProjectMap::loadTrack(const MidiTrack *const track)
{
    if (track->getPattern() == nullptr)
    {
        return;
    }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const Clip *clip = track->getPattern()->getUnchecked(i);

        auto *sequenceMap = new SequenceSet();
        this->patternMap[*clip] = UniquePointer<SequenceSet>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Note))
            {
                const Note *note = static_cast<const Note *>(event);
                sequenceMap->insert(*note);
            }
        }
    }
}

void PianoProjectMap::handleAsyncUpdate()
{
    this->repaint();
}
