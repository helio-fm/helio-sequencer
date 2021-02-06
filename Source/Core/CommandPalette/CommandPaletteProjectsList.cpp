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
#include "CommandPaletteProjectsList.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"

CommandPaletteProjectsList::CommandPaletteProjectsList(Workspace &workspace) :
    CommandPaletteActionsProvider(TRANS(I18n::CommandPalette::projects), '/', -1.f),
    workspace(workspace)
{
    this->reloadProjects();
    this->workspace.getUserProfile().addChangeListener(this);
}

CommandPaletteProjectsList::~CommandPaletteProjectsList()
{
    this->workspace.getUserProfile().removeChangeListener(this);
}

const CommandPaletteActionsProvider::Actions &CommandPaletteProjectsList::getActions() const
{
    return this->projects;
}

void CommandPaletteProjectsList::changeListenerCallback(ChangeBroadcaster *source)
{
    this->reloadProjects();
}

void CommandPaletteProjectsList::reloadProjects()
{
    this->projects.clearQuick();

    const auto defaultColor = findDefaultColour(Label::textColourId);

    this->projects.add(CommandPaletteAction::action(
        TRANS(I18n::Menu::workspaceProjectCreate), {}, 1.f)->
        withColour(defaultColor)->
        withCallback([](TextEditor &) {
            App::Workspace().createEmptyProject();
            return true;
        }));

    this->projects.add(CommandPaletteAction::action(
        TRANS(I18n::Menu::workspaceProjectOpen), {}, 1.f)->
        withColour(defaultColor)->
        withCallback([](TextEditor &) {
            App::Workspace().importProject("*.helio;*.mid;*.midi");
            return true;
        }));

    for (auto *projectInfo : this->workspace.getUserProfile().getProjects())
    {
        const bool isLoaded = this->workspace.hasLoadedProject(projectInfo);
        const auto action = [this, projectInfo](TextEditor &)
        {
            for (auto *loadedProject : this->workspace.getLoadedProjects())
            {
                if (loadedProject->getId() == projectInfo->getProjectId())
                {
                    // switch to it, don't unload:
                    //this->workspace.unloadProject(projectInfo->getProjectId(), false, false);

                    if (!loadedProject->selectFirstChildOfType<PianoTrackNode, PatternEditorNode>())
                    {
                        loadedProject->setSelected();
                    }

                    return true;
                }
            }

            if (!this->workspace.loadRecentProject(projectInfo))
            {
                this->workspace.getUserProfile().deleteProjectLocally(projectInfo->getProjectId());
            }

            return true;
        };

        constexpr auto orderOffset = 10.f; // after 'create' and 'open' actions
        const auto sinceLastOpened = Time::getCurrentTime() - projectInfo->getUpdatedAt();
        this->projects.add(CommandPaletteAction::action(projectInfo->getTitle(),
            App::getHumanReadableDate(projectInfo->getUpdatedAt()),
            orderOffset + float(sinceLastOpened.inSeconds()))->
            withColour(isLoaded ? defaultColor : defaultColor.withMultipliedAlpha(0.4f))->
            withCallback(action));
    }
}
