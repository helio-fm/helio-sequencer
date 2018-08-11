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

#include "ThemeSettings.h"

//[MiscUserDefs]
#include "App.h"
#include "SerializationKeys.h"
#include "ProjectTreeItem.h"
#include "BinaryData.h"
#include "ThemeSettingsItem.h"
#include "MenuPanel.h"
#include "ColourScheme.h"
#include "ColourSchemesManager.h"
#include "ModalDialogConfirmation.h"

#include "App.h"
#include "Config.h"
#include "HelioTheme.h"
#include "Workspace.h"
#include "MainWindow.h"
#include "MainLayout.h"

#define THEME_SETTINGS_ROW_HEIGHT (46)
//[/MiscUserDefs]

ThemeSettings::ThemeSettings()
{
    this->fontComboPrimer.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(fontComboPrimer.get());

    this->themesList.reset(new ListBox());
    this->addAndMakeVisible(themesList.get());


    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->currentScheme = ColourSchemesManager::getInstance().getCurrentScheme();

    this->themesList->setModel(this);
    this->themesList->setRowHeight(THEME_SETTINGS_ROW_HEIGHT);
    this->themesList->getViewport()->setScrollBarsShown(true, false);
    //[/UserPreSize]

    this->setSize(600, 350);

    //[Constructor]
    const int numSchemes = ColourSchemesManager::getInstance().getSchemes().size();
    this->setSize(600, 16 + numSchemes * THEME_SETTINGS_ROW_HEIGHT);
    ColourSchemesManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

ThemeSettings::~ThemeSettings()
{
    //[Destructor_pre]
    ColourSchemesManager::getInstance().removeChangeListener(this);
    //[/Destructor_pre]

    fontComboPrimer = nullptr;
    themesList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ThemeSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ThemeSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fontComboPrimer->setBounds(4, 4, getWidth() - 8, getHeight() - 8);
    themesList->setBounds(8, 8, getWidth() - 24, getHeight() - 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ThemeSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->themesList->updateContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *ThemeSettings::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    const auto &schemes = ColourSchemesManager::getInstance().getSchemes();

    if (rowNumber >= schemes.size()) { return existingComponentToUpdate; }

    const bool isCurrentScheme = (this->currentScheme->getResourceId() == schemes[rowNumber]->getResourceId());
    const bool isLastRow = (rowNumber == schemes.size() - 1);

    if (existingComponentToUpdate != nullptr)
    {
        if (ThemeSettingsItem *row = dynamic_cast<ThemeSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isCurrentScheme, schemes[rowNumber]);
        }
    }
    else
    {
        auto row = new ThemeSettingsItem(*this->themesList);
        row->updateDescription(isLastRow, isCurrentScheme, schemes[rowNumber]);
        return row;
    }

    return existingComponentToUpdate;
}

int ThemeSettings::getNumRows()
{
    const auto &themes = ColourSchemesManager::getInstance().getSchemes();
    return themes.size();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ThemeSettings" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="350">
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="1b5648cb76a38566" memberName="fontComboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="themesList" virtualName=""
                    explicitFocusOrder="0" pos="8 8 24M 16M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
