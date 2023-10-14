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
#include "SequencerOperations.h"

#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "ProjectTimeline.h"

#include "AnnotationEvent.h"
#include "KeySignatureEvent.h"

#include "NoteComponent.h"
#include "ClipComponent.h"
#include "PianoRoll.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"

#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"

#include "Pattern.h"

#include "UndoStack.h"
#include "AutomationTrackActions.h"

#include "ColourIDs.h"

// a big FIXME:
// most of this code assumes every track has its own undo stack;
// sounds weird, which it is, but that's how it was implemented back in 2014;
// later I've discovered that the idea of multiple undo stacks sucks,
// and left wrappers in sequence classes which use the project's undo stack;
// the code here still needs refactoring and passing common undo stack
// reference to each method.

using PianoChangeGroup = Array<Note>;
using AnnotationChangeGroup = Array<AnnotationEvent>;
using AutoChangeGroup = Array<AutomationEvent>;

template <typename T>
class ChangeGroupProxy final : public T, public ReferenceCountedObject
{
public:

    ChangeGroupProxy() {}
    using Ptr = ReferenceCountedObjectPtr<ChangeGroupProxy>;
};

using PianoChangeGroupProxy = ChangeGroupProxy<PianoChangeGroup>;
using AutoChangeGroupProxy = ChangeGroupProxy<AutoChangeGroup>;

using PianoChangeGroupsPerLayer = FlatHashMap<String, PianoChangeGroupProxy::Ptr, StringHash>;
using AutoChangeGroupsPerLayer = FlatHashMap<String, AutoChangeGroupProxy::Ptr, StringHash>;

template<typename TEvent, typename TGroup, typename TGroups>
void splitChangeGroupByLayers(const TGroup &group, TGroups &outGroups)
{
    for (int i = 0; i < group.size(); ++i)
    {
        const TEvent &note = group.getUnchecked(i);
        const String &trackId(note.getSequence()->getTrackId());
        
        typename ChangeGroupProxy<TGroup>::Ptr targetArray;
        
        if (outGroups.contains(trackId))
        {
            targetArray = outGroups[trackId];
        }
        else
        {
            targetArray = new ChangeGroupProxy<TGroup>();
        }
        
        targetArray->add(note);
        outGroups[trackId] = targetArray;
    }
}

// returns true if madeAnyChanges
template<typename TEvent, typename TLayer, typename TGroup, typename TGroups>
bool applyChanges(const TGroup &groupBefore,
                  const TGroup &groupAfter,
                  bool &didCheckpoint)
{
    bool madeAnyChanges = false;
    
    TGroups groupsBefore, groupsAfter;
    
    splitChangeGroupByLayers<TEvent, TGroup, TGroups>(groupBefore, groupsBefore);
    splitChangeGroupByLayers<TEvent, TGroup, TGroups>(groupAfter, groupsAfter);
    
    for (const auto &changeGroupsIterator : groupsBefore)
    {
        auto currentGroupBefore = changeGroupsIterator.second;
        auto currentGroupAfter = groupsAfter[changeGroupsIterator.first];
        
        MidiSequence *midiLayer = currentGroupBefore->getFirst().getSequence();
        TLayer *targetLayer = dynamic_cast<TLayer *>(midiLayer);
        jassert(targetLayer != nullptr);
        
        if (! didCheckpoint)
        {
            targetLayer->checkpoint();
            didCheckpoint = true;
        }
        
        targetLayer->changeGroup(*currentGroupBefore, *currentGroupAfter, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

// returns true if madeAnyChanges
template<typename TEvent, typename TLayer, typename TGroup, typename TGroups>
bool applyRemovals(const TGroup &groupToRemove,
                   bool &didCheckpoint)
{
    bool madeAnyChanges = false;
    
    TGroups groupsToRemove;
    
    splitChangeGroupByLayers<TEvent, TGroup, TGroups>(groupToRemove, groupsToRemove);
    
    for (const auto &changeGroupsIterator : groupsToRemove)
    {
        auto currentRemovalGroup = changeGroupsIterator.second;
        
        MidiSequence *midiLayer = currentRemovalGroup->getFirst().getSequence();
        TLayer *targetLayer = dynamic_cast<TLayer *>(midiLayer);
        jassert(targetLayer != nullptr);
        
        if (! didCheckpoint)
        {
            targetLayer->checkpoint();
            didCheckpoint = true;
        }
        
        targetLayer->removeGroup(*currentRemovalGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

// returns true if madeAnyChanges
template<typename TEvent, typename TLayer, typename TGroup, typename TGroups>
bool applyInsertions(const TGroup &groupToInsert,
                     bool &didCheckpoint)
{
    bool madeAnyChanges = false;
    
    TGroups groupsToInsert;
    
    splitChangeGroupByLayers<TEvent, TGroup, TGroups>(groupToInsert, groupsToInsert);
    
    for (const auto &changeGroupsIterator : groupsToInsert)
    {
        auto currentInsertionGroup = changeGroupsIterator.second;
        
        MidiSequence *midiLayer = currentInsertionGroup->getFirst().getSequence();
        TLayer *targetLayer = dynamic_cast<TLayer *>(midiLayer);
        jassert(targetLayer != nullptr);
        
        if (! didCheckpoint)
        {
            targetLayer->checkpoint();
            didCheckpoint = true;
        }
        
        targetLayer->insertGroup(*currentInsertionGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

bool applyPianoChanges(const PianoChangeGroup &groupBefore, const PianoChangeGroup &groupAfter, bool &didCheckpoint)
{ return applyChanges<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint); }

bool applyAutoChanges(const AutoChangeGroup &groupBefore, const AutoChangeGroup &groupAfter, bool &didCheckpoint)
{ return applyChanges<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint); }


bool applyPianoRemovals(const PianoChangeGroup &group, bool &didCheckpoint)
{ return applyRemovals<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint); }

// особенный случай - я хочу, чтоб хотя бы одно авто-событие на слое оставалось
bool applyAutoRemovals(const AutoChangeGroup &group, bool &didCheckpoint)
{
    bool madeAnyChanges = false;
    
    AutoChangeGroupsPerLayer groupsToRemove;
    
    splitChangeGroupByLayers<AutomationEvent, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, groupsToRemove);
    
    for (const auto &changeGroupsIterator : groupsToRemove)
    {
        auto currentRemovalGroup = changeGroupsIterator.second;
        
        MidiSequence *sequence = currentRemovalGroup->getFirst().getSequence();
        AutomationSequence *targetLayer = dynamic_cast<AutomationSequence *>(sequence);
        jassert(targetLayer != nullptr);
        
        if ((targetLayer->size() - currentRemovalGroup->size()) <= 1)
        {
            currentRemovalGroup->remove(0);
        }
        
        if (! didCheckpoint)
        {
            targetLayer->checkpoint();
            didCheckpoint = true;
        }
        
        targetLayer->removeGroup(*currentRemovalGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

bool applyPianoInsertions(const PianoChangeGroup &group, bool &didCheckpoint)
{ return applyInsertions<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint); }

bool applyAutoInsertions(const AutoChangeGroup &group, bool &didCheckpoint)
{ return applyInsertions<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, didCheckpoint); }

//===----------------------------------------------------------------------===//
// More helpers
//===----------------------------------------------------------------------===//

PianoSequence *SequencerOperations::getPianoSequence(const Lasso &selection)
{
    // assumes all selection only contains notes of a single sequence
    jassert(selection.getNumSelected() > 0);
    return static_cast<PianoSequence *>(selection.getFirstAs<NoteComponent>()->getNote().getSequence());
}

PianoSequence *SequencerOperations::getPianoSequence(const Clip &targetClip)
{
    return static_cast<PianoSequence *>(targetClip.getPattern()->getTrack()->getSequence());
}

float SequencerOperations::findStartBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return 0.f; }
    
    float startBeat = FLT_MAX;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *note = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        startBeat = jmin(startBeat, note->getBeat());
    }
    
    return startBeat;
}

float SequencerOperations::findEndBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return 0.f; }
    
    float endBeat = -FLT_MAX;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = static_cast<const NoteComponent *>(selection.getSelectedItem(i));
        const float beatPlusLength = nc->getBeat() + nc->getLength();
        endBeat = jmax(endBeat, beatPlusLength);
    }
    
    return endBeat;
}

float SequencerOperations::findStartBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    { return 0.f; }
    
    float startBeat = FLT_MAX;
    
    for (int i = 0; i < selection.size(); ++i)
    {
        if (startBeat > selection.getUnchecked(i).getBeat())
        {
            startBeat = selection.getUnchecked(i).getBeat();
        }
    }
    
    return startBeat;
}

float SequencerOperations::findStartBeat(const WeakReference<Lasso> selection)
{
    if (selection != nullptr) { return findStartBeat(*selection); }
    return 0.f;
}

float SequencerOperations::findEndBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    { return 0.f; }
    
    float endBeat = -FLT_MAX;
    
    for (int i = 0; i < selection.size(); ++i)
    {
        const Note &&nc = selection.getUnchecked(i);
        const float beatPlusLength = nc.getBeat() + nc.getLength();
        
        if (endBeat < beatPlusLength)
        {
            endBeat = beatPlusLength;
        }
    }
    
    return endBeat;
}

float SequencerOperations::findEndBeat(const WeakReference<Lasso> selection)
{
    if (selection != nullptr) { return findEndBeat(*selection); }
    return 0.f;
}

void SequencerOperations::cleanupOverlaps(Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() < 2)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    // 1 convert this
    //    ----
    // ------------
    // into this
    //    ---------
    // ------------
    
    // todo repeat while does any changes
    
    bool step1HasChanges = false;
    
    do
    {
        PianoChangeGroup group1Before, group1After;

        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            const auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaBeats = -FLT_MAX;
            const Note *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                auto *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
                if (nc->getKey() == nc2->getKey() &&
                    nc->getBeat() > nc2->getBeat() &&
                    (nc->getBeat() + nc->getLength()) < (nc2->getBeat() + nc2->getLength()))
                {
                    const float currentDelta = (nc2->getBeat() + nc2->getLength()) - (nc->getBeat() + nc->getLength());
                    
                    if (deltaBeats < currentDelta)
                    {
                        deltaBeats = currentDelta;
                        overlappingNote = &nc2->getNote();
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                const float newLength = nc->getLength() + deltaBeats;
                group1Before.add(nc->getNote());
                group1After.add(nc->getNote().withLength(newLength));
            }
        }
        
        step1HasChanges = applyPianoChanges(group1Before, group1After, didCheckpoint);
    }
    while (step1HasChanges);
    
    
    // 1 convert this
    //    -------------
    // ------------
    // into this
    //    -------------
    // ----------------
    
    bool step2HasChanges = false;

    do
    {
        PianoChangeGroup group2Before, group2After;
        
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            const auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaBeats = -FLT_MAX;
            const NoteComponent *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                auto *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
                if (nc->getKey() == nc2->getKey() &&
                    nc->getBeat() > nc2->getBeat() &&
                    nc->getBeat() < (nc2->getBeat() + nc2->getLength()) &&
                    (nc->getBeat() + nc->getLength()) > (nc2->getBeat() + nc2->getLength()))
                {
                    const float currentDelta = (nc->getBeat() + nc->getLength()) - (nc2->getBeat() + nc2->getLength());
                    
                    if (deltaBeats < currentDelta)
                    {
                        deltaBeats = currentDelta;
                        overlappingNote = nc2;
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                group2Before.add(overlappingNote->getNote());
                group2After.add(overlappingNote->getNote().withDeltaLength(deltaBeats));
            }
        }
        
        step2HasChanges = applyPianoChanges(group2Before, group2After, didCheckpoint);
    }
    while (step2HasChanges);
    
    
    // 3 convert this
    // ------------       ---------
    //    ---------    ------------
    // into this
    // ---                ---------
    //    ---------    ---         
    
    bool step3HasChanges = false;

    do
    {
        PianoChangeGroup group3Before, group3After;
        
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            const auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая перекрывает ее максимально
            
            float overlappingBeats = -FLT_MAX;
            const NoteComponent *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                auto *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
                if (nc->getKey() == nc2->getKey() &&
                    nc->getBeat() < nc2->getBeat() &&
                    (nc->getBeat() + nc->getLength()) >= (nc2->getBeat() + nc2->getLength()))
                {
                    // >0 : has overlap
                    const float overlapsWith = (nc->getBeat() + nc->getLength()) - nc2->getBeat();
                    
                    if (overlapsWith > overlappingBeats)
                    {
                        overlappingBeats = overlapsWith;
                        overlappingNote = nc2;
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                const float newLength = nc->getLength() - overlappingBeats;
                group3Before.add(nc->getNote());
                group3After.add(nc->getNote().withLength(newLength));
            }
        }
        
        step3HasChanges = applyPianoChanges(group3Before, group3After, didCheckpoint);
    }
    while (step3HasChanges);
    
    
    // remove duplicates
    
    FlatHashMap<MidiEvent::Id, Note> deferredRemoval;
    FlatHashMap<MidiEvent::Id, Note> unremovableNotes;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        
        for (int j = 0; j < selection.getNumSelected(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            
            const auto *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
            
            // full overlap
            //const bool isOverlappingNote = (nc->getKey() == nc2->getKey() &&
            //                                nc->getBeat() >= nc2->getBeat() &&
            //                                (nc->getBeat() + nc->getLength()) <= (nc2->getBeat() + nc2->getLength()));

            // partial overlaps also
            const bool isOverlappingNote = (nc->getKey() == nc2->getKey() &&
                                            nc->getBeat() >= nc2->getBeat() &&
                                            nc->getBeat() < (nc2->getBeat() + nc2->getLength()));
            
            const bool startsFromTheSameBeat = (nc->getKey() == nc2->getKey() &&
                                                nc->getBeat() == nc2->getBeat());
            
            const bool isOriginalNote = unremovableNotes.contains(nc2->getNote().getId());
            
            if (! isOriginalNote &&
                (isOverlappingNote || startsFromTheSameBeat))
            {
                unremovableNotes[nc->getNote().getId()] = nc->getNote();
                deferredRemoval[nc2->getNote().getId()] = nc2->getNote();
            }
        }
    }
    
    PianoChangeGroup removalGroup;
    for (const auto &deferredRemovalIterator : deferredRemoval)
    {
        removalGroup.add(deferredRemovalIterator.second);
    }

    applyPianoRemovals(removalGroup, didCheckpoint);
}

//make notes staccato (custom length applies)
void SequencerOperations::makeStaccato(Lasso &selection, float newLength, bool shouldCheckpoint)
{

    if (selection.getNumSelected() == 0) {
        DBG("no notes selected!");
        return;

    }
    if (newLength == 0) { return ;}   //don't bother trying to make notes any shorter than this

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    const auto operationId = UndoActionIDs::MakeStaccato;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    //const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint;

    PianoChangeGroup groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());

        Note newNote(nc->getNote().withLength(newLength));
        groupAfter.add(newNote);
    }

    if (groupBefore.size() > 0)
    {
        if (! didCheckpoint)
        {
            pianoSequence->checkpoint(transactionId);
            didCheckpoint = true;
        }
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

//a very ugly legato function. For now, it works well. I promise I will clean this up!
bool SequencerOperations::makeLegato(const Lasso& selection, float overlap, bool shouldCheckpoint /*= true*/)   //extends each note to the note that follows (target note is bottommost note that is to the right)
{
    if (selection.getNumSelected() == 0)
    {
        DBG("no notes selected!");
        return false;
    }

    auto* sequence = getPianoSequence(selection);
    jassert(sequence);

    //the sequence isn't neccecarily serial (organized from left to right)
    //therefore, we will need to continuously re-target the bottommost note that is to the right of the current subject
    //additionally, we will continuously need to pick the next subject that is the bottommost leftmost note that is above the subject

    PianoChangeGroup removals;
    PianoChangeGroup groupBefore, groupAfter;

    int selectionSize = selection.getNumSelected(); //for use in various for loops

    auto* subject = selection.getFirstAs<NoteComponent>(); //subject = the note we are currently extending
    auto* candidate = selection.getItemAs<NoteComponent>(selectionSize - 1); //candidate = the current note being considered to replace the current best candidate
    auto* target = selection.getItemAs<NoteComponent>(selectionSize - 1); //the note we are extending our subject to
    auto* bestCandidate = selection.getItemAs<NoteComponent>(selectionSize - 1); //the current best candidate (note that fits our selection criteria)

    //initializing various things
    float subjectBeat = -1 * FLT_MAX;
    int subjectKey = -1 * INT_MAX;
    float candidateBeat = FLT_MAX;
    int candidateKey = INT_MAX;
    float bestCandidateBeat = FLT_MAX;
    int bestCandidateKey = INT_MAX;
    float targetBeat = FLT_MAX;
    int targetKey = INT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i) //iterate through the selection to get the first subject (leftmost, bottommost note)
    {
        //re-initialize candidate variables
        candidate = selection.getItemAs<NoteComponent>(i);
        candidateBeat = candidate->getBeat();
        candidateKey = candidate->getKey();

        //if the candidate is to the left of the last best note, OR if it is equal but lower in key
        if ((candidateBeat < bestCandidateBeat) || (candidateBeat == bestCandidateBeat && candidateKey < bestCandidateKey))
        {
            bestCandidate = candidate;
            bestCandidateBeat = candidate->getBeat();
            bestCandidateKey = candidate->getKey();
        }
    }

    //subject is now the leftmost bottommost note in the selection
    //we're setting all this afterward just to make the code more readable later
    subject = bestCandidate;
    subjectBeat = bestCandidate->getBeat();
    subjectKey = bestCandidate->getKey();

    for (int notes = 0; notes < selection.getNumSelected(); ++notes) //since we have set our first subject (before beginning our proper loop), now we commence through the "real" legato loop.
    {
        //re-init best candidate variables
        bestCandidateBeat = FLT_MAX;
        bestCandidateKey = INT_MAX;

        for (int i = 0; i < selection.getNumSelected(); ++i) //iterate through the selection to get the target note(leftmost, bottommost note that is to the right of the subject)
        {
            //re-initialize candidate variables
            candidate = selection.getItemAs<NoteComponent>(i);
            candidateBeat = candidate->getBeat();
            candidateKey = candidate->getKey();

            //if the candidate is to the left of the last best note, OR if it is equal but lower in key, AND it is to the right of the subject
            if (candidateBeat < bestCandidateBeat && candidateBeat > subjectBeat)
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
            if (candidateBeat == bestCandidateBeat && candidateKey < bestCandidateKey && candidateBeat > subjectBeat)
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
        }

        //the target is now the leftmost note that is 1) to the right of the subject AND 2) among multilbe targets of the same beat, the lowest one
        target = bestCandidate;
        targetBeat = bestCandidate->getBeat();
        targetKey = bestCandidate->getKey();

        float newLength = subject->getLength(); //initialize as subject length (just in case)

        if (targetBeat != subjectBeat && targetKey != subjectKey) //only change length if target is not of the same beat (dirty hack)
        {
            newLength = targetBeat - subjectBeat + overlap; //new length is the new length + the overlap specified by overlap
        }
        if (targetBeat != subjectBeat && targetKey == subjectKey) //add slight buffer if they're the same key (so note on/off signals dont occur on the same beat (screws with some vsts)
        {
            newLength = targetBeat - subjectBeat - Globals::minNoteLength; // 64'th note
        }

        groupBefore.add(subject->getNote());

        groupAfter.add(subject->getNote().withLength(newLength)); //use fineMode is true

        //re-init best candidate variables
        bestCandidateBeat = FLT_MAX;
        bestCandidateKey = INT_MAX;

        for (int i = 0; i < selection.getNumSelected(); ++i) //set the next subject in the same way as before, but is must be at least above our last subject, or to the right.
        {
            candidate = selection.getItemAs<NoteComponent>(i); //re-initialize candidate variables
            candidateBeat = candidate->getBeat();
            candidateKey = candidate->getKey();

            if (candidateBeat < bestCandidateBeat && candidateBeat > subjectBeat)   //if its to the left of the prior while being greater than the subject
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
            if (candidateBeat == bestCandidateBeat && candidateKey < bestCandidateKey && candidateKey > subjectKey) //if it's the same as the prior
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
            if (candidateBeat == subjectBeat && candidateKey < bestCandidateKey && candidateKey > subjectKey)   //if it's the same beat as the prior
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
        }

        subject = bestCandidate; //our subject is now the bottommost leftmost note that is to the right of the subject or at least above it in key
        subjectBeat = bestCandidate->getBeat();
        subjectKey = bestCandidate->getKey();
    }


    if (groupBefore.isEmpty() && removals.isEmpty()) //ditch if either of the groups are empty
    {
        return false;
    }

    if (shouldCheckpoint) //undo checkpoint
    {
        sequence->checkpoint();
    }

    if (!groupBefore.isEmpty())
    {
        sequence->changeGroup(groupBefore, groupAfter, true);
    }

    if (!removals.isEmpty())
    {
        sequence->removeGroup(removals, true);
    }

    sequence->changeGroup(groupBefore, groupAfter, true);

    return true;
}

void SequencerOperations::retrograde(Lasso &selection, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() < 2)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    // 1. sort selection
    Array<Note> sortedSelection;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        sortedSelection.addSorted(nc->getNote(), nc->getNote());
    }

    PianoChangeGroup groupBefore;
    PianoChangeGroup groupAfter;

    // 2. pick a note at the start and at the end swap keys
    // (assumes selection is a melodic line, will work weird for chord sequences)
    int start = 0;
    int end = sortedSelection.size() - 1;
    do
    {
        const auto &n1 = sortedSelection.getReference(start);
        const auto &n2 = sortedSelection.getReference(end);

        groupBefore.add(n1);
        groupAfter.add(n1.withKey(n2.getKey()));

        groupBefore.add(n2);
        groupAfter.add(n2.withKey(n1.getKey()));

        start++;
        end--;
    }
    while (start < end);

    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
}

void SequencerOperations::melodicInversion(Lasso &selection, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() < 2)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    // 1. sort selection
    Array<Note> sortedSelection;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        sortedSelection.addSorted(nc->getNote(), nc->getNote());
    }

    Array<Note> groupBefore, groupAfter;

    // 2. invert key intervals between each note and the previous one
    // (as well as retrograde, this assumes selection is a melodic line,
    // and will work weird for chord sequences)
    int keyOffset = 0;
    for (int i = 0; i < sortedSelection.size() - 1; ++i)
    {
        const auto &prev = sortedSelection.getReference(i);
        const auto &next = sortedSelection.getReference(i + 1);
        const int deltaKey = next.getKey() - prev.getKey();

        // simple chromatic inversion, todo scale-aware?
        keyOffset += deltaKey * -2;

        groupBefore.add(next);
        groupAfter.add(next.withDeltaKey(keyOffset));
    }

    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
}

bool SequencerOperations::arpeggiate(Lasso &selection,
    const Temperament::Ptr temperament,
    const Scale::Ptr chordScale, Note::Key chordRoot, const Arpeggiator::Ptr arp,
    float durationMultiplier, float randomness,
    bool isReversed, bool isLimitedToChord,
    bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return false;
    }

    if (!arp->isValid())
    {
        return false;
    }
    
    bool didCheckpoint = !shouldCheckpoint;
    Array<Note> sortedRemovals;
    Array<Note> insertions;

    // 1. sort selection
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        sortedRemovals.addSorted(nc->getNote(), nc->getNote());
    }

    // 2. split chords
    Array<PianoChangeGroup> chords;

    float prevBeat = 0.f;
    int prevKey = 0;

    float nextBeat = 0.f;
    int nextKey = std::numeric_limits<int>::max();

    PianoChangeGroup currentChord;
    bool currentChordNotesHasSameBeat = true;

    for (int i = 0; i < sortedRemovals.size(); ++i)
    {
        if (i != (sortedRemovals.size() - 1))
        {
            nextKey = sortedRemovals.getUnchecked(i + 1).getKey();
            nextBeat = sortedRemovals.getUnchecked(i + 1).getBeat();
        }
        else
        {
            nextKey = sortedRemovals.getUnchecked(i).getKey() - 12;
            nextBeat = sortedRemovals.getUnchecked(i).getBeat() - 12;
        }

        const bool beatWillChange = (sortedRemovals.getUnchecked(i).getBeat() != nextBeat);
        const bool newChordWillStart = (beatWillChange && currentChord.size() > 1 && currentChordNotesHasSameBeat);
        const bool newSequenceWillStart = (sortedRemovals.getUnchecked(i).getKey() > prevKey &&
            sortedRemovals.getUnchecked(i).getKey() > nextKey);

        const bool chordEndsHere = newChordWillStart || newSequenceWillStart;

        if (beatWillChange)
        {
            currentChordNotesHasSameBeat = false;
        }

        currentChord.add(sortedRemovals.getUnchecked(i));

        if (chordEndsHere)
        {
            chords.add(currentChord);
            currentChord.clear();
            currentChordNotesHasSameBeat = true;
        }

        prevKey = sortedRemovals.getUnchecked(i).getKey();
        prevBeat = sortedRemovals.getUnchecked(i).getBeat();
    }

    const float selectionStartBeat = SequencerOperations::findStartBeat(sortedRemovals);

    // 3. arpeggiate every chord
    int arpKeyIndex = 0;
    float arpBeatOffset = 0.f;
    const float arpSequenceLength = arp->getSequenceLength();

    if (chords.size() == 0)
    {
        return false;
    }

    for (int i = 0; i < chords.size(); ++i)
    {
        const auto &chord = chords.getUnchecked(i);
        const float chordEnd = SequencerOperations::findEndBeat(chord);

        while (1)
        {
            const float beatOffset = selectionStartBeat + (arpBeatOffset * durationMultiplier);
            const float nextNoteBeat = beatOffset + (arp->getBeatFor(arpKeyIndex) * durationMultiplier);
            if (nextNoteBeat >= chordEnd)
            {
                if (isLimitedToChord)
                {
                    // every next chord is arpeggiated from the start of arp sequence
                    arpKeyIndex = 0;
                    arpBeatOffset = (chordEnd - selectionStartBeat) / durationMultiplier;
                }

                break;
            }

            insertions.add(arp->mapArpKeyIntoChordSpace(temperament,
                arpKeyIndex, beatOffset,
                chord, chordScale, chordRoot,
                isReversed, durationMultiplier, randomness));

            arpKeyIndex++;
            if (!arp->isKeyIndexValid(arpKeyIndex))
            {
                arpKeyIndex = 0;
                arpBeatOffset += arpSequenceLength;
            }
        }
    }

    // 4. remove selection and add result
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    if (!didCheckpoint)
    {
        pianoSequence->checkpoint();
        didCheckpoint = true;
    }

    pianoSequence->removeGroup(sortedRemovals, true);
    pianoSequence->insertGroup(insertions, true);

    return true;
}

void SequencerOperations::randomizeVolume(Lasso &selection, float factor, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = !shouldCheckpoint;
    Random random(Time::currentTimeMillis());

    PianoChangeGroup groupBefore, groupAfter;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (NoteComponent *nc = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            const float r = ((random.nextFloat() * 2.f) - 1.f) * factor; // (-1 .. 1) * factor
            const float v = nc->getNote().getVelocity();
            const float deltaV = (r < 0) ? (v * r) : ((1.f - v) * r);
            const float newVelocity = nc->getNote().getVelocity() + deltaV;
            
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withVelocity(newVelocity));
        }
    }
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
}

void SequencerOperations::fadeOutVolume(Lasso &selection, float factor, bool shouldCheckpoint)
{
    // Smooth fade out like
    // 1 - ((x / sqrt(x)) * factor)
    
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    float minBeat = FLT_MAX;
    float maxBeat = -FLT_MAX;
    bool didCheckpoint = !shouldCheckpoint;
    PianoChangeGroup groupBefore, groupAfter;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (NoteComponent *nc = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            minBeat = jmin(minBeat, nc->getBeat());
            maxBeat = jmax(maxBeat, nc->getBeat());
        }
    }
    
    const float selectionBeatLength = maxBeat - minBeat;
    
    if (selectionBeatLength <= 0)
    {
        return;
    }
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (NoteComponent *nc = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            const float localBeat = nc->getBeat() - minBeat;
            const float localX = (localBeat / selectionBeatLength) + 0.0001f; // not 0
            const float velocityMultiplier = 1.f - ((localX / sqrtf(localX)) * factor);
            const float newVelocity = nc->getNote().getVelocity() * velocityMultiplier;
            
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withVelocity(newVelocity));
        }
    }
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
}

void SequencerOperations::tuneVolume(Lasso &selection, float delta, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0 || delta == 0.f)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    const auto operationId = (delta > 0.f) ? UndoActionIDs::NotesVolumeUp : UndoActionIDs::NotesVolumeDown;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    const bool repeatsLastOperation = sequence->getLastUndoActionId() == transactionId;

    if (shouldCheckpoint && !repeatsLastOperation)
    {
        sequence->checkpoint(transactionId);
    }

    PianoChangeGroup groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->getNote().withDeltaVelocity(delta));
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::startTuning(Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return; }
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->startTuning();
    }
}

void SequencerOperations::changeVolumeLinear(Lasso &selection, float volumeDelta)
{
    if (selection.getNumSelected() == 0)
    { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    PianoChangeGroup groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningLinear(volumeDelta));
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::changeVolumeMultiplied(Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    PianoChangeGroup groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningMultiplied(volumeFactor));
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::changeVolumeSine(Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    { return; }
    
    const float numSines = 2;
    float midline = 0.f;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        midline += nc->getVelocity();
    }
    midline = midline / float(selection.getNumSelected());
    
    const float startBeat = SequencerOperations::findStartBeat(selection);
    const float endBeat = SequencerOperations::findEndBeat(selection);
    
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    PianoChangeGroup groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        const float phase = ((nc->getBeat() - startBeat) / (endBeat - startBeat)) * MathConstants<float>::pi * 2.f * numSines;
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningSine(volumeFactor, midline, phase));
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::endTuning(Lasso &selection)
{
    jassert(selection.getNumSelected() > 0);
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->endTuning();
    }
}

void SequencerOperations::copyToClipboard(Clipboard &clipboard, const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    SerializedData tree(Serialization::Clipboard::clipboard);

    float firstBeat = FLT_MAX;

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);
    const auto trackId = pianoSequence->getTrackId();

    SerializedData trackRoot(Serialization::Clipboard::track);

    // at the moment, copy-paste only works in the piano roll
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (const auto *noteComponent = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            trackRoot.appendChild(noteComponent->getNote().serialize());
            firstBeat = jmin(firstBeat, noteComponent->getBeat());
        }
    }

    tree.appendChild(trackRoot);

    tree.setProperty(Serialization::Clipboard::firstBeat, firstBeat);
    clipboard.copy(tree, false);
}

void SequencerOperations::pasteFromClipboard(Clipboard &clipboard, ProjectNode &project,
    WeakReference<MidiTrack> selectedTrack, float targetBeatPosition, bool shouldCheckpoint)
{
    if (selectedTrack == nullptr) { return; }

    const auto root = clipboard.getData().hasType(Serialization::Clipboard::clipboard) ?
        clipboard.getData() : clipboard.getData().getChildWithName(Serialization::Clipboard::clipboard);

    if (!root.isValid()) { return; }

    bool didCheckpoint = !shouldCheckpoint;

    const float targetBeat = roundf(targetBeatPosition * 1000.f) / 1000.f;
    const float firstBeat = root.getProperty(Serialization::Clipboard::firstBeat);
    const float deltaBeat = (targetBeat - roundBeat(firstBeat));

    forEachChildWithType(root, layerElement, Serialization::Core::track)
    {
        const String trackId = layerElement.getProperty(Serialization::Clipboard::trackId);

        // TODO:
        // Store track type meta-info (say, controller id) in copy-paste info. On paste,
        // 1. search for track with the same id
        // 2. search for track with the same type and controller
        // 3. take active track

        // Try to paste as many types of events as possible:

        // TODO the same for key signatures and time signatures?
        if (auto *annotationsSequence = dynamic_cast<AnnotationsSequence *>(selectedTrack->getSequence()))
        {
            jassertfalse;
        }
        else if (auto *automationSequence = dynamic_cast<AutomationSequence *>(selectedTrack->getSequence()))
        {
            Array<AutomationEvent> pastedEvents;
            forEachChildWithType(layerElement, autoElement, Serialization::Midi::automationEvent)
            {
                const auto &ae = AutomationEvent(automationSequence).withParameters(autoElement).withNewId();
                pastedEvents.add(ae.withDeltaBeat(deltaBeat));
            }

            if (pastedEvents.size() > 0)
            {
                if (!didCheckpoint)
                {
                    automationSequence->checkpoint();
                    didCheckpoint = true;
                }

                automationSequence->insertGroup(pastedEvents, true);
            }
        }
        else if (auto *pianoSequence = dynamic_cast<PianoSequence *>(selectedTrack->getSequence()))
        {
            Array<Note> pastedNotes;
            forEachChildWithType(layerElement, noteElement, Serialization::Midi::note)
            {
                const auto &n = Note(pianoSequence).withParameters(noteElement).withNewId();
                pastedNotes.add(n.withDeltaBeat(deltaBeat));
            }

            if (pastedNotes.size() > 0)
            {
                if (!didCheckpoint)
                {
                    pianoSequence->checkpoint();
                    didCheckpoint = true;
                }

                pianoSequence->insertGroup(pastedNotes, true);
            }
        }

        forEachChildWithType(root, patternElement, Serialization::Midi::pattern)
        {
            Array<Clip> pastedClips;
            if (auto *targetPattern = selectedTrack->getPattern())
            {
                forEachChildWithType(patternElement, clipElement, Serialization::Midi::clip)
                {
                    auto &&c = Clip(targetPattern).withParameters(clipElement).withNewId();
                    pastedClips.add(c.withDeltaBeat(deltaBeat));
                }
        
                if (pastedClips.size() > 0)
                {
                    if (!didCheckpoint)
                    {
                        targetPattern->checkpoint();
                        didCheckpoint = true;
                    }
        
                    for (Clip &c : pastedClips)
                    {
                        targetPattern->insert(c, true);
                    }
                }
            }
        }
    }
}

void SequencerOperations::deleteSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    OwnedArray<PianoChangeGroup> selectionsByTrack;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const Note &note = selection.getItemAs<NoteComponent>(i)->getNote();
        const MidiSequence *ownerSequence = note.getSequence();
        Array<Note> *arrayToAddTo = nullptr;

        for (int j = 0; j < selectionsByTrack.size(); ++j)
        {
            if (selectionsByTrack.getUnchecked(j)->size() > 0)
            {
                if (selectionsByTrack.getUnchecked(j)->getUnchecked(0).getSequence() == ownerSequence)
                {
                    arrayToAddTo = selectionsByTrack.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Note>();
            selectionsByTrack.add(arrayToAddTo);
        }

        arrayToAddTo->add(note);
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < selectionsByTrack.size(); ++i)
    {
        auto sequence = static_cast<PianoSequence *>(selectionsByTrack.getUnchecked(i)->getUnchecked(0).getSequence());

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            sequence->checkpoint();
        }

        sequence->removeGroup(*selectionsByTrack.getUnchecked(i), true);
    }
}

void SequencerOperations::shiftKeyRelative(Lasso &selection,
    int deltaKey, Transport *transport, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaKey == 0) { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    const auto operationId = deltaKey > 0 ? UndoActionIDs::KeyShiftUp : UndoActionIDs::KeyShiftDown;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    PianoChangeGroup groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
            
        Note newNote(nc->getNote().withDeltaKey(deltaKey));
        groupAfter.add(newNote);
            
        if (transport != nullptr && selection.getNumSelected() < 8)
        {
            transport->previewKey(pianoSequence->getTrackId(),
                newNote.getKey() + nc->getClip().getKey(),
                newNote.getVelocity() * nc->getClip().getVelocity(),
                newNote.getLength());
        }
    }
        
    if (groupBefore.size() > 0)
    {
        if (! didCheckpoint)
        {
            pianoSequence->checkpoint(transactionId);
            didCheckpoint = true;
        }
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::shiftInScaleKeyRelative(const Lasso &selection,
    WeakReference<MidiTrack> keySignatures, Scale::Ptr defaultScale, int deltaKey,
    Transport *transport, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaKey == 0) { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    const auto operationId = deltaKey > 0 ? UndoActionIDs::ScaleKeyShiftUp : UndoActionIDs::ScaleKeyShiftDown;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    Note::Key rootKey = 0;
    Scale::Ptr scale = defaultScale;

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        const auto absKey = nc->getNote().getKey() + nc->getClip().getKey();
        const auto absBeat = nc->getNote().getBeat() + nc->getClip().getBeat();

        if (keySignatures != nullptr)
        {
            findHarmonicContext(absBeat, absBeat, keySignatures, scale, rootKey);
        }

        // scale key calculations are always painful and hardly readable,
        // (this code kinda duplicates the code in ChordPreviewTool::buildChord),
        // please refactor this someday:
        const auto period = (absKey - rootKey) / scale->getBasePeriod();
        const auto periodOffset = period * scale->getBasePeriod();
        const auto targetKeyOffset = absKey % scale->getBasePeriod();
        const auto chromaticOffset = targetKeyOffset - rootKey;
        
        const auto inScaleKey = scale->getNearestScaleKey(chromaticOffset,
            deltaKey < 0 ? Scale::ScaleKeyAlignment::Floor : Scale::ScaleKeyAlignment::Ceil);

        const auto absAlignedInScale = scale->getChromaticKey(inScaleKey, periodOffset + rootKey, false);
        const auto absNewKey = scale->getChromaticKey(inScaleKey + deltaKey, periodOffset + rootKey, false);

        // если нота стоит в ладу, то поднимаем или опускаем на ступень лада,
        // если не в ладу, то поднимаем или опускаем до ближайшей ступени лада:
        const auto d = (absAlignedInScale == absKey) ?
            (absNewKey - absKey) : (absAlignedInScale - absKey);

        Note newNote(nc->getNote().withDeltaKey(d));

        groupBefore.add(nc->getNote());
        groupAfter.add(newNote);

        // lots of duplicate code again, fixme someday:
        if (transport != nullptr && selection.getNumSelected() < 8)
        {
            transport->previewKey(pianoSequence->getTrackId(),
                newNote.getKey() + nc->getClip().getKey(),
                newNote.getVelocity() * nc->getClip().getVelocity(),
                newNote.getLength());
        }
    }

    if (groupBefore.size() > 0)
    {
        if (!didCheckpoint)
        {
            pianoSequence->checkpoint(transactionId);
            didCheckpoint = true;
        }
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::shiftBeatRelative(Lasso &selection, float deltaBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaBeat == 0) { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    const auto operationId = deltaBeat > 0 ? UndoActionIDs::BeatShiftRight : UndoActionIDs::BeatShiftLeft;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    PianoChangeGroup groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
            
        Note newNote(nc->getNote().withDeltaBeat(deltaBeat));
        groupAfter.add(newNote);
    }
        
    if (groupBefore.size() > 0 && !didCheckpoint)
    {
        pianoSequence->checkpoint(transactionId);
        didCheckpoint = true;
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::shiftLengthRelative(Lasso &selection, float deltaLength, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaLength == 0.f) { return; }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    const auto operationId = deltaLength > 0 ? UndoActionIDs::LengthIncrease : UndoActionIDs::LengthDecrease;
    const auto transactionId = selection.generateLassoTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    PianoChangeGroup groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());

        Note newNote(nc->getNote().withDeltaLength(deltaLength));
        groupAfter.add(newNote);
    }

    if (groupBefore.size() > 0 && !didCheckpoint)
    {
        pianoSequence->checkpoint(transactionId);
        didCheckpoint = true;
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::invertChord(Lasso &selection, 
    int deltaKey, bool shouldCheckpoint, Transport *transport)
{
    if (selection.getNumSelected() == 0)
    { return; }

    bool didCheckpoint = !shouldCheckpoint;
    
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);
        
    // step 1. sort selection
    PianoChangeGroup selectedNotes;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        selectedNotes.addSorted(nc->getNote(), nc->getNote());
    }
        
    // step 2. detect target keys (upper or lower)
    PianoChangeGroup targetNotes;
        
    float prevBeat = 0.f;
    int prevKey = (deltaKey > 0) ? std::numeric_limits<int>::max() : 0;
        
    float nextBeat = 0.f;
    int nextKey = std::numeric_limits<int>::max();
        
    for (int i = 0; i < selectedNotes.size(); ++i)
    {
        if (i != (selectedNotes.size() - 1))
        {
            nextKey = selectedNotes[i + 1].getKey();
            nextBeat = selectedNotes[i + 1].getBeat();
        }
        else
        {
            nextKey = selectedNotes[i].getKey() + deltaKey;
            nextBeat = selectedNotes[i].getBeat() + deltaKey;
        }
            
        const bool isRootKey =
            (deltaKey > 0) ?
            (selectedNotes[i].getKey() < prevKey &&
                selectedNotes[i].getKey() < nextKey)
            :
            (selectedNotes[i].getKey() > prevKey &&
                selectedNotes[i].getKey() > nextKey);
            
        if (isRootKey)
        {
            targetNotes.add(selectedNotes[i]);
        }
            
        prevKey = selectedNotes[i].getKey();
        prevBeat = selectedNotes[i].getBeat();
    }
        
    // step 3. octave shift
    PianoChangeGroup groupBefore, groupAfter;
        
    for (auto && targetNote : targetNotes)
    {
        groupBefore.add(targetNote);
        groupAfter.add(targetNote.withDeltaKey(deltaKey));
    }
        
    if (groupBefore.size() > 0)
    {
        if (! didCheckpoint)
        {
            pianoSequence->checkpoint();
            didCheckpoint = true;
        }
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, true);
        
    // step 4. make em sound, if needed
    if (transport != nullptr && selection.getNumSelected() < 8)
    {
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            auto *nc = selection.getItemAs<NoteComponent>(i);
            transport->previewKey(pianoSequence->getTrackId(),
                nc->getNote().getKey() + nc->getClip().getKey(),
                nc->getVelocity() * nc->getClip().getVelocity(),
                nc->getLength());
        }
    }
}

void SequencerOperations::applyTuplets(Lasso &selection, Note::Tuplet tuplet, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    PianoChangeGroup groupBefore, groupAfter;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        if (nc->getNote().getTuplet() != tuplet)
        {
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withTuplet(tuplet));
        }
    }

    if (groupBefore.size() == 0)
    {
        return;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
}

static inline void doQuantize(float &startBeat, float &length, float bar)
{
    // todo for the future:
    // align with time signature events

    jassert(bar != 0.f);
    const float q = bar / float(Globals::beatsPerBar);
    const auto endBeat = startBeat + length;

    startBeat = roundf(startBeat * q) / q;
    const auto endBeatRound = roundf(endBeat * q) / q;

    // make sure the returned length is not too short:
    const auto minQuantizedBeat = (1.f / bar) * float(Globals::beatsPerBar);
    length = jmax(minQuantizedBeat, endBeatRound - startBeat);
}

bool SequencerOperations::quantize(const Lasso &selection, float bar, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return false;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    PianoChangeGroup removals;
    PianoChangeGroup groupBefore, groupAfter;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        float startBeat = nc->getBeat();
        float length = nc->getLength();

        doQuantize(startBeat, length, bar);

        if (startBeat == nc->getBeat() && length == nc->getLength())
        {
            continue;
        }

        bool duplicateFound = false;
        for (const auto &other : groupAfter)
        {
            if (nc->getKey() == other.getKey() &&
                startBeat == other.getBeat() &&
                length == other.getLength())
            {
                duplicateFound = true;
                break;
            }
        }

        if (duplicateFound)
        {
            removals.add(nc->getNote());
            continue;
        }

        groupBefore.add(nc->getNote());
        groupAfter.add(nc->getNote().withBeat(startBeat).withLength(length));
    }

    if (groupBefore.isEmpty() && removals.isEmpty())
    {
        return false;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    if (!groupBefore.isEmpty())
    {
        sequence->changeGroup(groupBefore, groupAfter, true);
    }

    if (!removals.isEmpty())
    {
        sequence->removeGroup(removals, true);
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
    return true;
}

bool SequencerOperations::quantize(WeakReference<MidiTrack> track,
    float bar, bool shouldCheckpoint /*= true*/)
{
    if (track->getSequence()->size() == 0)
    {
        return false;
    }

    auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
    if (sequence == nullptr)
    {
        return false;
    }

    
    PianoChangeGroup removals;
    PianoChangeGroup groupBefore, groupAfter;
    for (int i = 0; i < sequence->size(); ++i)
    {
        auto *note = static_cast<Note *>(sequence->getUnchecked(i));
        float startBeat = note->getBeat();
        float length = note->getLength();

        doQuantize(startBeat, length, bar);

        if (startBeat == note->getBeat() && length == note->getLength())
        {
            continue;
        }

        bool duplicateFound = false;
        for (const auto &other : groupAfter)
        {
            if (note->getKey() == other.getKey() &&
                startBeat == other.getBeat() &&
                length == other.getLength())
            {
                duplicateFound = true;
                break;
            }
        }

        if (duplicateFound)
        {
            removals.add(*note);
            continue;
        }

        groupBefore.add(*note);
        groupAfter.add(note->withBeat(startBeat).withLength(length));
    }

    if (groupBefore.isEmpty() && removals.isEmpty())
    {
        return false;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    if (!groupBefore.isEmpty())
    {
        sequence->changeGroup(groupBefore, groupAfter, true);
    }

    if (!removals.isEmpty())
    {
        sequence->removeGroup(removals, true);
    }

    return true;
}

int SequencerOperations::findAbsoluteRootKey(const Temperament::Ptr temperament,
    Note::Key relativeRoot, Note::Key keyToFindPeriodFor)
{
    const auto middleCOffset = temperament->getMiddleC() % temperament->getPeriodSize();
    const auto sequenceBasePeriod = (keyToFindPeriodFor - middleCOffset - relativeRoot) / temperament->getPeriodSize();
    const auto absRootKey = (sequenceBasePeriod * temperament->getPeriodSize()) + middleCOffset + relativeRoot;
    return absRootKey;
}

static inline void doRescaleLogic(PianoChangeGroup &groupBefore, PianoChangeGroup &groupAfter,
    const Note &note, Note::Key keyOffset, Scale::Ptr scaleA, Scale::Ptr scaleB)
{
    const auto noteKey = note.getKey() - keyOffset;
    const auto periodNumber = noteKey / scaleA->getBasePeriod();
    const auto inScaleKey = scaleA->getScaleKey(noteKey);
    if (inScaleKey >= 0)
    {
        const auto newChromaticKey = scaleB->getBasePeriod() * periodNumber
            + scaleB->getChromaticKey(inScaleKey, 0, false) + keyOffset;

        groupBefore.add(note);
        groupAfter.add(note.withKey(newChromaticKey));
    }
}

void SequencerOperations::rescale(Lasso &selection, Note::Key rootKey,
    Scale::Ptr scaleA, Scale::Ptr scaleB, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    PianoChangeGroup groupBefore, groupAfter;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        // todo clip key offset?
        doRescaleLogic(groupBefore, groupAfter, nc->getNote(), rootKey, scaleA, scaleB);
    }

    if (groupBefore.size() == 0)
    {
        return;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
}

bool SequencerOperations::rescale(const ProjectNode &project, float startBeat, float endBeat,
    Note::Key rootKey, Scale::Ptr scaleA, Scale::Ptr scaleB, bool shouldCheckpoint /*= true*/)
{
    bool hasMadeChanges = false;
    bool didCheckpoint = !shouldCheckpoint;

    const auto pianoTracks = project.findChildrenOfType<PianoTrackNode>();
    for (const auto *track : pianoTracks)
    {
        auto *sequence = static_cast<PianoSequence *>(track->getSequence());

        PianoChangeGroup groupBefore, groupAfter;

        // find events in between (only consider events of one clip!),
        // skipping clips of the same track if already processed any other:

        FlatHashSet<MidiEvent::Id> usedClips;

        for (int i = 0; i < sequence->size(); ++i)
        {
            const auto *note = static_cast<Note *>(sequence->getUnchecked(i));
            for (const auto *clip : track->getPattern()->getClips())
            {
                if (usedClips.contains(clip->getId()) || usedClips.size() == 0)
                {
                    if ((note->getBeat() + clip->getBeat()) >= startBeat &&
                        (note->getBeat() + clip->getBeat()) < endBeat)
                    {
                        const auto keyOffset = rootKey - clip->getKey();
                        doRescaleLogic(groupBefore, groupAfter, *note, keyOffset, scaleA, scaleB);
                        usedClips.insert(clip->getId());
                    }
                }
            }
        }

        if (groupBefore.size() == 0)
        {
            continue;
        }

        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        hasMadeChanges = true;
        sequence->changeGroup(groupBefore, groupAfter, true);
    }

    return hasMadeChanges;
}

bool SequencerOperations::remapNotesToTemperament(const ProjectNode &project,
    Temperament::Ptr temperament, bool shouldCheckpoint)
{
    bool hasMadeChanges = false;
    bool didCheckpoint = !shouldCheckpoint;

    const auto currentTemperament = project.getProjectInfo()->getTemperament();
    const auto chromaticMapFrom = currentTemperament->getChromaticMap();
    const auto chromaticMapTo = temperament->getChromaticMap();

    if (chromaticMapFrom == nullptr || chromaticMapTo == nullptr ||
        !chromaticMapFrom->isValid() || !chromaticMapTo->isValid())
    {
        jassertfalse;
        return false;
    }

    const auto periodSizeBefore = currentTemperament->getPeriodSize();
    const auto periodSizeAfter = temperament->getPeriodSize();

    // a helper to find a key signature at certain beat
    // works similarly to findHarmonicContext, but simpler:
    auto *keySignatures = project.getTimeline()->getKeySignatures()->getSequence();
    const auto findRootKey = [keySignatures](float beat)
    {
        if (keySignatures->size() == 0)
        {
            return 0;
        }

        const KeySignatureEvent *context = nullptr;

        for (int i = 0; i < keySignatures->size(); ++i)
        {
            auto *ks = keySignatures->getUnchecked(i);
            if (context == nullptr || ks->getBeat() <= beat)
            {
                // take the first one no matter where it resides;
                // if event is still before the sequence beat, update the context anyway:
                context = static_cast<KeySignatureEvent *>(ks);
            }
            else if (ks->getBeat() >= beat)
            {
                // no need to look further
                break;
            }
        }

        if (context != nullptr)
        {
            // we've found the only context that doesn't change within a sequence:
            return context->getRootKey();
        }

        return 0;
    };

    const auto pianoTracks = project.findChildrenOfType<PianoTrackNode>();
    for (const auto *track : pianoTracks)
    {
        // upscaling temperament from twelve-tone is really straightforward,
        // but we'll also support downscaling from larger temperament to smaller one:
        // for that we'll just round each key to the nearest key of chromatic approximation scale
        auto *sequence = static_cast<PianoSequence *>(track->getSequence());

        Array<Note> notesBefore, notesAfter;
        for (int n = 0; n < sequence->size(); ++n)
        {
            const auto *note = static_cast<Note *>(sequence->getUnchecked(n));

            const auto rootKeyBefore = findRootKey(note->getBeat());
            const auto rootIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(rootKeyBefore);
            const auto rootKeyAfter = chromaticMapTo->getChromaticKey(rootIndexInChromaticMap, 0, true);

            const auto key = note->getKey() - rootKeyBefore;
            const auto periodNum = key / periodSizeBefore;
            const auto relativeKey = key % periodSizeBefore;

            // now we need to round relative key to the nearest one in chromaticMapFrom 
            const auto keyIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(relativeKey);
            const auto newRelativeKey = chromaticMapTo->getChromaticKey(keyIndexInChromaticMap, rootKeyAfter, false);
            const auto newKey = periodNum * periodSizeAfter + newRelativeKey;

            notesBefore.add(*note);
            notesAfter.add(note->withKey(newKey));
        }

        // same mapping rules apply to any keys, so we will adjust clip key offsets as well

        auto *pattern = track->getPattern();

        Array<Clip> clipsBefore, clipsAfter;

        for (int i = 0; i < pattern->size(); ++i)
        {
            const auto *clip = pattern->getUnchecked(i);

            const auto key = clip->getKey();
            const auto periodNum = key / periodSizeBefore;
            const auto relativeKey = key % periodSizeBefore;
            const auto keySign = (key > 0) - (key < 0); // key offset can be negative

            const auto keyIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(relativeKey);
            const auto newRelativeKey = chromaticMapTo->getChromaticKey(keyIndexInChromaticMap, 0, false);
            const auto newKey = periodNum * periodSizeAfter + newRelativeKey * keySign;

            clipsBefore.add(*clip);
            clipsAfter.add(clip->withKey(newKey));
        }

        // now just apply changes

        if (notesBefore.isEmpty() && clipsBefore.isEmpty())
        {
            continue;
        }

        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        hasMadeChanges = true;

        if (!notesBefore.isEmpty())
        {
            sequence->changeGroup(notesBefore, notesAfter, true);
        }

        if (!clipsBefore.isEmpty())
        {
            pattern->changeGroup(clipsBefore, clipsAfter, true);
        }
    }

    return hasMadeChanges;
}

bool SequencerOperations::remapKeySignaturesToTemperament(KeySignaturesSequence *keySignatures,
    Temperament::Ptr currentTemperament, Temperament::Ptr otherTemperament,
    const Array<Scale::Ptr> &availableScales, bool shouldCheckpoint /*= true*/)
{
    bool hasMadeChanges = false;
    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < keySignatures->size(); ++i)
    {
        // this will map the root key using temperaments' chromatic maps,
        // then try to do the same with all the keys of the key signature's scale,
        // then try to find the most similar (or equivalent) scale for the other temperament,
        // and, if found, use it, and if not found, just use the converted scale
        auto *signature = static_cast<KeySignatureEvent *>(keySignatures->getUnchecked(i));

        auto originalScale = signature->getScale();

        Array<int> newKeys;
        for (const auto &k : originalScale->getKeys())
        {
            const auto keyIndexInChromaticMap = currentTemperament->
                getChromaticMap()->getNearestScaleKey(k);

            const auto newKey = otherTemperament->getChromaticMap()->
                getChromaticKey(keyIndexInChromaticMap, 0, true);

            newKeys.add(newKey);
        }

        // this will be the default one, if the equivalent is not found:
        Scale::Ptr convertedScale(new Scale(originalScale->getUnlocalizedName(),
            newKeys, otherTemperament->getPeriodSize()));

        // but let's search for the most similar scale (if there are any):
        Scale::Ptr similarScale = nullptr;
        int minDifference = INT_MAX;
        for (const auto s : availableScales)
        {
            if (s->getBasePeriod() != otherTemperament->getPeriodSize())
            {
                continue;
            }

            const auto diff = s->getDifferenceFrom(convertedScale);
            if (diff < minDifference)
            {
                minDifference = diff;
                similarScale = s;
            }
        }

        const auto rootIndexInChromaticMap = currentTemperament->
            getChromaticMap()->getNearestScaleKey(signature->getRootKey());

        const auto newRootKey = otherTemperament->getChromaticMap()->
            getChromaticKey(rootIndexInChromaticMap, 0, true);

        if (!didCheckpoint)
        {
            keySignatures->checkpoint();
            didCheckpoint = true;
        }

        keySignatures->change(*signature, signature->withRootKey(newRootKey)
            .withScale(similarScale != nullptr ? similarScale : convertedScale), true);

        hasMadeChanges = true;
    }

    return hasMadeChanges;
}

// Tries to detect if there's one key signature that affects the whole sequence.
// If there's none, of if there's more than one, returns false.
bool SequencerOperations::findHarmonicContext(float startBeat, float endBeat,
    WeakReference<MidiTrack> keysTrack, Scale::Ptr &outScale, Note::Key &outRootKey)
{
    jassert(keysTrack != nullptr);

    if (const auto *keySignatures =
        dynamic_cast<KeySignaturesSequence *>(keysTrack->getSequence()))
    {
        if (keySignatures->size() == 0)
        {
            return false;
        }

        const KeySignatureEvent *context = nullptr;

        for (int i = 0; i < keySignatures->size(); ++i)
        {
            const auto event = keySignatures->getUnchecked(i);
            if (context == nullptr || event->getBeat() <= startBeat)
            {
                // Take the first one no matter where it resides;
                // If event is still before the sequence start, update the context anyway:
                context = static_cast<KeySignatureEvent *>(event);
            }
            else if (event->getBeat() > startBeat && event->getBeat() < endBeat)
            {
                // Harmonic context is already here and changes within a sequence:
                return false;
            }
            else if (event->getBeat() >= endBeat)
            {
                // No need to look further
                break;
            }
        }

        if (context != nullptr)
        {
            // We've found the only context that doesn't change within a sequence:
            outScale = context->getScale();
            outRootKey = context->getRootKey();
            return true;
        }
    }

    return false;
}

bool SequencerOperations::findHarmonicContext(const Lasso &selection, const Clip &clip,
    WeakReference<MidiTrack> keysTrack, Scale::Ptr &outScale, Note::Key &outRootKey)
{
    const auto startBeat = SequencerOperations::findStartBeat(selection) + clip.getBeat();
    const auto endBeat = SequencerOperations::findEndBeat(selection) + clip.getBeat();
    return SequencerOperations::findHarmonicContext(startBeat, endBeat, keysTrack, outScale, outRootKey);
}

void SequencerOperations::duplicateSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    // fixme: cleanup this; the selection will always contain notes for 1 track
    OwnedArray<PianoChangeGroup> selectionsByTrack;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const Note &note = selection.getItemAs<NoteComponent>(i)->getNote();
        const MidiSequence *ownerSequence = note.getSequence();
        Array<Note> *arrayToAddTo = nullptr;

        for (int j = 0; j < selectionsByTrack.size(); ++j)
        {
            if (selectionsByTrack.getUnchecked(j)->size() > 0)
            {
                if (selectionsByTrack.getUnchecked(j)->getUnchecked(0).getSequence() == ownerSequence)
                {
                    arrayToAddTo = selectionsByTrack.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Note>();
            selectionsByTrack.add(arrayToAddTo);
        }

        arrayToAddTo->add(note.withNewId());
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < selectionsByTrack.size(); ++i)
    {
        auto sequence = static_cast<PianoSequence *>(selectionsByTrack.getUnchecked(i)->getUnchecked(0).getSequence());

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            sequence->checkpoint();
        }

        sequence->insertGroup(*selectionsByTrack.getUnchecked(i), true);
    }
}

Clip &SequencerOperations::findClosestClip(Lasso &selection, WeakReference<MidiTrack> track, float &outMinDistance)
{
    float selectionFirstBeat = FLT_MAX;
    float selectionLastBeat = -FLT_MAX;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &note = selection.getItemAs<NoteComponent>(i)->getNote();
        selectionFirstBeat = jmin(note.getBeat(), selectionFirstBeat);
        selectionLastBeat = jmax(note.getBeat(), selectionLastBeat);
    }

    const auto &sourceClip = selection.getFirstAs<NoteComponent>()->getClip();
    const auto selectionStart = selectionFirstBeat + sourceClip.getBeat();
    const auto selectionEnd = selectionLastBeat + sourceClip.getBeat() + 1.f;

    auto *result = track->getPattern()->getClips().getFirst();
    auto *targetSequence = static_cast<PianoSequence *>(track->getSequence());

    outMinDistance = FLT_MAX;
    for (auto *clip : track->getPattern()->getClips())
    {
        const auto targetStart = targetSequence->getFirstBeat() + clip->getBeat();
        const auto targetEnd = targetSequence->getLastBeat() + clip->getBeat();
        const float distance = fabs(targetStart - selectionStart + targetEnd - selectionEnd);

        if (outMinDistance > distance)
        {
            outMinDistance = distance;
            result = clip;
        }
    }

    return *result;
}

Array<Note> SequencerOperations::moveSelection(Lasso &selection,
    Clip &targetClip, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0) { return {}; }

    auto *targetSequence = getPianoSequence(targetClip);
    auto *sourceSequence = getPianoSequence(selection);

    // here we need to calculate offsets so that the content 'stays in place':
    const auto &sourceClip = selection.getFirstAs<NoteComponent>()->getClip();
    const auto deltaBeat = sourceClip.getBeat() - targetClip.getBeat();
    const auto deltaKey = sourceClip.getKey() - targetClip.getKey();

    Array<Note> toRemove, toInsert;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &note = selection.getItemAs<NoteComponent>(i)->getNote();
        toRemove.add(note);
        toInsert.add(note.withDeltaBeat(deltaBeat).withDeltaKey(deltaKey).withNewId(targetSequence));
    }

    if (shouldCheckpoint)
    {
        sourceSequence->checkpoint();
    }

    const auto toReturn = toInsert; // have a copy, toInsert will be emptied
    sourceSequence->removeGroup(toRemove, true);
    targetSequence->insertGroup(toInsert, true);
    return toReturn;
}

Array<Note> SequencerOperations::cutNotes(const Array<Note> &notes,
    const Array<float> &relativeCutBeats, bool shouldCheckpoint)
{
    if (notes.isEmpty())
    {
        return {};
    }

    bool didCheckpoint = !shouldCheckpoint;

    Array<Note> newEventsToTheRight;
    PianoChangeGroup shortenedNotes;
    for (int i = 0; i < notes.size(); ++i)
    {
        const Note &n = notes.getUnchecked(i);
        const float cutBeat = relativeCutBeats.getUnchecked(i);
        if (cutBeat > 0.f && cutBeat < n.getLength())
        {
            shortenedNotes.add(n.withLength(cutBeat));
            newEventsToTheRight.add(n.withDeltaBeat(cutBeat).withDeltaLength(-cutBeat).withNewId());
        }
    }

    applyPianoChanges(notes, shortenedNotes, didCheckpoint);
    applyPianoInsertions(newEventsToTheRight, didCheckpoint);
    return newEventsToTheRight;
}

bool SequencerOperations::mergeNotes(const Note &note1, const Note &note2, bool shouldCheckpoint /*= true*/)
{
    if (note1.getKey() != note2.getKey() ||
        note1.getSequence() != note2.getSequence())
    {
        return false;
    }

    auto *sequence = static_cast<PianoSequence *>(note1.getSequence());
    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    // should we take the target note's velocity instead?
    const auto velocity = (note1.getVelocity() + note2.getVelocity()) / 2.f;
    const auto startBeat = jmin(note1.getBeat(), note2.getBeat());
    const auto endBeat = jmax(note1.getBeat() + note1.getLength(), note2.getBeat() + note2.getLength());
    Note mergedNote(sequence, note1.getKey(), startBeat, endBeat - startBeat, velocity);

    sequence->remove(note1, true);
    sequence->remove(note2, true);
    sequence->insert(mergedNote, true);

    return true;
}

//===----------------------------------------------------------------------===//
// Tempo helpers
//===----------------------------------------------------------------------===//

bool SequencerOperations::setOneTempoForTrack(WeakReference<MidiTrack> track,
    float startBeat, float endBeat, int bpmValue, bool shouldCheckpoint)
{
    bool didCheckpoint = !shouldCheckpoint;

    if (!dynamic_cast<AutomationTrackNode *>(track.get()) || !track->isTempoTrack())
    {
        jassertfalse;
        return false;
    }

    // first, make sure we have exactly 2 events in the sequence
    auto *sequence = static_cast<AutomationSequence *>(track->getSequence());

    if (sequence->size() > 2)
    {
        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        Array<AutomationEvent> redundantEvents;
        for (int i = 1; i < sequence->size() - 1; ++i)
        {
            auto *event = static_cast<AutomationEvent *>(sequence->getUnchecked(i));
            redundantEvents.add(*event);
        }
        sequence->removeGroup(redundantEvents, true);
    }

    while (sequence->size() < 2)
    {
        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        sequence->insert(AutomationEvent(sequence, startBeat, 0.5f), true);
    }

    // update events if needed
    auto *event1 = static_cast<AutomationEvent *>(sequence->getUnchecked(0));
    auto *event2 = static_cast<AutomationEvent *>(sequence->getUnchecked(1));

    if (event1->getControllerValueAsBPM() != bpmValue || event1->getBeat() != startBeat)
    {
        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        sequence->change(*event1, event1->withBeat(startBeat).withTempoBpm(bpmValue), true);
    }

    if (event2->getControllerValueAsBPM() != bpmValue || event2->getBeat() != endBeat)
    {
        if (!didCheckpoint)
        {
            sequence->checkpoint();
            didCheckpoint = true;
        }

        sequence->change(*event2, event2->withBeat(endBeat).withTempoBpm(bpmValue), true);
    }

    return didCheckpoint;
}

bool SequencerOperations::setOneTempoForProject(ProjectNode &project,
    int bpmValue, bool shouldCheckpoint)
{
    bool didCheckpoint = !shouldCheckpoint;

    // make sure there's only one tempo track with exactly one clip:

    const auto automations = project.findChildrenOfType<AutomationTrackNode>();

    AutomationTrackNode *tempoTrackOne = nullptr;
    Array<AutomationTrackNode *> tracksToDelete;
    for (auto *track : automations)
    {
        if (track->isTempoTrack())
        {
            if (tempoTrackOne == nullptr)
            {
                tempoTrackOne = track;
            }
            else
            {
                tracksToDelete.add(track);
            }
        }
    }

    auto *undoStack = project.getUndoStack();

    if (!tracksToDelete.isEmpty())
    {
        if (!didCheckpoint)
        {
            undoStack->beginNewTransaction();
            didCheckpoint = true;
        }

        for (const auto *trackToDelete : tracksToDelete)
        {
            undoStack->perform(new AutomationTrackRemoveAction(project,
                &project, trackToDelete->getTrackId()));
        }
    }

    // no tempo tracks found, create one
    if (tempoTrackOne == nullptr)
    {
        if (!didCheckpoint)
        {
            undoStack->beginNewTransaction();
            didCheckpoint = true;
        }

        String outTrackId;
        String instrumentId; // the instrument id doesn't matter for the tempo track, it'll be empty
        const auto preset =
            SequencerOperations::createAutoTrackTemplate(project,
                TRANS(I18n::Defaults::tempoTrackName),
                MidiTrack::tempoController, instrumentId, outTrackId);

        undoStack->perform(new AutomationTrackInsertAction(project,
            &project, preset, TRANS(I18n::Defaults::tempoTrackName)));

        tempoTrackOne = project.findTrackById<AutomationTrackNode>(outTrackId);
        jassert(tempoTrackOne != nullptr);
    }
    else if (tempoTrackOne->getPattern()->size() > 1)
    {
        if (!didCheckpoint)
        {
            undoStack->beginNewTransaction();
            didCheckpoint = true;
        }

        Array<Clip> redundantClips;
        for (int i = 1; i < tempoTrackOne->getPattern()->size(); ++i)
        {
            redundantClips.add(*tempoTrackOne->getPattern()->getUnchecked(i));
        }

        tempoTrackOne->getPattern()->removeGroup(redundantClips, true);
    }

    jassert(tempoTrackOne->getPattern()->size() == 1);

    // place the clip correctly, if needed
    if (tempoTrackOne->getPattern()->getFirstBeat() != 0.f)
    {
        if (!didCheckpoint)
        {
            undoStack->beginNewTransaction();
            didCheckpoint = true;
        }

        auto *clip = tempoTrackOne->getPattern()->getUnchecked(0);
        tempoTrackOne->getPattern()->change(*clip, clip->withBeat(0.f), true);
    }

    // finally:
    const auto range = project.getProjectBeatRange();
    return setOneTempoForTrack(tempoTrackOne,
        range.getStart(), range.getEnd(), bpmValue, !didCheckpoint);
}

bool SequencerOperations::shiftTempoForProject(ProjectNode &project, int bpmDelta, bool shouldCheckpoint)
{
    if (bpmDelta == 0)
    {
        return false;
    }

    const auto transactionId = (bpmDelta > 0) ? UndoActionIDs::ShiftTempoUp : UndoActionIDs::ShiftTempoDown;
    const bool repeatsLastOperation = project.getUndoStack()->getUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastOperation;

    for (const auto *track : project.getTracks())
    {
        if (!track->isTempoTrack())
        {
            continue;
        }

        auto *autoSequence = dynamic_cast<AutomationSequence *>(track->getSequence());
        assert(autoSequence != nullptr);

        Array<AutomationEvent> eventsBefore;
        Array<AutomationEvent> eventsAfter;

        for (int i = 0; i < autoSequence->size(); ++i)
        {
            const auto *event = static_cast<AutomationEvent *>(autoSequence->getUnchecked(i));
            auto newEvent = event->withTempoBpm(event->getControllerValueAsBPM() + bpmDelta);

            // might not have changed if already at min/max
            if (event->getControllerValue() == newEvent.getControllerValue())
            {
                continue;
            }

            eventsBefore.add(*event);
            eventsAfter.add(move(newEvent));
        }

        if (eventsBefore.isEmpty())
        {
            continue;
        }

        if (!didCheckpoint)
        {
            autoSequence->checkpoint(transactionId);
            didCheckpoint = true;
        }

        autoSequence->changeGroup(eventsBefore, eventsAfter, true);
    }

    return didCheckpoint;
}

//===----------------------------------------------------------------------===//
// Templates
//===----------------------------------------------------------------------===//

UniquePointer<MidiTrackNode> SequencerOperations::createPianoTrack(const Lasso &selection)
{
    if (selection.getNumSelected() == 0) { return {}; }

    Array<Note> events;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        events.add(selection.getItemAs<NoteComponent>(i)->getNote());
    }

    const auto &clip = selection.getFirstAs<NoteComponent>()->getClip();

    return createPianoTrack(events, { clip });
}

UniquePointer<MidiTrackNode> SequencerOperations::createPianoTrack(const PianoSequence *source, const Clip &clip)
{
    Array<Note> events;
    for (int i = 0; i < source->size(); ++i)
    {
        const auto *note = static_cast<Note *>(source->getUnchecked(i));
        events.add(*note);
    }

    return createPianoTrack(events, { clip });
}

UniquePointer<MidiTrackNode> SequencerOperations::createPianoTrack(const Array<Note> &events, const Pattern *pattern)
{
    if (events.size() == 0) { return {}; }

    Array<Clip> clips;
    for (const auto *clip : pattern->getClips())
    {
        clips.add(*clip);
    }

    return createPianoTrack(events, clips);
}

UniquePointer<MidiTrackNode> SequencerOperations::createPianoTrack(const Array<Note> &events, const Array<Clip> &clips)
{
    if (events.size() == 0) { return {}; }

    const auto *track = events.getReference(0).getSequence()->getTrack();
    const auto instrumentId = track->getTrackInstrumentId();
    const auto colour = track->getTrackColour();
    const auto channel = track->getTrackChannel();
    const auto *timeSignature = track->getTimeSignatureOverride();

    UniquePointer<MidiTrackNode> newNode(new PianoTrackNode({}));
    auto *pattern = newNode->getPattern();
    auto *sequence = static_cast<PianoSequence *>(newNode->getSequence());

    PianoChangeGroup copiedContent;
    for (const auto &note : events)
    {
        copiedContent.add(note.withNewId(sequence));
    }
    sequence->reset();
    sequence->insertGroup(copiedContent, false);

    Array<Clip> copiedClips;
    for (const auto &clip : clips)
    {
        copiedClips.add(clip.withNewId(pattern));
    }

    pattern->reset();
    pattern->insertGroup(copiedClips, false);

    newNode->setTrackColour(colour, false, dontSendNotification);
    newNode->setTrackChannel(channel, false, dontSendNotification);
    newNode->setTrackInstrumentId(instrumentId, false, dontSendNotification);
    if (timeSignature != nullptr)
    {
        newNode->setTimeSignatureOverride(*timeSignature, false, dontSendNotification);
    }

    return newNode;
}

UniquePointer<MidiTrackNode> SequencerOperations::createAutomationTrack(const AutomationSequence *source, const Clip &clip)
{
    Array<AutomationEvent> events;
    for (int i = 0; i < source->size(); ++i)
    {
        const auto *event = static_cast<AutomationEvent *>(source->getUnchecked(i));
        events.add(*event);
    }

    return createAutomationTrack(events, { clip });
}

UniquePointer<MidiTrackNode> SequencerOperations::createAutomationTrack(const Array<AutomationEvent> &events, const Pattern *pattern)
{
    if (events.size() == 0) { return {}; }

    Array<Clip> clips;
    for (const auto *clip : pattern->getClips())
    {
        clips.add(*clip);
    }

    return createAutomationTrack(events, clips);
}

UniquePointer<MidiTrackNode> SequencerOperations::createAutomationTrack(const Array<AutomationEvent> &events, const Array<Clip> &clips)
{
    if (events.size() == 0) { return {}; }

    const auto *track = events.getReference(0).getSequence()->getTrack();
    const auto instrumentId = track->getTrackInstrumentId();
    const auto cc = track->getTrackControllerNumber();
    const auto colour = track->getTrackColour();
    const auto channel = track->getTrackChannel();
    const auto *timeSignature = track->getTimeSignatureOverride();

    UniquePointer<MidiTrackNode> newTrack(new AutomationTrackNode({}));
    auto *sequence = static_cast<AutomationSequence *>(newTrack->getSequence());
    auto *pattern = newTrack->getPattern();

    AutoChangeGroup copiedContent;
    for (const auto &event : events)
    {
        copiedContent.add(event.withNewId(sequence));
    }
    sequence->reset();
    sequence->insertGroup(copiedContent, false);

    Array<Clip> copiedClips;
    for (const auto &clip : clips)
    {
        copiedClips.add(clip.withNewId(pattern));
    }
    pattern->reset();
    pattern->insertGroup(copiedClips, false);

    newTrack->setTrackColour(colour, false, dontSendNotification);
    newTrack->setTrackChannel(channel, false, dontSendNotification);
    newTrack->setTrackInstrumentId(instrumentId, false, dontSendNotification);
    newTrack->setTrackControllerNumber(cc, dontSendNotification);
    if (timeSignature != nullptr)
    {
        newTrack->setTimeSignatureOverride(*timeSignature, false, dontSendNotification);
    }

    return newTrack;
}

String SequencerOperations::generateNextNameForNewTrack(const String &name, const StringArray &allNames)
{
    if (!allNames.contains(name) || name.isEmpty())
    {
        return name;
    }

    StringArray tokens;
    tokens.addTokens(name, true);
    if (tokens.isEmpty())
    {
        jassertfalse;
        return name;
    }

    const int last = tokens.size() - 1;
    auto suffix = tokens.getReference(last).getLargeIntValue();
    if (suffix > 0)
    {
        tokens.remove(last); // suffix already exists
    }
    else
    {
        suffix = 1; // no suffix, will start from 2
    }

    String newName;
    do
    {
        suffix++;
        newName = tokens.joinIntoString(" ") + " " + String(suffix);
    } while (allNames.contains(newName));

    return newName;
}

SerializedData SequencerOperations::createPianoTrackTemplate(ProjectNode &project,
    const String &name, float beatPosition, const String &instrumentId, String &outTrackId)
{
    auto newNode = make<PianoTrackNode>(name);

    // We need to have at least one clip on a pattern:
    const Clip clip(newNode->getPattern(), beatPosition);
    newNode->getPattern()->insert(clip, false);

    Random r;
    const auto colours = ColourIDs::getColoursList();
    newNode->setTrackColour(colours[r.nextInt(colours.size())], false, dontSendNotification);
    newNode->setTrackInstrumentId(instrumentId, false, dontSendNotification);

    // insert a single note just so there is a visual anchor in the piano roll:
    const int middleC = project.getProjectInfo()->getTemperament()->getMiddleC();
    auto *pianoSequence = static_cast<PianoSequence *>(newNode->getSequence());
    pianoSequence->insert(Note(pianoSequence, middleC, 0.f,
        float(Globals::beatsPerBar), 0.5f), false);

    outTrackId = newNode->getTrackId();
    return newNode->serialize();
}

SerializedData SequencerOperations::createAutoTrackTemplate(ProjectNode &project,
    const String &name, int controllerNumber, const String &instrumentId, String &outTrackId)
{
    auto newNode = make<AutomationTrackNode>(name);

    // We need to have at least one clip on a pattern:
    const Clip clip(newNode->getPattern());
    newNode->getPattern()->insert(clip, false);

    auto *autoSequence = static_cast<AutomationSequence *>(newNode->getSequence());

    newNode->setTrackInstrumentId(instrumentId, false, dontSendNotification);
    newNode->setTrackColour(Colours::royalblue, false, dontSendNotification);
    newNode->setTrackControllerNumber(controllerNumber, dontSendNotification);

    // init with a couple of events
    const float cv1 = newNode->isOnOffAutomationTrack() ? 1.f : 0.5f;
    const float cv2 = newNode->isOnOffAutomationTrack() ? 0.f : 0.5f;

    // second event is placed at the end of the track for convenience:
    const auto beatRange = project.getProjectBeatRange();
    autoSequence->insert(AutomationEvent(autoSequence, beatRange.getStart(), cv1), false);
    autoSequence->insert(AutomationEvent(autoSequence, beatRange.getEnd(), cv2), false);

    outTrackId = newNode->getTrackId();
    return newNode->serialize();
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class SequencerOperationsTests final : public UnitTest
{
public:
    SequencerOperationsTests() : UnitTest("Sequencer operations tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        beginTest("Generate next name for new track");

        expectEquals({ "Recording" },
            SequencerOperations::generateNextNameForNewTrack("Recording", { "Track 1", "Track 2" }));

        expectEquals({ "Recording 2" },
            SequencerOperations::generateNextNameForNewTrack("Recording", { "Track 1", "Recording", "Track 2" }));

        expectEquals({ "Recording 3" },
            SequencerOperations::generateNextNameForNewTrack("Recording", { "Track 1", "Recording", "Track 2", "Recording 2" }));

        expectEquals({ "Recording" },
            SequencerOperations::generateNextNameForNewTrack("Recording", { "Track 1", "Track 2", "Recording 2" }));

        expectEquals({ "Duplicate 2" },
            SequencerOperations::generateNextNameForNewTrack("Duplicate", { "Duplicate", "Duplicate", "Track A", "Recording" }));
    }
};

static SequencerOperationsTests sequencerOperationsTests;

#endif
