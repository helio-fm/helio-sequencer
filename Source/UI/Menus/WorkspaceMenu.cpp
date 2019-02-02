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
#include "RootTreeItem.h"
#include "ProjectTreeItem.h"
#include "SettingsTreeItem.h"
#include "OrchestraPitTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "Workspace.h"
#include "CommandIDs.h"

WorkspaceMenu::WorkspaceMenu(Workspace &workspace) :
    workspace(workspace)
{
    this->showMainMenu(MenuPanel::SlideRight);
}

void WorkspaceMenu::handleCommandMessage(int commandId)
{
    //switch (commandId)
    //{
    //default:
    //    break;
    //}
}

void WorkspaceMenu::showMainMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    
    //menu.add(MenuItem::item(Icons::create,
    //    TRANS("menu::workspace::project::create"))->withAction([this]()
    //    {}));

    //menu.add(MenuItem::item(Icons::browse,
    //    TRANS("menu::workspace::project::open"))->withAction([this]()
    //    {}));

    auto *root = this->workspace.getTreeRoot();

    if (auto *settings = root->findChildOfType<SettingsTreeItem>())
    {
        menu.add(MenuItem::item(Icons::settings, TRANS("tree::settings"))->
            disabledIf(settings->isSelected())->withAction([this, settings]()
        {
            settings->setSelected(true, true);
            this->dismiss();
        }));
    }

    if (auto *instruments = root->findChildOfType<OrchestraPitTreeItem>())
    {
        menu.add(MenuItem::item(Icons::orchestraPit, TRANS("tree::instruments"))->
            disabledIf(instruments->isSelected())->withAction([this, instruments]()
        {
            instruments->setSelected(true, true);
            this->dismiss();
        }));
    }

    const auto &projects = root->findChildrenOfType<ProjectTreeItem>();
    for (int i = 0; i < projects.size(); ++i)
    {
        const bool isShowingCurrent = projects[i]->isSelectedOrHasSelectedChild();
        menu.add(MenuItem::item(Icons::project, projects[i]->getName())->
            disabledIf(isShowingCurrent)->withAction([this, project = projects[i]]()
        {
            project->selectChildOfType<PianoTrackTreeItem>();
            this->dismiss();
        }));
    }

    this->updateContent(menu, animationType);
}

//void WorkspaceMenu::showNewProjectMenu(AnimationType animationType)
//{
//    MenuPanel::Menu menu;
//    menu.add(MenuItem::item(Icons::back,
//        TRANS("menu::back"))->withTimer()->withAction([this]()
//        {
//            this->showCreateItemsMenu(MenuPanel::SlideRight);
//        }));
//    
//    this->updateContent(menu, animationType);
//}
