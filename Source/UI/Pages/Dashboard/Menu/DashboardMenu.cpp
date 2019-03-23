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

//[Headers]
#include "Common.h"
//[/Headers]

#include "DashboardMenu.h"

//[MiscUserDefs]
#include "RecentProjectRow.h"
#include "ProjectNode.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "CommandIDs.h"
#include "ComponentIDs.h"

#if HELIO_DESKTOP
#    define DEFAULT_RECENT_FILES_ROW_HEIGHT (56)
#elif HELIO_MOBILE
#    define DEFAULT_RECENT_FILES_ROW_HEIGHT (56)
#endif
//[/MiscUserDefs]

DashboardMenu::DashboardMenu(Workspace *parentWorkspace)
    : workspace(parentWorkspace)
{
    this->listBox.reset(new ListBox());
    this->addAndMakeVisible(listBox.get());


    //[UserPreSize]
    this->setFocusContainer(false);
    //[/UserPreSize]

    this->setSize(450, 500);

    //[Constructor]
    this->listBox->setRowHeight(DEFAULT_RECENT_FILES_ROW_HEIGHT);
    this->listBox->getViewport()->setScrollBarsShown(false, false);

    this->listBox->setModel(this);
    this->updateListContent();
    //[/Constructor]
}

DashboardMenu::~DashboardMenu()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    listBox = nullptr;

    //[Destructor]
    //[/Destructor]
}

void DashboardMenu::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DashboardMenu::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    listBox->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DashboardMenu::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void DashboardMenu::loadFile(RecentProjectInfo::Ptr project)
{
    if (!this->workspace->loadRecentProject(project))
    {
        // TODO test if it would be better to just remote the project from the list
        this->workspace->getUserProfile().deleteProjectLocally(project->getProjectId());
    }

    this->updateListContent();
}

void DashboardMenu::unloadFile(RecentProjectInfo::Ptr project)
{
    this->workspace->unloadProject(project->getProjectId(), false, false);
    this->updateListContent();
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *DashboardMenu::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    const auto &list = this->workspace->getUserProfile().getProjects();
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
            ScopedPointer<RecentProjectRow> newRow(new RecentProjectRow(*this, *this->listBox));
            newRow->updateDescription(item, isLoaded, isLastRow);
            newRow->setSelected(isRowSelected);
            existingComponentToUpdate = newRow;
            return newRow.release();
        }
    }
    else
    {
        ScopedPointer<RecentProjectRow> row(new RecentProjectRow(*this, *this->listBox));
        row->updateDescription(item, isLoaded, isLastRow);
        row->setSelected(isRowSelected);
        return row.release();
    }

    return existingComponentToUpdate;
}

void DashboardMenu::listBoxItemClicked(int row, const MouseEvent &e) {}
void DashboardMenu::paintListBoxItem(int, Graphics &, int, int, bool) {}

int DashboardMenu::getNumRows()
{
    return this->workspace->getUserProfile().getProjects().size();
}

void DashboardMenu::updateListContent()
{
    this->loadedProjectIds.clear();
    for (const auto *project : this->workspace->getLoadedProjects())
    {
        this->loadedProjectIds.emplace(project->getId());
    }
    this->listBox->updateContent();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DashboardMenu" template="../../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel"
                 constructorParams="Workspace *parentWorkspace" variableInitialisers="workspace(parentWorkspace)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="450" initialHeight="500">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="ae05579f2fbb226b" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
