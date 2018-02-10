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
#include "PianoTrackTreeItem.h"
#include "PianoSequenceDeltas.h"
#include "MidiTrackDeltas.h"
#include "PatternDiffHelpers.h"
#include "Note.h"
#include "PianoSequence.h"
#include "SerializationKeys.h"

using namespace VCS;

static XmlElement *mergePath(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeMute(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeColour(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeInstrument(const XmlElement *state, const XmlElement *changes);

static XmlElement *mergeNotesAdded(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeNotesRemoved(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeNotesChanged(const XmlElement *state, const XmlElement *changes);

static NewSerializedDelta createPathDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createMuteDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createColourDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createInstrumentDiff(const XmlElement *state, const XmlElement *changes);

static Array<NewSerializedDelta> createEventsDiffs(const XmlElement *state, const XmlElement *changes);

static void deserializeLayerChanges(const XmlElement *state, const XmlElement *changes,
    OwnedArray<Note> &stateNotes, OwnedArray<Note> &changesNotes);

static NewSerializedDelta serializeLayerChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges,  const String &deltaType);

static XmlElement *serializeLayer(Array<const MidiEvent *> changes, const String &tag);
static bool checkIfDeltaIsNotesType(const Delta *delta);


PianoTrackDiffLogic::PianoTrackDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const String PianoTrackDiffLogic::getType() const
{
    return Serialization::Core::pianoLayer;
}

// assuming this is used only on checkout and resetting changes
void PianoTrackDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *PianoTrackDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    // на входе - два набора дельт и их данных
    // на выходе один набор дельт, который потом станет RevisionItem'ом
    // и который потом будет передаваться на мерж при перемещении хэда или мерже ревизий.

    // политика такая - если дельта определенного типа (например, LayerPath) не найдена в источнике
    // или найдена, но не равна источнику !XmlElement::isEquivalentTo:
    // для простых свойств диффом будет замена старого новым,
    // для слоев событий или списков аннотация (которые я планирую сделать потом) - полноценный дифф

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);

        ScopedPointer<XmlElement> myDeltaData(this->target.createDeltaDataFor(i));
        ScopedPointer<XmlElement> stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->getType() == stateDelta->getType())
            {
                deltaFoundInState = true;
                stateDeltaData = initialState.createDeltaDataFor(j);
                dataHasChanged = (! myDeltaData->isEquivalentTo(stateDeltaData, true));
                break;
            }
        }

        if (!deltaFoundInState || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->getType() == MidiTrackDeltas::trackPath)
            {
                NewSerializedDelta fullDelta = createPathDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == MidiTrackDeltas::trackMute)
            {
                NewSerializedDelta fullDelta = createMuteDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == MidiTrackDeltas::trackColour)
            {
                NewSerializedDelta fullDelta = createColourDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == MidiTrackDeltas::trackInstrument)
            {
                NewSerializedDelta fullDelta = createInstrumentDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            // дифф рассчитывает, что у состояния будет одна нотная дельта типа notesAdded
            // остальные тут не имеют смысла //else if (this->checkIfDeltaIsNotesType(myDelta))
            else if (myDelta->getType() == PianoSequenceDeltas::notesAdded)
            {
                Array<NewSerializedDelta> fullDeltas = 
                    createEventsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
            else if (myDelta->getType() == PatternDeltas::clipsAdded)
            {
                Array<NewSerializedDelta> fullDeltas = 
                    PatternDiffHelpers::createClipsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
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
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        bool deltaFoundInChanges = false;

        // for every supported type we need to spit out 
        // a delta of type eventsAdded with all events merged in there

        ScopedPointer<Delta> notesDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PianoSequenceDeltas::notesAdded));

        ScopedPointer<XmlElement> notesDeltaData;

        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        ScopedPointer<XmlElement> clipsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == MidiTrackDeltas::trackPath)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergePath(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == MidiTrackDeltas::trackMute)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeMute(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == MidiTrackDeltas::trackColour)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeColour(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == MidiTrackDeltas::trackInstrument)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeInstrument(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                checkIfDeltaIsNotesType(stateDelta) &&
                checkIfDeltaIsNotesType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = (notesDeltaData != nullptr);

                if (targetDelta->getType() == PianoSequenceDeltas::notesAdded)
                {
                    notesDeltaData = mergeNotesAdded(incrementalMerge ? notesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PianoSequenceDeltas::notesRemoved)
                {
                    notesDeltaData = mergeNotesRemoved(incrementalMerge ? notesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PianoSequenceDeltas::notesChanged)
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
                const bool incrementalMerge = (clipsDeltaData != nullptr);

                if (targetDelta->getType() == PatternDeltas::clipsAdded)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsAdded(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PatternDeltas::clipsRemoved)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsRemoved(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PatternDeltas::clipsChanged)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsChanged(incrementalMerge ? clipsDeltaData : stateDeltaData, targetDeltaData);
                }
            }
        }

        if (notesDeltaData != nullptr)
        {
            diff->addOwnedDelta(notesDelta.release(), notesDeltaData.release());
        }

        if (clipsDeltaData != nullptr)
        {
            diff->addOwnedDelta(clipsDelta.release(), clipsDeltaData.release());
        }

        if (! deltaFoundInChanges)
        {
            auto stateDeltaCopy = new Delta(*stateDelta);
            diff->addOwnedDelta(stateDeltaCopy, stateDeltaData.release());
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
        ScopedPointer<XmlElement> clipsDeltaData;
        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));
            const bool foundMissingClip = !stateHasClips && PatternDiffHelpers::checkIfDeltaIsPatternType(targetDelta);
            if (foundMissingClip)
            {
                ScopedPointer<XmlElement> emptyClipDeltaData(serializeLayer({}, PatternDeltas::clipsAdded));
                const bool incrementalMerge = (clipsDeltaData != nullptr);

                if (targetDelta->getType() == PatternDeltas::clipsAdded)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsAdded(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PatternDeltas::clipsRemoved)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsRemoved(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == PatternDeltas::clipsChanged)
                {
                    clipsDeltaData = PatternDiffHelpers::mergeClipsChanged(incrementalMerge ? clipsDeltaData : emptyClipDeltaData, targetDeltaData);
                }
            }
        }
        
        if (clipsDeltaData != nullptr)
        {
            diff->addOwnedDelta(clipsDelta.release(), clipsDeltaData.release());
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Merge
//===----------------------------------------------------------------------===//

XmlElement *mergePath(const XmlElement *state, const XmlElement *changes)
{
    return new XmlElement(*changes);
}

XmlElement *mergeMute(const XmlElement *state, const XmlElement *changes)
{
    return new XmlElement(*changes);
}

XmlElement *mergeColour(const XmlElement *state, const XmlElement *changes)
{
    return new XmlElement(*changes);
}

XmlElement *mergeInstrument(const XmlElement *state, const XmlElement *changes)
{
    return new XmlElement(*changes);
}

XmlElement *mergeNotesAdded(const XmlElement *state, const XmlElement *changes)
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

XmlElement *mergeNotesRemoved(const XmlElement *state, const XmlElement *changes)
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

XmlElement *mergeNotesChanged(const XmlElement *state, const XmlElement *changes)
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

NewSerializedDelta createPathDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(DeltaDescription("moved from {x}", state->getStringAttribute(Serialization::VCS::delta)),
                          MidiTrackDeltas::trackPath);
    return res;
}

NewSerializedDelta createMuteDiff(const XmlElement *state, const XmlElement *changes)
{
    const bool muted = MidiTrack::isTrackMuted(changes->getStringAttribute(Serialization::VCS::delta));
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(muted ? 
        DeltaDescription("muted") : DeltaDescription("unmuted"),
        MidiTrackDeltas::trackMute);
    return res;
}

NewSerializedDelta createColourDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("color changed"), 
        MidiTrackDeltas::trackColour);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta createInstrumentDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("instrument changed"), 
        MidiTrackDeltas::trackInstrument);
    res.deltaData = new XmlElement(*changes);
    return res;
}

Array<NewSerializedDelta> createEventsDiffs(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    deserializeLayerChanges(state, changes, stateNotes, changesNotes);

    Array<NewSerializedDelta> res;


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


void deserializeLayerChanges(const XmlElement *state,
        const XmlElement *changes,
        OwnedArray<Note> &stateNotes,
        OwnedArray<Note> &changesNotes)
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::note)
        {
            auto note = new Note();
            note->deserialize(*e);
            stateNotes.addSorted(*note, note);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::note)
        {
            auto note = new Note();
            note->deserialize(*e);
            changesNotes.addSorted(*note, note);
        }
    }
}

NewSerializedDelta serializeLayerChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const String &deltaType)
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = serializeLayer(changes, deltaType);
    return changesFullDelta;
}

XmlElement *serializeLayer(Array<const MidiEvent *> changes, const String &tag)
{
    ValueTree tree(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        tree.addChild(event->serialize());
    }

    return tree;
}

bool checkIfDeltaIsNotesType(const Delta *delta)
{
    return (delta->getType() == PianoSequenceDeltas::notesAdded ||
            delta->getType() == PianoSequenceDeltas::notesRemoved ||
            delta->getType() == PianoSequenceDeltas::notesChanged);
}
