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
#include "WorkspaceMenu.h"
#include "RootNode.h"
#include "ProjectNode.h"
#include "SettingsNode.h"
#include "OrchestraPitNode.h"
#include "PianoTrackNode.h"
#include "Workspace.h"
#include "CommandIDs.h"

WorkspaceMenu::WorkspaceMenu(Workspace &workspace) :
    workspace(workspace)
{
    this->showMainMenu(MenuPanel::SlideRight);
}

void WorkspaceMenu::showMainMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;

    auto *root = this->workspace.getTreeRoot();

    if (auto *settings = root->findChildOfType<SettingsNode>())
    {
        menu.add(MenuItem::item(Icons::settings, TRANS(I18n::Tree::settings))->
            disabledIf(settings->isSelected())->
            closesMenu()->
            withAction([this, settings]()
            {
                this->workspace.stopPlaybackForAllProjects();
                settings->setSelected();
            }));
    }

    if (auto *instruments = root->findChildOfType<OrchestraPitNode>())
    {
        menu.add(MenuItem::item(Icons::orchestraPit, TRANS(I18n::Tree::instruments))->
            disabledIf(instruments->isSelected())->
            closesMenu()->
            withAction([this, instruments]()
            {
                this->workspace.stopPlaybackForAllProjects();
                instruments->setSelected();
            }));
    }

    const auto projects = root->findChildrenOfType<ProjectNode>();
    for (int i = 0; i < projects.size(); ++i)
    {
        const auto *project = projects.getUnchecked(i);
        const bool isShowingCurrent = project->isSelectedOrHasSelectedChild();
        menu.add(MenuItem::item(Icons::project, project->getName())->
            disabledIf(isShowingCurrent)->
            closesMenu()->
            withAction([this, project]()
            {
                this->workspace.stopPlaybackForAllProjects();
                project->selectChildOfType<PianoTrackNode>();
            }));
    }

    this->updateContent(menu, animationType);
}
