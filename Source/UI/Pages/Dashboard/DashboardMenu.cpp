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

#include "Common.h"
#include "DashboardMenu.h"

#include "RecentProjectRow.h"
#include "ProjectNode.h"
#include "Workspace.h"

DashboardMenu::DashboardMenu(Workspace &parentWorkspace) :
    workspace(parentWorkspace)
{
    this->setFocusContainerType(Component::FocusContainerType::none);

    this->listBox = make<ListBox>();
    this->addAndMakeVisible(this->listBox.get());
    this->listBox->setRowHeight(56);
    this->listBox->getViewport()->setScrollBarsShown(false, false);
    this->listBox->setModel(this);

    this->updateListContent();
}

void DashboardMenu::resized()
{
    this->listBox->setBounds(this->getLocalBounds());
}

void DashboardMenu::loadFile(RecentProjectInfo::Ptr project)
{
    if (!this->workspace.loadRecentProject(project))
    {
        // TODO test if it would be better to just remove the project from the list
        this->workspace.getUserProfile().deleteProjectLocally(project->getProjectId());
    }

    this->updateListContent();
}

void DashboardMenu::unloadFile(RecentProjectInfo::Ptr project)
{
    this->workspace.unloadProject(project->getProjectId(), false);
    this->updateListContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *DashboardMenu::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    const auto &list = this->workspace.getUserProfile().getProjects();
    const bool isLastRow = (rowNumber == (list.size() - 1));
    if (rowNumber >= list.size()) { return existingComponentToUpdate; }

    const auto item = list[rowNumber];
    if (item == nullptr) { return existingComponentToUpdate; }

    const bool isLoaded = this->loadedProjectIds.contains(item->getProjectId());

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<RecentProjectRow *>(existingComponentToUpdate))
        {
            row->updateDescription(item, isLoaded, isLastRow);
            row->setSelected(isRowSelected);
        }
        else
        {
            delete existingComponentToUpdate;
            auto newRow = make<RecentProjectRow>(*this, *this->listBox);
            newRow->updateDescription(item, isLoaded, isLastRow);
            newRow->setSelected(isRowSelected);
            existingComponentToUpdate = newRow.get();
            return newRow.release();
        }
    }
    else
    {
        auto row = make<RecentProjectRow>(*this, *this->listBox);
        row->updateDescription(item, isLoaded, isLastRow);
        row->setSelected(isRowSelected);
        return row.release();
    }

    return existingComponentToUpdate;
}
int DashboardMenu::getNumRows()
{
    return this->workspace.getUserProfile().getProjects().size();
}

void DashboardMenu::updateListContent()
{
    this->loadedProjectIds.clear();
    for (const auto *project : this->workspace.getLoadedProjects())
    {
        this->loadedProjectIds.emplace(project->getId());
    }
    this->listBox->updateContent();
}
