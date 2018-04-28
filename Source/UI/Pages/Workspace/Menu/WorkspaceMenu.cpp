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

#include "WorkspaceMenu.h"

//[MiscUserDefs]
#include "RecentProjectRow.h"
#include "SignInRow.h"
#include "CreateProjectRow.h"
#include "OpenProjectRow.h"
#include "App.h"
#include "MainLayout.h"
#include "SessionService.h"
#include "AuthorizationDialog.h"
#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "App.h"
#include "CommandIDs.h"
#include "ComponentIDs.h"

#if HELIO_DESKTOP
#   define HAS_OPEN_PROJECT_ROW 1
#else
#   define HAS_OPEN_PROJECT_ROW 0
#endif

//[/MiscUserDefs]

WorkspaceMenu::WorkspaceMenu(Workspace *parentWorkspace)
    : workspace(parentWorkspace)
{
    addAndMakeVisible (component = new ShadowHorizontalFading());
    addAndMakeVisible (listBox = new ListBox());

    addAndMakeVisible (separator1 = new SeparatorHorizontalFadingReversed());
    addAndMakeVisible (separator2 = new SeparatorHorizontalFading());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (450, 500);

    //[Constructor]
    this->listBox->setRowHeight(DEFAULT_RECENT_FILES_ROW_HEIGHT);
    this->listBox->getViewport()->setScrollBarsShown(false, false);

    this->listBox->setModel(this);
    this->setFocusContainer(false);

    // FIXME update on workspace's recent files list changes
    //[/Constructor]
}

WorkspaceMenu::~WorkspaceMenu()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;
    listBox = nullptr;
    separator1 = nullptr;
    separator2 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void WorkspaceMenu::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void WorkspaceMenu::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (22, 2, getWidth() - 44, getHeight() - 4);
    listBox->setBounds (48, 2, getWidth() - 96, getHeight() - 5);
    separator1->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 3);
    separator2->setBounds (0, getHeight() - 3, getWidth() - 2, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void WorkspaceMenu::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::CreateNewProject)
    {
        this->workspace->createEmptyProject();
        this->listBox->updateContent();
    }
    else if (commandId == CommandIDs::OpenProject)
    {
        this->workspace->importProject("*.helio;*.hp");
        this->listBox->updateContent();
    }
    else if (commandId == CommandIDs::LoginLogout)
    {
        const bool isLoggedIn = SessionService::isLoggedIn();

        if (!isLoggedIn)
        {
            this->listBox->updateContent();
            App::Layout().showModalComponentUnowned(new AuthorizationDialog());
        }
        else
        {
            App::Helio().getSessionService()->signOut();
            this->listBox->updateContent();
        }
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void WorkspaceMenu::loadFile(RecentFileDescription::Ptr fileDescription)
{
    if (!this->workspace->onClickedLoadRecentFile(fileDescription))
    {
        this->workspace->getRecentFilesList().removeById(fileDescription->projectId);
    }

    this->listBox->updateContent();

#if HAS_OPEN_PROJECT_ROW
    this->listBox->scrollToEnsureRowIsOnscreen(3);
#else
    this->listBox->scrollToEnsureRowIsOnscreen(2);
#endif
}

void WorkspaceMenu::unloadFile(RecentFileDescription::Ptr fileDescription)
{
    this->workspace->onClickedUnloadRecentFile(fileDescription);
    this->listBox->updateContent();
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *WorkspaceMenu::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
#if HAS_OPEN_PROJECT_ROW
    const int signInRowIndex = 0;
    const int openProjectRowIndex = 1;
    const int createProjectRowIndex = 2;
    const int numStaticCells = 3;
#else
    const int signInRowIndex = 0;
    const int createProjectRowIndex = 1;
    const int numStaticCells = 2;
#endif

    if (rowNumber == signInRowIndex)
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (nullptr == dynamic_cast<SignInRow *>(existingComponentToUpdate))
            {
                delete existingComponentToUpdate;
                existingComponentToUpdate = new SignInRow(*this, *this->listBox);
                return existingComponentToUpdate;
            }
            else if (SignInRow *row = dynamic_cast<SignInRow *>(existingComponentToUpdate))
            {
                row->updateContent();
            }
        }
        else
        {
            auto row = new SignInRow(*this, *this->listBox);
            return row;
        }
    }

#if HAS_OPEN_PROJECT_ROW
    if (rowNumber == openProjectRowIndex)
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (nullptr == dynamic_cast<OpenProjectRow *>(existingComponentToUpdate))
            {
                delete existingComponentToUpdate;
                existingComponentToUpdate = new OpenProjectRow(*this, *this->listBox);
                return existingComponentToUpdate;
            }
        }
        else
        {
            auto row = new OpenProjectRow(*this, *this->listBox);
            return row;
        }
    }
#endif

    if (rowNumber == createProjectRowIndex)
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (nullptr == dynamic_cast<CreateProjectRow *>(existingComponentToUpdate))
            {
                delete existingComponentToUpdate;
                existingComponentToUpdate = new CreateProjectRow(*this, *this->listBox);
                return existingComponentToUpdate;
            }
        }
        else
        {
            auto row = new CreateProjectRow(*this, *this->listBox);
            return row;
        }
    }

    const int numFiles = this->workspace->getRecentFilesList().getNumItems();
    const bool isLastRow = (rowNumber == (numFiles + numStaticCells - 1));

    const int fileIndex = rowNumber - numStaticCells;
    const RecentFileDescription::Ptr item = this->workspace->getRecentFilesList().getItem(fileIndex);
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

void WorkspaceMenu::listBoxItemClicked(int row, const MouseEvent &e)
{
}

int WorkspaceMenu::getNumRows()
{
    const int signInRow = 1;
    const int createProjectRow = 1;
#if HAS_OPEN_PROJECT_ROW
    const int openProjectRow = 1;
#else
    const int openProjectRow = 0;
#endif

    return this->workspace->getRecentFilesList().getNumItems() + signInRow + openProjectRow + createProjectRow;
}

void WorkspaceMenu::paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="WorkspaceMenu" template="../../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel"
                 constructorParams="Workspace *parentWorkspace" variableInitialisers="workspace(parentWorkspace)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="450" initialHeight="500">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="c3e4abefb218ce57" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="22 2 44M 4M" sourceFile="../../../Themes/ShadowHorizontalFading.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="ae05579f2fbb226b" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="48 2 96M 5M" class="ListBox" params=""/>
  <JUCERCOMP name="" id="a09914d60dab2768" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc 0 0M 3" sourceFile="../../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="5b285323b956eb4e" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="0 0Rr 2M 3" sourceFile="../../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
