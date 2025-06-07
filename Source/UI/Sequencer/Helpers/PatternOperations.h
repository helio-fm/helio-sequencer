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

class Clip;
class Lasso;
class ProjectNode;
class Pattern;

struct PatternOperations final
{
    static float findStartBeat(const Lasso &selection);
    static float findEndBeat(const Lasso &selection);

    static void deleteSelection(const Lasso &selection,
        ProjectNode &project, bool shouldCheckpoint = true);
    static void deleteSelection(const Array<Clip> &selection,
        ProjectNode &project, bool shouldCheckpoint = true);

    static void duplicateSelection(const Lasso &selection, bool shouldCheckpoint = true);

    // this one transposes all clips and updates all key signatures:
    static void transposeProject(ProjectNode &project,
        int deltaKey, int64 transactionId, bool shouldCheckpoint = true);
    // this one just transposes selected clips:
    // (set forceCheckpoint to avoid coalescing similar transpositions into one)
    static void transposeClips(const Lasso &selection, int deltaKey,
        bool shouldCheckpoint = true, bool forceCheckpoint = false);

    static void tuneClips(const Lasso &selection, float deltaVelocity, bool shouldCheckpoint = true);
    static void shiftBeatRelative(const Lasso &selection, float deltaBeat, bool shouldCheckpoint = true);

    static void cutClip(ProjectNode &project, const Clip &clip, float absCutBeat,
        bool shouldRenameNewTrack, bool shouldCheckpoint = true);

    static bool lassoContainsMutedClip(const Lasso &selection);
    static bool lassoContainsSoloedClip(const Lasso &selection);

    static void toggleSoloClip(const Clip &clip, bool shouldCheckpoint = true);
    static void toggleMuteClip(const Clip &clip, bool shouldCheckpoint = true);

    static void toggleSoloClips(const Lasso &selection, bool shouldCheckpoint = true);
    static void toggleMuteClips(const Lasso &selection, bool shouldCheckpoint = true);

    static void quantize(const Lasso &selection, float bar, bool shouldCheckpoint = true);

    static bool shiftTempo(const Lasso &selection, int bpmDelta, bool shouldCheckpoint = true);

    static void mergeClips(ProjectNode &project, const Clip &targetClip,
        const Array<Clip> &sourceClips, bool shouldCheckpoint = true);

    // simply reverse the clips order for each row
    static void retrograde(ProjectNode &project, const Lasso &selection, bool shouldCheckpoint = true);

    // will "apply" the modifiers stack by doing either of the following:
    // if there's a single clip in the pattern, will delete all events
    // in the sequence and insert events from the generated sequence,
    // and clear the clip's parametric modifiers stack after all that;
    // if there's more than one clip, will delete it and insert
    // a new track of the same parameters with generated events in it
    static bool applyModifiersStack(const Clip &clip, bool shouldCheckpoint = true);

    // toggle enable/disable all modifiers:
    // if any modifiers are enabled, disable all, otherwise enable all
    static bool toggleMuteModifiersStack(const Clip &clip, bool shouldCheckpoint = true);
    static void toggleMuteModifiersStack(const Lasso &selection, bool shouldCheckpoint = true);

    // if all selected clips are assigned to the same instrument,
    // this returns that instrument id, otherwise returns an empty string:
    static String getSelectedInstrumentId(const Lasso &selection);
};
