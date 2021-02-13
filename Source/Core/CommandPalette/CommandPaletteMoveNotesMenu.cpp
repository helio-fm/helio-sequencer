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
#include "CommandPaletteMoveNotesMenu.h"

#include "Pattern.h"
#include "PianoRoll.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "SequencerOperations.h"
#include "MainLayout.h"

CommandPaletteMoveNotesMenu::CommandPaletteMoveNotesMenu(PianoRoll &roll, ProjectNode &project) :
    CommandPaletteActionsProvider(TRANS(I18n::CommandPalette::moveNotes), ':', -2.f),
    roll(roll),
    project(project)
{
    this->project.addListener(this);
}

CommandPaletteMoveNotesMenu::~CommandPaletteMoveNotesMenu()
{
    this->project.removeListener(this);
}

const CommandPaletteActionsProvider::Actions &CommandPaletteMoveNotesMenu::getActions() const
{
    if (!this->actionsCacheOutdated)
    {
        return this->actionsCache;
    }

    const auto defaultColor = findDefaultColour(Label::textColourId);

    this->actionsCache.clearQuick();
    this->actionsCache.add(CommandPaletteAction::action(
        TRANS(I18n::Menu::Selection::notesToTrack), {}, 0.f)->
        withColour(defaultColor)->
        withCallback([](TextEditor &)
    {
        App::Layout().broadcastCommandMessage(CommandIDs::NewTrackFromSelection);
        return true;
    }));

    for (auto *targetTrack : this->project.findChildrenOfType<PianoTrackNode>())
    {
        if (targetTrack == this->roll.getActiveTrack())
        {
            continue;
        }

        static const float orderOffset = 10.f; // after the 'extract as new track' action
        this->actionsCache.add(CommandPaletteAction::action(targetTrack->getTrackName(),
            this->getName(), orderOffset)->
            withColour(targetTrack->getTrackColour())->
            withCallback([this, targetTrack](TextEditor &)
        {
            auto &closestClip = SequencerOperations::findClosestClip(this->roll.getLassoSelection(), targetTrack);
            SequencerOperations::moveSelection(this->roll.getLassoSelection(), closestClip, true);
            this->project.setEditableScope(targetTrack, closestClip, false);
            return true;
        }));
    }

    this->actionsCacheOutdated = false;
    return this->actionsCache;
}

void CommandPaletteMoveNotesMenu::onAddClip(const Clip &clip)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onRemoveClip(const Clip &clip)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onAddTrack(MidiTrack *const track)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onRemoveTrack(MidiTrack *const track)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onChangeTrackProperties(MidiTrack *const track)
{
    this->actionsCacheOutdated = true; // the name might have changed
}

void CommandPaletteMoveNotesMenu::onChangeTrackBeatRange(MidiTrack *const track)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->actionsCacheOutdated = true;
}

void CommandPaletteMoveNotesMenu::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->actionsCacheOutdated = true;
}
