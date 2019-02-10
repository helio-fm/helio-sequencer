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

class Clip;
class Lasso;
class ProjectNode;

struct PatternOperations final
{
    static void deleteSelection(const Lasso &selection, ProjectNode &project, bool shouldCheckpoint = true);

    static void transposeClips(const Lasso &selection, int deltaKey, bool shouldCheckpoint = true);
    static void tuneClips(const Lasso &selection, float deltaVelocity, bool shouldCheckpoint = true);
    static void shiftBeatRelative(Lasso &selection, float deltaBeat, bool shouldCheckpoint = true);

    static void cutClip(ProjectNode &project, const Clip &clip,
        float relativeCutBeat, bool shouldCheckpoint = true);
};
