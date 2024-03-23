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
#include "PatternOperations.h"
#include "SequencerOperations.h"
#include "ProjectNode.h"
#include "UndoStack.h"
#include "PianoTrackNode.h"
#include "PianoSequence.h"
#include "AutomationTrackNode.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "Note.h"
#include "AutomationEvent.h"
#include "Clip.h"
#include "ClipComponent.h"
#include "Pattern.h"
#include "Lasso.h"
#include "UndoActionIDs.h"

float PatternOperations::findStartBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    {
        return 0.f;
    }

    float startBeat = FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *cc = selection.getItemAs<ClipComponent>(i);
        const auto *track = cc->getClip().getPattern()->getTrack();
        const auto sequenceStart = track->getSequence()->getFirstBeat();
        startBeat = jmin(startBeat, sequenceStart + cc->getBeat());
    }

    return startBeat;
}

float PatternOperations::findEndBeat(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    {
        return 0.f;
    }

    float endBeat = -FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *cc = selection.getItemAs<ClipComponent>(i);
        const auto *sequence = cc->getClip().getPattern()->getTrack()->getSequence();
        const auto sequenceStart = sequence->getFirstBeat();
        const auto sequenceLength = sequence->isEmpty() ?
            Globals::Defaults::emptyClipLength : sequence->getLengthInBeats();

        const float beatPlusLength = sequenceStart + cc->getBeat() + sequenceLength;
        endBeat = jmax(endBeat, beatPlusLength);
    }

    return endBeat;
}

void PatternOperations::deleteSelection(const Lasso &selection,
    ProjectNode &project, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    Array<Clip> clips;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        clips.add(selection.getItemAs<ClipComponent>(i)->getClip());
    }

    PatternOperations::deleteSelection(clips, project, shouldCheckpoint);
}

void PatternOperations::deleteSelection(const Array<Clip> &selection,
    ProjectNode &project, bool shouldCheckpoint /*= true*/)
{
    if (selection.isEmpty())
    {
        return;
    }

    OwnedArray<Array<Clip>> groupedByPattern;
    for (const auto &clip : selection)
    {
        Pattern *ownerPattern = clip.getPattern();
        Array<Clip> *arrayToAddTo = nullptr;

        for (auto *patternSelection : groupedByPattern)
        {
            if (!patternSelection->isEmpty() &&
                patternSelection->getUnchecked(0).getPattern() == ownerPattern)
            {
                arrayToAddTo = patternSelection;
                break;
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Clip>();
            groupedByPattern.add(arrayToAddTo);
        }

        arrayToAddTo->add(clip);
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (auto *patternSelection : groupedByPattern)
    {
        auto *pattern = patternSelection->getUnchecked(0).getPattern();

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            pattern->checkpoint();
        }

        // Delete the entire track if all its clips have been selected:
        if (pattern->size() == patternSelection->size())
        {
            project.removeTrack(*pattern->getTrack());
        }
        else
        {
            pattern->removeGroup(*patternSelection, true);
        }
    }
}

void PatternOperations::transposeClips(const Lasso &selection, int deltaKey, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaKey == 0)
    {
        return;
    }

    auto *pattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaKey > 0 ? UndoActionIDs::ClipTransposeUp : UndoActionIDs::ClipTransposeDown;
    const auto transactionId = selection.generateTransactionId(operationId);
    const bool repeatsLastAction = pattern->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        auto *track = clip.getPattern()->getTrack();

        // only a piano clip can be transposed
        if (nullptr == dynamic_cast<PianoSequence *>(track->getSequence()))
        {
            continue;
        }

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            pattern->checkpoint(transactionId);
        }

        clip.getPattern()->change(clip, clip.withDeltaKey(deltaKey), true);
    }
}

void PatternOperations::tuneClips(const Lasso &selection, float deltaVelocity, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaVelocity == 0.f)
    {
        return;
    }

    auto *pattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaVelocity > 0 ? UndoActionIDs::ClipVolumeUp : UndoActionIDs::ClipVolumeDown;
    const auto transactionId = selection.generateTransactionId(operationId);
    const bool repeatsLastAction = pattern->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        const auto changes = clip.withDeltaVelocity(deltaVelocity);
        if (clip.getVelocity() != changes.getVelocity())
        {
            if (!didCheckpoint)
            {
                didCheckpoint = true;
                clip.getPattern()->checkpoint(transactionId);
            }

            clip.getPattern()->change(clip, changes, true);
        }
    }
}

void PatternOperations::shiftBeatRelative(const Lasso &selection, float deltaBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaBeat == 0)
    {
        return;
    }

    auto *firstPattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaBeat > 0 ? UndoActionIDs::BeatShiftRight : UndoActionIDs::BeatShiftLeft;
    const auto transactionId = selection.generateTransactionId(operationId);
    const bool repeatsLastAction = firstPattern->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *cc = selection.getItemAs<ClipComponent>(i);

        auto *pattern = cc->getClip().getPattern();
        if (!didCheckpoint)
        {
            pattern->checkpoint(transactionId);
            didCheckpoint = true;
        }

        pattern->change(cc->getClip(), cc->getClip().withDeltaBeat(deltaBeat), true);
    }
}

void PatternOperations::cutClip(ProjectNode &project, const Clip &clip,
    float relativeCutBeat, bool shouldRenameNewTrack, bool shouldCheckpoint)
{
    MidiTrack *track = clip.getPattern()->getTrack();

    const String newName = shouldRenameNewTrack ?
        SequencerOperations::generateNextNameForNewTrack(track->getTrackName(), project.getAllTrackNames()) :
        track->getTrackName();

    const float cutBeat = relativeCutBeat - clip.getBeat();

    // If this is a piano roll, need to cut events, if any intersect the given beat.
    // Create a new track - either piano or automation, depending on the selected one.
    // Delete events and perform track insert action, where track name would have a suffix like "counterpoint 3".

    if (auto *pianoTrack = dynamic_cast<PianoTrackNode *>(track))
    {
        Array<Note> intersectedEvents;
        Array<float> intersectionPoints;
        auto *sequence = static_cast<PianoSequence *>(track->getSequence());
        for (int i = 0; i < sequence->size(); ++i)
        {
            auto *note = static_cast<Note *>(sequence->getUnchecked(i));
            if (note->getBeat() < cutBeat && (note->getBeat() + note->getLength()) > cutBeat)
            {
                intersectedEvents.add(*note);
                intersectionPoints.add(cutBeat - note->getBeat());
            }
        }

        // assumes that any changes will be done anyway, i.e. too simple check, but ok for now
        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        SequencerOperations::cutNotes(intersectedEvents, intersectionPoints, false);

        Array<Note> eventsToBeMoved;
        for (int i = 0; i < sequence->size(); ++i)
        {
            auto *note = static_cast<Note *>(sequence->getUnchecked(i));
            if (note->getBeat() >= cutBeat)
            {
                eventsToBeMoved.add(*note);
            }
        }

        const auto newTrack = SequencerOperations::createPianoTrack(eventsToBeMoved, clip.getPattern());
        const auto trackTemplate = newTrack->serialize();

        sequence->removeGroup(eventsToBeMoved, true);
        project.getUndoStack()->perform(new PianoTrackInsertAction(project, &project, trackTemplate, newName));
    }
    else if (auto *autoTrack = dynamic_cast<AutomationTrackNode *>(track))
    {
        Array<AutomationEvent> eventsToBeMoved;
        auto *sequence = static_cast<AutomationSequence *>(track->getSequence());
        for (int i = 0; i < sequence->size(); ++i)
        {
            auto *event = static_cast<AutomationEvent *>(sequence->getUnchecked(i));
            if (event->getBeat() >= cutBeat)
            {
                eventsToBeMoved.add(*event);
            }
        }

        const auto newTrack = SequencerOperations::createAutomationTrack(eventsToBeMoved, clip.getPattern());
        const auto trackTemplate = newTrack->serialize();

        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        sequence->removeGroup(eventsToBeMoved, true);
        project.getUndoStack()->perform(new AutomationTrackInsertAction(project, &project, trackTemplate, newName));
    }
}

void PatternOperations::duplicateSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    OwnedArray<Array<Clip>> changesByPattern;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const Clip &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        const Pattern *ownerPattern = clip.getPattern();
        Array<Clip> *arrayToAddTo = nullptr;

        for (int j = 0; j < changesByPattern.size(); ++j)
        {
            if (changesByPattern.getUnchecked(j)->size() > 0)
            {
                if (changesByPattern.getUnchecked(j)->getUnchecked(0).getPattern() == ownerPattern)
                {
                    arrayToAddTo = changesByPattern.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Clip>();
            changesByPattern.add(arrayToAddTo);
        }

        arrayToAddTo->add(clip.withNewId());
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < changesByPattern.size(); ++i)
    {
        auto *pattern = changesByPattern.getUnchecked(i)->getUnchecked(0).getPattern();

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            pattern->checkpoint();
        }

        pattern->insertGroup(*changesByPattern.getUnchecked(i), true);
    }
}

bool PatternOperations::lassoContainsMutedClip(const Lasso &selection)
{
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (selection.getItemAs<ClipComponent>(i)->getClip().isMuted())
        {
            return true;
        }
    }

    return false;
}

bool PatternOperations::lassoContainsSoloedClip(const Lasso &selection)
{
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (selection.getItemAs<ClipComponent>(i)->getClip().isSoloed())
        {
            return true;
        }
    }

    return false;
}

void PatternOperations::toggleMuteClips(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    // If have at least one muted clip, unmute all, otherwise mute all
    const bool newMuteState = !lassoContainsMutedClip(selection);

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();

        if (clip.isMuted() == newMuteState)
        {
            continue;
        }

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            clip.getPattern()->checkpoint();
        }

        clip.getPattern()->change(clip, clip.withMute(newMuteState), true);
    }
}

void PatternOperations::toggleSoloClips(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    // when having at least one soloed clip, unsolo all, otherwise solo all
    const bool newSoloState = !lassoContainsSoloedClip(selection);

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();

        if (clip.isSoloed() == newSoloState ||
            (!clip.canBeSoloed() && !clip.isSoloed()))
        {
            continue;
        }

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            clip.getPattern()->checkpoint();
        }

        clip.getPattern()->change(clip, clip.withSolo(newSoloState), true);
    }
}

void PatternOperations::toggleMuteClip(const Clip &clip, bool shouldCheckpoint /*= true*/)
{
    auto *pattern = clip.getPattern();

    if (shouldCheckpoint)
    {
        pattern->checkpoint();
    }

    pattern->change(clip, clip.withMute(!clip.isMuted()), true);
}

void PatternOperations::toggleSoloClip(const Clip &clip, bool shouldCheckpoint /*= true*/)
{
    auto *pattern = clip.getPattern();

    // the solo flag can only be set in piano tracks
    if (!clip.canBeSoloed() && !clip.isSoloed())
    {
        return;
    }

    if (shouldCheckpoint)
    {
        pattern->checkpoint();
    }

    pattern->change(clip, clip.withSolo(!clip.isSoloed()), true);
}

void PatternOperations::quantize(const Lasso &selection,
    float bar, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    FlatHashSet<String, StringHash> processedTracks;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        const auto *track = clip.getPattern()->getTrack();
        if (processedTracks.contains(track->getTrackId()))
        {
            continue;
        }

        auto *pianoSequence = dynamic_cast<PianoSequence *>(track->getSequence());
        if (pianoSequence == nullptr)
        {
            jassertfalse; // will only quantize notes
            continue;
        }

        const auto hasMadeChanges = SequencerOperations::quantize(*pianoSequence, bar, !didCheckpoint);
        didCheckpoint = didCheckpoint || hasMadeChanges;

        processedTracks.insert(track->getTrackId());
    }
}

bool PatternOperations::shiftTempo(const Lasso &selection,
    int bpmDelta, bool shouldCheckpoint /*= true*/)
{
    if (bpmDelta == 0 || selection.getNumSelected() == 0)
    {
        return false;
    }

    auto *pattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = bpmDelta > 0 ? UndoActionIDs::ShiftTempoUp : UndoActionIDs::ShiftTempoDown;
    const auto transactionId = selection.generateTransactionId(operationId);
    const bool repeatsLastOperation = pattern->getLastUndoActionId() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastOperation;

    FlatHashSet<String, StringHash> processedTracks;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        auto *track = clip.getPattern()->getTrack();

        if (!track->isTempoTrack())
        {
            continue;
        }

        if (processedTracks.contains(track->getTrackId()))
        {
            continue;
        }

        auto *autoSequence = dynamic_cast<AutomationSequence *>(track->getSequence());
        if (autoSequence == nullptr)
        {
            continue;
        }

        Array<AutomationEvent> eventsBefore;
        Array<AutomationEvent> eventsAfter;

        for (int j = 0; j < autoSequence->size(); ++j)
        {
            const auto *event = static_cast<AutomationEvent *>(autoSequence->getUnchecked(j));
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

        processedTracks.insert(track->getTrackId());
        autoSequence->changeGroup(eventsBefore, eventsAfter, true);
    }

    return didCheckpoint;
}

void PatternOperations::mergeClips(ProjectNode &project, const Clip &targetClip,
    const Array<Clip> &sourceClips, bool shouldCheckpoint /*= true*/)
{
    if (sourceClips.isEmpty())
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    auto *targetPattern = targetClip.getPattern();
    auto *targetSequence = targetPattern->getTrack()->getSequence();

    // if the target clip has no instances, just merge all sources into it,
    // but if it has instances, duplicate and delete it first,
    // so that instances are not messed up, and merge into the newly created clip
    const auto *clipToMergeInto = &targetClip;
    const auto shouldMoveTheTargetClip = targetPattern->size() > 1;

    if (shouldMoveTheTargetClip)
    {
        if (!didCheckpoint)
        {
            didCheckpoint = true;
            project.checkpoint();
        }

        const auto *targetTrack = targetPattern->getTrack();
        const auto targetTrackName = targetTrack->getTrackName();

        // duplicate and delete the original
        UniquePointer<MidiTrackNode> trackPreset = nullptr;
        if (const auto *pianoCloneSource = dynamic_cast<PianoSequence *>(targetSequence))
        {
            trackPreset = SequencerOperations::createPianoTrack(pianoCloneSource, targetClip);
        }
        else if (const auto *autoCloneSource = dynamic_cast<AutomationSequence *>(targetSequence))
        {
            trackPreset = SequencerOperations::createAutomationTrack(autoCloneSource, targetClip);
        }

        if (trackPreset == nullptr)
        {
            jassertfalse;
            return;
        }

        const auto newTrackId = trackPreset->getTrackId();
        const auto newTrackTemplate = trackPreset->serialize();

        // don't remove the entire track here because we've already checked that there are more clip instances
        targetPattern->remove(targetClip, true);

        if (dynamic_cast<PianoSequence *>(targetSequence))
        {
            project.getUndoStack()->perform(new
                PianoTrackInsertAction(project, &project, newTrackTemplate, targetTrackName));
        }
        else if (dynamic_cast<AutomationSequence *>(targetSequence))
        {
            project.getUndoStack()->perform(new
                AutomationTrackInsertAction(project, &project, newTrackTemplate, targetTrackName));
        }

        auto *newlyAddedTrack = project.findTrackById<MidiTrackNode>(newTrackId);
        jassert(newlyAddedTrack != nullptr);

        // important:
        targetSequence = newlyAddedTrack->getSequence();
        clipToMergeInto = newlyAddedTrack->getPattern()->getClips().getFirst();
    }

    // actual merging
    if (auto *pianoTargetSequence = dynamic_cast<PianoSequence *>(targetSequence))
    {
        for (const auto &clip : sourceClips)
        {
            auto *sourceSequence = clip.getPattern()->getTrack()->getSequence();
            if (auto *pianoSourceSequence = dynamic_cast<PianoSequence *>(sourceSequence))
            {
                if (!didCheckpoint)
                {
                    didCheckpoint = true;
                    project.checkpoint();
                }

                // copy notes to target sequence with corrected beats, keys and velocities
                Array<Note> notesToInsert;
                for (auto *event : *sourceSequence)
                {
                    auto *note = static_cast<Note *>(event);
                    const auto deltaKey = clip.getKey() - clipToMergeInto->getKey();
                    const auto deltaBeat = clip.getBeat() - clipToMergeInto->getBeat();
                    const auto newVelocity = note->getVelocity() * (clip.getVelocity() / clipToMergeInto->getVelocity());
                    notesToInsert.add(note->withDeltaBeat(deltaBeat)
                        .withDeltaKey(deltaKey)
                        .withVelocity(newVelocity)
                        .withNewId(pianoTargetSequence));
                }

                pianoTargetSequence->insertGroup(notesToInsert, true);

                // delete the entire track if the clip has only one instance:
                if (clip.getPattern()->size() == 1)
                {
                    project.removeTrack(*clip.getPattern()->getTrack());
                }
                else
                {
                    clip.getPattern()->remove(clip, true);
                }
            }
            else
            {
                jassertfalse; // please don't pass the clips that can't be merged
            }
        }
    }
    else if (auto *autoTargetSequence = dynamic_cast<AutomationSequence *>(targetSequence))
    {
        for (const auto &clip : sourceClips)
        {
            auto *sourceSequence = clip.getPattern()->getTrack()->getSequence();
            if (auto *autoSourceSequence = dynamic_cast<AutomationSequence *>(sourceSequence))
            {
                if (autoTargetSequence->getTrack()->getTrackControllerNumber() !=
                    autoSourceSequence->getTrack()->getTrackControllerNumber())
                {
                    jassertfalse; // please don't pass the clips that can't be merged
                    continue;
                }

                if (!didCheckpoint)
                {
                    didCheckpoint = true;
                    project.checkpoint();
                }

                // copy events to target sequence with corrected beats
                Array<AutomationEvent> eventsToInsert;
                for (auto *event : *sourceSequence)
                {
                    auto *ae = static_cast<AutomationEvent *>(event);
                    const auto deltaBeat = clip.getBeat() - clipToMergeInto->getBeat();
                    eventsToInsert.add(ae->withDeltaBeat(deltaBeat)
                        .withNewId(autoTargetSequence));
                }

                autoTargetSequence->insertGroup(eventsToInsert, true);

                // delete the entire track if the clip has only one instance:
                if (clip.getPattern()->size() == 1)
                {
                    project.removeTrack(*clip.getPattern()->getTrack());
                }
                else
                {
                    clip.getPattern()->remove(clip, true);
                }
            }
            else
            {
                jassertfalse; // please don't pass the clips that can't be merged
            }
        }
    }
}

struct SortClipsByAbsoluteBeatPosition final
{
    static int compareElements(const Clip &first, const Clip &second)
    {
        const auto *firstTrack = first.getPattern()->getTrack();
        const auto firstBeat = first.getBeat() +
            firstTrack->getSequence()->getFirstBeat();

        const auto *secondTrack = second.getPattern()->getTrack();
        const auto secondBeat = second.getBeat() +
            secondTrack->getSequence()->getFirstBeat();

        const float diff = firstBeat - secondBeat;
        return (diff > 0.f) - (diff < 0.f);
    }
};

void PatternOperations::retrograde(ProjectNode &project, const Lasso &selection, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() < 2)
    {
        return;
    }

    if (shouldCheckpoint)
    {
        project.checkpoint();
    }

    const auto grouping = project.getTrackGroupingMode();

    // group selection by rows and sort each group
    FlatHashMap<String, Array<Clip>> groupedRows;
    static SortClipsByAbsoluteBeatPosition kSort;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto *cc = selection.getItemAs<ClipComponent>(i);
        const auto groupKey = cc->getClip().getPattern()->getTrack()->getTrackGroupKey(grouping);

        if (groupedRows.contains(groupKey))
        {
            groupedRows[groupKey].addSorted(kSort, cc->getClip());
        }
        else
        {
            groupedRows[groupKey] = { cc->getClip() };
        }
    }

    for (const auto &rowGroup : groupedRows)
    {
        int start = 0;
        int end = rowGroup.second.size() - 1;
        float previousLengthDelta = 0.f;
        do
        {
            const auto &c1 = rowGroup.second.getReference(start);
            const auto &c2 = rowGroup.second.getReference(end);

            // clips can belong to different tracks, because that depends on grouping
            // so the actual beat position also depends on sequence start:

            const auto *firstTrack = c1.getPattern()->getTrack();
            const auto firstBeat = c1.getBeat() + firstTrack->getSequence()->getFirstBeat();
            const auto firstSequenceLength = firstTrack->getSequence()->getLengthInBeats();

            const auto *secondTrack = c2.getPattern()->getTrack();
            const auto secondBeat = c2.getBeat() + secondTrack->getSequence()->getFirstBeat();
            const auto secondSequenceLength = secondTrack->getSequence()->getLengthInBeats();

            const auto beatDelta = secondBeat - firstBeat;
            const auto lengthDelta = secondSequenceLength - firstSequenceLength;

            c1.getPattern()->change(c1, c1.withDeltaBeat(beatDelta + previousLengthDelta + lengthDelta), true);

            if (start < end) // when start == end, it's the single remaining odd clip
            {
                c2.getPattern()->change(c2, c2.withDeltaBeat(-beatDelta + previousLengthDelta), true);
            }

            previousLengthDelta += lengthDelta;

            start++;
            end--;
        } while (start <= end);
    }
}

bool PatternOperations::applyModifiersStack(const Clip &clip, bool shouldCheckpoint /*= true*/)
{
    auto *pattern = clip.getPattern();
    auto *generatedSequence = dynamic_cast<PianoSequence *>(pattern->
        getProject()->getGeneratedSequences()->getSequenceFor(clip));

    if (generatedSequence == nullptr)
    {
        jassertfalse; // modifiers only support transforming notes at the moment
        return false;
    }

    auto *sourceSequence = dynamic_cast<PianoSequence *>(pattern->getTrack()->getSequence());
    if (sourceSequence == nullptr)
    {
        jassertfalse;
        return false;
    }

    auto *project = pattern->getProject();

    if (shouldCheckpoint)
    {
        project->checkpoint();
    }

    if (pattern->size() == 1)
    {
        {
            Array<Note> removedEvents;
            for (int i = 0; i < sourceSequence->size(); ++i)
            {
                const auto *note = static_cast<Note *>(sourceSequence->getUnchecked(i));
                removedEvents.add(*note);
            }

            sourceSequence->removeGroup(removedEvents, true);
        }

        {
            Array<Note> newEvents;
            for (int i = 0; i < generatedSequence->size(); ++i)
            {
                const auto *paramsToCopy = static_cast<Note *>(generatedSequence->getUnchecked(i));
                newEvents.add(Note(sourceSequence, *paramsToCopy));
            }

            sourceSequence->insertGroup(newEvents, true);
        }

        // finally, delete all modifiers
        pattern->change(clip, clip.withModifiers({}), true);
    }
    else
    {
        // make the clip "unique" if the pattern has more than 1 clip
        auto trackPreset = SequencerOperations::createPianoTrack(
            generatedSequence, clip.withModifiers({}));

        const auto trackId = trackPreset->getTrackId();
        const auto trackName = pattern->getTrack()->getTrackName();

        pattern->remove(clip, true);
        project->getUndoStack()->perform(new PianoTrackInsertAction(*project,
            project, trackPreset->serialize(), trackName));

        // focus on it
        auto *newlyAddedTrack = project->findTrackById<MidiTrackNode>(trackId);
        auto *tracksSingleClip = newlyAddedTrack->getPattern()->getUnchecked(0);
        project->setEditableScope(*tracksSingleClip, false);
    }

    return true;
}

bool PatternOperations::toggleMuteModifiersStack(const Clip &clip, bool shouldCheckpoint /*= true*/)
{
    if (clip.getModifiers().isEmpty())
    {
        return false;
    }

    auto *pattern = clip.getPattern();
    auto *project = pattern->getProject();

    if (shouldCheckpoint)
    {
        project->checkpoint();
    }

    const auto newEnabledFlag = !clip.hasEnabledModifiers();
    Array<SequenceModifier::Ptr> updatedModifiers;
    for (const auto &modifier : clip.getModifiers())
    {
        updatedModifiers.add(modifier->withEnabledFlag(newEnabledFlag));
    }

    return pattern->change(clip, clip.withModifiers(move(updatedModifiers)), true);
}

void PatternOperations::toggleMuteModifiersStack(const Lasso &selection, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();

        if (PatternOperations::toggleMuteModifiersStack(clip, !didCheckpoint))
        {
            didCheckpoint = true;
        }
    }
}

String PatternOperations::getSelectedInstrumentId(const Lasso &selection)
{
    String id;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        const auto &instrumentId = clip.getPattern()->getTrack()->getTrackInstrumentId();

        if (id.isNotEmpty() && id != instrumentId)
        {
            return {};
        }

        id = instrumentId;
    }

    return id;
}
