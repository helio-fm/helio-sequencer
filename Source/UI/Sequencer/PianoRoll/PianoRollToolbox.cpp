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
#include "PianoRollToolbox.h"
#include "ProjectTreeItem.h"
#include "Lasso.h"
#include "Note.h"
#include "AnnotationEvent.h"
#include "AutomationEvent.h"
#include "NoteComponent.h"
#include "AutomationSequence.h"
#include "PianoSequence.h"
#include "AnnotationsSequence.h"
#include "InternalClipboard.h"
#include "Arpeggiator.h"
#include "Transport.h"
#include <float.h>
#include <math.h>

#if !defined M_PI
#define M_PI 3.14159265358979323846
#endif

// все эти адские костыли нужны только затем, чтоб операции выполнялись послойно
//===----------------------------------------------------------------------===//

typedef Array<Note> PianoChangeGroup;
typedef Array<AnnotationEvent> AnnotationChangeGroup;
typedef Array<AutomationEvent> AutoChangeGroup;

template <typename T>
class ChangeGroupProxy : public T, public ReferenceCountedObject
{
public:
    ChangeGroupProxy() {}
    typedef ReferenceCountedObjectPtr<ChangeGroupProxy> Ptr;
};

typedef ChangeGroupProxy<PianoChangeGroup> PianoChangeGroupProxy;
typedef ChangeGroupProxy<AnnotationChangeGroup> AnnotationChangeGroupProxy;
typedef ChangeGroupProxy<AutoChangeGroup> AutoChangeGroupProxy;

typedef HashMap< String, PianoChangeGroupProxy::Ptr > PianoChangeGroupsPerLayer;
typedef HashMap< String, AnnotationChangeGroupProxy::Ptr > AnnotationChangeGroupsPerLayer;
typedef HashMap< String, AutoChangeGroupProxy::Ptr > AutoChangeGroupsPerLayer;

template< typename TEvent, typename TGroup, typename TGroups >
void splitChangeGroupByLayers(const TGroup &group, TGroups &outGroups)
{
    for (int i = 0; i < group.size(); ++i)
    {
        const TEvent &note = group.getUnchecked(i);
        const String &layerId(note.getLayer()->getLayerIdAsString());
        
        typename ChangeGroupProxy<TGroup>::Ptr targetArray;
        
        if (outGroups.contains(layerId))
        {
            targetArray = outGroups[layerId];
        }
        else
        {
            targetArray = new ChangeGroupProxy<TGroup>();
        }
        
        targetArray->add(note);
        outGroups.set(layerId, targetArray);
    }
}

void splitPianoGroupByLayers(const PianoChangeGroup &group, PianoChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<Note, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, outGroups); }

void splitAnnotationsGroupByLayers(const AnnotationChangeGroup &group, AnnotationChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<AnnotationEvent, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, outGroups); }

void splitAutoGroupByLayers(const AutoChangeGroup &group, AutoChangeGroupsPerLayer &outGroups)
{ splitChangeGroupByLayers<AutomationEvent, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, outGroups); }

// returns true if madeAnyChanges
template< typename TEvent, typename TLayer, typename TGroup, typename TGroups >
bool applyChanges(const TGroup &groupBefore,
                  const TGroup &groupAfter,
                  bool &didCheckpoint,
                  bool shouldCheckpoint)
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
            if (shouldCheckpoint)
            {
                targetLayer->checkpoint();
                didCheckpoint = true;
            }
        }
        
        targetLayer->changeGroup(*currentGroupBefore, *currentGroupAfter, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

// returns true if madeAnyChanges
template< typename TEvent, typename TLayer, typename TGroup, typename TGroups >
bool applyRemovals(const TGroup &groupToRemove,
                   bool &didCheckpoint,
                   bool shouldCheckpoint)
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
            if (shouldCheckpoint)
            {
                targetLayer->checkpoint();
                didCheckpoint = true;
            }
        }
        
        targetLayer->removeGroup(*currentRemovalGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

// returns true if madeAnyChanges
template< typename TEvent, typename TLayer, typename TGroup, typename TGroups >
bool applyInsertions(const TGroup &groupToInsert,
                     bool &didCheckpoint,
                     bool shouldCheckpoint)
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
            if (shouldCheckpoint)
            {
                targetLayer->checkpoint();
                didCheckpoint = true;
            }
        }
        
        targetLayer->insertGroup(*currentInsertionGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}


bool applyPianoChanges(const PianoChangeGroup &groupBefore, const PianoChangeGroup &groupAfter, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyChanges<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint); }

bool applyAnnotationChanges(const AnnotationChangeGroup &groupBefore, const AnnotationChangeGroup &groupAfter, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyChanges<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint); }

bool applyAutoChanges(const AutoChangeGroup &groupBefore, const AutoChangeGroup &groupAfter, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyChanges<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint); }


bool applyPianoRemovals(const PianoChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyRemovals<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }

bool applyAnnotationRemovals(const AnnotationChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyRemovals<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }

// особенный случай - я хочу, чтоб хотя бы одно авто-событие на слое оставалось
bool applyAutoRemovals(const AutoChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{
    bool madeAnyChanges = false;
    
    AutoChangeGroupsPerLayer groupsToRemove;
    
    splitChangeGroupByLayers<AutomationEvent, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, groupsToRemove);
    
    AutoChangeGroupsPerLayer::Iterator changeGroupsIterator(groupsToRemove);
    
    while (changeGroupsIterator.next())
    {
        ChangeGroupProxy<AutoChangeGroup>::Ptr currentRemovalGroup(changeGroupsIterator.getValue());
        
        MidiSequence *midiLayer = currentRemovalGroup->getFirst().getLayer();
        AutomationSequence *targetLayer = dynamic_cast<AutomationSequence *>(midiLayer);
        jassert(targetLayer != nullptr);
        
        if ((targetLayer->size() - currentRemovalGroup->size()) <= 1)
        {
            currentRemovalGroup->remove(0);
        }
        
        if (! didCheckpoint)
        {
            if (shouldCheckpoint)
            {
                targetLayer->checkpoint();
                didCheckpoint = true;
            }
        }
        
        targetLayer->removeGroup(*currentRemovalGroup, true);
        madeAnyChanges = true;
    }
    
    return madeAnyChanges;
}

//bool applyAutoRemovals(const AutoChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
//{ applyRemovals<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }


bool applyPianoInsertions(const PianoChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyInsertions<Note, PianoSequence, PianoChangeGroup, PianoChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }

bool applyAnnotationInsertions(const AnnotationChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyInsertions<AnnotationEvent, AnnotationsSequence, AnnotationChangeGroup, AnnotationChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }

bool applyAutoInsertions(const AutoChangeGroup &group, bool &didCheckpoint, bool shouldCheckpoint)
{ return applyInsertions<AutomationEvent, AutomationSequence, AutoChangeGroup, AutoChangeGroupsPerLayer>(group, didCheckpoint, shouldCheckpoint); }


static PianoSequence *getPianoLayer(SelectionProxyArray::Ptr selection)
{
	const auto &firstEvent = selection->getFirstAs<NoteComponent>()->getNote();
	PianoSequence *pianoLayer = static_cast<PianoSequence *>(firstEvent.getSequence());
	return pianoLayer;
}


static AutomationEvent *eventAtPosition(float beatPosition, AutomationSequence *layer)
{
    for (int i = 0; i < layer->size(); ++i)
    {
        AutomationEvent *event = static_cast<AutomationEvent *>(layer->getUnchecked(i));
        
        if (event->getBeat() == beatPosition)
        {
            return event;
        }
    }
    
    return nullptr;
}

static bool isPedalDownAtPosition(float beatPosition, AutomationSequence *pedalLayer)
{
    bool lastEventPedalDown = DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE;
    float lastEventBeat = -FLT_MAX;
    
    for (int i = 0; i < pedalLayer->size(); ++i)
    {
        AutomationEvent *event = static_cast<AutomationEvent *>(pedalLayer->getUnchecked(i));
        
        if (beatPosition >= lastEventBeat &&
            beatPosition < event->getBeat())
        {
            return lastEventPedalDown;
        }

        lastEventPedalDown = event->isPedalDownEvent();
        lastEventBeat = event->getBeat();
    }
    
    return lastEventPedalDown;
}

static bool isPedalUpAtPosition(float beatPosition, AutomationSequence *pedalLayer)
{
    bool lastEventPedalUp = ! DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE;
    float lastEventBeat = -FLT_MAX;
    
    for (int i = 0; i < pedalLayer->size(); ++i)
    {
        AutomationEvent *event = static_cast<AutomationEvent *>(pedalLayer->getUnchecked(i));
        
        if (beatPosition >= lastEventBeat &&
            beatPosition < event->getBeat())
        {
            return lastEventPedalUp;
        }
        
        lastEventPedalUp = event->isPedalUpEvent();
        lastEventBeat = event->getBeat();
    }
    
    return lastEventPedalUp;
}

float PianoRollToolbox::findStartBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return 0.f; }
    
    float startBeat = FLT_MAX;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
		NoteComponent *note = static_cast<NoteComponent *>(selection.getSelectedItem(i));
		startBeat = jmin(startBeat, note->getBeat());
    }
    
    return startBeat;
}

float PianoRollToolbox::findEndBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return 0.f; }
    
    float endBeat = -FLT_MAX;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const NoteComponent *nc = static_cast<const NoteComponent *>(selection.getSelectedItem(i));
        const float beatPlusLength = nc->getBeat() + nc->getLength();
		endBeat = jmax(endBeat, beatPlusLength);
    }
    
    return endBeat;
}

float PianoRollToolbox::findStartBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    { return 0.f; }
    
    float startBeat = FLT_MAX;
    
    for (int i = 0; i < selection.size(); ++i)
    {
        if (startBeat > selection.getUnchecked(i).getBeat())
        {
            startBeat = selection.getUnchecked(i).getBeat();
            //Logger::writeToLog("> " + String(startBeat));
        }
    }
    
    return startBeat;
}

float PianoRollToolbox::findEndBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    { return 0.f; }
    
    float endBeat = -FLT_MAX;
    
    for (int i = 0; i < selection.size(); ++i)
    {
        const Note &&nc = selection.getUnchecked(i);
        const float beatPlusLength = nc.getBeat() + nc.getLength();
        //Logger::writeToLog(String(endBeat) + " < " + String(beatPlusLength) + " = " + String(endBeat < beatPlusLength));
        
        if (endBeat < beatPlusLength)
        {
            endBeat = beatPlusLength;
            //Logger::writeToLog(String(endBeat) + " <");
        }
    }
    
    return endBeat;
}

static float snappedBeat(float beat, float snapsPerBeat)
{
    return roundf(beat / snapsPerBeat) * snapsPerBeat;
}



void PianoRollToolbox::wipeSpace(Array<MidiSequence *> layers,
                                float startBeat, float endBeat,
                                bool shouldKeepCroppedNotes, bool shouldCheckpoint)
{
    // массив 1: найти все события внутри области
    // массив 2: создать ноты-обрезки, для тех нот, которые полностью не вмещаются
    // отложенно удалить все из массива 1
    // и добавить все из массива 2

    bool didCheckpoint = false;
    
    PianoChangeGroup pianoRemoveGroup;
    PianoChangeGroup pianoInsertGroup;

    AnnotationChangeGroup annotationsRemoveGroup;
    AutoChangeGroup autoRemoveGroup;
   
    for (int i = 0; i < layers.size(); ++i)
    {
        if (nullptr != dynamic_cast<PianoSequence *>(layers.getUnchecked(i)))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i)))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AutomationSequence *>(layers.getUnchecked(i)))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(layers.getUnchecked(i));
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
    
    applyPianoRemovals(pianoRemoveGroup, didCheckpoint, shouldCheckpoint);
    applyAnnotationRemovals(annotationsRemoveGroup, didCheckpoint, shouldCheckpoint);
    applyAutoRemovals(autoRemoveGroup, didCheckpoint, shouldCheckpoint);

    if (shouldKeepCroppedNotes)
    {
        applyPianoInsertions(pianoInsertGroup, didCheckpoint, shouldCheckpoint);
    }
}

void PianoRollToolbox::shiftEventsToTheLeft(Array<MidiSequence *> layers, float targetBeat, float beatOffset, bool shouldCheckpoint)
{
    bool didCheckpoint = false;
    
    PianoChangeGroup pianoGroupBefore;
    PianoChangeGroup pianoGroupAfter;
    
    AnnotationChangeGroup annotationsGroupBefore;
    AnnotationChangeGroup annotationsGroupAfter;
    
    AutoChangeGroup autoGroupBefore;
    AutoChangeGroup autoGroupAfter;
    
    for (int i = 0; i < layers.size(); ++i)
    {
        if (nullptr != dynamic_cast<PianoSequence *>(layers.getUnchecked(i)))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i)))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AutomationSequence *>(layers.getUnchecked(i)))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(layers.getUnchecked(i));
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
    
    applyPianoChanges(pianoGroupBefore, pianoGroupAfter, didCheckpoint, shouldCheckpoint);
    applyAnnotationChanges(annotationsGroupBefore, annotationsGroupAfter, didCheckpoint, shouldCheckpoint);
    applyAutoChanges(autoGroupBefore, autoGroupAfter, didCheckpoint, shouldCheckpoint);
}

void PianoRollToolbox::shiftEventsToTheRight(Array<MidiSequence *> layers, float targetBeat, float beatOffset, bool shouldCheckpoint)
{
    bool didCheckpoint = false;
    
    PianoChangeGroup groupBefore;
    PianoChangeGroup groupAfter;
    
    AnnotationChangeGroup annotationsGroupBefore;
    AnnotationChangeGroup annotationsGroupAfter;
    
    AutoChangeGroup autoGroupBefore;
    AutoChangeGroup autoGroupAfter;
    
    for (int i = 0; i < layers.size(); ++i)
    {
        if (nullptr != dynamic_cast<PianoSequence *>(layers.getUnchecked(i)))
        {
            PianoSequence *layer = dynamic_cast<PianoSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i)))
        {
            AnnotationsSequence *layer = dynamic_cast<AnnotationsSequence *>(layers.getUnchecked(i));
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
        else if (nullptr != dynamic_cast<AutomationSequence *>(layers.getUnchecked(i)))
        {
            AutomationSequence *layer = dynamic_cast<AutomationSequence *>(layers.getUnchecked(i));
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
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint);
    applyAnnotationChanges(annotationsGroupBefore, annotationsGroupAfter, didCheckpoint, shouldCheckpoint);
    applyAutoChanges(autoGroupBefore, autoGroupAfter, didCheckpoint, shouldCheckpoint);
}


void PianoRollToolbox::snapSelection(Lasso &selection, float snapsPerBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = false;
    
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
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint);
}


void PianoRollToolbox::removeOverlaps(Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = false;
    
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
            //Logger::writeToLog("snap");
            group0Before.add(nc->getNote());
            group0After.add(nc->getNote().withBeat(startBeatSnap).withLength(lengthSnap));
        }
    }
    
    applyPianoChanges(group0Before, group0After, didCheckpoint, shouldCheckpoint);
    
    
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
                //Logger::writeToLog("edit1");
                const float newLength = nc->getLength() + deltaBeats;
                group1Before.add(nc->getNote());
                group1After.add(nc->getNote().withLength(newLength));
            }
        }
        
        step1HasChanges = applyPianoChanges(group1Before, group1After, didCheckpoint, shouldCheckpoint);
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
        step2HasChanges = false;
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
                //Logger::writeToLog("edit2");
                group2Before.add(overlappingNote->getNote());
                group2After.add(overlappingNote->getNote().withDeltaLength(deltaBeats));
            }
        }
        
        step2HasChanges = applyPianoChanges(group2Before, group2After, didCheckpoint, shouldCheckpoint);
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
        step3HasChanges = false;
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
                //Logger::writeToLog("edit3 " + String(nc->getNote().getLength()) + ":" + String(newLength));
                group3Before.add(nc->getNote());
                group3After.add(nc->getNote().withLength(newLength));
            }
        }
        
        step3HasChanges = applyPianoChanges(group3Before, group3After, didCheckpoint, shouldCheckpoint);
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

    applyPianoRemovals(removalGroup, didCheckpoint, shouldCheckpoint);
}

void PianoRollToolbox::removeDuplicates(Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    { return; }
    
    HashMap<MidiEvent::Id, Note> deferredRemoval;
    HashMap<MidiEvent::Id, Note> unremovableNotes;
    
    bool didCheckpoint = false;

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
    
    applyPianoRemovals(removalGroup, didCheckpoint, shouldCheckpoint);
}


void PianoRollToolbox::moveToLayer(Lasso &selection, MidiSequence *layer, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    PianoSequence *targetLayer = dynamic_cast<PianoSequence *>(layer);
    jassert(targetLayer != nullptr);

    bool didCheckpoint = false;
    PianoChangeGroupsPerLayer deferredRemovals;
    PianoChangeGroupProxy::Ptr insertionsForTargetLayer(new PianoChangeGroupProxy());
    
    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        MidiSequence *midiLayer = layerSelection->getFirstAs<NoteComponent>()->getNote().getLayer();

        if (PianoSequence *sourcePianoLayer = dynamic_cast<PianoSequence *>(midiLayer))
        {
            // здесь - смотрим, если
            if (sourcePianoLayer != targetLayer)
            {
                // пройтись по всем нотам, если в целевом слое уже есть такая же нота - только удалить из исходного
                // иначе - вырезать из исходного, добавить в целевой
                // и чекпойнт, если еще не сделали, и если надо
                
                PianoChangeGroupProxy::Ptr removalsForThisLayer(new PianoChangeGroupProxy());
                
                for (int i = 0; i < layerSelection->size(); ++i)
                {
                    const Note &n1 = layerSelection->getItemAs<NoteComponent>(i)->getNote();
                    
                    bool targetHasTheSameNote = false;
                    
                    for (int j = 0; j < targetLayer->size(); ++j)
                    {
                        const Note *n2 = static_cast<Note *>(targetLayer->getUnchecked(j));
                        
                        if (fabs(n1.getBeat() - n2->getBeat()) < 0.01f &&
                            fabs(n1.getLength() - n2->getLength()) < 0.01f &&
                            n1.getKey() == n2->getKey())
                        {
                            Logger::writeToLog("targetHasTheSameNote");
                            targetHasTheSameNote = true;
                            break;
                        }
                    }
                    
                    removalsForThisLayer->add(n1);
                    
                    if (! targetHasTheSameNote)
                    {
                        insertionsForTargetLayer->add(n1);
                    }
                    
                    if (!didCheckpoint && shouldCheckpoint)
                    {
                        didCheckpoint = true;
                        sourcePianoLayer->checkpoint();
                        targetLayer->checkpoint(); // compatibility with per-layer undo system?
                    }
                }
                
                deferredRemovals.set(sourcePianoLayer->getLayerIdAsString(), removalsForThisLayer);
            }
        }
    }
    
    // events removal
    PianoChangeGroupsPerLayer::Iterator deferredRemovalIterator(deferredRemovals);
    while (deferredRemovalIterator.next())
    {
        PianoChangeGroupProxy::Ptr groupToRemove(deferredRemovalIterator.getValue());
        MidiSequence *midiLayer = groupToRemove->getFirst().getLayer();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(midiLayer);
        pianoLayer->removeGroup(*groupToRemove, true);
    }
    
    // events insertions
    targetLayer->insertGroup(*insertionsForTargetLayer, true);
}


bool PianoRollToolbox::arpeggiateUsingClipboardAsPattern(Lasso &selection, bool shouldCheckpoint)
{
    XmlElement *xml = InternalClipboard::getCurrentContent();
    Arpeggiator arp = Arpeggiator().withSequenceFromXml(*xml);
    
    if (arp.isEmpty())
    {
        return false;
    }
    
    return PianoRollToolbox::arpeggiate(selection, arp, shouldCheckpoint);
}


bool PianoRollToolbox::arpeggiate(Lasso &selection,
                                 const Arpeggiator &arp,
                                 bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return false;
    }

    if (arp.isEmpty())
    {
        return false;
    }
    
    bool didCheckpoint = false;
    PianoChangeGroupsPerLayer deferredRemovals;
    PianoChangeGroupsPerLayer deferredInsertions;

    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());

		PianoSequence *pianoLayer = getPianoLayer(layerSelection);
		
        jassert(pianoLayer);

        // 1. sort selection
        PianoChangeGroupProxy::Ptr sortedSelection(new PianoChangeGroupProxy());
        
        for (int i = 0; i < layerSelection->size(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            sortedSelection->addSorted(nc->getNote(), nc->getNote());
        }
        
        const float selectionStartBeat = PianoRollToolbox::findStartBeat(*sortedSelection);
        
        
        // 2. split chords (and sequences)
        Array<PianoChangeGroup> chords;
        
        float prevBeat = 0.f;
        int prevKey = 0;
        
        float nextBeat = 0.f;
        int nextKey = 128;
        
        PianoChangeGroup currentChord;
        bool currentChordNotesHasSameBeat = true;
        
        for (int i = 0; i < sortedSelection->size(); ++i)
        {
            if (i != (sortedSelection->size() - 1))
            {
                nextKey = sortedSelection->getUnchecked(i + 1).getKey();
                nextBeat = sortedSelection->getUnchecked(i + 1).getBeat();
            }
            else
            {
                nextKey = sortedSelection->getUnchecked(i).getKey() - 12;
                nextBeat = sortedSelection->getUnchecked(i).getBeat() - 12;
            }
            
            const bool beatWillChange = (sortedSelection->getUnchecked(i).getBeat() != nextBeat);
            const bool newChordWillStart = (beatWillChange && currentChord.size() > 1 && currentChordNotesHasSameBeat);
            const bool newSequenceWillStart = (sortedSelection->getUnchecked(i).getKey() > prevKey &&
                                               sortedSelection->getUnchecked(i).getKey() > nextKey);
            const bool chordEndsHere = newChordWillStart || newSequenceWillStart;
            
            if (beatWillChange)
            { currentChordNotesHasSameBeat = false; }
            
            currentChord.add(sortedSelection->getUnchecked(i));
            //Logger::writeToLog(String(sortedSelection[i].getKey()) + ", nc " + String(newChordWillStart) + ", ns " + String(newSequenceWillStart));
            
            if (chordEndsHere)
            {
                //Logger::writeToLog("--");
                chords.add(currentChord);
                currentChord.clear();
                currentChordNotesHasSameBeat = true;
            }
            
            prevKey = sortedSelection->getUnchecked(i).getKey();
            prevBeat = sortedSelection->getUnchecked(i).getBeat();
        }
        
        // 3. parse arp
        Array<Arpeggiator::Key> arpKeys(arp.createArpKeys());
        
        // 4. arpeggiate every chord
        PianoChangeGroupProxy::Ptr result(new PianoChangeGroupProxy());
        
        float patternBeatOffset = 0.f;
        int patternEventIndex = 0;
        
        if (chords.size() == 0)
        {
            return false;
        }
        
        // needed for chord-mapped arp
        const PianoChangeGroup &&firstChord = chords.getUnchecked(0);
        const float firstChordStart = PianoRollToolbox::findStartBeat(firstChord);
        const float firstChordEnd = PianoRollToolbox::findEndBeat(firstChord);
        const float firstChordLength = firstChordEnd - firstChordStart;
        
        for (int i = 0; i < chords.size(); ++i)
        {
            const PianoChangeGroup &&chord = chords.getUnchecked(i);
            const float chordStart = PianoRollToolbox::findStartBeat(chord);
            const float chordEnd = PianoRollToolbox::findEndBeat(chord);
            //const float chordLength = chordEnd - chordStart;
            
            if (arp.hasRelativeMapping())
            {
                // Arp sequence as is
                while (1)
                {
                    const int maxKeyIndex = chord.size() - 1;
                    int keyIndex = jmin(arpKeys[patternEventIndex].keyIndex, maxKeyIndex);
                    
                    if (arp.isReversed())
                    { keyIndex = maxKeyIndex - keyIndex; }
                    
                    const float newBeat = selectionStartBeat + patternBeatOffset + arpKeys[patternEventIndex].beatStart;
                    const float newLength = arpKeys[patternEventIndex].beatLength;
                    const float newVelocity = (chord[keyIndex].getVelocity() + arpKeys[patternEventIndex].velocity) / 2.f;
                    
                    if (newBeat >= chordEnd)
                    {
                        if (arp.limitsToChord())
                        {
                            patternEventIndex = 0;
                            patternBeatOffset = chordEnd - selectionStartBeat;
                        }
                        
                        break;
                    }
                    
                    result->add(chord[keyIndex].
                                withDeltaKey(arpKeys[patternEventIndex].octaveShift).
                                withBeat(newBeat).
                                withLength(newLength).
                                withVelocity(newVelocity).
                                copyWithNewId());
                    
                    patternEventIndex += 1;
                    
                    if (patternEventIndex >= arpKeys.size())
                    {
                        patternEventIndex = 0;
                        patternBeatOffset += arpKeys[patternEventIndex].sequenceLength;
                    }
                }
            }
            else
            {
                // Map sequence to every chord's length (todo - or first chird's length?)
                for (auto && arpKey : arpKeys)
                {
                    // arp supports up to 12 notes in a single chord or progression to be arped
                    const int maxKeyIndex = chord.size() - 1;
                    int keyIndex = jmin(arpKey.keyIndex, maxKeyIndex);
                    
                    if (arp.isReversed())
                    { keyIndex = maxKeyIndex - keyIndex; }
                    
                    //const float newBeat = chordStart + chordLength * arpKeys[j].absStart;
                    //const float newLength = chordLength * arpKeys[j].absLength;
                    
                    // todo fix pauses with long chords
                    const float newBeat = chordStart + firstChordLength * arpKey.absStart;
                    const float newLength = firstChordLength * arpKey.absLength;
                    const float newVelocity = (chord[keyIndex].getVelocity() + arpKey.velocity) / 2.f;
                    
                    if (newBeat >= chordEnd)
                    {
                        break;
                    }
                    
                    result->add(chord[keyIndex].
                                withDeltaKey(arpKey.octaveShift).
                                withBeat(newBeat).
                                withLength(newLength).
                                withVelocity(newVelocity).
                                copyWithNewId());
                }
            }
        }
        
        // 5. remove selection and add result
        if (! didCheckpoint)
        {
            if (shouldCheckpoint)
            {
                pianoLayer->checkpoint();
                didCheckpoint = true;
            }
        }
        
        deferredRemovals.set(pianoLayer->getLayerIdAsString(), sortedSelection);
        deferredInsertions.set(pianoLayer->getLayerIdAsString(), result);
    }
    
    // events removal
    PianoChangeGroupsPerLayer::Iterator deferredRemovalIterator(deferredRemovals);
    while (deferredRemovalIterator.next())
    {
        PianoChangeGroupProxy::Ptr groupToRemove(deferredRemovalIterator.getValue());
        MidiSequence *midiLayer = groupToRemove->getFirst().getLayer();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(midiLayer);
        pianoLayer->removeGroup(*groupToRemove, true);
    }
    
    // events insertions
    PianoChangeGroupsPerLayer::Iterator deferredInsertionsIterator(deferredInsertions);
    while (deferredInsertionsIterator.next())
    {
        PianoChangeGroupProxy::Ptr groupToInsert(deferredInsertionsIterator.getValue());
        MidiSequence *midiLayer = groupToInsert->getFirst().getLayer();
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(midiLayer);
        pianoLayer->insertGroup(*groupToInsert, true);
    }
    
    return true;
}

void PianoRollToolbox::randomizeVolume(Lasso &selection, float factor, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    bool didCheckpoint = false;
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
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint);
}

void PianoRollToolbox::fadeOutVolume(Lasso &selection, float factor, bool shouldCheckpoint)
{
    // Smooth fade out like
    // 1 - ((x / sqrt(x)) * factor)
    
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    float minBeat = FLT_MAX;
    float maxBeat = -FLT_MAX;
    bool didCheckpoint = false;
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
    
    applyPianoChanges(groupBefore, groupAfter, didCheckpoint, shouldCheckpoint);
}

void PianoRollToolbox::startTuning(Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return; }
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->startTuning();
    }
}

void PianoRollToolbox::changeVolumeLinear(Lasso &selection, float volumeDelta)
{
    if (selection.getNumSelected() == 0)
    { return; }

    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
		PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);

        PianoChangeGroup groupBefore, groupAfter;

        for (int i = 0; i < layerSelection->size(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningLinear(volumeDelta));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void PianoRollToolbox::changeVolumeMultiplied(Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    { return; }

    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);

        PianoChangeGroup groupBefore, groupAfter;
        
        //const double t1 = Time::getMillisecondCounterHiRes();
        
        for (int i = 0; i < layerSelection->size(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningMultiplied(volumeFactor));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
        
        //const double t2 = Time::getMillisecondCounterHiRes();
        //const double t3 = t2 - t1;
        
        //Logger::writeToLog(String(t3));
    }
}

void PianoRollToolbox::changeVolumeSine(Lasso &selection, float volumeFactor)
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
    
    const float startBeat = PianoRollToolbox::findStartBeat(selection);
    const float endBeat = PianoRollToolbox::findEndBeat(selection);
    
    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);
        
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < layerSelection->size(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            const float phase = ((nc->getBeat() - startBeat) / (endBeat - startBeat)) * M_PI * 2.f * numSines;
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuningSine(volumeFactor, midline, phase));
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void PianoRollToolbox::endTuning(Lasso &selection)
{
    jassert(selection.getNumSelected() > 0);
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->endTuning();
    }
}

void PianoRollToolbox::deleteSelection(Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    { return; }

    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);
        
        PianoChangeGroup notesToDelete;
        
        for (int i = 0; i < layerSelection->size(); ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            notesToDelete.add(nc->getNote());
        }
        
        for (auto && i : notesToDelete)
        {
            pianoLayer->remove(i, true);
        }
    }
}

void PianoRollToolbox::shiftKeyRelative(Lasso &selection,
	int deltaKey, bool shouldCheckpoint, Transport *transport)
{
    if (selection.getNumSelected() == 0)
    { return; }

    bool didCheckpoint = false;
    
    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);

        const int numSelected = layerSelection->size();
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < numSelected; ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            
            Note newNote(nc->getNote().withDeltaKey(deltaKey));
            groupAfter.add(newNote);
            
            if (transport != nullptr && numSelected < 8)
            {
                transport->sendMidiMessage(pianoLayer->getLayerIdAsString(),
					MidiMessage::noteOn(pianoLayer->getChannel(), newNote.getKey(), newNote.getVelocity()));
            }
        }
        
        if (groupBefore.size() > 0)
        {
            if (! didCheckpoint)
            {
                if (shouldCheckpoint)
                {
                    pianoLayer->checkpoint();
                    didCheckpoint = true;
                }
            }
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void PianoRollToolbox::shiftBeatRelative(Lasso &selection, float deltaBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    { return; }

    bool didCheckpoint = false;

    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);

        const int numSelected = layerSelection->size();
        PianoChangeGroup groupBefore, groupAfter;
        
        for (int i = 0; i < numSelected; ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
            groupBefore.add(nc->getNote());
            
            Note newNote(nc->getNote().withDeltaBeat(deltaBeat));
            groupAfter.add(newNote);
        }
        
        if (groupBefore.size() > 0)
        {
            if (! didCheckpoint)
            {
                if (shouldCheckpoint)
                {
                    pianoLayer->checkpoint();
                    didCheckpoint = true;
                }
            }
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
    }
}

void PianoRollToolbox::inverseChord(Lasso &selection, 
	int deltaKey, bool shouldCheckpoint, Transport *transport)
{
    if (selection.getNumSelected() == 0)
    { return; }

    bool didCheckpoint = false;
    
    const Lasso::GroupedSelections &selections = selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);
    
    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        PianoSequence *pianoLayer = getPianoLayer(layerSelection);
        jassert(pianoLayer);

        const int numSelected = layerSelection->size();
        
        // step 1. sort selection
        PianoChangeGroup selectedNotes;
        
        for (int i = 0; i < numSelected; ++i)
        {
            NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
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
        
        //Logger::writeToLog("Shifting " + String(targetNotes.size()) + " notes");
        
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
                if (shouldCheckpoint)
                {
                    pianoLayer->checkpoint();
                    didCheckpoint = true;
                }
            }
        }
        
        pianoLayer->changeGroup(groupBefore, groupAfter, true);
        
        // step 4. make em sound, if needed
        if (transport != nullptr && numSelected < 8)
        {
            for (int i = 0; i < numSelected; ++i)
            {
                NoteComponent *nc = static_cast<NoteComponent *>(layerSelection->getUnchecked(i));
                transport->sendMidiMessage(pianoLayer->getLayerIdAsString(),
					MidiMessage::noteOn(pianoLayer->getChannel(), nc->getKey(), nc->getVelocity()));
            }
        }
    }
}
