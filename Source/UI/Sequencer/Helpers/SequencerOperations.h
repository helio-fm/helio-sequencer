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

#pragma once

class Transport;
class ProjectNode;
class MidiSequence;
class MidiTrackNode;
class MidiTrack;
class Pattern;
class Clipboard;
class AutomationSequence;
class KeySignaturesSequence;
class TimeSignaturesAggregator;

#include "Note.h"
#include "Lasso.h"
#include "AutomationEvent.h"
#include "Scale.h"
#include "Temperament.h"
#include "Arpeggiator.h"
#include "PianoSequence.h"

struct SequencerOperations final
{
    static float findStartBeat(const NoteListBase &notes);
    static float findEndBeat(const NoteListBase &notes);
    static float findStartBeat(const WeakReference<Lasso> selection);
    static float findEndBeat(const WeakReference<Lasso> selection);
    static float findStartBeat(const Array<Note> &selection);
    static float findEndBeat(const Array<Note> &selection);

    static void previewSelection(const Lasso &selection, Transport &transport, int maxSize = 7);
    
    static Clip &findClosestClip(const Lasso &selection,
        WeakReference<MidiTrack> track, float &outDistance);

    static String findClosestOverlappingAnnotation(const Lasso &selection,
        WeakReference<MidiTrack> annotations);

    static PianoSequence *getPianoSequence(const NoteListBase &notes);
    static PianoSequence *getPianoSequence(const Clip &targetClip);

    static Arpeggiator::Ptr makeArpeggiator(const String &name,
        const Lasso &selection,
        const Temperament::Ptr temperament,
        const Scale::Ptr scale, Note::Key scaleRootKey, 
        WeakReference<TimeSignaturesAggregator> timeContext);

    static bool arpeggiate(const NoteListBase &notes, const Clip &clip,
        const Arpeggiator::Ptr arpeggiator,
        const Temperament::Ptr temperament,
        WeakReference<KeySignaturesSequence> harmonicContext,
        WeakReference<TimeSignaturesAggregator> timeContext,
        float durationMultiplier, float randomness, bool reversed = false, bool limitToChord = false,
        bool undoable = true, bool shouldCheckpoint = true);

    static bool isBarStart(float absBeat,
        WeakReference<TimeSignaturesAggregator> timeContext);

    // for hotkey commands:
    static void randomizeVolume(const Lasso &selection, float factor = 0.5f, bool shouldCheckpoint = true);
    static void fadeOutVolume(const Lasso &selection, float factor = 0.5f, bool shouldCheckpoint = true);
    static void tuneVolume(const Lasso &selection, float delta, bool shouldCheckpoint = true);

    // for interactive volume changes:
    static void startTuning(const Lasso &selection);
    static void changeVolumeLinear(const Lasso &selection, float volumeDelta);
    static void changeVolumeMultiplied(const Lasso &selection, float volumeFactor);
    static void changeVolumeSine(const Lasso &selection, float volumeFactor);
    static void endTuning(const Lasso &selection);

    static void copyToClipboard(Clipboard &clipboard, const Lasso &selection);
    static void pasteFromClipboard(Clipboard &clipboard, ProjectNode &project,
        WeakReference<MidiTrack> selectedTrack, float targetBeatPosition, bool shouldCheckpoint = true);

    static void deleteSelection(const Lasso &selection, bool shouldCheckpoint = true);
    static void duplicateSelection(const Lasso &selection, bool shouldCheckpoint = true);

    static Array<Note> moveSelection(const Lasso &selection,
        Clip &targetClip, bool shouldCheckpoint = true);

    // set forceCheckpoint to avoid coalescing similar transpositions into one
    static void shiftKeyRelative(const NoteListBase &notes, int deltaKey,
        bool undoable = true, bool shouldCheckpoint = true, bool forceCheckpoint = false);

    // pass deltaKey=0 to do snap-to-scale:
    static void shiftInScaleKeyRelative(const NoteListBase &notes, const Clip &clip,
        WeakReference<KeySignaturesSequence> keySignatures, Scale::Ptr defaultScale,
        int deltaKey, bool undoable = true, bool shouldCheckpoint = true);

    static void shiftBeatRelative(const NoteListBase &notes,
        float deltaBeat, bool undoable = true, bool shouldCheckpoint = true);

    static void shiftLengthRelative(const NoteListBase &notes,
        float deltaLength, bool undoable = true, bool shouldCheckpoint = true);

    static void invertChord(const NoteListBase &notes, int deltaKey,
        bool undoable = true, bool shouldCheckpoint = true);

    static void cleanupOverlaps(const NoteListBase &notes, bool undoable = true, bool shouldCheckpoint = true);

    static void retrograde(const NoteListBase &notes, bool undoable = true, bool shouldCheckpoint = true);
    static void melodicInversion(const NoteListBase &notes, bool undoable = true, bool shouldCheckpoint = true);

    static void makeStaccato(const NoteListBase &notes,
        float newLength, bool undoable = true, bool shouldCheckpoint = true);
    static bool makeLegato(const NoteListBase &notes,
        float overlap, bool undoable = true, bool shouldCheckpoint = true);

    static bool quantize(const NoteListBase &notes, float bar,
        bool undoable = true, bool shouldCheckpoint = true);

    static void applyTuplets(const NoteListBase &notes, Note::Tuplet tuplet,
        bool undoable = true, bool shouldCheckpoint = true);

    static void rescale(const NoteListBase &notes, const Clip &clip,
        WeakReference<KeySignaturesSequence> harmonicContext, Scale::Ptr targetScale,
        bool undoable = true, bool shouldCheckpoint = true);

    static bool rescale(const ProjectNode &project, float startBeat, float endBeat,
        Note::Key rootKey, Scale::Ptr scaleA, Scale::Ptr scaleB,
        bool undoable = true, bool shouldCheckpoint = true);

    static bool remapNotesToTemperament(const ProjectNode &project,
        Temperament::Ptr temperament, bool shouldUseChromaticMaps = true,
        bool shouldCheckpoint = true);

    static bool remapKeySignaturesToTemperament(KeySignaturesSequence *keySignatures,
        Temperament::Ptr currentTemperament, Temperament::Ptr otherTemperament,
        const Array<Scale::Ptr> &availableScales, bool shouldCheckpoint = true);

    static bool findHarmonicContext(const NoteListBase &notes, const Clip &clip,
        WeakReference<MidiTrack> keySignatures, Scale::Ptr &outScale,
        Note::Key &outRootKey, String &outKeyName);
    static bool findHarmonicContext(float startBeat, float endBeat,
        WeakReference<MidiTrack> keySignatures, Scale::Ptr &outScale,
        Note::Key &outRootKey, String &outKeyName);
    static bool findHarmonicContext(float startBeat, float endBeat,
        WeakReference<KeySignaturesSequence> keySignatures, Scale::Ptr &outScale,
        Note::Key &outRootKey, String &outKeyName);

    static Array<Note> cutNotes(const Array<Note> &notes,
        const Array<float> &relativeCutBeats, bool shouldCheckpoint = true);
    static bool mergeNotes(const Note &note1, const Note &note2, bool shouldCheckpoint = true);

    static bool setOneTempoForProject(ProjectNode &project, int bpmValue, bool shouldCheckpoint = true);
    static bool shiftTempoForProject(ProjectNode &project, int bpmDelta, bool shouldCheckpoint = true);
    static bool setOneTempoForTrack(WeakReference<MidiTrack> track,
        float startBeat, float endBeat, int bpmValue, bool shouldCheckpoint = true);

    static SerializedData createPianoTrackTemplate(ProjectNode &project,
       const String &name, float beatPosition, const String &instrumentId, String &outTrackId);
    static SerializedData createAutoTrackTemplate(ProjectNode &project,
        const String &name, int controllerNumber, const String &instrumentId, String &outTrackId);

    // Creates new (duplicated) tracks from events of existing tracks
    static UniquePointer<MidiTrackNode> createPianoTrack(const Lasso &selection);
    static UniquePointer<MidiTrackNode> createPianoTrack(const PianoSequence *source, const Clip &clip);
    static UniquePointer<MidiTrackNode> createPianoTrack(const Array<Note> &events, const Pattern *pattern);
    static UniquePointer<MidiTrackNode> createPianoTrack(const Array<Note> &events, const Array<Clip> &clips);
    static UniquePointer<MidiTrackNode> createAutomationTrack(const AutomationSequence *source, const Clip &clip);
    static UniquePointer<MidiTrackNode> createAutomationTrack(const Array<AutomationEvent> &events, const Pattern *pattern);
    static UniquePointer<MidiTrackNode> createAutomationTrack(const Array<AutomationEvent> &events, const Array<Clip> &clips);

    static String generateNextNameForNewTrack(const String &name, const StringArray &allNames);

};
