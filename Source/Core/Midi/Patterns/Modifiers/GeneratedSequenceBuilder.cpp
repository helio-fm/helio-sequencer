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
#include "GeneratedSequenceBuilder.h"
#include "PianoSequence.h"
#include "ProjectNode.h"
#include "MidiEvent.h"
#include "Pattern.h"

GeneratedSequenceBuilder::GeneratedSequenceBuilder(ProjectNode &project) :
    project(project)
{
    this->project.addListener(this);
}

GeneratedSequenceBuilder::~GeneratedSequenceBuilder()
{
    this->project.removeListener(this);
}

MidiSequence *GeneratedSequenceBuilder::getSequenceFor(const Clip &clip)
{
    if (!clip.isValid() || !clip.hasModifiers())
    {
        jassertfalse; // probably shouldn't be called in this case
        return nullptr;
    }

    const auto foundSequence = this->generatedSequences.find(clip);
    if (foundSequence == this->generatedSequences.end() || // not generated yet
        this->clipsToUpdate.contains(clip)) // or the cached sequence is stale
    {
        // this method was probably called before the update for this clip
        // was triggered, let's update it synchronously
        // (warning: some duplicate code here)

        auto *originalTrack = clip.getPattern()->getTrack();
        jassert(dynamic_cast<PianoSequence *>(originalTrack->getSequence()));
        auto sequence = make<PianoSequence>(*originalTrack, *this,
            static_cast<const PianoSequence &>(*originalTrack->getSequence()));

        for (const auto &modifier : clip.getModifiers())
        {
            modifier->processSequence(this->project, clip, *sequence);
        }

        this->project.broadcastReloadGeneratedSequence(clip, sequence.get());
        // todo test if beat range updates are needed:
        //this->project.broadcastChangeTrackBeatRange();

        this->generatedSequences[clip] = move(sequence);
        this->clipsToUpdate.erase(clip);

        return this->generatedSequences[clip].get();
    }

    return foundSequence->second.get();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void GeneratedSequenceBuilder::onAddMidiEvent(const MidiEvent &event)
{
    if (!event.isTypeOf(MidiEvent::Type::Auto))
    {
        this->triggerAsyncUpdatesForTrack(event.getSequence()->getTrack());
    }
}

void GeneratedSequenceBuilder::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (!newEvent.isTypeOf(MidiEvent::Type::Auto))
    {
        this->triggerAsyncUpdatesForTrack(newEvent.getSequence()->getTrack());
    }
}

void GeneratedSequenceBuilder::onRemoveMidiEvent(const MidiEvent &event)
{
    if (!event.isTypeOf(MidiEvent::Type::Auto))
    {
        this->triggerAsyncUpdatesForTrack(event.getSequence()->getTrack());
    }
}

void GeneratedSequenceBuilder::onAddClip(const Clip &clip)
{
    if (clip.hasModifiers())
    {
        this->triggerAsyncUpdateForClip(clip);
    }
}

void GeneratedSequenceBuilder::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (oldClip.getKey() != newClip.getKey() ||
        oldClip.getBeat() != newClip.getBeat() ||
        // pointer-wise comparison of modifier arrays here
        // as a minor trade-off for performance: we don't care much
        // if we may occasionally rebuild the same modifiers
        oldClip.getModifiers() != newClip.getModifiers())
    {
        this->triggerAsyncUpdateForClip(newClip);
    }
}

void GeneratedSequenceBuilder::onRemoveClip(const Clip &clip)
{
    jassert(!this->clipsToUpdate.contains(clip));
    this->clipsToUpdate.erase(clip); // just in case

    // clean up immediately to avoid dealing with deleted objects later
    if (this->generatedSequences.contains(clip))
    {
        this->project.broadcastReloadGeneratedSequence(clip, nullptr);
        this->generatedSequences.erase(clip);
        return;
    }
}

void GeneratedSequenceBuilder::onAddTrack(MidiTrack *const track)
{
    this->triggerAsyncUpdatesForTrack(track);
}

void GeneratedSequenceBuilder::onRemoveTrack(MidiTrack *const track)
{
    // clean up immediately to avoid dealing with deleted objects later
    for (auto it = this->clipsToUpdate.begin(); it != this->clipsToUpdate.end() ;)
    {
        if (it->getPattern()->getTrack() == track)
        {
            jassertfalse; // shouldn't happen I guess
            it = this->clipsToUpdate.erase(it);
        }
        else
        {
            it++;
        }
    }

    for (auto it = this->generatedSequences.begin(); it != this->generatedSequences.end() ;)
    {
        const auto &clip = it->first;
        if (clip.getPattern()->getTrack() == track)
        {
            this->project.broadcastReloadGeneratedSequence(clip, nullptr);
            it = this->generatedSequences.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void GeneratedSequenceBuilder::onReloadProjectContent(const Array<MidiTrack *> &tracks, const ProjectMetadata *meta)
{
    this->generatedSequences.clear();

    for (auto *track : tracks)
    {
        this->triggerAsyncUpdatesForTrack(track);
    }
}

void GeneratedSequenceBuilder::onChangeProjectInfo(const ProjectMetadata *info)
{
    // re-generate all, temperament might have changed:
    for (auto &it : this->generatedSequences)
    {
        this->triggerAsyncUpdateForClip(it.first);
    }
}

//===--------------------------------------------------------------------------===//
// Async updates
//===--------------------------------------------------------------------------===//

void GeneratedSequenceBuilder::triggerAsyncUpdatesForTrack(WeakReference<MidiTrack> track)
{
    jassert(track != nullptr);

    if (track->getPattern() == nullptr)
    {
        return;
    }

    for (const auto *clip : track->getPattern()->getClips())
    {
        if (clip->hasModifiers())
        {
            this->triggerAsyncUpdateForClip(*clip);
        }
    }
}

void GeneratedSequenceBuilder::triggerAsyncUpdateForClip(const Clip &clip)
{
    // clips are compared by ids, but their parameters could differ:
    this->clipsToUpdate.erase(clip);
    this->clipsToUpdate.insert(clip); // last update wins
    this->triggerAsyncUpdate();
}

void GeneratedSequenceBuilder::handleAsyncUpdate()
{
    if (this->clipsToUpdate.empty())
    {
        return;
    }

    for (const auto &clip : this->clipsToUpdate)
    {
        if (!clip.isValid())
        {
            jassertfalse;
            continue;
        }

        if (!clip.hasModifiers())
        {
            if (this->generatedSequences.contains(clip))
            {
                this->project.broadcastReloadGeneratedSequence(clip, nullptr);
                this->generatedSequences.erase(clip);
            }

            continue;
        }

        auto *originalTrack = clip.getPattern()->getTrack();
        jassert(dynamic_cast<PianoSequence *>(originalTrack->getSequence()));
        auto sequence = make<PianoSequence>(*originalTrack, *this,
            static_cast<const PianoSequence &>(*originalTrack->getSequence()));

        // the clip object stored in clipsToUpdate is a copy with all
        // the valid parameters, which we can use for generating a sequence,
        // but it will be erased after that, and we'll need a reference
        // to the original clip in the pattern to pass it to the listeners:
        const int i = originalTrack->getPattern()->indexOfSorted(&clip);
        if (i < 0)
        {
            jassertfalse; // deleted already? shouldn't happen ever
            continue;
        }

        const auto *originalClip = originalTrack->getPattern()->getUnchecked(i);

        for (const auto &modifier : originalClip->getModifiers())
        {
            modifier->processSequence(this->project, *originalClip, *sequence);
        }

        this->project.broadcastReloadGeneratedSequence(*originalClip, sequence.get());
        this->generatedSequences[clip] = move(sequence);
    }

    this->project.broadcastChangeProjectBeatRange();
    // todo test if beat range updates are needed:
    //this->project.broadcastChangeTrackBeatRange();

    this->clipsToUpdate.clear();
}
