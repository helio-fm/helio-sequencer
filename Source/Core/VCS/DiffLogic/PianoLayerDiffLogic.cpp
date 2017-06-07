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
#include "PianoLayerDiffLogic.h"
#include "PianoLayerTreeItem.h"
#include "PianoLayerDeltas.h"

#include "Note.h"
#include "PianoLayer.h"
#include "ProjectEventDispatcher.h"
#include "SerializationKeys.h"

using namespace VCS;

class EmptyEventDispatcher : public ProjectEventDispatcher
{
public:
    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override {}
    void onEventAdded(const MidiEvent &event) override {}
    void onEventRemoved(const MidiEvent &event) override {}
    void onLayerChanged(const MidiLayer *layer) override {}
    void onBeatRangeChanged() override {}
};


PianoLayerDiffLogic::PianoLayerDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem)
{

}

PianoLayerDiffLogic::~PianoLayerDiffLogic()
{

}

const String PianoLayerDiffLogic::getType() const
{
    return Serialization::Core::pianoLayer;
}


// предполагается, что это используется только применительно
// к айтемам проекта. в 2х случаях - при чекауте и при ресете изменений.
void PianoLayerDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *PianoLayerDiffLogic::createDiff(const TrackedItem &initialState) const
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
            if (myDelta->getType() == PianoLayerDeltas::layerPath)
            {
                NewSerializedDelta fullDelta = this->createPathDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == PianoLayerDeltas::layerMute)
            {
                NewSerializedDelta fullDelta = this->createMuteDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == PianoLayerDeltas::layerColour)
            {
                NewSerializedDelta fullDelta = this->createColourDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == PianoLayerDeltas::layerInstrument)
            {
                NewSerializedDelta fullDelta = this->createInstrumentDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            // дифф рассчитывает, что у состояния будет одна нотная дельта типа notesAdded
            // остальные тут не имеют смысла
            //else if (this->checkIfDeltaIsNotesType(myDelta))
            else if (myDelta->getType() == PianoLayerDeltas::notesAdded)
            {
                Array<NewSerializedDelta> fullDeltas = this->createEventsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
        }
    }

    return diff;
}


Diff *PianoLayerDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    // merge-политика по умолчанию:
    // на каждую дельту таргета пытаемся наложить все дельты изменений.
    // (если типы дельт соответствуют друг другу)

    // здесь кроется баг - если какого-то типа дельты не было в модели изначально, с первого коммита,
    // его не будет и в стейте, и он постоянно будет светиться в изменениях ((
    // по уму, сначала надо добавить те дельты, типов которых нет в стейте (пропуская дельты нот!)
    // но этот кейс, скорее всего, никогда не всплывет. на всякий случай - todo
    // (мне счас типа не до этого)
    
    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        bool deltaFoundInChanges = false;

        // для нот в итоге надо выдать одну дельту типа NotesAdded
        // на которую наложить все дельты изменений нот одно за другим.

        // todo правильно назвать эту дельту
        ScopedPointer<Delta> notesDelta(new Delta(DeltaDescription(Serialization::VCS::headStateDelta), PianoLayerDeltas::notesAdded));
        ScopedPointer<XmlElement> notesDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == PianoLayerDeltas::layerPath)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergePath(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == PianoLayerDeltas::layerMute)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeMute(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == PianoLayerDeltas::layerColour)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeColour(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == PianoLayerDeltas::layerInstrument)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeInstrument(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                this->checkIfDeltaIsNotesType(stateDelta) && this->checkIfDeltaIsNotesType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == PianoLayerDeltas::notesAdded)
                {
                    if (notesDeltaData != nullptr)
                    { notesDeltaData = this->mergeNotesAdded(notesDeltaData, targetDeltaData); }
                    else
                    { notesDeltaData = this->mergeNotesAdded(stateDeltaData, targetDeltaData); }
                }
                else if (targetDelta->getType() == PianoLayerDeltas::notesRemoved)
                {
                    if (notesDeltaData != nullptr)
                    { notesDeltaData = this->mergeNotesRemoved(notesDeltaData, targetDeltaData); }
                    else
                    { notesDeltaData = this->mergeNotesRemoved(stateDeltaData, targetDeltaData); }
                }
                else if (targetDelta->getType() == PianoLayerDeltas::notesChanged)
                {
                    if (notesDeltaData != nullptr)
                    { notesDeltaData = this->mergeNotesChanged(notesDeltaData, targetDeltaData); }
                    else
                    { notesDeltaData = this->mergeNotesChanged(stateDeltaData, targetDeltaData); }
                }
            }
        }

        // нужно будет именовать финальную дельту типа "xx notes"
        if (notesDeltaData != nullptr)
        {
            diff->addOwnedDelta(notesDelta.release(), notesDeltaData.release());
        }

        if (! deltaFoundInChanges)
        {
            auto stateDeltaCopy = new Delta(*stateDelta);
            diff->addOwnedDelta(stateDeltaCopy, stateDeltaData.release());
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Merge
//===----------------------------------------------------------------------===//

XmlElement *PianoLayerDiffLogic::mergePath(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *PianoLayerDiffLogic::mergeMute(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *PianoLayerDiffLogic::mergeColour(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *PianoLayerDiffLogic::mergeInstrument(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *PianoLayerDiffLogic::mergeNotesAdded(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    PianoLayer emptyLayer(dispatcher);
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // на всякий пожарный, ищем, нет ли в состоянии нот с теми же id, где нет - добавляем
    HashMap<MidiEvent::Id, int> stateIDs;
    
    for (int j = 0; j < stateNotes.size(); ++j)
    {
        const Note *stateNote(stateNotes.getUnchecked(j));
        stateIDs.set(stateNote->getID(), j);
    }

    for (int i = 0; i < changesNotes.size(); ++i)
    {
        //bool foundNoteInState = false;

        const Note *changesNote(changesNotes.getUnchecked(i));
        bool foundNoteInState = stateIDs.contains(changesNote->getID());

        //for (int j = 0; j < stateNotes.size(); ++j)
        //{
        //    const Note *stateNote = static_cast<Note *>(stateNotes.getUnchecked(j));

        //    if (stateNote->getID() == changesNote->getID())
        //    {
        //        foundNoteInState = true;
        //        break;
        //    }
        //}

        if (! foundNoteInState)
        {
            result.add(changesNote);
        }
    }

    return this->serializeLayer(result, PianoLayerDeltas::notesAdded);
}

XmlElement *PianoLayerDiffLogic::mergeNotesRemoved(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    PianoLayer emptyLayer(dispatcher);
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    // добавляем все ноты из состояния, которых нет в изменениях
    HashMap<MidiEvent::Id, int> changesIDs;

    for (int j = 0; j < changesNotes.size(); ++j)
    {
        const Note *changesNote(changesNotes.getUnchecked(j));
        changesIDs.set(changesNote->getID(), j);
    }
    
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        const Note *stateNote(stateNotes.getUnchecked(i));
        //bool foundNoteInChanges = false;
        bool foundNoteInChanges = changesIDs.contains(stateNote->getID());

        //for (int j = 0; j < changesNotes.size(); ++j)
        //{
        //    const Note *changesNote = static_cast<Note *>(changesNotes.getUnchecked(j));

        //    if (stateNote->getID() == changesNote->getID())
        //    {
        //        foundNoteInChanges = true;
        //        break;
        //    }
        //}

        if (! foundNoteInChanges)
        {
            result.add(stateNote);
        }
    }

    return this->serializeLayer(result, PianoLayerDeltas::notesAdded);
}

XmlElement *PianoLayerDiffLogic::mergeNotesChanged(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    PianoLayer emptyLayer(dispatcher);
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // снова ищем по id и заменяем
    HashMap<MidiEvent::Id, const Note *> changesIDs;
    
    for (int j = 0; j < changesNotes.size(); ++j)
    {
        const Note *changesNote(changesNotes.getUnchecked(j));
        changesIDs.set(changesNote->getID(), changesNote);
    }

    for (int i = 0; i < stateNotes.size(); ++i)
    {
        const Note *stateNote(stateNotes.getUnchecked(i));
        //bool foundNoteInChanges = false;

        if (changesIDs.contains(stateNote->getID()))
        {
            const Note *changesNote = changesIDs[stateNote->getID()];
            result.removeAllInstancesOf(stateNote);
            result.addIfNotAlreadyThere(changesNote);
        }

        //for (int j = 0; j < changesNotes.size(); ++j)
        //{
        //    const Note *changesNote = static_cast<Note *>(changesNotes.getUnchecked(j));

        //    if (stateNote->getID() == changesNote->getID())
        //    {
        //        foundNoteInChanges = true;
        //        result.removeAllInstancesOf(stateNote);
        //        result.addIfNotAlreadyThere(changesNote);

        //        break;
        //    }
        //}

        //jassert(foundNoteInChanges);
    }

    return this->serializeLayer(result, PianoLayerDeltas::notesAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

NewSerializedDelta PianoLayerDiffLogic::createPathDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(DeltaDescription("moved from {x}", state->getStringAttribute(Serialization::VCS::delta)),
                          PianoLayerDeltas::layerPath);
    return res;
}

NewSerializedDelta PianoLayerDiffLogic::createMuteDiff(const XmlElement *state, const XmlElement *changes) const
{
    const bool muted = MidiLayer::isMuted(changes->getStringAttribute(Serialization::VCS::delta));
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(muted ? DeltaDescription("muted") : DeltaDescription("unmuted"), PianoLayerDeltas::layerMute);
    return res;
}

NewSerializedDelta PianoLayerDiffLogic::createColourDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("color changed"), PianoLayerDeltas::layerColour);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta PianoLayerDiffLogic::createInstrumentDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("instrument changed"), PianoLayerDeltas::layerInstrument);
    res.deltaData = new XmlElement(*changes);
    return res;
}

Array<NewSerializedDelta> PianoLayerDiffLogic::createEventsDiffs(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    PianoLayer emptyLayer(dispatcher);
    OwnedArray<Note> stateNotes;
    OwnedArray<Note> changesNotes;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

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
            if (stateNote->getID() == changesNote->getID())
            {
                foundNoteInChanges = true;

                const bool noteHasChanged = (stateNote->getKey() != changesNote->getKey() ||
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

            if (stateNote->getID() == changesNote->getID())
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
        res.add(this->serializeChanges(addedNotes,
                                       "added {x} notes",
                                       addedNotes.size(),
                                       PianoLayerDeltas::notesAdded));
    }

    if (removedNotes.size() > 0)
    {
        res.add(this->serializeChanges(removedNotes,
                                       "removed {x} notes",
                                       removedNotes.size(),
                                       PianoLayerDeltas::notesRemoved));
    }

    if (changedNotes.size() > 0)
    {
        res.add(this->serializeChanges(changedNotes,
                                       "changed {x} notes",
                                       changedNotes.size(),
                                       PianoLayerDeltas::notesChanged));
    }

    return res;
}


void PianoLayerDiffLogic::deserializeChanges(MidiLayer &layer,
        const XmlElement *state,
        const XmlElement *changes,
        OwnedArray<Note> &stateNotes,
        OwnedArray<Note> &changesNotes) const
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::note)
        {
            auto note = new Note(&layer, 0, 0, 0, 0);
            note->deserialize(*e);
            stateNotes.addSorted(*note, note);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::note)
        {
            auto note = new Note(&layer, 0, 0, 0, 0);
            note->deserialize(*e);
            changesNotes.addSorted(*note, note);
        }
    }
}

NewSerializedDelta PianoLayerDiffLogic::serializeChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const String &deltaType) const
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = this->serializeLayer(changes, deltaType);
    return changesFullDelta;
}

XmlElement *PianoLayerDiffLogic::serializeLayer(Array<const MidiEvent *> changes,
        const String &tag) const
{
    auto xml = new XmlElement(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

bool PianoLayerDiffLogic::checkIfDeltaIsNotesType(const Delta *delta) const
{
    return (delta->getType() == PianoLayerDeltas::notesAdded ||
            delta->getType() == PianoLayerDeltas::notesRemoved ||
            //myDelta->getType() == PianoLayerDeltas::notesShifted ||
            //myDelta->getType() == PianoLayerDeltas::notesTransposed ||
            //myDelta->getType() == PianoLayerDeltas::notesTuned ||
            delta->getType() == PianoLayerDeltas::notesChanged);
}
