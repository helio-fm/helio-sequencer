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
#include "TimeSignaturesAggregator.h"

#include "Pattern.h"

#include "UndoStack.h"
#include "AutomationTrackActions.h"

#include "ColourIDs.h"

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

PianoSequence *SequencerOperations::getPianoSequence(const NoteListBase &notes)
{
    // assumes all selection only contains notes of a single sequence
    jassert(notes.size() > 0);
    return static_cast<PianoSequence *>(notes.getNoteUnchecked(0).getSequence());
}

PianoSequence *SequencerOperations::getPianoSequence(const Clip &targetClip)
{
    return static_cast<PianoSequence *>(targetClip.getPattern()->getTrack()->getSequence());
}

float SequencerOperations::findStartBeat(const NoteListBase &notes)
{
    if (notes.size() == 0)
    {
        return 0.f;
    }
    
    float startBeat = FLT_MAX;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        startBeat = jmin(startBeat, note.getBeat());
    }
    
    return startBeat;
}

float SequencerOperations::findEndBeat(const NoteListBase &notes)
{
    if (notes.size() == 0)
    {
        return 0.f;
    }
    
    float endBeat = -FLT_MAX;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        endBeat = jmax(endBeat, note.getBeat() + note.getLength());
    }
    
    return endBeat;
}

float SequencerOperations::findStartBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    {
        return 0.f;
    }
    
    float startBeat = FLT_MAX;
    for (const auto &note : selection)
    {
        startBeat = jmin(startBeat, note.getBeat());
    }
    
    return startBeat;
}

float SequencerOperations::findStartBeat(const WeakReference<Lasso> selection)
{
    if (selection == nullptr)
    {
        return 0.f;
    }

    return SequencerOperations::findStartBeat(*selection);
}

float SequencerOperations::findEndBeat(const Array<Note> &selection)
{
    if (selection.size() == 0)
    {
        return 0.f;
    }
    
    float endBeat = -FLT_MAX;
    for (const auto &note : selection)
    {
        endBeat = jmax(endBeat, note.getBeat() + note.getLength());
    }
    
    return endBeat;
}

float SequencerOperations::findEndBeat(const WeakReference<Lasso> selection)
{
    if (selection == nullptr)
    {
        return 0.f;
    }

    return SequencerOperations::findEndBeat(*selection);
}

void SequencerOperations::previewSelection(const Lasso &selection, Transport &transport, int maxSize /*= 7*/)
{
    if (selection.size() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    if (selection.getNumSelected() <= maxSize)
    {
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            auto *nc = selection.getItemAs<NoteComponent>(i);
            transport.previewKey(sequence->getTrack()->getTrackId(),
                sequence->getTrack()->getTrackChannel(),
                nc->getNote().getKey() + nc->getClip().getKey(),
                nc->getVelocity() * nc->getClip().getVelocity(),
                nc->getLength());
        }
    }
}

void SequencerOperations::cleanupOverlaps(const NoteListBase &notes, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() < 2)
    {
        return;
    }

    bool didCheckpoint = !shouldCheckpoint;

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    // convert this
    //    ----
    // ------------
    // into this
    //    ---------
    // ------------
    
    bool step1HasChanges = false;
    
    do
    {
        Array<Note> group1Before, group1After;

        for (int i = 0; i < notes.size(); ++i)
        {
            const auto &note = notes.getNoteUnchecked(i);
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaLength = -FLT_MAX;
            const Note *overlappingNote = nullptr;
            
            for (int j = 0; j < notes.size(); ++j)
            {
                const auto &otherNote = notes.getNoteUnchecked(j);
                
                if (note.getKey() == otherNote.getKey() &&
                    note.getBeat() > otherNote.getBeat() &&
                    (note.getBeat() + note.getLength()) < (otherNote.getBeat() + otherNote.getLength()))
                {
                    const float currentDelta =
                        (otherNote.getBeat() + otherNote.getLength()) - (note.getBeat() + note.getLength());
                    
                    if (deltaLength < currentDelta)
                    {
                        deltaLength = currentDelta;
                        overlappingNote = &otherNote;
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                group1Before.add(note);
                group1After.add(note.withDeltaLength(deltaLength));
            }
        }
        
        step1HasChanges = !group1Before.isEmpty();

        if (step1HasChanges)
        {
            if (!didCheckpoint)
            {
                pianoSequence->checkpoint();
                didCheckpoint = true;
            }

            pianoSequence->changeGroup(group1Before, group1After, undoable);
        }
    }
    while (step1HasChanges);
    

    // convert this
    //    -------------
    // ------------
    // into this
    //    -------------
    // ----------------
    
    bool step2HasChanges = false;

    do
    {
        Array<Note> group2Before, group2After;
        
        for (int i = 0; i < notes.size(); ++i)
        {
            const auto &note = notes.getNoteUnchecked(i);
            
            // для каждой ноты найти ноту, которая полностью перекрывает ее на максимальную длину
            
            float deltaLength = -FLT_MAX;
            const Note *overlappingNote = nullptr;
            
            for (int j = 0; j < notes.size(); ++j)
            {
                const auto &otherNote = notes.getNoteUnchecked(j);
                
                if (note.getKey() == otherNote.getKey() &&
                    note.getBeat() > otherNote.getBeat() &&
                    note.getBeat() < (otherNote.getBeat() + otherNote.getLength()) &&
                    (note.getBeat() + note.getLength()) > (otherNote.getBeat() + otherNote.getLength()))
                {
                    const float currentDelta =
                        (note.getBeat() + note.getLength()) - (otherNote.getBeat() + otherNote.getLength());
                    
                    if (deltaLength < currentDelta)
                    {
                        deltaLength = currentDelta;
                        overlappingNote = &otherNote;
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                group2Before.add(*overlappingNote);
                group2After.add(overlappingNote->withDeltaLength(deltaLength));
            }
        }

        step2HasChanges = !group2Before.isEmpty();

        if (step2HasChanges)
        {
            if (!didCheckpoint)
            {
                pianoSequence->checkpoint();
                didCheckpoint = true;
            }

            pianoSequence->changeGroup(group2Before, group2After, undoable);
        }
    }
    while (step2HasChanges);
    
    
    // convert this
    // ------------       ---------
    //    ---------    ------------
    // into this
    // ---                ---------
    //    ---------    ---         
    
    bool step3HasChanges = false;

    do
    {
        Array<Note> group3Before, group3After;
        
        for (int i = 0; i < notes.size(); ++i)
        {
            const auto &note = notes.getNoteUnchecked(i);
            
            // для каждой ноты найти ноту, которая перекрывает ее максимально
            
            float overlappingBeats = -FLT_MAX;
            const Note *overlappingNote = nullptr;
            
            for (int j = 0; j < notes.size(); ++j)
            {
                const auto &otherNote = notes.getNoteUnchecked(j);
                
                if (note.getKey() == otherNote.getKey() &&
                    note.getBeat() < otherNote.getBeat() &&
                    (note.getBeat() + note.getLength()) >= (otherNote.getBeat() + otherNote.getLength()))
                {
                    // >0 : has overlap
                    const float overlapsWith = (note.getBeat() + note.getLength()) - otherNote.getBeat();
                    
                    if (overlapsWith > overlappingBeats)
                    {
                        overlappingBeats = overlapsWith;
                        overlappingNote = &otherNote;
                    }
                }
            }
            
            if (overlappingNote != nullptr)
            {
                group3Before.add(note);
                group3After.add(note.withDeltaLength(-overlappingBeats));
            }
        }

        step3HasChanges = !group3Before.isEmpty();

        if (step3HasChanges)
        {
            if (!didCheckpoint)
            {
                pianoSequence->checkpoint();
                didCheckpoint = true;
            }

            pianoSequence->changeGroup(group3Before, group3After, undoable);
        }
    }
    while (step3HasChanges);
    
    // remove duplicates
    
    FlatHashMap<MidiEvent::Id, Note> deferredRemoval;
    FlatHashMap<MidiEvent::Id, Note> unremovableNotes;
    
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        
        for (int j = 0; j < notes.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            
            const auto &otherNote = notes.getNoteUnchecked(j);
            
            // full overlap (shouldn't happen at this point)
            //const bool isOverlappingNote = (note.getKey() == otherNote.getKey() &&
            //    note.getBeat() >= otherNote.getBeat() &&
            //    (note.getBeat() + note.getLength()) <= (otherNote.getBeat() + otherNote.getLength()));

            // partial overlap
            const bool isOverlappingNote =
                (note.getKey() == otherNote.getKey() &&
                    note.getBeat() >= otherNote.getBeat() &&
                    note.getBeat() < (otherNote.getBeat() + otherNote.getLength()));
            
            const bool startsFromTheSameBeat =
                (note.getKey() == otherNote.getKey() &&
                    note.getBeat() == otherNote.getBeat());
            
            const bool isOriginalNote = unremovableNotes.contains(otherNote.getId());
            
            if (!isOriginalNote &&
                (isOverlappingNote || startsFromTheSameBeat))
            {
                unremovableNotes[note.getId()] = note;
                deferredRemoval[otherNote.getId()] = otherNote;
            }
        }
    }
    
    Array<Note> removalGroup;
    for (const auto &deferredRemovalIterator : deferredRemoval)
    {
        removalGroup.add(deferredRemovalIterator.second);
    }

    if (!removalGroup.isEmpty())
    {
        if (!didCheckpoint)
        {
            pianoSequence->checkpoint();
            didCheckpoint = true;
        }

        pianoSequence->removeGroup(removalGroup, undoable);
    }
}

void SequencerOperations::makeStaccato(const NoteListBase &notes,
    float newLength, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0 || newLength == 0.f)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        groupBefore.add(note);
        groupAfter.add(note.withLength(newLength));
    }

    if (groupBefore.size() > 0 && shouldCheckpoint)
    {
        pianoSequence->checkpoint();
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
}

// extends each note to the note that follows (target note is bottommost note that is to the right)
bool SequencerOperations::makeLegato(const NoteListBase &notes,
    float overlap, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);

    if (notes.size() == 0)
    {
        jassertfalse;
        return false;
    }

    auto *sequence = getPianoSequence(notes);
    jassert(sequence);

    // the sequence isn't neccecarily serial (organized from left to right)
    // therefore, we will need to continuously re-target the bottommost note that is to the right of the current subject
    // additionally, we will continuously need to pick the next subject that is the bottommost leftmost note that is above the subject

    Array<Note> groupBefore, groupAfter;

    const int numNotes = notes.size();
    auto *subject = &notes.getNoteUnchecked(0); // subject = the note we are currently extending
    auto *candidate = &notes.getNoteUnchecked(numNotes - 1); // candidate = the current note being considered to replace the current best candidate
    auto *target = &notes.getNoteUnchecked(numNotes - 1); // the note we are extending our subject to
    auto *bestCandidate = &notes.getNoteUnchecked(numNotes - 1); // the current best candidate (note that fits our selection criteria)

    // initializing various things
    float subjectBeat = -1 * FLT_MAX;
    int subjectKey = -1 * INT_MAX;
    float candidateBeat = FLT_MAX;
    int candidateKey = INT_MAX;
    float bestCandidateBeat = FLT_MAX;
    int bestCandidateKey = INT_MAX;
    float targetBeat = FLT_MAX;
    int targetKey = INT_MAX;

    for (int i = 0; i < notes.size(); ++i) // iterate through the selection to get the first subject (leftmost, bottommost note)
    {
        // re-initialize candidate variables
        candidate = &notes.getNoteUnchecked(i);
        candidateBeat = candidate->getBeat();
        candidateKey = candidate->getKey();

        // if the candidate is to the left of the last best note, OR if it is equal but lower in key
        if ((candidateBeat < bestCandidateBeat) || (candidateBeat == bestCandidateBeat && candidateKey < bestCandidateKey))
        {
            bestCandidate = candidate;
            bestCandidateBeat = candidate->getBeat();
            bestCandidateKey = candidate->getKey();
        }
    }

    // subject is now the leftmost bottommost note in the selection
    // we're setting all this afterward just to make the code more readable later
    subject = bestCandidate;
    subjectBeat = bestCandidate->getBeat();
    subjectKey = bestCandidate->getKey();

    for (int i = 0; i < notes.size(); ++i) // since we have set our first subject (before beginning our proper loop), now we commence through the "real" legato loop.
    {
        // re-init best candidate variables
        bestCandidateBeat = FLT_MAX;
        bestCandidateKey = INT_MAX;

        for (int j = 0; j < notes.size(); ++j) // iterate through the selection to get the target note(leftmost, bottommost note that is to the right of the subject)
        {
            // re-initialize candidate variables
            candidate = &notes.getNoteUnchecked(j);
            candidateBeat = candidate->getBeat();
            candidateKey = candidate->getKey();

            // if the candidate is to the left of the last best note, OR if it is equal but lower in key, AND it is to the right of the subject
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

        // the target is now the leftmost note that is 1) to the right of the subject AND 2) among multilbe targets of the same beat, the lowest one
        target = bestCandidate;
        targetBeat = bestCandidate->getBeat();
        targetKey = bestCandidate->getKey();

        float newLength = subject->getLength(); // initialize as subject length (just in case)

        if (targetBeat != subjectBeat && targetKey != subjectKey) // only change length if target is not of the same beat (dirty hack)
        {
            newLength = targetBeat - subjectBeat + overlap; // new length is the new length + the overlap specified by overlap
        }
        if (targetBeat != subjectBeat && targetKey == subjectKey)
        {
            newLength = targetBeat - subjectBeat;
        }

        groupBefore.add(*subject);
        groupAfter.add(subject->withLength(newLength));

        // re-init best candidate variables
        bestCandidateBeat = FLT_MAX;
        bestCandidateKey = INT_MAX;

        for (int j = 0; j < notes.size(); ++j) // set the next subject in the same way as before, but is must be at least above our last subject, or to the right.
        {
            candidate = &notes.getNoteUnchecked(j); // re-initialize candidate variables
            candidateBeat = candidate->getBeat();
            candidateKey = candidate->getKey();

            if (candidateBeat < bestCandidateBeat && candidateBeat > subjectBeat) // if its to the left of the prior while being greater than the subject
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
            if (candidateBeat == bestCandidateBeat && candidateKey < bestCandidateKey && candidateKey > subjectKey) // if it's the same as the prior
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
            if (candidateBeat == subjectBeat && candidateKey < bestCandidateKey && candidateKey > subjectKey) // if it's the same beat as the prior
            {
                bestCandidate = candidate;
                bestCandidateBeat = candidate->getBeat();
                bestCandidateKey = candidate->getKey();
            }
        }

        subject = bestCandidate; // our subject is now the bottommost leftmost note that is to the right of the subject or at least above it in key
        subjectBeat = bestCandidate->getBeat();
        subjectKey = bestCandidate->getKey();
    }

    if (groupBefore.isEmpty()) // ditch if either of the groups are empty
    {
        return false;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    if (!groupBefore.isEmpty())
    {
        sequence->changeGroup(groupBefore, groupAfter, undoable);
    }

    return true;
}

void SequencerOperations::retrograde(const NoteListBase &notes,
    bool undoable /*= true*/, bool shouldCheckpoint /*= true*/)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() < 2)
    {
        return;
    }

    // sort the selection
    Array<Note> sortedSelection;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        sortedSelection.addSorted(note, note);
    }

    Array<Note> groupBefore;
    Array<Note> groupAfter;

    // pick a note at the start and at the end swapping their keys
    // (assumes the selection is a melodic line, will work weird for chord sequences)
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

    auto *sequence = getPianoSequence(notes);
    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, undoable);
}

void SequencerOperations::melodicInversion(const NoteListBase &notes,
    bool undoable /*= true*/, bool shouldCheckpoint /*= true*/)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() < 2)
    {
        return;
    }

    // sort the selection
    Array<Note> sortedSelection;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        sortedSelection.addSorted(note, note);
    }

    Array<Note> groupBefore, groupAfter;

    // invert key intervals between each note and the previous one
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

    auto *sequence = getPianoSequence(notes);
    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, undoable);
}

bool SequencerOperations::isBarStart(float absBeat,
    WeakReference<TimeSignaturesAggregator> timeContext)
{
    const TimeSignatureEvent *currentTimeSignature = nullptr;
    for (int i = 0; i < timeContext->getSequence()->size(); ++i)
    {
        const auto *ts = static_cast<const TimeSignatureEvent *>
            (timeContext->getSequence()->getUnchecked(i));

        if (currentTimeSignature == nullptr || ts->getBeat() <= absBeat)
        {
            currentTimeSignature = ts;
        }
        else if (ts->getBeat() > absBeat)
        {
            break;
        }
    }

    const float barLengthInBeats = currentTimeSignature != nullptr ?
        currentTimeSignature->getBarLengthInBeats() :
        timeContext->getDefaultMeterBarLength();

    const float timeSignatureStartBeat = currentTimeSignature != nullptr ?
        currentTimeSignature->getBeat() :
        timeContext->getDefaultMeterStartBeat();

    return fmodf(absBeat - timeSignatureStartBeat, barLengthInBeats) == 0.f;
}

Arpeggiator::Ptr SequencerOperations::makeArpeggiator(const String &name,
    const Lasso &selection,
    const Temperament::Ptr temperament,
    const Scale::Ptr scale, Note::Key scaleRootKeyModuloPeriod,
    WeakReference<TimeSignaturesAggregator> timeContext)
{
    if (selection.getNumSelected() == 0)
    {
        jassertfalse;
        return {};
    }

    Array<Note> selectedNotes;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        const auto nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        selectedNotes.add(nc->getNote());
    }

    const auto &clip = selection.getFirstAs<NoteComponent>()->getClip();

    auto sequenceMeanKey = 0;
    auto sequenceStartBeat = FLT_MAX;
    for (const auto &note : selectedNotes)
    {
        sequenceStartBeat = jmin(sequenceStartBeat, note.getBeat());
        sequenceMeanKey += (note.getKey() + clip.getKey());
    }
    sequenceMeanKey /= selectedNotes.size();

    const auto arpRootKey = scaleRootKeyModuloPeriod +
        (sequenceMeanKey / temperament->getPeriodSize()) * temperament->getPeriodSize();

    Array<Arpeggiator::Key> keys;
    static Arpeggiator::Key sorter;

    for (const auto &note : selectedNotes)
    {
        const auto relativeChromaticKey = note.getKey() + clip.getKey() - arpRootKey;
        const auto scaleKeyModuloPeriod = scale->getScaleKey(relativeChromaticKey);

        const auto absoluteBeat = note.getBeat() + clip.getBeat();
        const bool isBarStart = SequencerOperations::isBarStart(absoluteBeat, timeContext);

        // Ignore all non-scale keys
        // (-1 means the chromatic key is not in a target scale)
        if (scaleKeyModuloPeriod >= 0)
        {
            const auto relativeBeat = note.getBeat() - sequenceStartBeat;
            const auto period = int(floorf(float(relativeChromaticKey) / float(scale->getBasePeriod())));
            keys.addSorted(sorter,
                Arpeggiator::Key(scaleKeyModuloPeriod, period, relativeBeat,
                    note.getLength(), note.getVelocity(), isBarStart));
        }
    }

    return Arpeggiator::Ptr(new Arpeggiator(name, move(keys)));
}

bool SequencerOperations::arpeggiate(const NoteListBase &notes,
    const Clip &clip,
    const Arpeggiator::Ptr arp,
    const Temperament::Ptr temperament,
    WeakReference<KeySignaturesSequence> harmonicContext,
    WeakReference<TimeSignaturesAggregator> timeContext,
    float durationMultiplier, float randomness,
    bool isReversed, bool isLimitedToChord,
    bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        jassertfalse;
        return false;
    }

    if (!arp->isValid())
    {
        jassertfalse;
        return false;
    }

    Array<Note> sortedRemovals;
    Array<Note> insertions;

    // sort the selection
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        sortedRemovals.addSorted(note, note);
    }

    // split into chords
    Array<Array<Note>> chords;

    Array<Note> currentChord;
    for (int i = 0; i < sortedRemovals.size(); ++i)
    {
        const auto thisNote = sortedRemovals.getUnchecked(i);
        const bool isLastNote = i == (sortedRemovals.size() - 1);

        const float nextBeat = isLastNote ? thisNote.getBeat() :
            sortedRemovals.getUnchecked(i + 1).getBeat();

        // input chords for the arpeggiator will have absolute keys (clip key offset
        // included, so it's simpler for the arpeggiator to align to nearest in-scale keys),
        // but relative beats (no clip beat offset, because arpeggiator's keys start from 0);
        // later we'll subtract the clip key offset before inserting new notes into sequence
        currentChord.add(sortedRemovals.getUnchecked(i).withDeltaKey(clip.getKey()));

        const bool newChordWillStart = currentChord.size() >= 3 &&
            nextBeat >= (thisNote.getBeat() + thisNote.getLength());

        if (newChordWillStart || isLastNote)
        {
            chords.add(currentChord);
            currentChord.clear();
        }
    }

    if (chords.size() == 0)
    {
        jassertfalse;
        return false;
    }

    // try to clean up chords by removing octave intervals (at most one, if any)
    for (auto &chord : chords)
    {
        for (int i = 0; i < chord.size() - 1; ++i)
        {
            if (chord.getUnchecked(i).getKey() ==
                chord.getUnchecked(i + 1).getKey() - temperament->getPeriodSize())
            {
                chord.remove(i);
                break;
            }
        }
    }

    // arpeggiate every chord
    int arpKeyIndex = 0;
    float arpBeatOffset = 0.f;
    const float selectionStartBeat =
        SequencerOperations::findStartBeat(sortedRemovals) + clip.getBeat();

    const auto getAbsoluteBeatOffset =
        [&selectionStartBeat, &arpBeatOffset, &durationMultiplier]()
    {
        return selectionStartBeat + (arpBeatOffset * durationMultiplier);
    };

    Scale::Ptr scale = Scale::makeNaturalMajorScale();
    Note::Key scaleRootKey = 0;
    String scaleRootKeyName;

    for (int i = 0; i < chords.size(); ++i)
    {
        const auto &chord = chords.getUnchecked(i);
        const float chordStart = SequencerOperations::findStartBeat(chord) + clip.getBeat();
        const float chordEnd = SequencerOperations::findEndBeat(chord) + clip.getBeat();

        // finding a key signature at the chord's start beat
        // (note: not using findHarmonicContext(chordStart, chordEnd..
        // to get one key signature even if the chord crosses several signatures)
        SequencerOperations::findHarmonicContext(chordStart, chordStart,
            harmonicContext, scale, scaleRootKey, scaleRootKeyName);

        while (1)
        {
            const float nextNoteBeat = getAbsoluteBeatOffset() +
                (arp->getBeatFor(arpKeyIndex) * durationMultiplier);

            if (SequencerOperations::isBarStart(nextNoteBeat, timeContext))
            {
                // try to be smarter about time signatures
                arp->skipToBarStart(arpKeyIndex, arpBeatOffset);
            }

            if (nextNoteBeat >= chordEnd)
            {
                if (isLimitedToChord)
                {
                    // arpeggiate every chord from the start of the arp's sequence
                    arpKeyIndex = 0;
                    arpBeatOffset = (chordEnd - selectionStartBeat) / durationMultiplier;
                }

                break;
            }

            // see the comment above about clip's key/beat offsets
            const float sequenceBeatOffset = getAbsoluteBeatOffset() - clip.getBeat();
            insertions.add(arp->mapArpKeyIntoChordSpace(temperament,
                arpKeyIndex, sequenceBeatOffset,
                chord, scale, scaleRootKey,
                isReversed, durationMultiplier, randomness)
                    .withDeltaKey(-clip.getKey()));

            arp->proceedToNextKey(arpKeyIndex, arpBeatOffset);
        }
    }

    // remove the selection and add result
    auto *sequence = getPianoSequence(notes);
    jassert(sequence != nullptr);

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->removeGroup(sortedRemovals, undoable);
    sequence->insertGroup(insertions, undoable);

    return true;
}

void SequencerOperations::randomizeVolume(const Lasso &selection, float factor, bool shouldCheckpoint)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);
    
    Random random(Time::currentTimeMillis());

    Array<Note> groupBefore, groupAfter;
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        if (auto *nc = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            const float r = ((random.nextFloat() * 2.f) - 1.f) * factor; // (-1 .. 1) * factor
            const float v = nc->getNote().getVelocity();
            const float deltaV = (r < 0) ? (v * r) : ((1.f - v) * r);
            const float newVelocity = nc->getNote().getVelocity() + deltaV;
            
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withVelocity(newVelocity));
        }
    }

    if (!groupBefore.isEmpty())
    {
        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        sequence->changeGroup(groupBefore, groupAfter, true);
    }
}

void SequencerOperations::fadeOutVolume(const Lasso &selection, float factor, bool shouldCheckpoint)
{
    // Smooth fade out like
    // 1 - ((x / sqrt(x)) * factor)
    
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    float minBeat = FLT_MAX;
    float maxBeat = -FLT_MAX;
    Array<Note> groupBefore, groupAfter;
    
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
        if (auto *nc = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            const float localBeat = nc->getBeat() - minBeat;
            const float localX = (localBeat / selectionBeatLength) + 0.0001f; // not 0
            const float velocityMultiplier = 1.f - ((localX / sqrtf(localX)) * factor);
            const float newVelocity = nc->getNote().getVelocity() * velocityMultiplier;
            
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->getNote().withVelocity(newVelocity));
        }
    }
    
    if (!groupBefore.isEmpty())
    {
        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        sequence->changeGroup(groupBefore, groupAfter, true);
    }
}

void SequencerOperations::tuneVolume(const Lasso &selection, float delta, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0 || delta == 0.f)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);
    jassert(sequence);

    const auto operationId = (delta > 0.f) ? UndoActionIDs::NotesVolumeUp : UndoActionIDs::NotesVolumeDown;
    const auto transactionId = selection.generateTransactionId(operationId);
    const bool repeatsLastOperation = sequence->getLastUndoActionId() == transactionId;

    if (shouldCheckpoint && !repeatsLastOperation)
    {
        sequence->checkpoint(transactionId);
    }

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->getNote().withDeltaVelocity(delta));
    }

    sequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::startTuning(const Lasso &selection)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->startTuning();
    }
}

void SequencerOperations::changeVolumeLinear(const Lasso &selection, float volumeDelta)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningLinear(volumeDelta));
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::changeVolumeMultiplied(const Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }

    const auto factor = jlimit(-1.f, 1.f, volumeFactor);
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    Array<Note> groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningMultiplied(factor));
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::changeVolumeSine(const Lasso &selection, float volumeFactor)
{
    if (selection.getNumSelected() == 0)
    {
        return;
    }
    
    const auto factor = jlimit(-1.f, 1.f, volumeFactor);

    const float numSines = 2;
    float midline = 0.f;
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        midline += nc->anchor.getVelocity();
    }
    midline = midline / float(selection.getNumSelected());
    
    const float startBeat = SequencerOperations::findStartBeat(selection);
    const float endBeat = SequencerOperations::findEndBeat(selection);
    
    auto *pianoSequence = getPianoSequence(selection);
    jassert(pianoSequence);

    Array<Note> groupBefore, groupAfter;
        
    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        const float phase = ((nc->getBeat() - startBeat) / (endBeat - startBeat)) * MathConstants<float>::pi * 2.f * numSines;
        groupBefore.add(nc->getNote());
        groupAfter.add(nc->continueTuningSine(factor, midline, phase));
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, true);
}

void SequencerOperations::endTuning(const Lasso &selection)
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
    if (selectedTrack == nullptr || clipboard.getData().isEmpty())
    {
        return;
    }

    const auto root = clipboard.getData().hasType(Serialization::Clipboard::clipboard) ?
        clipboard.getData() : clipboard.getData().getChildWithName(Serialization::Clipboard::clipboard);

    if (!root.isValid())
    {
        return;
    }

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
                pastedEvents.add(AutomationEvent(automationSequence)
                    .withParameters(autoElement)
                    .withDeltaBeat(deltaBeat)
                    .withNewId());
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
                pastedNotes.add(Note(pianoSequence)
                    .withParameters(noteElement)
                    .withDeltaBeat(deltaBeat)
                    .withNewId());
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
                    pastedClips.add(Clip(targetPattern)
                        .withParameters(clipElement)
                        .withDeltaBeat(deltaBeat)
                        .withNewId());
                }
        
                if (pastedClips.size() > 0)
                {
                    if (!didCheckpoint)
                    {
                        targetPattern->checkpoint();
                        didCheckpoint = true;
                    }
        
                    for (auto &c : pastedClips)
                    {
                        targetPattern->insert(c, true);
                    }
                }
            }
        }
    }
}

void SequencerOperations::shiftKeyRelative(const NoteListBase &notes,
    int deltaKey, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0 || deltaKey == 0)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    const auto operationId = deltaKey > 0 ? UndoActionIDs::KeyShiftUp : UndoActionIDs::KeyShiftDown;
    const auto transactionId = notes.generateTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    Array<Note> groupBefore, groupAfter;
        
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        groupBefore.add(note);
        groupAfter.add(note.withDeltaKey(deltaKey));
    }
        
    if (!groupBefore.isEmpty())
    {
        if (shouldCheckpoint && !repeatsLastAction)
        {
            pianoSequence->checkpoint(transactionId);
        }

        pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
    }
}

void SequencerOperations::shiftInScaleKeyRelative(const NoteListBase &notes, const Clip &clip,
    WeakReference<KeySignaturesSequence> keySignatures, Scale::Ptr defaultScale,
    int deltaKey, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        return;
    }

    Note::Key rootKey = 0;
    String rootKeyName;
    Scale::Ptr scale = defaultScale;

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        const auto absKey = note.getKey() + clip.getKey();
        const auto absBeat = note.getBeat() + clip.getBeat();

        if (keySignatures != nullptr)
        {
            findHarmonicContext(absBeat, absBeat, keySignatures, scale, rootKey, rootKeyName);
        }

        // scale key calculations are always painful and hardly readable,
        // (this code kinda duplicates the code in ChordPreviewTool::buildChord),
        // please refactor this someday:
        const auto period = (absKey - rootKey) / scale->getBasePeriod();
        const auto periodOffset = period * scale->getBasePeriod();
        const auto chromaticOffset = (absKey - rootKey) % scale->getBasePeriod();

        const auto inScaleKey = scale->getNearestScaleKey(chromaticOffset,
            deltaKey == 0 ? Scale::ScaleKeyAlignment::Round :
                (deltaKey < 0 ? Scale::ScaleKeyAlignment::Floor : Scale::ScaleKeyAlignment::Ceil));

        const auto absAlignedInScale = scale->getChromaticKey(inScaleKey, periodOffset + rootKey, false);
        const auto absNewKey = scale->getChromaticKey(inScaleKey + deltaKey, periodOffset + rootKey, false);

        // если нота стоит в ладу, то поднимаем или опускаем на ступень лада,
        // если не в ладу, то округляем до ближайшей ступени лада:
        const auto d = (absAlignedInScale == absKey) ?
            (absNewKey - absKey) : (absAlignedInScale - absKey);

        groupBefore.add(note);
        groupAfter.add(note.withDeltaKey(d));
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    if (!groupBefore.isEmpty())
    {
        if (shouldCheckpoint)
        {
            const auto operationId = deltaKey == 0 ? UndoActionIDs::SnapToScale :
                (deltaKey > 0 ? UndoActionIDs::ScaleKeyShiftUp : UndoActionIDs::ScaleKeyShiftDown);
            const auto transactionId = notes.generateTransactionId(operationId);
            const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;
            if (!repeatsLastAction)
            {
                pianoSequence->checkpoint(transactionId);
            }
        }

        pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
    }
}

void SequencerOperations::shiftBeatRelative(const NoteListBase &notes,
    float deltaBeat, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0 || deltaBeat == 0.f)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    const auto operationId = deltaBeat > 0 ? UndoActionIDs::BeatShiftRight : UndoActionIDs::BeatShiftLeft;
    const auto transactionId = notes.generateTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    Array<Note> groupBefore, groupAfter;
        
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        groupBefore.add(note);
        groupAfter.add(note.withDeltaBeat(deltaBeat));
    }
        
    if (groupBefore.size() > 0 &&
        shouldCheckpoint && !repeatsLastAction)
    {
        pianoSequence->checkpoint(transactionId);
    }
        
    pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
}

void SequencerOperations::shiftLengthRelative(const NoteListBase &notes,
    float deltaLength, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0 || deltaLength == 0.f)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);

    const auto operationId = deltaLength > 0 ? UndoActionIDs::LengthIncrease : UndoActionIDs::LengthDecrease;
    const auto transactionId = notes.generateTransactionId(operationId);
    const bool repeatsLastAction = pianoSequence->getLastUndoActionId() == transactionId;

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        groupBefore.add(note);
        groupAfter.add(note.withDeltaLength(deltaLength));
    }

    if (groupBefore.size() > 0 &&
        shouldCheckpoint && !repeatsLastAction)
    {
        pianoSequence->checkpoint(transactionId);
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
}

void SequencerOperations::invertChord(const NoteListBase &notes,
    int deltaKey, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        return;
    }

    auto *pianoSequence = getPianoSequence(notes);
    jassert(pianoSequence);
        
    // sort selection
    Array<Note> selectedNotes;
        
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        selectedNotes.addSorted(note, note);
    }
        
    // detect target keys (upper or lower)
    Array<Note> targetNotes;
        
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

    if (targetNotes.isEmpty())
    {
        return;
    }

    // octave shift
    Array<Note> groupBefore, groupAfter;

    for (auto && targetNote : targetNotes)
    {
        groupBefore.add(targetNote);
        groupAfter.add(targetNote.withDeltaKey(deltaKey));
    }

    if (shouldCheckpoint)
    {
        pianoSequence->checkpoint();
    }

    pianoSequence->changeGroup(groupBefore, groupAfter, undoable);
}

void SequencerOperations::applyTuplets(const NoteListBase &notes,
    Note::Tuplet tuplet, bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(notes);
    jassert(sequence);

    Array<Note> groupBefore, groupAfter;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        if (note.getTuplet() != tuplet)
        {
            groupBefore.add(note);
            groupAfter.add(note.withTuplet(tuplet));
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

    sequence->changeGroup(groupBefore, groupAfter, undoable);
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

bool SequencerOperations::quantize(const NoteListBase &notes, float bar,
    bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        return false;
    }

    auto *sequence = getPianoSequence(notes);
    jassert(sequence);

    Array<Note> removals;
    Array<Note> groupBefore, groupAfter;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);
        float startBeat = note.getBeat();
        float length = note.getLength();

        doQuantize(startBeat, length, bar);

        if (startBeat == note.getBeat() && length == note.getLength())
        {
            continue;
        }

        bool duplicateFound = false;
        for (const auto &other : groupAfter)
        {
            if (note.getKey() == other.getKey() &&
                startBeat == other.getBeat() &&
                length == other.getLength())
            {
                duplicateFound = true;
                break;
            }
        }

        if (duplicateFound)
        {
            removals.add(note);
            continue;
        }

        groupBefore.add(note);
        groupAfter.add(note.withBeat(startBeat).withLength(length));
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
        sequence->changeGroup(groupBefore, groupAfter, undoable);
    }

    if (!removals.isEmpty())
    {
        sequence->removeGroup(removals, undoable);
    }

    return true;
}

static inline void doRescaleLogic(Array<Note> &groupBefore, Array<Note> &groupAfter,
    const Note &note, Note::Key keyOffset, Scale::Ptr scaleA, Scale::Ptr scaleB)
{
    const auto noteKey = note.getKey() - keyOffset;
    const auto periodNumber = noteKey / scaleA->getBasePeriod();
    const auto inScaleKey = scaleA->getScaleKey(noteKey);
    if (inScaleKey >= 0)
    {
        const auto newChromaticKey = scaleB->getBasePeriod() * periodNumber +
            scaleB->getChromaticKey(inScaleKey, 0, false) + keyOffset;

        groupBefore.add(note);
        groupAfter.add(note.withKey(newChromaticKey));
    }
}

void SequencerOperations::rescale(const NoteListBase &notes, const Clip &clip,
    WeakReference<KeySignaturesSequence> harmonicContext, Scale::Ptr targetScale,
    bool undoable, bool shouldCheckpoint)
{
    jassert(targetScale != nullptr);
    jassert(undoable || !shouldCheckpoint);
    if (notes.size() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(notes);
    jassert(sequence);

    Scale::Ptr scale = Scale::makeNaturalMajorScale();
    Note::Key scaleRootKey = 0;
    String scaleRootKeyName;

    Array<Note> groupBefore, groupAfter;
    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getNoteUnchecked(i);

        const float noteStart = note.getBeat() + clip.getBeat();
        SequencerOperations::findHarmonicContext(noteStart, noteStart,
            harmonicContext, scale, scaleRootKey, scaleRootKeyName);

        const auto keyOffset = scaleRootKey - clip.getKey();
        doRescaleLogic(groupBefore, groupAfter, note, keyOffset, scale, targetScale);
    }

    if (groupBefore.size() == 0)
    {
        return;
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->changeGroup(groupBefore, groupAfter, undoable);
}

bool SequencerOperations::rescale(const ProjectNode &project, float startBeat, float endBeat,
    Note::Key rootKey, Scale::Ptr scaleA, Scale::Ptr scaleB,
    bool undoable, bool shouldCheckpoint)
{
    jassert(undoable || !shouldCheckpoint);

    bool hasMadeChanges = false;
    bool didCheckpoint = !shouldCheckpoint;

    const auto pianoTracks = project.findChildrenOfType<PianoTrackNode>();
    for (const auto *track : pianoTracks)
    {
        auto *sequence = dynamic_cast<PianoSequence *>(track->getSequence());
        jassert(sequence != nullptr);

        Array<Note> groupBefore, groupAfter;

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
        sequence->changeGroup(groupBefore, groupAfter, undoable);
    }

    return hasMadeChanges;
}

bool SequencerOperations::remapNotesToTemperament(const ProjectNode &project,
    Temperament::Ptr temperament, bool shouldUseChromaticMaps, bool shouldCheckpoint)
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
        auto *pattern = track->getPattern();
        auto *sequence = static_cast<PianoSequence *>(track->getSequence());

        Array<Note> notesBefore, notesAfter;
        for (int n = 0; n < sequence->size(); ++n)
        {
            const auto *note = static_cast<Note *>(sequence->getUnchecked(n));
            
            // simply using the first clip's position to determine harmonic context,
            // (there could be several clips, in which case I have no idea what to do)
            const auto rootKeyBefore = findRootKey(note->getBeat() + pattern->getFirstBeat());
            const auto key = note->getKey() - rootKeyBefore;
            const auto periodNum = key / periodSizeBefore;
            const auto relativeKey = key % periodSizeBefore;

            // key signatures are converted in a different method
            // (tech debt warning: these two lines should be the same in both methods)
            const auto rootIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(rootKeyBefore);
            const auto rootKeyAfter = chromaticMapTo->getChromaticKey(rootIndexInChromaticMap, 0, true);

            // convert notes either by using the temperaments' chromatic maps,
            // or by assuming equal temperaments and using proportions (more accurate):
            int newKey = 0;
            if (shouldUseChromaticMaps)
            {
                // round the relative key to the nearest one in chromaticMapFrom 
                const auto keyIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(relativeKey);
                const auto newRelativeKey = chromaticMapTo->getChromaticKey(keyIndexInChromaticMap, rootKeyAfter, false);
                newKey = periodNum * periodSizeAfter + newRelativeKey;
            }
            else
            {
                const auto relativeKeyAsFraction = double(relativeKey) / double(periodSizeBefore);
                const auto newRelativeKey = int(round(periodSizeAfter * relativeKeyAsFraction));
                newKey = periodNum * periodSizeAfter + newRelativeKey + rootKeyAfter;
            }

            jassert(newKey != 0);
            notesBefore.add(*note);
            notesAfter.add(note->withKey(newKey));
        }

        // adjust clip key offsets in a similar way
        Array<Clip> clipsBefore, clipsAfter;
        for (int i = 0; i < pattern->size(); ++i)
        {
            const auto *clip = pattern->getUnchecked(i);

            const auto key = clip->getKey();
            const auto periodNum = abs(key) / periodSizeBefore;
            const auto relativeKey = abs(key) % periodSizeBefore;
            const auto keySign = (key > 0) - (key < 0); // key offset can be negative

            int newKey = 0;
            if (shouldUseChromaticMaps)
            {
                const auto keyIndexInChromaticMap = chromaticMapFrom->getNearestScaleKey(relativeKey);
                const auto newRelativeKey = chromaticMapTo->getChromaticKey(keyIndexInChromaticMap, 0, false);
                newKey = (periodNum * periodSizeAfter + newRelativeKey) * keySign;
            }
            else
            {
                const auto relativeKeyAsFraction = double(relativeKey) / double(periodSizeBefore);
                const auto newRelativeKey = int(round(periodSizeAfter * relativeKeyAsFraction));
                newKey = (periodNum * periodSizeAfter + newRelativeKey) * keySign;
            }

            if (key != newKey)
            {
                clipsBefore.add(*clip);
                clipsAfter.add(clip->withKey(newKey));
            }
        }

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
        for (const auto &s : availableScales)
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

        keySignatures->change(*signature, signature->withRootKey(newRootKey, {})
            .withScale(similarScale != nullptr ? similarScale : convertedScale), true);

        hasMadeChanges = true;
    }

    return hasMadeChanges;
}

// Tries to detect if there's one key signature that affects the whole sequence.
// If there's none, of if there's more than one, returns false.
bool SequencerOperations::findHarmonicContext(float startBeat, float endBeat,
    WeakReference<KeySignaturesSequence> keySignatures, Scale::Ptr &outScale,
    Note::Key &outRootKey, String &outKeyName)
{
    if (keySignatures == nullptr ||
        keySignatures->size() == 0)
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
        outKeyName = context->getRootKeyName();
        return true;
    }

    return false;
}

bool SequencerOperations::findHarmonicContext(float startBeat, float endBeat,
    WeakReference<MidiTrack> keysTrack, Scale::Ptr &outScale,
    Note::Key &outRootKey, String &outKeyName)
{
    jassert(keysTrack != nullptr);

    if (auto *keySignatures =
        dynamic_cast<KeySignaturesSequence *>(keysTrack->getSequence()))
    {
        return SequencerOperations::findHarmonicContext(startBeat, endBeat,
            keySignatures, outScale, outRootKey, outKeyName);
    }

    return false;
}

bool SequencerOperations::findHarmonicContext(const NoteListBase &notes, const Clip &clip,
    WeakReference<MidiTrack> keysTrack, Scale::Ptr &outScale, Note::Key &outRootKey, String &outKeyName)
{
    const auto startBeat = SequencerOperations::findStartBeat(notes) + clip.getBeat();
    const auto endBeat = SequencerOperations::findEndBeat(notes) + clip.getBeat();
    return SequencerOperations::findHarmonicContext(startBeat, endBeat, keysTrack, outScale, outRootKey, outKeyName);
}

void SequencerOperations::duplicateSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.size() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);

    Array<Note> newNotes;
    for (int i = 0; i < selection.size(); ++i)
    {
        newNotes.add(selection.getNoteUnchecked(i).withNewId());
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->insertGroup(newNotes, true);
}

void SequencerOperations::deleteSelection(const Lasso &selection, bool shouldCheckpoint)
{
    if (selection.size() == 0)
    {
        return;
    }

    auto *sequence = getPianoSequence(selection);

    Array<Note> removedNotes;
    for (int i = 0; i < selection.size(); ++i)
    {
        removedNotes.add(selection.getNoteUnchecked(i));
    }

    if (shouldCheckpoint)
    {
        sequence->checkpoint();
    }

    sequence->removeGroup(removedNotes, true);
}

String SequencerOperations::findClosestOverlappingAnnotation(const Lasso &selection, WeakReference<MidiTrack> annotationsTrack)
{
    if (selection.getNumSelected() == 0)
    {
        jassertfalse;
        return {};
    }

    const auto &sourceClip = selection.getFirstAs<NoteComponent>()->getClip();
    const float selectionStart = SequencerOperations::findStartBeat(selection) + sourceClip.getBeat();
    const float selectionEnd = SequencerOperations::findEndBeat(selection) + sourceClip.getBeat();

    String result;
    float minDistance = FLT_MAX;
    for (int i = 0; i < annotationsTrack->getSequence()->size(); ++i)
    {
        const auto *event = annotationsTrack->getSequence()->getUnchecked(i);
        jassert(dynamic_cast<const AnnotationEvent *>(event));
        const auto *annotation = static_cast<const AnnotationEvent *>(event);

        const auto annotationStart = annotation->getBeat();
        const auto annotationEnd = annotation->getBeat() + annotation->getLength();
        const float distance = fabs(annotationStart - selectionStart + annotationEnd - selectionEnd);
        const bool isOverlapping = selectionStart <= annotationEnd && annotationStart <= selectionEnd;

        if (minDistance > distance && isOverlapping)
        {
            minDistance = distance;
            result = annotation->getDescription();
        }
    }

    return result;
}

Clip &SequencerOperations::findClosestClip(const Lasso &selection, WeakReference<MidiTrack> track, float &outMinDistance)
{
    jassert(selection.getNumSelected() > 0);
    const auto &sourceClip = selection.getFirstAs<NoteComponent>()->getClip();
    const float selectionStart = SequencerOperations::findStartBeat(selection) + sourceClip.getBeat();
    const float selectionEnd = SequencerOperations::findEndBeat(selection) + sourceClip.getBeat();

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

Array<Note> SequencerOperations::moveSelection(const Lasso &selection,
    Clip &targetClip, bool shouldCheckpoint /*= true*/)
{
    if (selection.getNumSelected() == 0)
    {
        return {};
    }

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

    auto *sequence = static_cast<PianoSequence *>(notes.getFirst().getSequence());

    Array<Note> newEventsToTheRight;
    Array<Note> shortenedNotesBefore, shortenedNotesAfter;

    for (int i = 0; i < notes.size(); ++i)
    {
        const auto &note = notes.getUnchecked(i);
        const float cutBeat = relativeCutBeats.getUnchecked(i);
        if (cutBeat > 0.f && cutBeat < note.getLength())
        {
            shortenedNotesBefore.add(note);
            shortenedNotesAfter.add(note.withLength(cutBeat));
            newEventsToTheRight.add(note.withDeltaBeat(cutBeat).withDeltaLength(-cutBeat).withNewId());
        }
    }

    const auto insertedEventsCopy = newEventsToTheRight; // newEventsToTheRight will be emptied
    if (!shortenedNotesBefore.isEmpty() && !newEventsToTheRight.isEmpty())
    {
        if (shouldCheckpoint)
        {
            sequence->checkpoint();
        }

        sequence->changeGroup(shortenedNotesBefore, shortenedNotesAfter, true);
        sequence->insertGroup(newEventsToTheRight, true);
    }

    return insertedEventsCopy;
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
        jassert(autoSequence != nullptr);

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

    Array<Note> copiedContent;
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

    Array<AutomationEvent> copiedContent;
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
