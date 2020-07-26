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
#include "PianoClipComponent.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "PlayerThread.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "MidiTrack.h"
#include "PatternRoll.h"

PianoClipComponent::PianoClipComponent(ProjectNode &project, MidiSequence *sequence,
    HybridRoll &roll, const Clip &clip) :
    ClipComponent(roll, clip),
    project(project),
    sequence(sequence)
{
    this->setPaintingIsUnclipped(true);
    this->reloadTrackMap();
    this->project.addListener(this);
}

PianoClipComponent::~PianoClipComponent()
{
    this->project.removeListener(this);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoClipComponent::paint(Graphics &g)
{
    // Draw the frame, set the colour, etc:
    ClipComponent::paint(g);

    for (const auto &note : this->displayedNotes)
    {
        const auto *ns = note.getSequence();
        const float sequenceLength = ns->getLengthInBeats();
        const float beat = note.getBeat() - ns->getFirstBeat();
        const auto key = jlimit(0, this->keyboardSize, note.getKey() + this->clip.getKey());
        const float x = static_cast<float>(this->getWidth()) * (beat / sequenceLength);
        const float w = static_cast<float>(this->getWidth()) * (note.getLength() / sequenceLength);
        const float h = static_cast<float>(this->getHeight());
        const int y = static_cast<int>(h - key * h / static_cast<float>(this->keyboardSize));
        g.fillRect(x, static_cast<float>(y), jmax(0.25f, w), 1.f);
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoClipComponent::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(oldEvent);
        const Note &newNote = static_cast<const Note &>(newEvent);
        if (newNote.getSequence() != this->sequence) { return; }

        if (this->displayedNotes.contains(note))
        {
            this->displayedNotes.erase(note);
            this->displayedNotes.insert(newNote);
        }

        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        if (note.getSequence() != this->sequence) { return; }

        this->displayedNotes.insert(note);
        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        if (note.getSequence() != this->sequence) { return; }

        if (this->displayedNotes.contains(note))
        {
            this->displayedNotes.erase(note);
        }

        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (this->clip == oldClip)
    {
        this->updateColours(); // transparency depends on clip velocity
        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onChangeTrackProperties(MidiTrack *const track)
{
    if (track->getSequence() != this->sequence) { return; }
    this->roll.triggerBatchRepaintFor(this);
}

void PianoClipComponent::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    if (this->sequence != nullptr)
    {
        this->keyboardSize = this->project.getProjectInfo()->getKeyboardSize();
        this->reloadTrackMap();
    }
}

void PianoClipComponent::onChangeProjectInfo(const ProjectMetadata *info)
{
    if (this->keyboardSize != info->getKeyboardSize())
    {
        this->keyboardSize = info->getKeyboardSize();
        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onAddTrack(MidiTrack *const track)
{
    if (track->getSequence() == this->sequence &&
        track->getSequence()->size() > 0)
    {
        this->reloadTrackMap();
        this->roll.triggerBatchRepaintFor(this);
    }
}

void PianoClipComponent::onRemoveTrack(MidiTrack *const track)
{
    if (track->getSequence() != this->sequence) { return; }

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const Note &note = static_cast<const Note &>(*track->getSequence()->getUnchecked(i));
        if (this->displayedNotes.contains(note))
        {
            this->displayedNotes.erase(note);
        }
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void PianoClipComponent::reloadTrackMap()
{
    this->displayedNotes.clear();

    for (auto *track : this->project.getTracks())
    {
        if (track->getSequence() != this->sequence) { continue; }

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            auto *event = track->getSequence()->getUnchecked(j);
            if (event != nullptr && event->isTypeOf(MidiEvent::Type::Note))
            {
                auto *note = static_cast<Note *>(event);
                this->displayedNotes.insert(*note);
            }
        }
    }
}

void PianoClipComponent::setShowRecordingMode(bool isRecording)
{
    this->flags.isRecordingTarget = isRecording;
    this->updateColours();
    this->roll.triggerBatchRepaintFor(this);
}
