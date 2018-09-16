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
#include "App.h"
#include "MainLayout.h"
#include "SessionService.h"
#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "App.h"
#include "CommandIDs.h"
#include "ComponentIDs.h"
//[/MiscUserDefs]

DashboardMenu::DashboardMenu(Workspace *parentWorkspace)
    : workspace(parentWorkspace)
{
    this->listBox.reset(new ListBox());
    this->addAndMakeVisible(listBox.get());


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(450, 500);

    //[Constructor]
    this->listBox->setRowHeight(DEFAULT_RECENT_FILES_ROW_HEIGHT);
    this->listBox->getViewport()->setScrollBarsShown(false, false);

    this->listBox->setModel(this);
    this->setFocusContainer(false);

    // FIXME update on workspace's recent files list changes
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

void DashboardMenu::loadFile(RecentFileDescription::Ptr fileDescription)
{
    if (!this->workspace->onClickedLoadRecentFile(fileDescription))
    {
        this->workspace->getProjectsList().removeById(fileDescription->projectId);
    }

    this->listBox->updateContent();
}

void DashboardMenu::unloadFile(RecentFileDescription::Ptr fileDescription)
{
    this->workspace->onClickedUnloadRecentFile(fileDescription);
    this->listBox->updateContent();
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *DashboardMenu::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    const int numFiles = this->workspace->getProjectsList().getNumItems();
    const bool isLastRow = (rowNumber == (numFiles - 1));

    const int fileIndex = rowNumber;
    const RecentFileDescription::Ptr item = this->workspace->getProjectsList().getItem(fileIndex);
    //Logger::writeToLog(String(fileIndex));

    if (item == nullptr) { return existingComponentToUpdate; }

    if (existingComponentToUpdate != nullptr)
    {
        if (RecentProjectRow *row = dynamic_cast<RecentProjectRow *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, item);
            row->setSelected(isRowSelected);
        }
        else
        {
            delete existingComponentToUpdate;
            auto row2 = new RecentProjectRow(*this, *this->listBox);
            row2->updateDescription(isLastRow, item);
            row2->setSelected(isRowSelected);
            existingComponentToUpdate = row2;
            return row2;
        }
    }
    else
    {
        auto row = new RecentProjectRow(*this, *this->listBox);
        row->updateDescription(isLastRow, item);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

void DashboardMenu::listBoxItemClicked(int row, const MouseEvent &e) {}
void DashboardMenu::paintListBoxItem(int, Graphics &, int, int, bool) {}

int DashboardMenu::getNumRows()
{
    return this->workspace->getProjectsList().getNumItems();
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
