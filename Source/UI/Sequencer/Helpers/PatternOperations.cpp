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
#include "PatternOperations.h"
#include "SequencerOperations.h"
#include "ProjectNode.h"
#include "UndoStack.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "Note.h"
#include "AutomationEvent.h"
#include "Clip.h"
#include "ClipComponent.h"
#include "Pattern.h"
#include "Lasso.h"
#include "SerializationKeys.h"
#include "CommandIDs.h"

static Pattern *getPattern(SelectionProxyArray::Ptr selection)
{
    const auto &firstClip = selection->getFirstAs<ClipComponent>()->getClip();
    return firstClip.getPattern();
}

static String generateTransactionId(int commandId, const Lasso &selection)
{
    return String(commandId) + String(selection.getId());
}

void PatternOperations::deleteSelection(const Lasso &selection, ProjectNode &project, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0) { return; }

    OwnedArray<Array<Clip>> selections;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const Clip clip = selection.getItemAs<ClipComponent>(i)->getClip();
        Pattern *ownerPattern = clip.getPattern();
        Array<Clip> *arrayToAddTo = nullptr;

        for (int j = 0; j < selections.size(); ++j)
        {
            if (selections.getUnchecked(j)->size() > 0)
            {
                if (selections.getUnchecked(j)->getUnchecked(0).getPattern() == ownerPattern)
                {
                    arrayToAddTo = selections.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Clip>();
            selections.add(arrayToAddTo);
        }

        arrayToAddTo->add(clip);
    }

    bool didCheckpoint = !shouldCheckpoint;

    for (int i = 0; i < selections.size(); ++i)
    {
        Pattern *pattern = (selections.getUnchecked(i)->getUnchecked(0).getPattern());

        if (!didCheckpoint)
        {
            didCheckpoint = true;
            pattern->checkpoint();
        }

        // Delete the entire track if all its clips have been selected:
        if (pattern->size() == selections.getUnchecked(i)->size())
        {
            project.removeTrack(*pattern->getTrack());
        }
        else
        {
            pattern->removeGroup(*selections.getUnchecked(i), true);
            // At least one clip should always remain:
            //for (Clip &c : *selections.getUnchecked(i))
            //{
            //    if (pattern->size() > 1)
            //    {
            //        pattern->remove(c, true);
            //    }
            //}
        }
    }
}

void PatternOperations::transposeClips(const Lasso &selection, int deltaKey, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaKey == 0) { return; }

    auto *pattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaKey > 0 ? CommandIDs::ClipTransposeUp : CommandIDs::ClipTransposeDown;
    const auto &transactionId = generateTransactionId(operationId, selection);
    const bool repeatsLastAction = pattern->getLastUndoDescription() == transactionId;

    if (shouldCheckpoint && !repeatsLastAction)
    {
        pattern->checkpoint(transactionId);
    }

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto &clip = selection.getItemAs<ClipComponent>(i)->getClip();
        clip.getPattern()->change(clip, clip.withDeltaKey(deltaKey), true);
    }
}

void PatternOperations::tuneClips(const Lasso &selection, float deltaVelocity, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaVelocity == 0.f) { return; }

    auto *pattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaVelocity > 0 ? CommandIDs::ClipVolumeUp : CommandIDs::ClipVolumeDown;
    const auto &transactionId = generateTransactionId(operationId, selection);
    const bool repeatsLastAction = pattern->getLastUndoDescription() == transactionId;

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

void PatternOperations::shiftBeatRelative(Lasso &selection, float deltaBeat, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0 || deltaBeat == 0) { return; }

    auto *firstPattern = selection.getFirstAs<ClipComponent>()->getClip().getPattern();
    const auto operationId = deltaBeat > 0 ? CommandIDs::BeatShiftRight : CommandIDs::BeatShiftLeft;
    const auto &transactionId = generateTransactionId(operationId, selection);
    const bool repeatsLastAction = firstPattern->getLastUndoDescription() == transactionId;

    bool didCheckpoint = !shouldCheckpoint || repeatsLastAction;

    for (const auto &s : selection.getGroupedSelections())
    {
        const auto trackSelection(s.second);
        Pattern *pattern = getPattern(trackSelection);
        jassert(pattern);

        const int numSelected = trackSelection->size();
        Array<Clip> groupBefore, groupAfter;

        for (int i = 0; i < numSelected; ++i)
        {
            auto *cc = static_cast<ClipComponent *>(trackSelection->getUnchecked(i));
            groupBefore.add(cc->getClip());
            groupAfter.add(cc->getClip().withDeltaBeat(deltaBeat));
        }

        if (groupBefore.size() > 0)
        {
            if (!didCheckpoint)
            {
                pattern->checkpoint(transactionId);
                didCheckpoint = true;
            }
        }

        pattern->changeGroup(groupBefore, groupAfter, true);
    }
}

static String generateNextNameForNewTrack(const String &name, const StringArray &allNames)
{
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

void PatternOperations::cutClip(ProjectNode &project, const Clip &clip, float relativeCutBeat, bool shouldCheckpoint)
{
    MidiTrack *track = clip.getPattern()->getTrack();
    const auto allTrackNames(project.getAllTrackNames());
    const String newName = generateNextNameForNewTrack(track->getTrackName(), allTrackNames);
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

        SequencerOperations::cutEvents(intersectedEvents, intersectionPoints, false);

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
        const ValueTree trackTemplate = newTrack->serialize();

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
        const ValueTree trackTemplate = newTrack->serialize();

        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        sequence->removeGroup(eventsToBeMoved, true);
        project.getUndoStack()->perform(new AutomationTrackInsertAction(project, &project, trackTemplate, newName));
    }
}
