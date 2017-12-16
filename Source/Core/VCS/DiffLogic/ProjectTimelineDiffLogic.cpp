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

#include "AnnotationsSequence.h"
#include "TimeSignaturesSequence.h"
#include "KeySignaturesSequence.h"
#include "SerializationKeys.h"

using namespace VCS;

// TODO refactor, lots of duplicated code

static XmlElement *mergeAnnotationsAdded(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeAnnotationsRemoved(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeAnnotationsChanged(const XmlElement *state, const XmlElement *changes);

static XmlElement *mergeTimeSignaturesAdded(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeTimeSignaturesRemoved(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeTimeSignaturesChanged(const XmlElement *state, const XmlElement *changes);

static XmlElement *mergeKeySignaturesAdded(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeKeySignaturesRemoved(const XmlElement *state, const XmlElement *changes);
static XmlElement *mergeKeySignaturesChanged(const XmlElement *state, const XmlElement *changes);

static Array<NewSerializedDelta> createAnnotationsDiffs(const XmlElement *state, const XmlElement *changes);
static Array<NewSerializedDelta> createTimeSignaturesDiffs(const XmlElement *state, const XmlElement *changes);
static Array<NewSerializedDelta> createKeySignaturesDiffs(const XmlElement *state, const XmlElement *changes);

static void deserializeChanges(const XmlElement *state, const XmlElement *changes,
    OwnedArray<MidiEvent> &stateEvents, OwnedArray<MidiEvent> &changesEvents);

static NewSerializedDelta serializeChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges, const String &deltaType);

static XmlElement *serializeLayer(Array<const MidiEvent *> changes, const String &tag);

static bool checkIfDeltaIsAnnotationType(const Delta *delta);
static bool checkIfDeltaIsTimeSignatureType(const Delta *delta);
static bool checkIfDeltaIsKeySignatureType(const Delta *delta);

ProjectTimelineDiffLogic::ProjectTimelineDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const String ProjectTimelineDiffLogic::getType() const
{
    return Serialization::Core::projectTimeline;
}

// assuming this is used only on checkout and resetting changes
void ProjectTimelineDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *ProjectTimelineDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

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
                stateDeltaData = initialState.createDeltaDataFor(j);
                deltaFoundInState = (stateDeltaData != nullptr);
                dataHasChanged = (! myDeltaData->isEquivalentTo(stateDeltaData, true));
                break;
            }
        }

        if (!deltaFoundInState || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
            {
                const auto fullDeltas =
                    createAnnotationsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
            else if (myDelta->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
            {
                const auto fullDeltas =
                    createTimeSignaturesDiffs(stateDeltaData, myDeltaData);
                
                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
            else if (myDelta->getType() == ProjectTimelineDeltas::keySignaturesAdded)
            {
                const auto fullDeltas =
                    createKeySignaturesDiffs(stateDeltaData, myDeltaData);

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

    // step 1:
    // the default policy is merging all changes
    // from changes into target (of corresponding types)
    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        // for every supported type we need to spit out 
        // a delta of type eventsAdded with all events merged in there
        ScopedPointer<Delta> annotationsDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                      ProjectTimelineDeltas::annotationsAdded));

        ScopedPointer<Delta> timeSignaturesDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                      ProjectTimelineDeltas::timeSignaturesAdded));

        ScopedPointer<Delta> keySignaturesDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                ProjectTimelineDeltas::keySignaturesAdded));

        ScopedPointer<XmlElement> annotationsDeltaData;
        ScopedPointer<XmlElement> timeSignaturesDeltaData;
        ScopedPointer<XmlElement> keySignaturesDeltaData;

        bool deltaFoundInChanges = false;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool bothDeltasAreAnnotationType =
                checkIfDeltaIsAnnotationType(stateDelta) && checkIfDeltaIsAnnotationType(targetDelta);

            const bool bothDeltasAreTimeSignatureType =
                checkIfDeltaIsTimeSignatureType(stateDelta) && checkIfDeltaIsTimeSignatureType(targetDelta);

            const bool bothDeltasAreKeySignatureType =
                checkIfDeltaIsKeySignatureType(stateDelta) && checkIfDeltaIsKeySignatureType(targetDelta);

            if (bothDeltasAreAnnotationType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = (annotationsDeltaData != nullptr);

                if (targetDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
                {
                    annotationsDeltaData = mergeAnnotationsAdded(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsRemoved)
                {
                    annotationsDeltaData = mergeAnnotationsRemoved(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsChanged)
                {
                    annotationsDeltaData = mergeAnnotationsChanged(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
            }
            else if (bothDeltasAreTimeSignatureType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = (timeSignaturesDeltaData != nullptr);
                
                if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesAdded(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesRemoved)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesRemoved(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesChanged)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesChanged(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
            }
            else if (bothDeltasAreKeySignatureType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = (keySignaturesDeltaData != nullptr);

                if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesAdded)
                {
                    keySignaturesDeltaData = mergeKeySignaturesAdded(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesRemoved)
                {
                    keySignaturesDeltaData = mergeKeySignaturesRemoved(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesChanged)
                {
                    keySignaturesDeltaData = mergeKeySignaturesChanged(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
            }
        }

        if (annotationsDeltaData != nullptr)
        {
            diff->addOwnedDelta(annotationsDelta.release(), annotationsDeltaData.release());
        }
        
        if (timeSignaturesDeltaData != nullptr)
        {
            diff->addOwnedDelta(timeSignaturesDelta.release(), timeSignaturesDeltaData.release());
        }
        
        if (keySignaturesDeltaData != nullptr)
        {
            diff->addOwnedDelta(keySignaturesDelta.release(), keySignaturesDeltaData.release());
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
    // which has a history tree with timeline initialised with annotation deltas
    // and time signature deltas, but missing key signature deltas (introduced later)

    bool stateHasAnnotations = false;
    bool stateHasTimeSignatures = false;
    bool stateHasKeySignatures = false;

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        stateHasAnnotations = stateHasAnnotations || checkIfDeltaIsAnnotationType(stateDelta);
        stateHasTimeSignatures = stateHasTimeSignatures || checkIfDeltaIsTimeSignatureType(stateDelta);
        stateHasKeySignatures = stateHasKeySignatures || checkIfDeltaIsKeySignatureType(stateDelta);
    }

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        ScopedPointer<Delta> annotationsDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                ProjectTimelineDeltas::annotationsAdded));

        ScopedPointer<Delta> keySignaturesDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                ProjectTimelineDeltas::keySignaturesAdded));

        ScopedPointer<Delta> timeSignaturesDelta(
            new Delta(DeltaDescription(Serialization::VCS::headStateDelta),
                ProjectTimelineDeltas::timeSignaturesAdded));

        ScopedPointer<XmlElement> annotationsDeltaData;
        ScopedPointer<XmlElement> keySignaturesDeltaData;
        ScopedPointer<XmlElement> timeSignaturesDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool foundMissingKeySignature = !stateHasKeySignatures && checkIfDeltaIsKeySignatureType(targetDelta);
            const bool foundMissingTimeSignature = !stateHasTimeSignatures && checkIfDeltaIsTimeSignatureType(targetDelta);
            const bool foundMissingAnnotation = !stateHasAnnotations && checkIfDeltaIsAnnotationType(targetDelta);

            if (foundMissingKeySignature)
            {
                const bool incrementalMerge = (keySignaturesDeltaData != nullptr);
                ScopedPointer<XmlElement> emptyKeySignaturesDeltaData(serializeLayer({}, ProjectTimelineDeltas::keySignaturesAdded));

                if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesAdded)
                {
                    keySignaturesDeltaData = mergeKeySignaturesAdded(
                        incrementalMerge ? keySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesRemoved)
                {
                    keySignaturesDeltaData = mergeKeySignaturesRemoved(
                        incrementalMerge ? keySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::keySignaturesChanged)
                {
                    keySignaturesDeltaData = mergeKeySignaturesChanged(
                        incrementalMerge ? keySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
            }
            else if (foundMissingTimeSignature)
            {
                const bool incrementalMerge = (timeSignaturesDeltaData != nullptr);
                ScopedPointer<XmlElement> emptyTimeSignaturesDeltaData(serializeLayer({}, ProjectTimelineDeltas::timeSignaturesAdded));

                if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesAdded(
                        incrementalMerge ? timeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesRemoved)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesRemoved(
                        incrementalMerge ? timeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::timeSignaturesChanged)
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesChanged(
                        incrementalMerge ? timeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
            }
            else if (foundMissingAnnotation)
            {
                const bool incrementalMerge = (annotationsDeltaData != nullptr);
                ScopedPointer<XmlElement> emptyAnnotationDeltaData(serializeLayer({}, ProjectTimelineDeltas::annotationsAdded));

                if (targetDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
                {
                    annotationsDeltaData = mergeAnnotationsAdded(
                        incrementalMerge ? annotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsRemoved)
                {
                    annotationsDeltaData = mergeAnnotationsRemoved(
                        incrementalMerge ? annotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
                else if (targetDelta->getType() == ProjectTimelineDeltas::annotationsChanged)
                {
                    annotationsDeltaData = mergeAnnotationsChanged(
                        incrementalMerge ? annotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
            }
        }

        if (annotationsDeltaData != nullptr)
        {
            diff->addOwnedDelta(annotationsDelta.release(), annotationsDeltaData.release());
        }

        if (timeSignaturesDeltaData != nullptr)
        {
            diff->addOwnedDelta(timeSignaturesDelta.release(), timeSignaturesDeltaData.release());
        }

        if (keySignaturesDeltaData != nullptr)
        {
            diff->addOwnedDelta(keySignaturesDelta.release(), keySignaturesDeltaData.release());
        }
    }

    return diff;
}

//===----------------------------------------------------------------------===//
// Merge annotations
//===----------------------------------------------------------------------===//

XmlElement *mergeAnnotationsAdded(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;

    result.addArray(stateEvents);

    // check if state doesn't already have events with the same ids, then add
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const AnnotationEvent *changesEvent =
            static_cast<AnnotationEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const AnnotationEvent *stateEvent =
                static_cast<AnnotationEvent *>(stateEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }

        if (! foundEventInState)
        {
            result.add(changesEvent);
        }
    }

    return serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}

XmlElement *mergeAnnotationsRemoved(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const AnnotationEvent *stateEvent =
            static_cast<AnnotationEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const AnnotationEvent *changesEvent =
                static_cast<AnnotationEvent *>(changesEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                break;
            }
        }

        if (! foundEventInChanges)
        {
            result.add(stateEvent);
        }
    }

    return serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}

XmlElement *mergeAnnotationsChanged(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;

    result.addArray(stateEvents);

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const AnnotationEvent *stateEvent =
            static_cast<AnnotationEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const AnnotationEvent *changesEvent =
                static_cast<AnnotationEvent *>(changesEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                result.removeAllInstancesOf(stateEvent);
                result.addIfNotAlreadyThere(changesEvent);

                break;
            }
        }

        //jassert(foundEventInChanges);
    }

    return serializeLayer(result, ProjectTimelineDeltas::annotationsAdded);
}

//===----------------------------------------------------------------------===//
// Merge time signatures
//===----------------------------------------------------------------------===//

XmlElement *mergeTimeSignaturesAdded(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);
    
    Array<const MidiEvent *> result;
    
    result.addArray(stateEvents);
    
    // check if state doesn't already have events with the same ids, then add
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const TimeSignatureEvent *changesEvent =
            static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(i));
        
        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const TimeSignatureEvent *stateEvent =
                static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(j));
            
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }
        
        if (! foundEventInState)
        {
            result.add(changesEvent);
        }
    }
    
    return serializeLayer(result, ProjectTimelineDeltas::timeSignaturesAdded);
}

XmlElement *mergeTimeSignaturesRemoved(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);
    
    Array<const MidiEvent *> result;
    
    // add all events that are missing in changes
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const TimeSignatureEvent *stateEvent =
            static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(i));
        
        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const TimeSignatureEvent *changesEvent =
                static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(j));
            
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                break;
            }
        }
        
        if (! foundEventInChanges)
        {
            result.add(stateEvent);
        }
    }
    
    return serializeLayer(result, ProjectTimelineDeltas::timeSignaturesAdded);
}

XmlElement *mergeTimeSignaturesChanged(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);
    
    Array<const MidiEvent *> result;
    result.addArray(stateEvents);
    
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const TimeSignatureEvent *stateEvent =
            static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(i));
        
        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const TimeSignatureEvent *changesEvent =
                static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(j));
            
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                result.removeAllInstancesOf(stateEvent);
                result.addIfNotAlreadyThere(changesEvent);
                
                break;
            }
        }
        
        //jassert(foundEventInChanges);
    }
    
    return serializeLayer(result, ProjectTimelineDeltas::timeSignaturesAdded);
}

//===----------------------------------------------------------------------===//
// Merge key signatures
//===----------------------------------------------------------------------===//

XmlElement *mergeKeySignaturesAdded(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;

    result.addArray(stateEvents);

    // check if state doesn't already have events with the same ids, then add
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const KeySignatureEvent *changesEvent =
            static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const KeySignatureEvent *stateEvent =
                static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }

        if (!foundEventInState)
        {
            result.add(changesEvent);
        }
    }

    return serializeLayer(result, ProjectTimelineDeltas::keySignaturesAdded);
}

XmlElement *mergeKeySignaturesRemoved(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;

    // add all events that are missing in changes
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const KeySignatureEvent *stateEvent =
            static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const KeySignatureEvent *changesEvent =
                static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                break;
            }
        }

        if (!foundEventInChanges)
        {
            result.add(stateEvent);
        }
    }

    return serializeLayer(result, ProjectTimelineDeltas::keySignaturesAdded);
}

XmlElement *mergeKeySignaturesChanged(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<const MidiEvent *> result;
    result.addArray(stateEvents);

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const KeySignatureEvent *stateEvent =
            static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const KeySignatureEvent *changesEvent =
                static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                result.removeAllInstancesOf(stateEvent);
                result.addIfNotAlreadyThere(changesEvent);

                break;
            }
        }

        //jassert(foundEventInChanges);
    }

    return serializeLayer(result, ProjectTimelineDeltas::keySignaturesAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

Array<NewSerializedDelta> createAnnotationsDiffs(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<NewSerializedDelta> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const AnnotationEvent *stateEvent =
            static_cast<AnnotationEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const AnnotationEvent *changesEvent =
                static_cast<AnnotationEvent *>(changesEvents.getUnchecked(j));

            // state event was found in changes, add `changed` records
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;

                const bool eventHasChanged =
                    (stateEvent->getBeat() != changesEvent->getBeat() ||
                     stateEvent->getColour() != changesEvent->getColour() ||
                     stateEvent->getDescription() != changesEvent->getDescription());

                if (eventHasChanged)
                {
                    changedEvents.add(changesEvent);
                }

                break;
            }
        }

        // state event was not found in changes, add `removed` record
        if (! foundEventInChanges)
        {
            removedEvents.add(stateEvent);
        }
    }

    // search for the new events missing in state
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const AnnotationEvent *changesEvent =
            static_cast<AnnotationEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const AnnotationEvent *stateEvent =
                static_cast<AnnotationEvent *>(stateEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }

        if (! foundEventInState)
        {
            addedEvents.add(changesEvent);
        }
    }

    // serialize deltas, if any
    if (addedEvents.size() > 0)
    {
        res.add(serializeChanges(addedEvents,
            "added {x} annotations",
            addedEvents.size(),
            ProjectTimelineDeltas::annotationsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeChanges(removedEvents,
            "removed {x} annotations",
            removedEvents.size(),
            ProjectTimelineDeltas::annotationsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeChanges(changedEvents,
            "changed {x} annotations",
            changedEvents.size(),
            ProjectTimelineDeltas::annotationsChanged));
    }

    return res;
}

Array<NewSerializedDelta> createTimeSignaturesDiffs(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    
    deserializeChanges(state, changes, stateEvents, changesEvents);
    
    Array<NewSerializedDelta> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;
    
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const TimeSignatureEvent *stateEvent =
            static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(i));
        
        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const TimeSignatureEvent *changesEvent =
                static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(j));
            
            // state event was found in changes, add `changed` records
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                
                const bool eventHasChanged =
                    (stateEvent->getBeat() != changesEvent->getBeat() ||
                     stateEvent->getNumerator() != changesEvent->getNumerator() ||
                     stateEvent->getDenominator() != changesEvent->getDenominator());
                
                if (eventHasChanged)
                {
                    changedEvents.add(changesEvent);
                }
                
                break;
            }
        }
        
        // state event was not found in changes, add `removed` record
        if (! foundEventInChanges)
        {
            removedEvents.add(stateEvent);
        }
    }
    
    // search for the new events missing in state
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const TimeSignatureEvent *changesEvent =
            static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(i));
        
        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const TimeSignatureEvent *stateEvent =
                static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(j));
            
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }
        
        if (! foundEventInState)
        {
            addedEvents.add(changesEvent);
        }
    }
    
    // serialize deltas, if any
    if (addedEvents.size() > 0)
    {
        res.add(serializeChanges(addedEvents,
            "added {x} time signatures",
            addedEvents.size(),
            ProjectTimelineDeltas::timeSignaturesAdded));
    }
    
    if (removedEvents.size() > 0)
    {
        res.add(serializeChanges(removedEvents,
            "removed {x} time signatures",
            removedEvents.size(),
            ProjectTimelineDeltas::timeSignaturesRemoved));
    }
    
    if (changedEvents.size() > 0)
    {
        res.add(serializeChanges(changedEvents,
            "changed {x} time signatures",
            changedEvents.size(),
            ProjectTimelineDeltas::timeSignaturesChanged));
    }
    
    return res;
}

Array<NewSerializedDelta> createKeySignaturesDiffs(const XmlElement *state, const XmlElement *changes)
{
    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeChanges(state, changes, stateEvents, changesEvents);

    Array<NewSerializedDelta> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const KeySignatureEvent *stateEvent =
            static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const KeySignatureEvent *changesEvent =
                static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(j));

            // state event was found in changes, add `changed` records
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;

                const bool eventHasChanged =
                    (stateEvent->getBeat() != changesEvent->getBeat() ||
                        stateEvent->getRootKey() != changesEvent->getRootKey() ||
                        ! stateEvent->getScale().isEquivalentTo(changesEvent->getScale()));

                if (eventHasChanged)
                {
                    changedEvents.add(changesEvent);
                }

                break;
            }
        }

        // state event was not found in changes, add `removed` record
        if (!foundEventInChanges)
        {
            removedEvents.add(stateEvent);
        }
    }

    // search for the new events missing in state
    for (int i = 0; i < changesEvents.size(); ++i)
    {
        bool foundEventInState = false;
        const KeySignatureEvent *changesEvent =
            static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(i));

        for (int j = 0; j < stateEvents.size(); ++j)
        {
            const KeySignatureEvent *stateEvent =
                static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(j));

            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInState = true;
                break;
            }
        }

        if (!foundEventInState)
        {
            addedEvents.add(changesEvent);
        }
    }

    // serialize deltas, if any
    if (addedEvents.size() > 0)
    {
        res.add(serializeChanges(addedEvents,
            "added {x} key signatures",
            addedEvents.size(),
            ProjectTimelineDeltas::keySignaturesAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeChanges(removedEvents,
            "removed {x} key signatures",
            removedEvents.size(),
            ProjectTimelineDeltas::keySignaturesRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeChanges(changedEvents,
            "changed {x} key signatures",
            changedEvents.size(),
            ProjectTimelineDeltas::keySignaturesChanged));
    }

    return res;
}


//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

void deserializeChanges(const XmlElement *state, const XmlElement *changes,
    OwnedArray<MidiEvent> &stateEvents, OwnedArray<MidiEvent> &changesEvents)
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
        {
            AnnotationEvent *event = new AnnotationEvent();
            event->deserialize(*e);
            stateEvents.addSorted(*event, event);
        }

        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::timeSignature)
        {
            TimeSignatureEvent *event = new TimeSignatureEvent();
            event->deserialize(*e);
            stateEvents.addSorted(*event, event);
        }

        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::keySignature)
        {
            KeySignatureEvent *event = new KeySignatureEvent();
            event->deserialize(*e);
            stateEvents.addSorted(*event, event);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::annotation)
        {
            AnnotationEvent *event = new AnnotationEvent();
            event->deserialize(*e);
            changesEvents.addSorted(*event, event);
        }
        
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::timeSignature)
        {
            TimeSignatureEvent *event = new TimeSignatureEvent();
            event->deserialize(*e);
            changesEvents.addSorted(*event, event);
        }

        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::keySignature)
        {
            KeySignatureEvent *event = new KeySignatureEvent();
            event->deserialize(*e);
            changesEvents.addSorted(*event, event);
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

bool checkIfDeltaIsAnnotationType(const Delta *delta)
{
    return (delta->getType() == ProjectTimelineDeltas::annotationsAdded ||
        delta->getType() == ProjectTimelineDeltas::annotationsChanged ||
        delta->getType() == ProjectTimelineDeltas::annotationsRemoved);
}

bool checkIfDeltaIsTimeSignatureType(const Delta *delta)
{
    return (delta->getType() == ProjectTimelineDeltas::timeSignaturesAdded ||
        delta->getType() == ProjectTimelineDeltas::timeSignaturesChanged ||
        delta->getType() == ProjectTimelineDeltas::timeSignaturesRemoved);
}

bool checkIfDeltaIsKeySignatureType(const Delta *delta)
{
    return (delta->getType() == ProjectTimelineDeltas::keySignaturesAdded ||
        delta->getType() == ProjectTimelineDeltas::keySignaturesRemoved ||
        delta->getType() == ProjectTimelineDeltas::keySignaturesChanged);
}
