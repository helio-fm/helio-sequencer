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
#include "AutomationTrackTreeItem.h"
#include "AutoSequenceDeltas.h"
#include "PatternDiffHelpers.h"
#include "AutomationEvent.h"
#include "AutomationSequence.h"
#include "SerializationKeys.h"

using namespace VCS;

static XmlElement *mergePath(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeMute(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeColour(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeInstrument(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeController(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeEventsAdded(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeEventsRemoved(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeEventsChanged(const XmlElement *state, const XmlElement *changes);

static NewSerializedDelta createPathDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createMuteDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createColourDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createInstrumentDiff(const XmlElement *state, const XmlElement *changes);
static NewSerializedDelta createControllerDiff(const XmlElement *state, const XmlElement *changes);

static Array<NewSerializedDelta> createEventsDiffs(const XmlElement *state, const XmlElement *changes);

static void deserializeChanges(const XmlElement *state,
    const XmlElement *changes,
    OwnedArray<MidiEvent> &stateNotes,
    OwnedArray<MidiEvent> &changesNotes);

static NewSerializedDelta serializeChanges(Array<const MidiEvent *> changes,
    const String &description,
    int64 numChanges,
    const String &deltaType);

static XmlElement *serializeLayer(Array<const MidiEvent *> changes, const String &tag);
static bool checkIfDeltaIsEventsType(const Delta *delta);

AutomationTrackDiffLogic::AutomationTrackDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const String AutomationTrackDiffLogic::getType() const
{
    return Serialization::Core::autoLayer;
}

// assuming this is used only on checkout and resetting changes
void AutomationTrackDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
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
            if (myDelta->getType() == AutoSequenceDeltas::layerPath)
            {
                NewSerializedDelta fullDelta = createPathDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoSequenceDeltas::layerMute)
            {
                NewSerializedDelta fullDelta = createMuteDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoSequenceDeltas::layerColour)
            {
                NewSerializedDelta fullDelta = createColourDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoSequenceDeltas::layerInstrument)
            {
                NewSerializedDelta fullDelta = createInstrumentDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoSequenceDeltas::layerController)
            {
                NewSerializedDelta fullDelta = createControllerDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoSequenceDeltas::eventsAdded)
            {
                Array<NewSerializedDelta> fullDeltas = createEventsDiffs(stateDeltaData, myDeltaData);

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

Diff *AutomationTrackDiffLogic::createMergedItem(const TrackedItem &initialState) const
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

        ScopedPointer<Delta> eventsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            AutoSequenceDeltas::eventsAdded));

        ScopedPointer<XmlElement> eventsDeltaData;

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

                if (targetDelta->getType() == AutoSequenceDeltas::layerPath)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergePath(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::layerMute)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeMute(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::layerColour)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeColour(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::layerInstrument)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeInstrument(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::layerController)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = mergeController(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                checkIfDeltaIsEventsType(stateDelta) && 
                checkIfDeltaIsEventsType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = (eventsDeltaData != nullptr);

                if (targetDelta->getType() == AutoSequenceDeltas::eventsAdded)
                {
                    eventsDeltaData = mergeEventsAdded(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::eventsRemoved)
                {
                    eventsDeltaData = mergeEventsRemoved(incrementalMerge ? eventsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == AutoSequenceDeltas::eventsChanged)
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

        if (eventsDeltaData != nullptr)
        {
            diff->addOwnedDelta(eventsDelta.release(), eventsDeltaData.release());
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
    // which was introduced later.

    bool stateHasClips = false;
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

XmlElement *mergeController(const XmlElement *state, const XmlElement *changes)
{
    return new XmlElement(*changes);
}

XmlElement *mergeEventsAdded(const XmlElement *state, const XmlElement *changes)
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

XmlElement *mergeEventsRemoved(const XmlElement *state, const XmlElement *changes)
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

XmlElement *mergeEventsChanged(const XmlElement *state, const XmlElement *changes)
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

NewSerializedDelta createPathDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(DeltaDescription("moved from {x}", state->getStringAttribute(Serialization::VCS::delta)),
                          AutoSequenceDeltas::layerPath);
    return res;
}

NewSerializedDelta createMuteDiff(const XmlElement *state, const XmlElement *changes)
{
    const bool muted = MidiTrack::isTrackMuted(changes->getStringAttribute(Serialization::VCS::delta));
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(muted ? DeltaDescription("muted") : DeltaDescription("unmuted"), AutoSequenceDeltas::layerMute);
    return res;
}

NewSerializedDelta createColourDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("color changed"), AutoSequenceDeltas::layerColour);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta createInstrumentDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("instrument changed"), AutoSequenceDeltas::layerInstrument);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta createControllerDiff(const XmlElement *state, const XmlElement *changes)
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("controller changed"), AutoSequenceDeltas::layerController);
    res.deltaData = new XmlElement(*changes);
    return res;
}

Array<NewSerializedDelta> createEventsDiffs(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<NewSerializedDelta> res;
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

void deserializeChanges(const XmlElement *state,
        const XmlElement *changes,
        OwnedArray<MidiEvent> &stateNotes,
        OwnedArray<MidiEvent> &changesNotes)
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::event)
        {
            auto event = new AutomationEvent();
            event->deserialize(*e);
            stateNotes.addSorted(*event, event);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::event)
        {
            auto event = new AutomationEvent();
            event->deserialize(*e);
            changesNotes.addSorted(*event, event);
        }
    }
}

NewSerializedDelta serializeChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const String &deltaType)
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = serializeLayer(changes, deltaType);
    return changesFullDelta;
}

XmlElement *serializeLayer(Array<const MidiEvent *> changes, const String &tag)
{
    auto xml = new XmlElement(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

bool checkIfDeltaIsEventsType(const Delta *delta)
{
    return (delta->getType() == AutoSequenceDeltas::eventsAdded ||
            delta->getType() == AutoSequenceDeltas::eventsChanged ||
            delta->getType() == AutoSequenceDeltas::eventsRemoved);
}
