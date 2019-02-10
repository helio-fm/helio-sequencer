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
#include "SequencerOperations.h"
#include "ProjectNode.h"
#include "Note.h"
#include "AnnotationEvent.h"
#include "AutomationEvent.h"
#include "NoteComponent.h"
#include "ClipComponent.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "SerializationKeys.h"
#include "Arpeggiator.h"
#include "Transport.h"
#include "CommandIDs.h"

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
using AnnotationChangeGroupProxy = ChangeGroupProxy<AnnotationChangeGroup>;
using AutoChangeGroupProxy = ChangeGroupProxy<AutoChangeGroup>;

using PianoChangeGroupsPerLayer = HashMap<String, PianoChangeGroupProxy::Ptr>;
using AnnotationChangeGroupsPerLayer = HashMap<String, AnnotationChangeGroupProxy::Ptr>;
using AutoChangeGroupsPerLayer = HashMap<String, AutoChangeGroupProxy::Ptr>;

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
        outGroups.set(trackId, targetArray);
    }
}

void splitPianoGroupByLayers(const PianoChangeGroup &group, PianoChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<Note, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, outGroups); }

void splitAnnotationsGroupByLayers(const AnnotationChangeGroup &group, AnnotationChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<AnnotationEvent, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, outGroups); }

void splitAutoGroupByLayers(const AutoChangeGroup &group, AutoChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<AutomationEvent, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, outGroups); }

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
    
    typename TGroups::Iterator changeGroupsIterator(groupsBefore);
    
    while (changeGroupsIterator.next())
    {
        typename ChangeGroupProxy<TGroup>::Ptr currentGroupBefore(changeGroupsIterator.getValue());
        typename ChangeGroupProxy<TGroup>::Ptr currentGroupAfter(groupsAfter[changeGroupsIterator.getKey()]);
        
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
    
    typename TGroups::Iterator changeGroupsIterator(groupsToRemove);
    
    while (changeGroupsIterator.next())
    {
        typename ChangeGroupProxy<TGroup>::Ptr currentRemovalGroup(changeGroupsIterator.getValue());
        
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
    
    typename TGroups::Iterator changeGroupsIterator(groupsToInsert);
    
    while (changeGroupsIterator.next())
    {
        typename ChangeGroupProxy<TGroup>::Ptr currentInsertionGroup(changeGroupsIterator.getValue());
        
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

bool applyAnnotationChanges(const AnnotationChangeGroup &groupBefore, const AnnotationChangeGroup &groupAfter, bool &didCheckpoint)
{ return applyChanges<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint); }

bool applyAutoChanges(const AutoChangeGroup &groupBefore, const AutoChangeGroup &groupAfter, bool &didCheckpoint)
{ return applyChanges<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint); }


bool applyPianoRemovals(const PianoChangeGroup &group, bool &didCheckpoint)
{ return applyRemovals<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint); }

bool applyAnnotationRemovals(const AnnotationChangeGroup &group, bool &didCheckpoint)
{ return applyRemovals<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, didCheckpoint); }

// особенный случай - я хочу, чтоб хотя бы одно авто-событие на слое оставалось
bool applyAutoRemovals(const AutoChangeGroup &group, bool &didCheckpoint)
{
    bool madeAnyChanges = false;
    
    AutoChangeGroupsPerLayer groupsToRemove;
    
    splitChangeGroupByLayers<AutomationEvent, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, groupsToRemove);
    
    AutoChangeGroupsPerLayer::Iterator changeGroupsIterator(groupsToRemove);
    
    while (changeGroupsIterator.next())
    {
        ChangeGroupProxy<AutoChangeGroup>::Ptr currentRemovalGroup(changeGroupsIterator.getValue());
        
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

//bool applyAutoRemovals(const AutoChangeGroup &group, bool &didCheckpoint)
//{ applyRemovals<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, didCheckpoint); }


bool applyPianoInsertions(const PianoChangeGroup &group, bool &didCheckpoint)
{ return applyInsertions<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint); }

bool applyAnnotationInsertions(const AnnotationChangeGroup &group, bool &didCheckpoint)
{ return applyInsertions<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, didCheckpoint); }

bool applyAutoInsertions(const AutoChangeGroup &group, bool &didCheckpoint)
{ return applyInsertions<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, didCheckpoint); }

//===----------------------------------------------------------------------===//
// More helpers
//===----------------------------------------------------------------------===//

PianoSequence *SequencerOperations::getPianoSequence(const SelectionProxyArray::Ptr selection)
{
    const auto &firstEvent = selection->getFirstAs<NoteComponent>()->getNote();
    PianoSequence *pianoSequence = static_cast<PianoSequence *>(firstEvent.getSequence());
    return pianoSequence;
}

PianoSequence *SequencerOperations::getPianoSequence(const Lasso &selection)
{
    // assumes all selection only contains notes of a single sequence
    return static_cast<PianoSequence *>(selection.getFirstAs<NoteComponent>()->getNote().getSequence());
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

static float snappedBeat(float beat, float snapsPerBeat)
{
    return roundf(beat / snapsPerBeat) * snapsPerBeat;
}

// Transaction identifier, and why is it needed:
// Some actions, like dragging notes around, are performed in a single undo transaction,
// but, unlike mouse dragging (where it's clear when to start and when to end a transaction),
// hotkey-handled actions will always do a checkpoint at every keypress, so that
// pressing `cursor down` 5 times and `cursor up` 3 times will result in 8 undo actions,
// (there only should be 2, for transposing events down and up accordingly).
// So, Lasso class re-generates its random id every time it changes,
// and some transform operations here will use that id, combined with operation id
// to identify the transaction and see if the last one was exactly of the same type and target,
// and checkpoint could be skipped.

static String generateTransactionId(int commandId, const Lasso &selection)
{
    return String(commandId) + String(selection.getId());
}

void SequencerOperations::wipeSpace(Array<MidiTrack *> tracks,
                                float startBeat, float endBeat,
                                bool shouldKeepCroppedNotes /*= true*/, bool shouldCheckpoint /*= true*/)
{
    // массив 1: найти все события внутри области
    // массив 2: создать ноты-обрезки, для тех нот, которые полностью не вмещаются
    // отложенно удалить все из массива 1
    // и добавить все из массива 2

    bool didCheckpoint = !shouldCheckpoint;
    
    PianoChangeGroup pianoRemoveGroup;
    PianoChangeGroup pianoInsertGroup;

    AnnotationChangeGroup annotationsRemoveGroup;
    AutoChangeGroup autoRemoveGroup;
   
    for (int i = 0; i < tracks.size(); ++i)
    {
        const auto sequence = tracks.getUnchecked(i)->getSequence();

        if (nullptr != dynamic_cast<PianoSequence *>(sequence))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                Note *note = static_cast<Note *>(layer->getUnchecked(j));
                const float noteStartBeat = note->getBeat();
                const float noteEndBeat = note->getBeat() + note->getLength();
                
                const bool shouldBeDeleted = ((noteStartBeat < endBeat && noteStartBeat >= startBeat) ||
                                              (noteEndBeat > startBeat && noteEndBeat <= endBeat) ||
                                              (noteStartBeat < endBeat && noteEndBeat > endBeat));
                
                if (shouldBeDeleted)
                {
                    pianoRemoveGroup.add(*note);
                }
                
                if (shouldKeepCroppedNotes)
                {
                    const bool hasLeftPartToKeep = (noteStartBeat < startBeat && noteEndBeat > startBeat);
                    
                    if (hasLeftPartToKeep)
                    {
                        pianoInsertGroup.add(note->withLength(startBeat - noteStartBeat).copyWithNewId());
                    }
                    
                    const bool hasRightPartToKeep = (noteStartBeat < endBeat && noteEndBeat > endBeat);
                    
                    if (hasRightPartToKeep)
                    {
                        pianoInsertGroup.add(note->withBeat(endBeat).withLength(noteEndBeat - endBeat).copyWithNewId());
                    }
                }
            }
        }
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(sequence))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AnnotationEvent *annotation = static_cast<AnnotationEvent *>(layer->getUnchecked(j));
                const bool shouldBeDeleted = (annotation->getBeat() < endBeat && annotation->getBeat() >= startBeat);
                
                if (shouldBeDeleted)
                {
                    annotationsRemoveGroup.add(*annotation);
                }
            }
        }
        else if (nullptr != dynamic_cast<AutomationSequence *>(sequence))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AutomationEvent *event = static_cast<AutomationEvent *>(layer->getUnchecked(j));
                const bool shouldBeDeleted = (event->getBeat() < endBeat && event->getBeat() >= startBeat);
                
                if (shouldBeDeleted)
                {
                    autoRemoveGroup.add(*event);
                }
            }
        }
    }
    
    applyPianoRemovals(pianoRemoveGroup, didCheckpoint);
    applyAnnotationRemovals(annotationsRemoveGroup, didCheckpoint);
    applyAutoRemovals(autoRemoveGroup, didCheckpoint);

    if (shouldKeepCroppedNotes)
    {
        applyPianoInsertions(pianoInsertGroup, didCheckpoint);
    }
}

void SequencerOperations::shiftEventsToTheLeft(Array<MidiTrack *> tracks, float targetBeat, float beatOffset, bool shouldCheckpoint /*= true*/)
{
    bool didCheckpoint = !shouldCheckpoint;
    
    PianoChangeGroup pianoGroupBefore;
    PianoChangeGroup pianoGroupAfter;
    
    AnnotationChangeGroup annotationsGroupBefore;
    AnnotationChangeGroup annotationsGroupAfter;
    
    AutoChangeGroup autoGroupBefore;
    AutoChangeGroup autoGroupAfter;
    
    for (int i = 0; i < tracks.size(); ++i)
    {
        const auto sequence = tracks.getUnchecked(i)->getSequence();

        if (nullptr != dynamic_cast<PianoSequence *>(sequence))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                Note *note = static_cast<Note *>(layer->getUnchecked(j));

                if (note->getBeat() < targetBeat)
                {
                    pianoGroupBefore.add(*note);
                    pianoGroupAfter.add(note->withDeltaBeat(beatOffset));
                }
            }
        }
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(sequence))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AnnotationEvent *annotation = static_cast<AnnotationEvent *>(layer->getUnchecked(j));

                if (annotation->getBeat() < targetBeat)
                {
                    annotationsGroupBefore.add(*annotation);
                    annotationsGroupAfter.add(annotation->withDeltaBeat(beatOffset));
                }
            }
        }
        else if (nullptr != dynamic_cast<AutomationSequence *>(sequence))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AutomationEvent *event = static_cast<AutomationEvent *>(layer->getUnchecked(j));
                
                if (event->getBeat() < targetBeat)
                {
                    autoGroupBefore.add(*event);
                    autoGroupAfter.add(event->withDeltaBeat(beatOffset));
                }
            }
        }
    }
    
    applyPianoChanges(pianoGroupBefore, pianoGroupAfter, didCheckpoint);
    applyAnnotationChanges(annotationsGroupBefore, annotationsGroupAfter, didCheckpoint);
    applyAutoChanges(autoGroupBefore, autoGroupAfter, didCheckpoint);
}

void SequencerOperations::shiftEventsToTheRight(Array<MidiTrack *> tracks, float targetBeat, float beatOffset, bool shouldCheckpoint /*= true*/)
{
    bool didCheckpoint = !shouldCheckpoint;
    
    PianoChangeGroup groupBefore;
    PianoChangeGroup groupAfter;
    
    AnnotationChangeGroup annotationsGroupBefore;
    AnnotationChangeGroup annotationsGroupAfter;
    
    AutoChangeGroup autoGroupBefore;
    AutoChangeGroup autoGroupAfter;
    
    for (int i = 0; i < tracks.size(); ++i)
    {
        const auto sequence = tracks.getUnchecked(i)->getSequence();

        if (nullptr != dynamic_cast<PianoSequence *>(sequence))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                Note *note = static_cast<Note *>(layer->getUnchecked(j));
                
                if (note->getBeat() >= targetBeat)
                {
                    groupBefore.add(*note);
                    groupAfter.add(note->withDeltaBeat(beatOffset));
                }
            }
        }
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(sequence))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AnnotationEvent *annotation = static_cast<AnnotationEvent *>(layer->getUnchecked(j));
                
                if (annotation->getBeat() >= targetBeat)
                {
                    annotationsGroupBefore.add(*annotation);
                    annotationsGroupAfter.add(annotation->withDeltaBeat(beatOffset));
                }
            }
        }
        else if (nullptr != dynamic_cast<AutomationSequence *>(sequence))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(sequence);
            for (int j = 0; j < layer->size(); ++j)
            {
                AutomationEvent *event = static_cast<AutomationEvent *>(layer->getUnchecked(j));
                
                if (event->getBeat() >= targetBeat)
                {
                    autoGroupBefore.add(*event);
                    autoGroupAfter.add(event->withDeltaBeat(beatOffset));
                }
            }
        }
    }
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
    applyAnnotationChanges(annotationsGroupBefore, annotationsGroupAfter, didCheckpoint);
    applyAutoChanges(autoGroupBefore, autoGroupAfter, didCheckpoint);
}


void SequencerOperations::snapSelection(Lasso &selection, float snapsPerBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = !shouldCheckpoint;
    
    PianoChangeGroup groupBefore, groupAfter;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        
        const float startBeat = nc->getBeat();
        const float startBeatSnap = snappedBeat(startBeat, snapsPerBeat);
        
        const float endBeat = nc->getBeat() + nc->getLength();
        const float endBeatSnap = snappedBeat(endBeat, snapsPerBeat);
        const float lengthSnap = endBeatSnap - startBeatSnap;
        
        if (startBeat != startBeatSnap ||
            endBeat != endBeatSnap)
        {
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withBeat(startBeatSnap).withLength(lengthSnap));
        }
    }
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint);
}


void SequencerOperations::removeOverlaps(Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = !shouldCheckpoint;
    
    // 0 snap to 0.001 beat
    PianoChangeGroup group0Before, group0After;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        
        const float minSnap = 0.1f;
        const float startBeat = nc->getBeat();
        const float startBeatSnap = snappedBeat(startBeat, minSnap);
        
        const float endBeat = nc->getBeat() + nc->getLength();
        const float endBeatSnap = snappedBeat(endBeat, minSnap);
        const float lengthSnap = endBeatSnap - startBeatSnap;
        
        if (startBeat != startBeatSnap ||
            endBeat != endBeatSnap)
        {
            group0Before.add(nc->getNote());
            group0After.add(nc->getNote().withBeat(startBeatSnap).withLength(lengthSnap));
        }
    }
    
    applyPianoChanges(group0Before, group0After, didCheckpoint);
    
    
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
            NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaBeats = -FLT_MAX;
            const Note *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                NoteComponent *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
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
            NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaBeats = -FLT_MAX;
            const NoteComponent *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                NoteComponent *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
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
    // ------------    ------------
    //    ---------       ---------
    // into this
    // ---             ---
    //    ---------       ---------
    
    bool step3HasChanges = false;

    do
    {
        PianoChangeGroup group3Before, group3After;
        
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            
            // для каждой ноты найти ноту, которая перекрывает ее максимально
            
            float overlappingBeats = -FLT_MAX;
            const NoteComponent *overlappingNote = nullptr;
            
            for (int j = 0; j < selection.getNumSelected(); ++j)
            {
                NoteComponent *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
                
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
    
    HashMap<MidiEvent::Id, Note> deferredRemoval;
    HashMap<MidiEvent::Id, Note> unremovableNotes;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        
        for (int j = 0; j < selection.getNumSelected(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            
            NoteComponent *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
            
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
                unremovableNotes.set(nc->getNote().getId(), nc->getNote());
                deferredRemoval.set(nc2->getNote().getId(), nc2->getNote());
            }
        }
    }
    
    PianoChangeGroup removalGroup;
    HashMap<MidiEvent::Id, Note>::Iterator deferredRemovalIterator(deferredRemoval);
    while (deferredRemovalIterator.next())
    {
        removalGroup.add(deferredRemovalIterator.getValue());
    }

    applyPianoRemovals(removalGroup, didCheckpoint);
}

void SequencerOperations::removeDuplicates(Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    { return; }
    
    HashMap<MidiEvent::Id, Note> deferredRemoval;
    HashMap<MidiEvent::Id, Note> unremovableNotes;
    
    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        
        for (int j = 0; j < selection.getNumSelected(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            
            NoteComponent *nc2 = static_cast<NoteComponent *>(selection.getSelectedItem(j));
            
            // full overlap
            const bool isOverlappingNote = (nc->getKey() == nc2->getKey() &&
                                            nc->getBeat() >= nc2->getBeat() &&
                                            (nc->getBeat() + nc->getLength()) <= (nc2->getBeat() + nc2->getLength()));

            const bool startsFromTheSameBeat = (nc->getKey() == nc2->getKey() &&
                                                nc->getBeat() == nc2->getBeat());
            
            const bool isOriginalNote = unremovableNotes.contains(nc2->getNote().getId());

            if (! isOriginalNote &&
                (isOverlappingNote || startsFromTheSameBeat))
            {
                unremovableNotes.set(nc->getNote().getId(), nc->getNote());
                deferredRemoval.set(nc2->getNote().getId(), nc2->getNote());
            }
        }
    }
    
    PianoChangeGroup removalGroup;
    HashMap<MidiEvent::Id, Note>::Iterator deferredRemovalIterator(deferredRemoval);
    while (deferredRemovalIterator.next())
    {
        removalGroup.add(deferredRemovalIterator.getValue());
    }
    
    applyPianoRemovals(removalGroup, didCheckpoint);
}


void SequencerOperations::moveToLayer(Lasso &selection, MidiSequence *layer, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    PianoSequence *targetLayer = dynamic_cast<PianoSequence *>(layer);
    jassert(targetLayer != nullptr);

    bool didCheckpoint = !shouldCheckpoint;
    PianoChangeGroupsPerLayer deferredRemovals;
    PianoChangeGroupProxy::Ptr insertionsForTargetLayer(new PianoChangeGroupProxy());
    
    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        MidiSequence *midiLayer = trackSelection->getFirstAs<NoteComponent>()->getNote().getSequence();

        if (PianoSequence *sourcePianoLayer = dynamic_cast<PianoSequence *>(midiLayer))
        {
            if (sourcePianoLayer != targetLayer)
            {
                PianoChangeGroupProxy::Ptr removalsForThisLayer(new PianoChangeGroupProxy());
                
                for (int i = 0; i < trackSelection->size(); ++i)
                {
                    const Note &n1 = trackSelection->getItemAs<NoteComponent>(i)->getNote();
                    
                    bool targetHasTheSameNote = false;
                    
                    for (int j = 0; j < targetLayer->size(); ++j)
                    {
                        const Note *n2 = static_cast<Note *>(targetLayer->getUnchecked(j));
                        
                        if (fabs(n1.getBeat() - n2->getBeat()) < 0.01f &&
                            fabs(n1.getLength() - n2->getLength()) < 0.01f &&
                            n1.getKey() == n2->getKey())
                        {
                            DBG("targetHasTheSameNote");
                            targetHasTheSameNote = true;
                            break;
                        }
                    }
                    
                    removalsForThisLayer->add(n1);
                    
                    if (! targetHasTheSameNote)
                    {
                        insertionsForTargetLayer->add(n1);
                    }
                    
                    if (!didCheckpoint)
                    {
                        didCheckpoint = true;
                        sourcePianoLayer->checkpoint();
                        targetLayer->checkpoint(); // compatibility with per-layer undo system?
                    }
                }
                
                deferredRemovals.set(sourcePianoLayer->getTrackId(), removalsForThisLayer);
            }
        }
    }
    
    // events removal
    PianoChangeGroupsPerLayer::Iterator deferredRemovalIterator(deferredRemovals);
    while (deferredRemovalIterator.next())
    {
        PianoChangeGroupProxy::Ptr groupToRemove(deferredRemovalIterator.getValue());
        MidiSequence *midiLayer = groupToRemove->getFirst().getSequence();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(midiLayer);
        pianoLayer->removeGroup(*groupToRemove, true);
    }
    
    // events insertions
    targetLayer->insertGroup(*insertionsForTargetLayer, true);
}

bool SequencerOperations::arpeggiate(Lasso &selection,
    const Scale::Ptr chordScale, Note::Key chordRoot, const Arpeggiator::Ptr arp,
    bool reversed, bool limitToChord, bool shouldCheckpoint)
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
    int nextKey = 128;

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

        if (reversed)
        {
            // TODO sort chord keys in reverse order
        }

        // Arp sequence as is
        while (1)
        {
            const float beatOffset = selectionStartBeat + arpBeatOffset;
            const float nextNoteBeat = beatOffset + arp->getBeatFor(arpKeyIndex);
            if (nextNoteBeat >= chordEnd)
            {
                if (limitToChord)
                {
                    // Every next chord is arpeggiated from the start of arp sequence
                    arpKeyIndex = 0;
                    arpBeatOffset = chordEnd - selectionStartBeat;
                }
                        
                break;
            }

            insertions.add(arp->mapArpKeyIntoChordSpace(arpKeyIndex,
                beatOffset, chord, chordScale, chordRoot));

            arpKeyIndex++;
            if (arpKeyIndex >= arp->getNumKeys())
            {
                arpKeyIndex = 0;
                arpBeatOffset += arpSequenceLength;
            }
        }
    }

    // 4. remove selection and add result
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    if (! didCheckpoint)
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

    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        PianoSequence *pianoLayer = getPianoSequence(trackSelection);
        jassert(pianoLayer);

        PianoChangeGroup groupBefore, groupAfter;

        for (int i = 0; i < trackSelection->size(); ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningLinear(volumeDelta));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void SequencerOperations::changeVolumeMultiplied(Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    { return; }

    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        PianoSequence *pianoLayer = getPianoSequence(trackSelection);
        jassert(pianoLayer);

        PianoChangeGroup groupBefore, groupAfter;
        
        //const double t1 = Time::getMillisecondCounterHiRes();
        
        for (int i = 0; i < trackSelection->size(); ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningMultiplied(volumeFactor));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
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
    
    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        PianoSequence *pianoLayer = getPianoSequence(trackSelection);
        jassert(pianoLayer);
        
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < trackSelection->size(); ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            const float phase = ((nc->getBeat() - startBeat) / (endBeat - startBeat)) * MathConstants<float>::pi * 2.f * numSines;
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningSine(volumeFactor, midline, phase));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
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

    ValueTree tree(Serialization::Clipboard::clipboard);

    float firstBeat = FLT_MAX;

    for (const auto &s : selection.getGroupedSelections())
    {
        const String trackId(s.first);
        const auto trackSelection(s.second);

        ValueTree trackRoot(Serialization::Clipboard::track);
        //trackRoot.setProperty(Serialization::Clipboard::trackId, trackId, nullptr);
        //trackRoot.setProperty(Serialization::Clipboard::trackType, todo, nullptr);
        //trackRoot.setProperty(Serialization::Clipboard::trackMetaInfo, todo, nullptr);

        // Just copy all events, no matter what type they are
        for (int i = 0; i < trackSelection->size(); ++i)
        {
            if (const NoteComponent *noteComponent =
                dynamic_cast<NoteComponent *>(trackSelection->getUnchecked(i)))
            {
                trackRoot.appendChild(noteComponent->getNote().serialize(), nullptr);
                firstBeat = jmin(firstBeat, noteComponent->getBeat());
            }
            else if (const ClipComponent *clipComponent =
                dynamic_cast<ClipComponent *>(trackSelection->getUnchecked(i)))
            {
                trackRoot.appendChild(clipComponent->getClip().serialize(), nullptr);
                firstBeat = jmin(firstBeat, clipComponent->getBeat());
            }
        }

        tree.appendChild(trackRoot, nullptr);
    }

    tree.setProperty(Serialization::Clipboard::firstBeat, firstBeat, nullptr);
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

    forEachValueTreeChildWithType(root, layerElement, Serialization::Core::track)
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
            Array<AnnotationEvent> pastedAnnotations;
            forEachValueTreeChildWithType(layerElement, annotationElement, Serialization::Midi::annotation)
            {
                const auto &ae = AnnotationEvent(annotationsSequence).withParameters(annotationElement).copyWithNewId();
                pastedAnnotations.add(ae.withDeltaBeat(deltaBeat));
            }

            if (pastedAnnotations.size() > 0)
            {
                if (!didCheckpoint)
                {
                    annotationsSequence->checkpoint();
                    didCheckpoint = true;
                }

                annotationsSequence->insertGroup(pastedAnnotations, true);
            }
        }
        else if (auto *automationSequence = dynamic_cast<AutomationSequence *>(selectedTrack->getSequence()))
        {
            Array<AutomationEvent> pastedEvents;
            forEachValueTreeChildWithType(layerElement, autoElement, Serialization::Midi::automationEvent)
            {
                const auto &ae = AutomationEvent(automationSequence).withParameters(autoElement).copyWithNewId();
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
            forEachValueTreeChildWithType(layerElement, noteElement, Serialization::Midi::note)
            {
                const auto &n = Note(pianoSequence).withParameters(noteElement).copyWithNewId();
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

        forEachValueTreeChildWithType(root, patternElement, Serialization::Midi::pattern)
        {
            Array<Clip> pastedClips;
            if (auto *targetPattern = selectedTrack->getPattern())
            {
                forEachValueTreeChildWithType(patternElement, clipElement, Serialization::Midi::clip)
                {
                    Clip &&c = Clip(targetPattern).withParameters(clipElement).copyWithNewId();
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
    int deltaKey, bool shouldCheckpoint, Transport *transport)
{
    if (selection.getNumSelected() == 0 || deltaKey == 0) { return; }

    auto *sequence = selection.getFirstAs<NoteComponent>()->getNote().getSequence();
    const auto operationId = deltaKey > 0 ? CommandIDs::KeyShiftUp : CommandIDs::KeyShiftDown;
    const auto &transactionId = generateTransactionId(operationId, selection);
    const bool repeatsLastAction = sequence->getLastUndoDescription() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;
    
    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        PianoSequence *pianoLayer = getPianoSequence(trackSelection);
        jassert(pianoLayer);

        const int numSelected = trackSelection->size();
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < numSelected; ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            
            Note newNote(nc->getNote().withDeltaKey(deltaKey));
            groupAfter.add(newNote);
            
            if (transport != nullptr && numSelected < 8)
            {
                transport->sendMidiMessage(pianoLayer->getTrackId(),
                    MidiMessage::noteOn(newNote.getTrackChannel(), newNote.getKey(), newNote.getVelocity()));
            }
        }
        
        if (groupBefore.size() > 0)
        {
            if (! didCheckpoint)
            {
                pianoLayer->checkpoint(transactionId);
                didCheckpoint = true;
            }
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void SequencerOperations::shiftBeatRelative(Lasso &selection, float deltaBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaBeat == 0) { return; }

    auto *sequence = selection.getFirstAs<NoteComponent>()->getNote().getSequence();
    const auto operationId = deltaBeat > 0 ? CommandIDs::BeatShiftRight : CommandIDs::BeatShiftLeft;
    const auto &transactionId = generateTransactionId(operationId, selection);
    const bool repeatsLastAction = sequence->getLastUndoDescription() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        PianoSequence *pianoLayer = getPianoSequence(trackSelection);
        jassert(pianoLayer);

        const int numSelected = trackSelection->size();
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < numSelected; ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            
            Note newNote(nc->getNote().withDeltaBeat(deltaBeat));
            groupAfter.add(newNote);
        }
        
        if (groupBefore.size() > 0)
        {
            if (! didCheckpoint)
            {
                pianoLayer->checkpoint(transactionId);
                didCheckpoint = true;
            }
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void SequencerOperations::invertChord(Lasso &selection, 
    int deltaKey, bool shouldCheckpoint, Transport *transport)
{
    if (selection.getNumSelected() == 0)
    { return; }

    bool didCheckpoint = !shouldCheckpoint;
    
    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        auto *pianoSequence = getPianoSequence(trackSelection);
        jassert(pianoSequence);

        const int numSelected = trackSelection->size();
        
        // step 1. sort selection
        PianoChangeGroup selectedNotes;
        
        for (int i = 0; i < numSelected; ++i)
        {
            auto *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
            selectedNotes.addSorted(nc->getNote(), nc->getNote());
        }
        
        // step 2. detect target keys (upper or lower)
        PianoChangeGroup targetNotes;
        
        float prevBeat = 0.f;
        int prevKey = (deltaKey > 0) ? 128 : 0;
        
        float nextBeat = 0.f;
        int nextKey = 128;
        
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
        if (transport != nullptr && numSelected < 8)
        {
            for (int i = 0; i < numSelected; ++i)
            {
                NoteComponent *nc = static_cast<NoteComponent *>(trackSelection->getUnchecked(i));
                transport->sendMidiMessage(pianoSequence->getTrackId(),
                    MidiMessage::noteOn(pianoSequence->getChannel(), nc->getKey(), nc->getVelocity()));
            }
        }
    }
}

void SequencerOperations::rescale(Lasso &selection, Scale::Ptr scaleA, Scale::Ptr scaleB, bool shouldCheckpoint /*= true*/)
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
        const auto key = nc->getKey(); // todo clip offset
        const auto periodNumber = key / scaleA->getBasePeriod();
        const auto inScaleKey = scaleA->getScaleKey(key);
        if (inScaleKey >= 0)
        {
            const auto newChromaticKey = scaleB->getBasePeriod() * periodNumber + scaleB->getChromaticKey(inScaleKey);
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withKey(newChromaticKey));
        }
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
}

// Tries to detect if there's a one key signature that affects the whole sequence.
// If there's none, of if there's more than one, returns false.
bool SequencerOperations::findHarmonicContext(const Lasso &selection, const Clip &clip,
    WeakReference<MidiTrack> keysTrack, Scale::Ptr &outScale, Note::Key &outRootKey)
{
    const auto startBeat = SequencerOperations::findStartBeat(selection) + clip.getBeat();
    const auto endBeat = SequencerOperations::findEndBeat(selection) + clip.getBeat();

    if (const auto *keySignatures = dynamic_cast<KeySignaturesSequence *>(keysTrack->getSequence()))
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

void SequencerOperations::duplicateSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0) { return; }

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

        arrayToAddTo->add(note.copyWithNewId());
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

Array<Note> SequencerOperations::cutEvents(const Array<Note> &notes,
    const Array<float> &relativeCutBeats, bool shouldCheckpoint)
{
    if (notes.isEmpty()) { return {}; }

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
            newEventsToTheRight.add(n.withDeltaBeat(cutBeat).withDeltaLength(-cutBeat).copyWithNewId());
        }
    }

    applyPianoChanges(notes, shortenedNotes, didCheckpoint);
    applyPianoInsertions(newEventsToTheRight, didCheckpoint);
    return newEventsToTheRight;
}

ScopedPointer<MidiTrackNode> SequencerOperations::createPianoTrack(const Lasso &selection)
{
    if (selection.getNumSelected() == 0) { return {}; }

    const auto *track = selection.getFirstAs<NoteComponent>()->getNote().getSequence()->getTrack();
    const auto &instrumentId = track->getTrackInstrumentId();
    const auto &colour = track->getTrackColour();

    ScopedPointer<MidiTrackNode> newItem = new PianoTrackNode({});

    const Clip clip(track->getPattern());
    track->getPattern()->insert(clip, false);

    newItem->setTrackColour(colour, false);
    newItem->setTrackInstrumentId(instrumentId, false);

    PianoChangeGroup copiedContent;
    auto *sequence = static_cast<PianoSequence *>(newItem->getSequence());
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const Note &note = selection.getItemAs<NoteComponent>(i)->getNote();
        copiedContent.add(note.copyWithNewId(sequence));
    }

    sequence->reset();
    sequence->insertGroup(copiedContent, false);

    return newItem;
}

ScopedPointer<MidiTrackNode> SequencerOperations::createPianoTrack(const Array<Note> &events, const Pattern *clips)
{
    if (events.size() == 0) { return {}; }

    const auto *track = events.getReference(0).getSequence()->getTrack();
    const auto &instrumentId = track->getTrackInstrumentId();
    const auto &colour = track->getTrackColour();

    ScopedPointer<MidiTrackNode> newItem = new PianoTrackNode({});
    newItem->setTrackColour(colour, false);
    newItem->setTrackInstrumentId(instrumentId, false);

    PianoChangeGroup copiedContent;
    auto *sequence = static_cast<PianoSequence *>(newItem->getSequence());
    for (const auto &note : events) { copiedContent.add(note.copyWithNewId(sequence)); }
    sequence->reset();
    sequence->insertGroup(copiedContent, false);

    Array<Clip> copiedClips;
    auto *pattern = newItem->getPattern();
    for (const auto *clip : clips->getClips()) { copiedClips.add(clip->copyWithNewId(pattern)); }
    pattern->reset();
    pattern->insertGroup(copiedClips, false);

    return newItem;
}

ScopedPointer<MidiTrackNode> SequencerOperations::createAutomationTrack(const Array<AutomationEvent> &events, const Pattern *clips)
{
    if (events.size() == 0) { return{}; }

    const auto *track = events.getReference(0).getSequence()->getTrack();
    const auto &instrumentId = track->getTrackInstrumentId();
    const auto &cc = track->getTrackControllerNumber();
    const auto &colour = track->getTrackColour();

    ScopedPointer<MidiTrackNode> newItem = new AutomationTrackNode({});
    newItem->setTrackColour(colour, false);
    newItem->setTrackControllerNumber(cc, false);
    newItem->setTrackInstrumentId(instrumentId, false);

    AutoChangeGroup copiedContent;
    auto *sequence = static_cast<AutomationSequence *>(newItem->getSequence());
    for (const auto &event : events) { copiedContent.add(event.copyWithNewId(sequence)); }
    sequence->reset();
    sequence->insertGroup(copiedContent, false);

    Array<Clip> copiedClips;
    auto *pattern = newItem->getPattern();
    for (const auto *clip : clips->getClips()) { copiedClips.add(clip->copyWithNewId(pattern)); }
    pattern->reset();
    pattern->insertGroup(copiedClips, false);

    return newItem;
}
