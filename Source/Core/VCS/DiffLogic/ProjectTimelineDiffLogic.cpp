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
#include "ProjectTimelineDiffLogic.h"
#include "ProjectTimeline.h"
#include "ProjectTimelineDeltas.h"

#include "AnnotationEvent.h"
#include "AnnotationsLayer.h"
#include "MidiLayerOwner.h"
#include "SerializationKeys.h"

using namespace VCS;

class EmptyLayerOwner : public MidiLayerOwner
{
public:
    Transport *getTransport() const override { return nullptr; }
    String getXPath() const override { return ""; }
    void setXPath(const String &path) override {}
    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override {}
    void onEventAdded(const MidiEvent &event) override {}
    void onEventRemoved(const MidiEvent &event) override {}
    void onLayerChanged(const MidiLayer *layer) override {}
    void onBeatRangeChanged() override {}
};


ProjectTimelineDiffLogic::ProjectTimelineDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem)
{

}

ProjectTimelineDiffLogic::~ProjectTimelineDiffLogic()
{

}

const String ProjectTimelineDiffLogic::getType() const
{
    return Serialization::Core::projectTimeline;
}


// предполагается, что это используется только применительно
// к айтемам проекта. в 2х случаях - при чекауте и при ресете изменений.
void ProjectTimelineDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *ProjectTimelineDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    // на входе - два набора дельт и их данных
    // на выходе один набор дельт, который потом станет RevisionItem'ом
    // и который потом будет передаваться на мерж при перемещении хэда или мерже ревизий.

    // политика такая - если дельта определенного типа (например, LayerPath) не найдена в источнике
    // или найдена, но не равна источнику !XmlElement::isEquivalentTo:
    // для списков аннотаций - полноценный дифф

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
            if (myDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
            {
                Array<NewSerializedDelta> fullDeltas = this->createAnnotationsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
        }
    }

    return diff;
}


Diff *ProjectTimelineDiffLogic::createMergedItem(const TrackedItem &initialState) const
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
        ScopedPointer<Delta> eventsDelta(new Delta(DeltaDescription(Serialization::VCS::headStateDelta), ProjectTimelineDeltas::annotationsAdded));
        ScopedPointer<XmlElement> eventsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool bothDeltasAreEventsType =
                this->checkIfDeltaIsEventsType(stateDelta) && this->checkIfDeltaIsEventsType(targetDelta);

            if (bothDeltasAreEventsType)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
                {
                    if (eventsDeltaData != nullptr)
                    {
                        eventsDeltaData = this->mergeAnnotationsAdded(eventsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        eventsDeltaData = this->mergeAnnotationsAdded(stateDeltaData, targetDeltaData);
                    }
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsRemoved)
                {
                    if (eventsDeltaData != nullptr)
                    {
                        eventsDeltaData = this->mergeAnnotationsRemoved(eventsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        eventsDeltaData = this->mergeAnnotationsRemoved(stateDeltaData, targetDeltaData);
                    }
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsChanged)
                {
                    if (eventsDeltaData != nullptr)
                    {
                        eventsDeltaData = this->mergeAnnotationsChanged(eventsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        eventsDeltaData = this->mergeAnnotationsChanged(stateDeltaData, targetDeltaData);
                    }
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

XmlElement *ProjectTimelineDiffLogic::mergeAnnotationsAdded(const XmlElement *state, const XmlElement *changes) const
{
    EmptyLayerOwner emptyOwner;
    AnnotationsLayer emptyLayer(emptyOwner);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // на всякий пожарный, ищем, нет ли в состоянии нот с теми же id, где нет - добавляем
    for (int i = 0; i < changesNotes.size(); ++i)
    {
        bool foundNoteInState = false;
        const AnnotationEvent *changesNote = static_cast<AnnotationEvent *>(changesNotes.getUnchecked(i));

        for (int j = 0; j < stateNotes.size(); ++j)
        {
            const AnnotationEvent *stateNote = static_cast<AnnotationEvent *>(stateNotes.getUnchecked(j));

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

    return this->serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}

XmlElement *ProjectTimelineDiffLogic::mergeAnnotationsRemoved(const XmlElement *state, const XmlElement *changes) const
{
    EmptyLayerOwner emptyOwner;
    AnnotationsLayer emptyLayer(emptyOwner);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    // добавляем все ноты из состояния, которых нет в изменениях
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AnnotationEvent *stateNote = static_cast<AnnotationEvent *>(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const AnnotationEvent *changesNote = static_cast<AnnotationEvent *>(changesNotes.getUnchecked(j));

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

    return this->serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}

XmlElement *ProjectTimelineDiffLogic::mergeAnnotationsChanged(const XmlElement *state, const XmlElement *changes) const
{
    EmptyLayerOwner emptyOwner;
    AnnotationsLayer emptyLayer(emptyOwner);
    OwnedArray<MidiEvent> stateNotes;
    OwnedArray<MidiEvent> changesNotes;
    this->deserializeChanges(emptyLayer, state, changes, stateNotes, changesNotes);

    Array<const MidiEvent *> result;

    result.addArray(stateNotes);

    // снова ищем по id и заменяем
    for (int i = 0; i < stateNotes.size(); ++i)
    {
        bool foundNoteInChanges = false;
        const AnnotationEvent *stateNote = static_cast<AnnotationEvent *>(stateNotes.getUnchecked(i));

        for (int j = 0; j < changesNotes.size(); ++j)
        {
            const AnnotationEvent *changesNote = static_cast<AnnotationEvent *>(changesNotes.getUnchecked(j));

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

    return this->serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

Array<NewSerializedDelta> ProjectTimelineDiffLogic::createAnnotationsDiffs(const XmlElement *state, const XmlElement *changes) const
{
    EmptyLayerOwner emptyOwner;
    AnnotationsLayer emptyLayer(emptyOwner);
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
        const AnnotationEvent *stateEvent = static_cast<AnnotationEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const AnnotationEvent *changesEvent = static_cast<AnnotationEvent *>(changesEvents.getUnchecked(j));

            // нота из состояния - существует в изменениях. добавляем запись changed, если нужно.
            if (stateEvent->getID() == changesEvent->getID())
            {
                foundNoteInChanges = true;

                const bool eventHasChanged = (stateEvent->getBeat() != changesEvent->getBeat() ||
                                              stateEvent->getColour() != changesEvent->getColour() ||
                                              stateEvent->getDescription() != changesEvent->getDescription());

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
        const AnnotationEvent *changesNote = static_cast<AnnotationEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const AnnotationEvent *stateNote = static_cast<AnnotationEvent *>(stateEvents.getUnchecked(j));

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
                                       "added {x} annotations",
                                       addedEvents.size(),
                                       ProjectTimelineDeltas::annotationsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(this->serializeChanges(removedEvents,
                                       "removed {x} annotations",
                                       removedEvents.size(),
                                       ProjectTimelineDeltas::annotationsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(this->serializeChanges(changedEvents,
                                       "changed {x} annotations",
                                       changedEvents.size(),
                                       ProjectTimelineDeltas::annotationsChanged));
    }

    return res;
}

void ProjectTimelineDiffLogic::deserializeChanges(MidiLayer &layer,
        const XmlElement *state,
        const XmlElement *changes,
        OwnedArray<MidiEvent> &stateNotes,
        OwnedArray<MidiEvent> &changesNotes) const
{
    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
    {
        AnnotationEvent *event = new AnnotationEvent(&layer);
        event->deserialize(*e);
        stateNotes.addSorted(*event, event);
    }

    forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::annotation)
    {
        AnnotationEvent *event = new AnnotationEvent(&layer);
        event->deserialize(*e);
        changesNotes.addSorted(*event, event);
    }
}

NewSerializedDelta ProjectTimelineDiffLogic::serializeChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const String &deltaType) const
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = this->serializeLayer(changes, deltaType);
    return changesFullDelta;
}

XmlElement *ProjectTimelineDiffLogic::serializeLayer(Array<const MidiEvent *> changes,
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

bool ProjectTimelineDiffLogic::checkIfDeltaIsEventsType(const Delta *delta) const
{
    return (delta->getType() == ProjectTimelineDeltas::annotationsAdded ||
            delta->getType() == ProjectTimelineDeltas::annotationsChanged ||
            delta->getType() == ProjectTimelineDeltas::annotationsRemoved);
}
