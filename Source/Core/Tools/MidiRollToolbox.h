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
class Arpeggiator;
class ProjectTreeItem;
class MidiEventSelection;
class MidiLayer;
class Note;

class MidiRollToolbox
{
public:
    
    static float findStartBeat(const MidiEventSelection &selection);
    static float findEndBeat(const MidiEventSelection &selection);
    static float findStartBeat(const Array<Note> &selection);
    static float findEndBeat(const Array<Note> &selection);
    
    static void wipeSpace(Array<MidiLayer *> layers,
                          float startBeat,
                          float endBeat,
                          bool shouldKeepCroppedNotes = true,
                          bool shouldCheckpoint = true);
    
    static void shiftEventsToTheLeft(Array<MidiLayer *> layers,
                                     float targetBeat,
                                     float beatOffset,
                                     bool shouldCheckpoint = true);
    
    static void shiftEventsToTheRight(Array<MidiLayer *> layers,
                                      float targetBeat,
                                      float beatOffset,
                                      bool shouldCheckpoint = true);
    
    static void snapSelection(MidiEventSelection &selection, float snapsPerBeat, bool shouldCheckpoint = true);
    static void removeOverlaps(MidiEventSelection &selection, bool shouldCheckpoint = true);
    static void removeDuplicates(MidiEventSelection &selection, bool shouldCheckpoint = true);
    
    static void moveToLayer(MidiEventSelection &selection, MidiLayer *layer, bool shouldCheckpoint = true);
    
    static bool arpeggiateUsingClipboardAsPattern(MidiEventSelection &selection, bool shouldCheckpoint = true);
    static bool arpeggiate(MidiEventSelection &selection, const Arpeggiator &arp, bool shouldCheckpoint = true);

    static void randomizeVolume(MidiEventSelection &selection, float factor = 0.5f, bool shouldCheckpoint = true);
    static void fadeOutVolume(MidiEventSelection &selection, float factor = 0.5f, bool shouldCheckpoint = true);

    static void startTuning(MidiEventSelection &selection);
    static void changeVolumeLinear(MidiEventSelection &selection, float volumeDelta);
    static void changeVolumeMultiplied(MidiEventSelection &selection, float volumeFactor);
    static void changeVolumeSine(MidiEventSelection &selection, float volumeFactor);
    static void endTuning(MidiEventSelection &selection);
    
    static void deleteSelection(MidiEventSelection &selection);
    
    static void shiftKeyRelative(MidiEventSelection &selection, int deltaKey,
		bool shouldCheckpoint = true, Transport *transport = nullptr);

    static void shiftBeatRelative(MidiEventSelection &selection, float deltaBeat,
		bool shouldCheckpoint = true);
    
    static void inverseChord(MidiEventSelection &selection, int deltaKey,
		bool shouldCheckpoint = true, Transport *transport = nullptr);
    
};
