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
#include "ProjectTimelineDiffLogic.h"
#include "ProjectTimeline.h"
#include "AnnotationsSequence.h"
#include "TimeSignaturesSequence.h"
#include "KeySignaturesSequence.h"

// TODO refactor, lots of duplicated code
namespace VCS
{

static SerializedData mergeAnnotationsAdded(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAnnotationsRemoved(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAnnotationsChanged(const SerializedData &state, const SerializedData &changes);

static SerializedData mergeTimeSignaturesAdded(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeTimeSignaturesRemoved(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeTimeSignaturesChanged(const SerializedData &state, const SerializedData &changes);

static SerializedData mergeKeySignaturesAdded(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeKeySignaturesRemoved(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeKeySignaturesChanged(const SerializedData &state, const SerializedData &changes);

static Array<DeltaDiff> createAnnotationsDiffs(const SerializedData &state, const SerializedData &changes);
static Array<DeltaDiff> createTimeSignaturesDiffs(const SerializedData &state, const SerializedData &changes);
static Array<DeltaDiff> createKeySignaturesDiffs(const SerializedData &state, const SerializedData &changes);

static void deserializeTimelineChanges(const SerializedData &state, const SerializedData &changes,
    OwnedArray<MidiEvent> &stateEvents, OwnedArray<MidiEvent> &changesEvents);

static DeltaDiff serializeTimelineChanges(Array<const MidiEvent *> changes,
    const String &description, int64 numChanges, const Identifier &deltaType);

static SerializedData serializeTimelineSequence(Array<const MidiEvent *> changes, const Identifier &tag);

static bool checkIfDeltaIsAnnotationType(const Delta *delta);
static bool checkIfDeltaIsTimeSignatureType(const Delta *delta);
static bool checkIfDeltaIsKeySignatureType(const Delta *delta);

ProjectTimelineDiffLogic::ProjectTimelineDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

const Identifier ProjectTimelineDiffLogic::getType() const
{
    return Serialization::Core::projectTimeline;
}

Diff *ProjectTimelineDiffLogic::createDiff(const TrackedItem &initialState) const
{
    using namespace Serialization::VCS;
    
    auto *diff = new Diff(this->target);

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

            if (myDelta->hasType(stateDelta->getType()))
            {
                stateDeltaData = initialState.getDeltaData(j);
                deltaFoundInState = (stateDeltaData.isValid());
                dataHasChanged = (! myDeltaData.isEquivalentTo(stateDeltaData));
                break;
            }
        }

        if (!deltaFoundInState || dataHasChanged)
        {
            if (myDelta->hasType(AnnotationDeltas::annotationsAdded))
            {
                diff->applyDeltas(createAnnotationsDiffs(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(TimeSignatureDeltas::timeSignaturesAdded))
            {
                diff->applyDeltas(createTimeSignaturesDiffs(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(KeySignatureDeltas::keySignaturesAdded))
            {
                diff->applyDeltas(createKeySignaturesDiffs(stateDeltaData, myDeltaData));
            }
        }
    }

    return diff;
}

Diff *ProjectTimelineDiffLogic::createMergedItem(const TrackedItem &initialState) const
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

        // for every supported type we need to spit out 
        // a delta of type eventsAdded with all events merged in there
        auto annotationsDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            AnnotationDeltas::annotationsAdded);

        auto timeSignaturesDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            TimeSignatureDeltas::timeSignaturesAdded);

        auto keySignaturesDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            KeySignatureDeltas::keySignaturesAdded);

        SerializedData annotationsDeltaData;
        SerializedData timeSignaturesDeltaData;
        SerializedData keySignaturesDeltaData;

        bool deltaFoundInChanges = false;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            const bool bothDeltasAreAnnotationType =
                checkIfDeltaIsAnnotationType(stateDelta) && checkIfDeltaIsAnnotationType(targetDelta);

            const bool bothDeltasAreTimeSignatureType =
                checkIfDeltaIsTimeSignatureType(stateDelta) && checkIfDeltaIsTimeSignatureType(targetDelta);

            const bool bothDeltasAreKeySignatureType =
                checkIfDeltaIsKeySignatureType(stateDelta) && checkIfDeltaIsKeySignatureType(targetDelta);

            if (bothDeltasAreAnnotationType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = annotationsDeltaData.isValid();

                if (targetDelta->hasType(AnnotationDeltas::annotationsAdded))
                {
                    annotationsDeltaData = mergeAnnotationsAdded(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AnnotationDeltas::annotationsRemoved))
                {
                    annotationsDeltaData = mergeAnnotationsRemoved(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AnnotationDeltas::annotationsChanged))
                {
                    annotationsDeltaData = mergeAnnotationsChanged(
                        incrementalMerge ? annotationsDeltaData : stateDeltaData, targetDeltaData);
                }
            }
            else if (bothDeltasAreTimeSignatureType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = timeSignaturesDeltaData.isValid();
                
                if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesAdded))
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesAdded(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesRemoved))
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesRemoved(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesChanged))
                {
                    timeSignaturesDeltaData = mergeTimeSignaturesChanged(
                        incrementalMerge ? timeSignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
            }
            else if (bothDeltasAreKeySignatureType)
            {
                deltaFoundInChanges = true;
                const bool incrementalMerge = keySignaturesDeltaData.isValid();

                if (targetDelta->hasType(KeySignatureDeltas::keySignaturesAdded))
                {
                    keySignaturesDeltaData = mergeKeySignaturesAdded(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(KeySignatureDeltas::keySignaturesRemoved))
                {
                    keySignaturesDeltaData = mergeKeySignaturesRemoved(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(KeySignatureDeltas::keySignaturesChanged))
                {
                    keySignaturesDeltaData = mergeKeySignaturesChanged(
                        incrementalMerge ? keySignaturesDeltaData : stateDeltaData, targetDeltaData);
                }
            }
        }

        if (annotationsDeltaData.isValid())
        {
            diff->applyDelta(annotationsDelta.release(), annotationsDeltaData);
        }
        
        if (timeSignaturesDeltaData.isValid())
        {
            diff->applyDelta(timeSignaturesDelta.release(), timeSignaturesDeltaData);
        }
        
        if (keySignaturesDeltaData.isValid())
        {
            diff->applyDelta(keySignaturesDelta.release(), keySignaturesDeltaData);
        }

        if (! deltaFoundInChanges)
        {
            diff->applyDelta(stateDelta->createCopy(), stateDeltaData);
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

    {
        SerializedData mergedAnnotationsDeltaData;
        SerializedData emptyAnnotationDeltaData(AnnotationDeltas::annotationsAdded);
        auto annotationsDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            AnnotationDeltas::annotationsAdded);

        SerializedData mergedKeySignaturesDeltaData;
        SerializedData emptyKeySignaturesDeltaData(KeySignatureDeltas::keySignaturesAdded);
        auto keySignaturesDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            KeySignatureDeltas::keySignaturesAdded);

        SerializedData mergedTimeSignaturesDeltaData;
        SerializedData emptyTimeSignaturesDeltaData(TimeSignatureDeltas::timeSignaturesAdded);
        auto timeSignaturesDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            TimeSignatureDeltas::timeSignaturesAdded);

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const auto *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            const bool foundMissingKeySignature = !stateHasKeySignatures && checkIfDeltaIsKeySignatureType(targetDelta);
            const bool foundMissingTimeSignature = !stateHasTimeSignatures && checkIfDeltaIsTimeSignatureType(targetDelta);
            const bool foundMissingAnnotation = !stateHasAnnotations && checkIfDeltaIsAnnotationType(targetDelta);

            if (foundMissingKeySignature)
            {
                const bool incrementalMerge = mergedKeySignaturesDeltaData.isValid();

                if (targetDelta->hasType(KeySignatureDeltas::keySignaturesAdded))
                {
                    mergedKeySignaturesDeltaData = mergeKeySignaturesAdded(incrementalMerge ?
                        mergedKeySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(KeySignatureDeltas::keySignaturesRemoved))
                {
                    mergedKeySignaturesDeltaData = mergeKeySignaturesRemoved(incrementalMerge ?
                        mergedKeySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(KeySignatureDeltas::keySignaturesChanged))
                {
                    mergedKeySignaturesDeltaData = mergeKeySignaturesChanged(incrementalMerge ?
                        mergedKeySignaturesDeltaData : emptyKeySignaturesDeltaData, targetDeltaData);
                }
            }
            else if (foundMissingTimeSignature)
            {
                const bool incrementalMerge = mergedTimeSignaturesDeltaData.isValid();

                if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesAdded))
                {
                    mergedTimeSignaturesDeltaData = mergeTimeSignaturesAdded(incrementalMerge ?
                        mergedTimeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesRemoved))
                {
                    mergedTimeSignaturesDeltaData = mergeTimeSignaturesRemoved(incrementalMerge ?
                        mergedTimeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(TimeSignatureDeltas::timeSignaturesChanged))
                {
                    mergedTimeSignaturesDeltaData = mergeTimeSignaturesChanged(incrementalMerge ?
                        mergedTimeSignaturesDeltaData : emptyTimeSignaturesDeltaData, targetDeltaData);
                }
            }
            else if (foundMissingAnnotation)
            {
                const bool incrementalMerge = mergedAnnotationsDeltaData.isValid();

                if (targetDelta->hasType(AnnotationDeltas::annotationsAdded))
                {
                    mergedAnnotationsDeltaData = mergeAnnotationsAdded(incrementalMerge ?
                        mergedAnnotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AnnotationDeltas::annotationsRemoved))
                {
                    mergedAnnotationsDeltaData = mergeAnnotationsRemoved(incrementalMerge ?
                        mergedAnnotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
                else if (targetDelta->hasType(AnnotationDeltas::annotationsChanged))
                {
                    mergedAnnotationsDeltaData = mergeAnnotationsChanged(incrementalMerge ?
                        mergedAnnotationsDeltaData : emptyAnnotationDeltaData, targetDeltaData);
                }
            }
        }

        if (mergedAnnotationsDeltaData.isValid())
        {
            diff->applyDelta(annotationsDelta.release(), mergedAnnotationsDeltaData);
        }

        if (mergedTimeSignaturesDeltaData.isValid())
        {
            diff->applyDelta(timeSignaturesDelta.release(), mergedTimeSignaturesDeltaData);
        }

        if (mergedKeySignaturesDeltaData.isValid())
        {
            diff->applyDelta(keySignaturesDelta.release(), mergedKeySignaturesDeltaData);
        }
    }

    return diff;
}

//===----------------------------------------------------------------------===//
// Merge annotations
//===----------------------------------------------------------------------===//

SerializedData mergeAnnotationsAdded(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, AnnotationDeltas::annotationsAdded);
}

SerializedData mergeAnnotationsRemoved(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, AnnotationDeltas::annotationsAdded);
}

SerializedData mergeAnnotationsChanged(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, AnnotationDeltas::annotationsAdded);
}

//===----------------------------------------------------------------------===//
// Merge time signatures
//===----------------------------------------------------------------------===//

SerializedData mergeTimeSignaturesAdded(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);
    
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
    
    return serializeTimelineSequence(result, TimeSignatureDeltas::timeSignaturesAdded);
}

SerializedData mergeTimeSignaturesRemoved(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);
    
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
    
    return serializeTimelineSequence(result, TimeSignatureDeltas::timeSignaturesAdded);
}

SerializedData mergeTimeSignaturesChanged(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);
    
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
    
    return serializeTimelineSequence(result, TimeSignatureDeltas::timeSignaturesAdded);
}

//===----------------------------------------------------------------------===//
// Merge key signatures
//===----------------------------------------------------------------------===//

SerializedData mergeKeySignaturesAdded(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, KeySignatureDeltas::keySignaturesAdded);
}

SerializedData mergeKeySignaturesRemoved(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, KeySignatureDeltas::keySignaturesAdded);
}

SerializedData mergeKeySignaturesChanged(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

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

    return serializeTimelineSequence(result, KeySignatureDeltas::keySignaturesAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

Array<DeltaDiff> createAnnotationsDiffs(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

    Array<DeltaDiff> res;
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
                     stateEvent->getLength() != changesEvent->getLength() ||
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
        res.add(serializeTimelineChanges(addedEvents,
            "added {x} annotations",
            addedEvents.size(),
            AnnotationDeltas::annotationsAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(removedEvents,
            "removed {x} annotations",
            removedEvents.size(),
            AnnotationDeltas::annotationsRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(changedEvents,
            "changed {x} annotations",
            changedEvents.size(),
            AnnotationDeltas::annotationsChanged));
    }

    return res;
}

Array<DeltaDiff> createTimeSignaturesDiffs(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);
    
    Array<DeltaDiff> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;
    
    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const auto *stateEvent = static_cast<TimeSignatureEvent *>(stateEvents.getUnchecked(i));
        
        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const auto *changesEvent = static_cast<TimeSignatureEvent *>(changesEvents.getUnchecked(j));
            
            // state event was found in changes, add `changed` records
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;
                
                const bool eventHasChanged =
                    stateEvent->getBeat() != changesEvent->getBeat() ||
                    !stateEvent->getMeter().isEquivalentTo(changesEvent->getMeter());
                
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
        res.add(serializeTimelineChanges(addedEvents,
            "added {x} time signatures",
            addedEvents.size(),
            TimeSignatureDeltas::timeSignaturesAdded));
    }
    
    if (removedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(removedEvents,
            "removed {x} time signatures",
            removedEvents.size(),
            TimeSignatureDeltas::timeSignaturesRemoved));
    }
    
    if (changedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(changedEvents,
            "changed {x} time signatures",
            changedEvents.size(),
            TimeSignatureDeltas::timeSignaturesChanged));
    }
    
    return res;
}

Array<DeltaDiff> createKeySignaturesDiffs(const SerializedData &state, const SerializedData &changes)
{
    using namespace Serialization::VCS;

    OwnedArray<MidiEvent> stateEvents;
    OwnedArray<MidiEvent> changesEvents;
    deserializeTimelineChanges(state, changes, stateEvents, changesEvents);

    Array<DeltaDiff> res;
    Array<const MidiEvent *> addedEvents;
    Array<const MidiEvent *> removedEvents;
    Array<const MidiEvent *> changedEvents;

    for (int i = 0; i < stateEvents.size(); ++i)
    {
        bool foundEventInChanges = false;
        const auto *stateEvent =
            static_cast<KeySignatureEvent *>(stateEvents.getUnchecked(i));

        for (int j = 0; j < changesEvents.size(); ++j)
        {
            const auto *changesEvent =
                static_cast<KeySignatureEvent *>(changesEvents.getUnchecked(j));

            // state event was found in changes, add `changed` records
            if (stateEvent->getId() == changesEvent->getId())
            {
                foundEventInChanges = true;

                const bool eventHasChanged =
                    (stateEvent->getBeat() != changesEvent->getBeat() ||
                        stateEvent->getRootKey() != changesEvent->getRootKey() ||
                        stateEvent->getRootKeyName() != changesEvent->getRootKeyName() ||
                        ! stateEvent->getScale()->isEquivalentTo(changesEvent->getScale()));

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
        res.add(serializeTimelineChanges(addedEvents,
            "added {x} key signatures",
            addedEvents.size(),
            KeySignatureDeltas::keySignaturesAdded));
    }

    if (removedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(removedEvents,
            "removed {x} key signatures",
            removedEvents.size(),
            KeySignatureDeltas::keySignaturesRemoved));
    }

    if (changedEvents.size() > 0)
    {
        res.add(serializeTimelineChanges(changedEvents,
            "changed {x} key signatures",
            changedEvents.size(),
            KeySignatureDeltas::keySignaturesChanged));
    }

    return res;
}


//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

void deserializeTimelineChanges(const SerializedData &state, const SerializedData &changes,
    OwnedArray<MidiEvent> &stateEvents, OwnedArray<MidiEvent> &changesEvents)
{
    using namespace Serialization;

    if (state.isValid())
    {
        forEachChildWithType(state, e, Midi::annotation)
        {
            AnnotationEvent *event = new AnnotationEvent();
            event->deserialize(e);
            stateEvents.addSorted(*event, event);
        }

        forEachChildWithType(state, e, Midi::timeSignature)
        {
            TimeSignatureEvent *event = new TimeSignatureEvent();
            event->deserialize(e);
            stateEvents.addSorted(*event, event);
        }

        forEachChildWithType(state, e, Midi::keySignature)
        {
            KeySignatureEvent *event = new KeySignatureEvent();
            event->deserialize(e);
            stateEvents.addSorted(*event, event);
        }
    }

    if (changes.isValid())
    {
        forEachChildWithType(changes, e, Midi::annotation)
        {
            AnnotationEvent *event = new AnnotationEvent();
            event->deserialize(e);
            changesEvents.addSorted(*event, event);
        }
        
        forEachChildWithType(changes, e, Midi::timeSignature)
        {
            TimeSignatureEvent *event = new TimeSignatureEvent();
            event->deserialize(e);
            changesEvents.addSorted(*event, event);
        }

        forEachChildWithType(changes, e, Midi::keySignature)
        {
            KeySignatureEvent *event = new KeySignatureEvent();
            event->deserialize(e);
            changesEvents.addSorted(*event, event);
        }
    }
}

DeltaDiff serializeTimelineChanges(Array<const MidiEvent *> changes,
        const String &description, int64 numChanges, const Identifier &deltaType)
{
    DeltaDiff changesFullDelta;
    changesFullDelta.delta = make<Delta>(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = serializeTimelineSequence(changes, deltaType);
    return changesFullDelta;
}

SerializedData serializeTimelineSequence(Array<const MidiEvent *> changes, const Identifier &tag)
{
    SerializedData tree(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        const MidiEvent *event = changes.getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

bool checkIfDeltaIsAnnotationType(const Delta *d)
{
    using namespace Serialization::VCS;
    return (d->hasType(AnnotationDeltas::annotationsAdded) ||
        d->hasType(AnnotationDeltas::annotationsChanged) ||
        d->hasType(AnnotationDeltas::annotationsRemoved));
}

bool checkIfDeltaIsTimeSignatureType(const Delta *d)
{
    using namespace Serialization::VCS;
    return (d->hasType(TimeSignatureDeltas::timeSignaturesAdded) ||
        d->hasType(TimeSignatureDeltas::timeSignaturesChanged) ||
        d->hasType(TimeSignatureDeltas::timeSignaturesRemoved));
}

bool checkIfDeltaIsKeySignatureType(const Delta *d)
{
    using namespace Serialization::VCS;
    return (d->hasType(KeySignatureDeltas::keySignaturesAdded) ||
        d->hasType(KeySignatureDeltas::keySignaturesRemoved) ||
        d->hasType(KeySignatureDeltas::keySignaturesChanged));
}

}
