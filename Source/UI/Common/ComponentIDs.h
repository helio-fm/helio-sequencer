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

// Component ids are used to deliver hotkey messages
// and for any hierarchy-agnostic component access
namespace ComponentIDs
{
    static const String progressTooltipId = "ProgressTooltip";
    static const String pianoRollId = "PianoRoll";
    static const String patternRollId = "PatternRoll";
    static const String mainLayoutId = "MainLayout";
    static const String sequencerLayoutId = "SequencerLayout";
    static const String versionControlStage = "VersionControlStage";
    static const String versionControlHistory = "VersionControlHistory";
    static const String orchestraPit = "OrchestraPit";
    static const String keyboardMapping = "KeyboardMapping";
} // namespace ComponentIDs
