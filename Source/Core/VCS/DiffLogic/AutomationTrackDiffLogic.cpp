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

using namespace VCS;
using namespace Serialization::VCS;

static ValueTree mergePath(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeMute(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeColour(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeInstrument(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeController(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeEventsAdded(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeEventsRemoved(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeEventsChanged(const ValueTree &state, const ValueTree &changes);

static DeltaDiff createPathDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createMuteDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createColourDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createInstrumentDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createControllerDiff(const ValueTree &state, const ValueTree &changes);

static Array<DeltaDiff> createEventsDiffs(const ValueTree &state, const ValueTree &changes);

static void deserializeChanges(const ValueTree &state, const ValueTree &changes,
    OwnedArray<MidiEvent> &stateNotes, OwnedArray<MidiEvent> &changesNotes);

static DeltaDiff serializeChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges, const Identifier &deltaType);

static ValueTree serializeLayer(Array<const MidiEvent *> changes, const Identifier &tag);
static bool checkIfDeltaIsEventsType(const Delta *delta);

AutomationTrackDiffLogic::AutomationTrackDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const Identifier VCS::AutomationTrackDiffLogic::getType() const
{
    return Serialization::Core::automationTrack;
}

Diff *AutomationTrackDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

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
        ValueTree stateDeltaData;

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
            else if (myDelta->hasType(MidiTrackDeltas::trackController))
            {
                diff->applyDelta(createControllerDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(AutoSequenceDeltas::eventsAdded))
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

Diff *AutomationTrackDiffLogic::createMergedItem(const TrackedItem &initialState) const
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

        ScopedPointer<Delta> eventsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            AutoSequenceDeltas::eventsAdded));

        ValueTree eventsDeltaData;

        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        ValueTree clipsDeltaData;

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
                else if (targetDelta->hasType(MidiTrackDeltas::trackController))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeController(stateDeltaData, targetDeltaData);
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
                    eventsDeltaData = mergeEventsAdded(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AutoSequenceDeltas::eventsRemoved))
                {
                    eventsDeltaData = mergeEventsRemoved(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AutoSequenceDeltas::eventsChanged))
                {
                    eventsDeltaData = mergeEventsChanged(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
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

ValueTree mergeController(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeEventsAdded(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeChanges(state, changes, stateNotes, changesNotes);

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

    return serializeLayer(result, AutoSequenceDeltas::eventsAdded);
}

ValueTree mergeEventsRemoved(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeChanges(state, changes, stateNotes, changesNotes);

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

    return serializeLayer(result, AutoSequenceDeltas::eventsAdded);
}

ValueTree mergeEventsChanged(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    deserializeChanges(state, changes, stateNotes, changesNotes);

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

    return serializeLayer(result, AutoSequenceDeltas::eventsAdded);
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
    res.delta = new Delta(muted ? DeltaDescription("muted") :
        DeltaDescription("unmuted"),
        MidiTrackDeltas::trackMute);
    return res;
}

DeltaDiff createColourDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("color changed"), MidiTrackDeltas::trackColour);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createInstrumentDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("instrument changed"), MidiTrackDeltas::trackInstrument);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createControllerDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("controller changed"), MidiTrackDeltas::trackController);
    res.deltaData = changes.createCopy();
    return res;
}

Array<DeltaDiff> createEventsDiffs(const ValueTree &state, const ValueTree &changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    deserializeChanges(state, changes, stateEvents, changesEvents);

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
        res.add(serializeChanges(addedEvents,
            "added {x} events",
            addedEvents.size(),
            AutoSequenceDeltas::eventsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeChanges(removedEvents,
            "removed {x} events",
            removedEvents.size(),
            AutoSequenceDeltas::eventsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeChanges(changedEvents,
            "changed {x} events",
            changedEvents.size(),
            AutoSequenceDeltas::eventsChanged));
    }

    return res;
}

void deserializeChanges(const ValueTree &state, const ValueTree &changes,
        OwnedArray<MidiEvent> &stateNotes, OwnedArray<MidiEvent> &changesNotes)
{
    if (state.isValid())
    {
        forEachValueTreeChildWithType(state, e, Serialization::Midi::automationEvent)
        {
            auto event = new AutomationEvent();
            event->deserialize(e);
            stateNotes.addSorted(*event, event);
        }
    }

    if (changes.isValid())
    {
        forEachValueTreeChildWithType(changes, e, Serialization::Midi::automationEvent)
        {
            auto event = new AutomationEvent();
            event->deserialize(e);
            changesNotes.addSorted(*event, event);
        }
    }
}

DeltaDiff serializeChanges(Array<const MidiEvent *> changes,
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

bool checkIfDeltaIsEventsType(const Delta *d)
{
    return (d->hasType(AutoSequenceDeltas::eventsAdded) ||
            d->hasType(AutoSequenceDeltas::eventsChanged) ||
            d->hasType(AutoSequenceDeltas::eventsRemoved));
}
