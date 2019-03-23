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

#pragma once

class Transport;
class ProjectNode;
class MidiSequence;
class MidiTrackNode;
class MidiTrack;
class Pattern;
class Clipboard;
class PianoSequence;

#include "Note.h"
#include "Lasso.h"
#include "AutomationEvent.h"
#include "Scale.h"
#include "Arpeggiator.h"

struct SequencerOperations final
{
    static float findStartBeat(const Lasso &selection);
    static float findEndBeat(const Lasso &selection);
    static float findStartBeat(const WeakReference<Lasso> selection);
    static float findEndBeat(const WeakReference<Lasso> selection);
    static float findStartBeat(const Array<Note> &selection);
    static float findEndBeat(const Array<Note> &selection);

    static PianoSequence *getPianoSequence(const Lasso &selection);
    static PianoSequence *getPianoSequence(const SelectionProxyArray::Ptr selection);

    static void wipeSpace(Array<MidiTrack *> tracks, float startBeat, float endBeat,
        bool shouldKeepCroppedNotes = true,  bool shouldCheckpoint = true);
    
    static void shiftEventsToTheLeft(Array<MidiTrack *> tracks,
        float targetBeat,  float beatOffset, bool shouldCheckpoint = true);
    
    static void shiftEventsToTheRight(Array<MidiTrack *> tracks,
        float targetBeat, float beatOffset, bool shouldCheckpoint = true);
    
    static void snapSelection(Lasso &selection, float snapsPerBeat, bool shouldCheckpoint = true);
    static void removeOverlaps(Lasso &selection, bool shouldCheckpoint = true);
    static void removeDuplicates(Lasso &selection, bool shouldCheckpoint = true);
    
    static void moveToLayer(Lasso &selection, MidiSequence *layer, bool shouldCheckpoint = true);
    
    static bool arpeggiate(Lasso &selection,
        const Scale::Ptr chordScale, Note::Key chordRoot, const Arpeggiator::Ptr arp,
        bool reversed = false, bool limitToChord = false, bool shouldCheckpoint = true);

    static void randomizeVolume(Lasso &selection, float factor = 0.5f, bool shouldCheckpoint = true);
    static void fadeOutVolume(Lasso &selection, float factor = 0.5f, bool shouldCheckpoint = true);

    static void startTuning(Lasso &selection);
    static void changeVolumeLinear(Lasso &selection, float volumeDelta);
    static void changeVolumeMultiplied(Lasso &selection, float volumeFactor);
    static void changeVolumeSine(Lasso &selection, float volumeFactor);
    static void endTuning(Lasso &selection);
    
    static void copyToClipboard(Clipboard &clipboard, const Lasso &selection);
    static void pasteFromClipboard(Clipboard &clipboard, ProjectNode &project,
        WeakReference<MidiTrack> selectedTrack, float targetBeatPosition, bool shouldCheckpoint = true);

    static void deleteSelection(const Lasso &selection, bool shouldCheckpoint = true);
    static void duplicateSelection(const Lasso &selection, bool shouldCheckpoint = true);

    static void shiftKeyRelative(Lasso &selection, int deltaKey,
        bool shouldCheckpoint = true, Transport *transport = nullptr);

    static void shiftBeatRelative(Lasso &selection, float deltaBeat,
        bool shouldCheckpoint = true);
    
    static void invertChord(Lasso &selection, int deltaKey,
        bool shouldCheckpoint = true, Transport *transport = nullptr);

    static void rescale(Lasso &selection, Scale::Ptr scaleA, Scale::Ptr scaleB, bool shouldCheckpoint = true);

    static bool findHarmonicContext(const Lasso &selection, const Clip &clip,
        WeakReference<MidiTrack> keySignatures, Scale::Ptr &outScale, Note::Key &outRootKey);

    static Array<Note> cutEvents(const Array<Note> &notes,
        const Array<float> &relativeCutBeats, bool shouldCheckpoint = true);

    // Creates new tracks from events of existing tracks
    static ScopedPointer<MidiTrackNode> createPianoTrack(const Lasso &selection);
    static ScopedPointer<MidiTrackNode> createPianoTrack(const Array<Note> &events, const Pattern *pattern);
    static ScopedPointer<MidiTrackNode> createAutomationTrack(const Array<AutomationEvent> &events, const Pattern *pattern);
};
