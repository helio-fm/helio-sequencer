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
#include "AutomationTrackDiffLogic.h"
#include "AutomationTrackNode.h"
#include "PatternDiffHelpers.h"
#include "AutomationEvent.h"
#include "AutomationSequence.h"
#include "SerializationKeys.h"

namespace VCS
{

static SerializedData mergeAuthTrackPath(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoTrackColour(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoTrackInstrument(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoTrackController(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoEventsAdded(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoEventsRemoved(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAutoEventsChanged(const SerializedData &state, const SerializedData &changes);

static DeltaDiff createAutoTrackPathDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createAutoTrackColourDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createAutoTrackInstrumentDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createAutoTrackControllerDiff(const SerializedData &state, const SerializedData &changes);

static Array<DeltaDiff> createAutoEventsDiffs(const SerializedData &state, const SerializedData &changes);

static void deserializeAutoTrackChanges(const SerializedData &state, const SerializedData &changes,
    OwnedArray<MidiEvent> &stateNotes, OwnedArray<MidiEvent> &changesNotes);

static DeltaDiff serializeAutoTrackChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges, const Identifier &deltaType);

static SerializedData serializeAutoSequence(Array<const MidiEvent *> changes, const Identifier &tag);
static bool checkIfDeltaIsEventsType(const Delta *delta);

AutomationTrackDiffLogic::AutomationTrackDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const Identifier VCS::AutomationTrackDiffLogic::getType() const
{
    return Serialization::Core::automationTrack;
}

Diff *AutomationTrackDiffLogic::createDiff(const TrackedItem &initialState) const
{
    using namespace Serialization::VCS;

    auto *diff = new Diff(this->target);

    // на входе - два набора дельт и их данных
    // на выходе один набор дельт, который потом станет RevisionItem'ом
    // и который потом будет передаваться на мерж при перемещении хэда или мерже ревизий.

    // политика такая - если дельта определенного типа (например, LayerPath) не найдена в источнике
    // или найдена, но не равна источнику !XmlElement::isEquivalentTo:
    // для простых свойств диффом будет замена старого новым,
    // для слоев событий или списков аннотаций (которые я планирую сделать потом) - полноценный дифф

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);

        const auto myDeltaData(this->target.getDeltaData(i));
        SerializedData stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->getType() == stateDelta->getType())
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
                diff->applyDelta(createAutoTrackPathDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackColour))
            {
                diff->applyDelta(createAutoTrackColourDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackInstrument))
            {
                diff->applyDelta(createAutoTrackInstrumentDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(MidiTrackDeltas::trackController))
            {
                diff->applyDelta(createAutoTrackControllerDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(AutoSequenceDeltas::eventsAdded))
            {
                diff->applyDeltas(createAutoEventsDiffs(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(PatternDeltas::clipsAdded))
            {
                diff->applyDeltas(PatternDiffHelpers::createClipsDiffs(stateDeltaData, myDeltaData));
            }
        }
    }

    return diff;
}

Diff *AutomationTrackDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    using namespace Serialization::VCS;

    auto *diff = new Diff(this->target);

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

        UniquePointer<Delta> eventsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            AutoSequenceDeltas::eventsAdded));

        SerializedData eventsDeltaData;

        UniquePointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        SerializedData clipsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->hasType(MidiTrackDeltas::trackPath))
                {
                    UniquePointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    SerializedData diffDeltaData = mergeAuthTrackPath(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackColour))
                {
                    UniquePointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    SerializedData diffDeltaData = mergeAutoTrackColour(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackInstrument))
                {
                    UniquePointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    SerializedData diffDeltaData = mergeAutoTrackInstrument(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(MidiTrackDeltas::trackController))
                {
                    UniquePointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    SerializedData diffDeltaData = mergeAutoTrackController(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                checkIfDeltaIsEventsType(stateDelta) && 
                checkIfDeltaIsEventsType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = eventsDeltaData.isValid();

                if (targetDelta->hasType(AutoSequenceDeltas::eventsAdded))
                {
                    eventsDeltaData = mergeAutoEventsAdded(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AutoSequenceDeltas::eventsRemoved))
                {
                    eventsDeltaData = mergeAutoEventsRemoved(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AutoSequenceDeltas::eventsChanged))
                {
                    eventsDeltaData = mergeAutoEventsChanged(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
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

        if (eventsDeltaData.isValid())
        {
            diff->applyDelta(eventsDelta.release(), eventsDeltaData);
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
    // which was introduced later.

    bool stateHasClips = false;
    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        stateHasClips = stateHasClips || PatternDiffHelpers::checkIfDeltaIsPatternType(stateDelta);
    }

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        SerializedData clipsDeltaData;
        UniquePointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));
            const bool foundMissingClip = !stateHasClips && PatternDiffHelpers::checkIfDeltaIsPatternType(targetDelta);
            if (foundMissingClip)
            {
                SerializedData emptyClipDeltaData(serializeAutoSequence({}, PatternDeltas::clipsAdded));
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

SerializedData mergeAuthTrackPath(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeAutoTrackColour(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeAutoTrackInstrument(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeAutoTrackController(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeAutoEventsAdded(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeAutoTrackChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;
    result.addArray(stateNotes);

    // на всякий пожарный, ищем, нет ли в состоянии нот с теми же id, где нет - добавляем
    for (int i = 0; i < changesNotes.size(); ++i)
    {
        bool foundNoteInState = false;
        const AutomationEvent *changesNote = static_cast<AutomationEvent *>(changesNotes.getUnchecked(i));

        for (int j = 0; j < stateNotes.size(); ++j)
        {
            const AutomationEvent *stateNote = static_cast<AutomationEvent *>(stateNotes.getUnchecked(j));

            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInState = true;
                break;
            }
        }

        if (! foundNoteInState)
        {
            result.add(changesNote);
        }
    }

    return serializeAutoSequence(result, AutoSequenceDeltas::eventsAdded);
}

SerializedData mergeAutoEventsRemoved(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeAutoTrackChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    // добавляем все ноты из состояния, которых нет в изменениях
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AutomationEvent *stateNote = static_cast<AutomationEvent *>(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const AutomationEvent *changesNote = static_cast<AutomationEvent *>(changesNotes.getUnchecked(j));

            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInChanges = true;
                break;
            }
        }

        if (! foundNoteInChanges)
        {
            result.add(stateNote);
        }
    }

    return serializeAutoSequence(result, AutoSequenceDeltas::eventsAdded);
}

SerializedData mergeAutoEventsChanged(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeAutoTrackChanges(state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;
    result.addArray(stateNotes);

    // снова ищем по id и заменяем
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AutomationEvent *stateNote = static_cast<AutomationEvent *>(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const AutomationEvent *changesNote = static_cast<AutomationEvent *>(changesNotes.getUnchecked(j));

            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInChanges = true;
                result.removeAllInstancesOf(stateNote);
                result.addIfNotAlreadyThere(changesNote);

                break;
            }
        }

        //jassert(foundNoteInChanges);
    }

    return serializeAutoSequence(result, AutoSequenceDeltas::eventsAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

DeltaDiff createAutoTrackPathDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.deltaData = changes.createCopy();
    res.delta.reset(new Delta(DeltaDescription("moved from {x}",
        state.getProperty(Serialization::VCS::delta).toString()),
        MidiTrackDeltas::trackPath));
    return res;
}

DeltaDiff createAutoTrackColourDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta.reset(new Delta(DeltaDescription("color changed"), MidiTrackDeltas::trackColour));
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createAutoTrackInstrumentDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta.reset(new Delta(DeltaDescription("instrument changed"), MidiTrackDeltas::trackInstrument));
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createAutoTrackControllerDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta.reset(new Delta(DeltaDescription("controller changed"), MidiTrackDeltas::trackController));
    res.deltaData = changes.createCopy();
    return res;
}

Array<DeltaDiff> createAutoEventsDiffs(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    deserializeAutoTrackChanges(state, changes, stateEvents, changesEvents);

    Array<DeltaDiff> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;

    // собственно, само сравнение
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AutomationEvent *stateEvent = static_cast<AutomationEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const AutomationEvent *changesEvent = static_cast<AutomationEvent *>(changesEvents.getUnchecked(j));

            // нота из состояния - существует в изменениях. добавляем запись changed, если нужно.
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundNoteInChanges = true;

                const bool eventHasChanged = (stateEvent->getBeat() != changesEvent->getBeat() ||
                                              stateEvent->getCurvature() != changesEvent->getCurvature() ||
                                              stateEvent->getControllerValue() != changesEvent->getControllerValue());

                if (eventHasChanged)
                {
                    changedEvents.add(changesEvent);
                }

                break;
            }
        }

        // нота из состояния - в изменениях не найдена. добавляем запись removed.
        if (! foundNoteInChanges)
        {
            removedEvents.add(stateEvent);
        }
    }

    // теперь ищем в изменениях ноты, которые отсутствуют в состоянии
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundNoteInState = false;
        const AutomationEvent *changesNote = static_cast<AutomationEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const AutomationEvent *stateNote = static_cast<AutomationEvent *>(stateEvents.getUnchecked(j));

            if (stateNote->getId() == changesNote->getId())
            {
                foundNoteInState = true;
                break;
            }
        }

        // и пишем ее в список добавленных
        if (! foundNoteInState)
        {
            addedEvents.add(changesNote);
        }
    }

    // сериализуем диффы, если таковые есть

    if (addedEvents.size() > 0)
    {
        res.add(serializeAutoTrackChanges(addedEvents,
            "added {x} events",
            addedEvents.size(),
            AutoSequenceDeltas::eventsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeAutoTrackChanges(removedEvents,
            "removed {x} events",
            removedEvents.size(),
            AutoSequenceDeltas::eventsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeAutoTrackChanges(changedEvents,
            "changed {x} events",
            changedEvents.size(),
            AutoSequenceDeltas::eventsChanged));
    }

    return res;
}

void deserializeAutoTrackChanges(const SerializedData &state, const SerializedData &changes,
        OwnedArray<MidiEvent> &stateNotes, OwnedArray<MidiEvent> &changesNotes)
{
    if (state.isValid())
    {
        forEachChildWithType(state, e, Serialization::Midi::automationEvent)
        {
            auto *event = new AutomationEvent();
            event->deserialize(e);
            stateNotes.addSorted(*event, event);
        }
    }

    if (changes.isValid())
    {
        forEachChildWithType(changes, e, Serialization::Midi::automationEvent)
        {
            auto *event = new AutomationEvent();
            event->deserialize(e);
            changesNotes.addSorted(*event, event);
        }
    }
}

DeltaDiff serializeAutoTrackChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges, const Identifier &deltaType)
{
    DeltaDiff changesFullDelta;
    changesFullDelta.delta.reset(new Delta(DeltaDescription(description, numChanges), deltaType));
    changesFullDelta.deltaData = serializeAutoSequence(changes, deltaType);
    return changesFullDelta;
}

SerializedData serializeAutoSequence(Array<const MidiEvent *> changes, const Identifier &tag)
{
    SerializedData tree(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

bool checkIfDeltaIsEventsType(const Delta *d)
{
    using namespace Serialization::VCS;
    return (d->hasType(AutoSequenceDeltas::eventsAdded) ||
            d->hasType(AutoSequenceDeltas::eventsChanged) ||
            d->hasType(AutoSequenceDeltas::eventsRemoved));
}

}
