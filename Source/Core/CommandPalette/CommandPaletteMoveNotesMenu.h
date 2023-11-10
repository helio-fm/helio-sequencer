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

#include "ProjectListener.h"
#include "CommandPaletteActionsProvider.h"

class PianoRoll;
class ProjectNode;

class CommandPaletteMoveNotesMenu final :
    public CommandPaletteActionsProvider,
    public ProjectListener
{
public:

    CommandPaletteMoveNotesMenu(PianoRoll &roll, ProjectNode &project);
    ~CommandPaletteMoveNotesMenu() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;
    void onChangeTrackBeatRange(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;
    void onChangeViewEditableScope(MidiTrack *const track,
        const Clip &clip, bool shouldFocus) override;

protected:

    const Actions &getActions() const override;
    mutable Actions actions;

private:

    PianoRoll &roll;
    ProjectNode &project;

    mutable Actions actionsCache;
    mutable bool actionsCacheOutdated = true;

};
