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
#include "AutomationLayerDiffLogic.h"
#include "AutomationLayerTreeItem.h"
#include "AutoLayerDeltas.h"

#include "AutomationEvent.h"
#include "AutomationLayer.h"
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


AutomationLayerDiffLogic::AutomationLayerDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem)
{

}

AutomationLayerDiffLogic::~AutomationLayerDiffLogic()
{

}

const String AutomationLayerDiffLogic::getType() const
{
    return Serialization::Core::autoLayer;
}


// предполагается, что это используется только применительно
// к айтемам проекта. в 2х случаях - при чекауте и при ресете изменений.
void AutomationLayerDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *AutomationLayerDiffLogic::createDiff(const TrackedItem &initialState) const
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
            if (myDelta->getType() == AutoLayerDeltas::layerPath)
            {
                NewSerializedDelta fullDelta = this->createPathDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoLayerDeltas::layerMute)
            {
                NewSerializedDelta fullDelta = this->createMuteDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoLayerDeltas::layerColour)
            {
                NewSerializedDelta fullDelta = this->createColourDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoLayerDeltas::layerInstrument)
            {
                NewSerializedDelta fullDelta = this->createInstrumentDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == AutoLayerDeltas::layerController)
            {
                NewSerializedDelta fullDelta = this->createControllerDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            //else if (this->checkIfDeltaIsNotesType(myDelta))
            else if (myDelta->getType() == AutoLayerDeltas::eventsAdded)
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


Diff *AutomationLayerDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    // merge-политика по умолчанию:
    // на каждую дельту таргета пытаемся наложить все дельты изменений.
    // (если типы дельт соответствуют друг другу)

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        bool deltaFoundInChanges = false;

        // для нот в итоге надо выдать одну дельту типа eventsAdded
        // на которую наложить все дельты изменений нот одно за другим.
        ScopedPointer<Delta> eventsDelta(new Delta(DeltaDescription(Serialization::VCS::headStateDelta), AutoLayerDeltas::eventsAdded));
        ScopedPointer<XmlElement> eventsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == AutoLayerDeltas::layerPath)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergePath(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoLayerDeltas::layerMute)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeMute(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoLayerDeltas::layerColour)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeColour(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoLayerDeltas::layerInstrument)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeInstrument(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == AutoLayerDeltas::layerController)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeController(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
            }

            const bool bothDeltasAreNotesType =
                this->checkIfDeltaIsEventsType(stateDelta) && this->checkIfDeltaIsEventsType(targetDelta);

            if (bothDeltasAreNotesType)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == AutoLayerDeltas::eventsAdded)
                {
                    if (eventsDeltaData != nullptr)
                    { eventsDeltaData = this->mergeEventsAdded(eventsDeltaData, targetDeltaData); }
                    else
                    { eventsDeltaData = this->mergeEventsAdded(stateDeltaData, targetDeltaData); }
                }
                else if (targetDelta->getType() == AutoLayerDeltas::eventsRemoved)
                {
                    if (eventsDeltaData != nullptr)
                    { eventsDeltaData = this->mergeEventsRemoved(eventsDeltaData, targetDeltaData); }
                    else
                    { eventsDeltaData = this->mergeEventsRemoved(stateDeltaData, targetDeltaData); }
                }
                else if (targetDelta->getType() == AutoLayerDeltas::eventsChanged)
                {
                    if (eventsDeltaData != nullptr)
                    { eventsDeltaData = this->mergeEventsChanged(eventsDeltaData, targetDeltaData); }
                    else
                    { eventsDeltaData = this->mergeEventsChanged(stateDeltaData, targetDeltaData); }
                }
            }
        }

        if (eventsDeltaData != nullptr)
        {
            diff->addOwnedDelta(eventsDelta.release(), eventsDeltaData.release());
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

XmlElement *AutomationLayerDiffLogic::mergePath(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *AutomationLayerDiffLogic::mergeMute(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *AutomationLayerDiffLogic::mergeColour(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *AutomationLayerDiffLogic::mergeInstrument(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *AutomationLayerDiffLogic::mergeController(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *AutomationLayerDiffLogic::mergeEventsAdded(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    AutomationLayer emptyLayer(dispatcher);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

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

            if (stateNote->getID() == changesNote->getID())
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

    return this->serializeLayer(result, AutoLayerDeltas::eventsAdded);
}

XmlElement *AutomationLayerDiffLogic::mergeEventsRemoved(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    AutomationLayer emptyLayer(dispatcher);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    // добавляем все ноты из состояния, которых нет в изменениях
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AutomationEvent *stateNote = static_cast<AutomationEvent *>(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const AutomationEvent *changesNote = static_cast<AutomationEvent *>(changesNotes.getUnchecked(j));

            if (stateNote->getID() == changesNote->getID())
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

    return this->serializeLayer(result, AutoLayerDeltas::eventsAdded);
}

XmlElement *AutomationLayerDiffLogic::mergeEventsChanged(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    AutomationLayer emptyLayer(dispatcher);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

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

            if (stateNote->getID() == changesNote->getID())
            {
                foundNoteInChanges = true;
                result.removeAllInstancesOf(stateNote);
                result.addIfNotAlreadyThere(changesNote);

                break;
            }
        }

        //jassert(foundNoteInChanges);
    }

    return this->serializeLayer(result, AutoLayerDeltas::eventsAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

NewSerializedDelta AutomationLayerDiffLogic::createPathDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(DeltaDescription("moved from {x}", state->getStringAttribute(Serialization::VCS::delta)),
                          AutoLayerDeltas::layerPath);
    return res;
}

NewSerializedDelta AutomationLayerDiffLogic::createMuteDiff(const XmlElement *state, const XmlElement *changes) const
{
    const bool muted = MidiLayer::isMuted(changes->getStringAttribute(Serialization::VCS::delta));
    NewSerializedDelta res;
    res.deltaData = new XmlElement(*changes);
    res.delta = new Delta(muted ? DeltaDescription("muted") : DeltaDescription("unmuted"), AutoLayerDeltas::layerMute);
    return res;
}

NewSerializedDelta AutomationLayerDiffLogic::createColourDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("color changed"), AutoLayerDeltas::layerColour);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta AutomationLayerDiffLogic::createInstrumentDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("instrument changed"), AutoLayerDeltas::layerInstrument);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta AutomationLayerDiffLogic::createControllerDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("controller changed"), AutoLayerDeltas::layerController);
    res.deltaData = new XmlElement(*changes);
    return res;
}

Array<NewSerializedDelta> AutomationLayerDiffLogic::createEventsDiffs(const XmlElement *state, const XmlElement *changes) const
{
	EmptyEventDispatcher dispatcher;
    AutomationLayer emptyLayer(dispatcher);
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;

    // вот здесь по уму надо десериализовать слои
    // а для этого надо, чтоб в слоях не было ничего, кроме нот
    // поэтому пока есть, как есть, и это не критично
    this->deserializeChanges(emptyLayer, state, changes, stateEvents, changesEvents);

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
            if (stateEvent->getID() == changesEvent->getID())
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

            if (stateNote->getID() == changesNote->getID())
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
        res.add(this->serializeChanges(addedEvents,
                                       "added {x} events",
                                       addedEvents.size(),
                                       AutoLayerDeltas::eventsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(this->serializeChanges(removedEvents,
                                       "removed {x} events",
                                       removedEvents.size(),
                                       AutoLayerDeltas::eventsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(this->serializeChanges(changedEvents,
                                       "changed {x} events",
                                       changedEvents.size(),
                                       AutoLayerDeltas::eventsChanged));
    }

    return res;
}


void AutomationLayerDiffLogic::deserializeChanges(MidiLayer &layer,
        const XmlElement *state,
        const XmlElement *changes,
        OwnedArray<MidiEvent> &stateNotes,
        OwnedArray<MidiEvent> &changesNotes) const
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::event)
        {
            auto event = new AutomationEvent(&layer, 0.f, 0.f);
            event->deserialize(*e);
            stateNotes.addSorted(*event, event);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::event)
        {
            auto event = new AutomationEvent(&layer, 0.f, 0.f);
            event->deserialize(*e);
            changesNotes.addSorted(*event, event);
        }
    }
}

NewSerializedDelta AutomationLayerDiffLogic::serializeChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const String &deltaType) const
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = this->serializeLayer(changes, deltaType);
    return changesFullDelta;
}

XmlElement *AutomationLayerDiffLogic::serializeLayer(Array<const MidiEvent *> changes,
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

bool AutomationLayerDiffLogic::checkIfDeltaIsEventsType(const Delta *delta) const
{
    return (delta->getType() == AutoLayerDeltas::eventsAdded ||
            delta->getType() == AutoLayerDeltas::eventsChanged ||
            delta->getType() == AutoLayerDeltas::eventsRemoved);
}
