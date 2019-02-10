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
#include "PianoTrackDiffLogic.h"
#include "PianoTrackNode.h"
#include "PatternDiffHelpers.h"
#include "Note.h"
#include "PianoSequence.h"
#include "SerializationKeys.h"

using namespace VCS;
using namespace Serialization::VCS;

static ValueTree mergePath(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeMute(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeColour(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeInstrument(const ValueTree &state, const ValueTree &changes);

static ValueTree mergeNotesAdded(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeNotesRemoved(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeNotesChanged(const ValueTree &state, const ValueTree &changes);

static DeltaDiff createPathDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createMuteDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createColourDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createInstrumentDiff(const ValueTree &state, const ValueTree &changes);

static Array<DeltaDiff> createEventsDiffs(const ValueTree &state, const ValueTree &changes);

static void deserializeLayerChanges(const ValueTree &state, const ValueTree &changes,
    OwnedArray<Note> &stateNotes, OwnedArray<Note> &changesNotes);

static DeltaDiff serializeLayerChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges,  const Identifier &deltaType);

static ValueTree serializeLayer(Array<const MidiEvent *> changes, const Identifier &tag);
static bool checkIfDeltaIsNotesType(const Delta *delta);


PianoTrackDiffLogic::PianoTrackDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const Identifier PianoTrackDiffLogic::getType() const
{
    return Serialization::Core::pianoTrack;
}

Diff *PianoTrackDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);

        const auto myDeltaData(this->target.getDeltaData(i));
        ValueTree stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->hasType(stateDelta->getType()))
            {
                deltaFoundInState = true;
                stateDeltaData = initialState.getDeltaData(j);
                dataHasChanged = (! myDeltaData.isEquivalentTo(stateDeltaData));
                break;
            }
        }

        if (!deltaFoundInState || dataHasChanged)
        {
            if (myDelta->hasType(MidiTrackDeltas::trackPath))
            {
                diff->applyDelta(createPathDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackMute))
            {
                diff->applyDelta(createMuteDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackColour))
            {
                diff->applyDelta(createColourDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackInstrument))
            {
                diff->applyDelta(createInstrumentDiff(stateDeltaData, myDeltaData));
            }
            // дифф рассчитывает, что у состояния будет одна нотная дельта типа notesAdded
            // остальные тут не имеют смысла //else if (this->checkIfDeltaIsNotesType(myDelta))
            else if (myDelta->hasType(PianoSequenceDeltas::notesAdded))
            {
                diff->applyDeltas(createEventsDiffs(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(PatternDeltas::clipsAdded))
            {
                diff->applyDeltas(PatternDiffHelpers::createClipsDiffs(stateDeltaData, myDeltaData));
            }
        }
    }

    return diff;
}


Diff *PianoTrackDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    // step 1:
    // the default policy is merging all changes
    // from changes into target (of corresponding types)
    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        const auto stateDeltaData(initialState.getDeltaData(i));

        bool deltaFoundInChanges = false;

        // for every supported type we need to spit out 
        // a delta of type eventsAdded with all events merged in there

        ScopedPointer<Delta> notesDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PianoSequenceDeltas::notesAdded));

        ValueTree notesDeltaData;

        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        ValueTree clipsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            if (stateDelta->hasType(targetDelta->getType()))
            {
                deltaFoundInChanges = true;

                if (targetDelta->hasType(MidiTrackDeltas::trackPath))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergePath(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackMute))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeMute(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackColour))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeColour(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackInstrument))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeInstrument(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                checkIfDeltaIsNotesType(stateDelta) &&
                checkIfDeltaIsNotesType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = notesDeltaData.isValid();

                if (targetDelta->hasType(PianoSequenceDeltas::notesAdded))
                {
                    notesDeltaData = mergeNotesAdded(incrementalMerge ? notesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PianoSequenceDeltas::notesRemoved))
                {
                    notesDeltaData = mergeNotesRemoved(incrementalMerge ? notesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PianoSequenceDeltas::notesChanged))
                {
                    notesDeltaData = mergeNotesChanged(incrementalMerge ? notesDeltaData : stateDeltaData, targetDeltaData);
                }
            }

            const bool bothDeltasArePatternType =
                PatternDiffHelpers::checkIfDeltaIsPatternType(stateDelta) &&
                PatternDiffHelpers::checkIfDeltaIsPatternType(targetDelta);

            if (bothDeltasArePatternType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = clipsDeltaData.isValid();

                if (targetDelta->hasType(PatternDeltas::clipsAdded))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsAdded(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PatternDeltas::clipsRemoved))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsRemoved(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PatternDeltas::clipsChanged))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsChanged(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
            }
        }

        if (notesDeltaData.isValid())
        {
            diff->applyDelta(notesDelta.release(), notesDeltaData);
        }

        if (clipsDeltaData.isValid())
        {
            diff->applyDelta(clipsDelta.release(), clipsDeltaData);
        }

        if (! deltaFoundInChanges)
        {
            diff->applyDelta(stateDelta->createCopy(), stateDeltaData);
        }
    }

    // step 2:
    // resolve new delta types that may be missing in project history state,
    // e.g., a project that was created with earlier versions of the app,
    // which has a history tree with tracks initialised without patterns/clips
    // which was introduced later. As the app evolves, new types of deltas are
    // to be resolved here (for example, I'll need a `solo` track flag in future)

    bool stateHasClips = false;
    // TODO: bool stateHasSoloFlags = false;

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        stateHasClips = stateHasClips || PatternDiffHelpers::checkIfDeltaIsPatternType(stateDelta);
    }

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        ValueTree clipsDeltaData;
        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));
            const bool foundMissingClip = !stateHasClips && PatternDiffHelpers::checkIfDeltaIsPatternType(targetDelta);
            if (foundMissingClip)
            {
                ValueTree emptyClipDeltaData(serializeLayer({}, PatternDeltas::clipsAdded));
                const bool incrementalMerge = clipsDeltaData.isValid();

                if (targetDelta->hasType(PatternDeltas::clipsAdded))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsAdded(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PatternDeltas::clipsRemoved))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsRemoved(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(PatternDeltas::clipsChanged))
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsChanged(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
            }
        }
        
        if (clipsDeltaData.isValid())
        {
            diff->applyDelta(clipsDelta.release(), clipsDeltaData);
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Merge
//===----------------------------------------------------------------------===//

ValueTree mergePath(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeMute(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeColour(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeInstrument(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeNotesAdded(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    deserializeLayerChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // на всякий пожарный, ищем, нет ли в состоянии нот с теми же id, где нет - добавляем
    HashMap<MidiEvent::Id, int> stateIDs;
    
    for (int j = 0; j < stateNotes.size(); ++j)
    {
        const Note *stateNote(stateNotes.getUnchecked(j));
        stateIDs.set(stateNote->getId(), j);
    }

    for (int i = 0; i < changesNotes.size(); ++i)
    {
        const Note *changesNote(changesNotes.getUnchecked(i));
        bool foundNoteInState = stateIDs.contains(changesNote->getId());

        if (! foundNoteInState)
        {
            result.add(changesNote);
        }
    }

    return serializeLayer(result, PianoSequenceDeltas::notesAdded);
}

ValueTree mergeNotesRemoved(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    deserializeLayerChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    // добавляем все ноты из состояния, которых нет в изменениях
    HashMap<MidiEvent::Id, int> changesIDs;

    for (int j = 0; j < changesNotes.size(); ++j)
    {
        const Note *changesNote(changesNotes.getUnchecked(j));
        changesIDs.set(changesNote->getId(), j);
    }
    
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        const Note *stateNote(stateNotes.getUnchecked(i));
        const bool foundNoteInChanges = changesIDs.contains(stateNote->getId());
        if (! foundNoteInChanges)
        {
            result.add(stateNote);
        }
    }

    return serializeLayer(result, PianoSequenceDeltas::notesAdded);
}

ValueTree mergeNotesChanged(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    deserializeLayerChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // снова ищем по id и заменяем
    HashMap<MidiEvent::Id, const Note *> changesIDs;
    
    for (int j = 0; j < changesNotes.size(); ++j)
    {
        const Note *changesNote(changesNotes.getUnchecked(j));
        changesIDs.set(changesNote->getId(), changesNote);
    }

    for (int i = 0; i < stateNotes.size(); ++i)
    {
        const Note *stateNote(stateNotes.getUnchecked(i));
        if (changesIDs.contains(stateNote->getId()))
        {
            const Note *changesNote = changesIDs[stateNote->getId()];
            result.removeAllInstancesOf(stateNote);
            result.addIfNotAlreadyThere(changesNote);
        }
    }

    return serializeLayer(result, PianoSequenceDeltas::notesAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

DeltaDiff createPathDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.deltaData = changes.createCopy();
    res.delta = new Delta(DeltaDescription("moved from {x}",
        state.getProperty(Serialization::VCS::delta).toString()),
        MidiTrackDeltas::trackPath);
    return res;
}

DeltaDiff createMuteDiff(const ValueTree &state, const ValueTree &changes)
{
    const bool muted = MidiTrack::isTrackMuted(changes.getProperty(Serialization::VCS::delta));
    DeltaDiff res;
    res.deltaData = changes.createCopy();
    res.delta = new Delta(muted ? 
        DeltaDescription("muted") : DeltaDescription("unmuted"),
        MidiTrackDeltas::trackMute);
    return res;
}

DeltaDiff createColourDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("color changed"), 
        MidiTrackDeltas::trackColour);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createInstrumentDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("instrument changed"), 
        MidiTrackDeltas::trackInstrument);
    res.deltaData = changes.createCopy();
    return res;
}

Array<DeltaDiff> createEventsDiffs(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    deserializeLayerChanges(state, changes, stateNotes, changesNotes);

    Array<DeltaDiff> res;


    Array<const MidiEvent *> addedNotes;
    Array<const MidiEvent *> removedNotes;
    Array<const MidiEvent *> changedNotes;

    // собственно, само сравнение
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const Note *stateNote(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const Note *changesNote(changesNotes.getUnchecked(j));

            // нота из состояния - существует в изменениях. добавляем запись changed, если нужно.
            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInChanges = true;

                const bool noteHasChanged =
                    (stateNote->getKey() != changesNote->getKey() ||
                    stateNote->getBeat() != changesNote->getBeat() ||
                    stateNote->getLength() != changesNote->getLength() ||
                    stateNote->getVelocity() != changesNote->getVelocity());

                if (noteHasChanged)
                {
                    changedNotes.add(changesNote);
                }

                break;
            }
        }

        // нота из состояния - в изменениях не найдена. добавляем запись removed.
        if (! foundNoteInChanges)
        {
            removedNotes.add(stateNote);
        }
    }

    // теперь ищем в изменениях ноты, которые отсутствуют в состоянии
    for (int i = 0; i < changesNotes.size(); ++i)
    {
        bool foundNoteInState = false;
        const Note *changesNote(changesNotes.getUnchecked(i));

        for (int j = 0; j < stateNotes.size(); ++j)
        {
            const Note *stateNote(stateNotes.getUnchecked(j));

            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInState = true;
                break;
            }
        }

        // и пишем ее в список добавленных
        if (! foundNoteInState)
        {
            addedNotes.add(changesNote);
        }
    }

    // сериализуем диффы, если таковые есть

    if (addedNotes.size() > 0)
    {
        res.add(serializeLayerChanges(addedNotes,
            "added {x} notes",
            addedNotes.size(),
            PianoSequenceDeltas::notesAdded));
    }

    if (removedNotes.size() > 0)
    {
        res.add(serializeLayerChanges(removedNotes,
            "removed {x} notes",
            removedNotes.size(),
            PianoSequenceDeltas::notesRemoved));
    }

    if (changedNotes.size() > 0)
    {
        res.add(serializeLayerChanges(changedNotes,
            "changed {x} notes",
            changedNotes.size(),
            PianoSequenceDeltas::notesChanged));
    }

    return res;
}


void deserializeLayerChanges(const ValueTree &state, const ValueTree &changes,
        OwnedArray<Note> &stateNotes, OwnedArray<Note> &changesNotes)
{
    if (state.isValid())
    {
        forEachValueTreeChildWithType(state, e, Serialization::Midi::note)
        {
            auto note = new Note();
            note->deserialize(e);
            stateNotes.addSorted(*note, note);
        }
    }

    if (changes.isValid())
    {
        forEachValueTreeChildWithType(changes, e, Serialization::Midi::note)
        {
            auto note = new Note();
            note->deserialize(e);
            changesNotes.addSorted(*note, note);
        }
    }
}

DeltaDiff serializeLayerChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const Identifier &deltaType)
{
    DeltaDiff changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = serializeLayer(changes, deltaType);
    return changesFullDelta;
}

ValueTree serializeLayer(Array<const MidiEvent *> changes, const Identifier &tag)
{
    ValueTree tree(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}

bool checkIfDeltaIsNotesType(const Delta *d)
{
    return (d->hasType(PianoSequenceDeltas::notesAdded) ||
            d->hasType(PianoSequenceDeltas::notesRemoved) ||
            d->hasType(PianoSequenceDeltas::notesChanged));
}
