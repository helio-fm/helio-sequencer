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
#include "UserProfile.h"

CommandPaletteProjectsList::CommandPaletteProjectsList(Workspace &workspace) :
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

    // todo first items == create new, open, import midi
    for (auto *projectInfo : this->workspace.getUserProfile().getProjects())
    {
        const bool isLoaded = this->workspace.hasLoadedProject(projectInfo);
        const CommandPaletteAction::Callback action = [this, projectInfo](TextEditor &ed)
        {
            for (auto *loadedProject : this->workspace.getLoadedProjects())
            {
                if (loadedProject->getId() == projectInfo->getProjectId())
                {
                    // switch to it, don't unload:
                    //this->workspace.unloadProject(projectInfo->getProjectId(), false, false);

                    if (!loadedProject->selectChildOfType<PianoTrackNode>())
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

        const auto defaultColor = findDefaultColour(Label::textColourId);
        const auto sinceLastOpened = Time::getCurrentTime() - projectInfo->getUpdatedAt();
        this->projects.add(CommandPaletteAction::action(projectInfo->getTitle(),
            App::getHumanReadableDate(projectInfo->getUpdatedAt()), float(sinceLastOpened.inSeconds()))->
            withColour(isLoaded ? defaultColor : defaultColor.contrasting())->
            withCallback(action));
    }
}
