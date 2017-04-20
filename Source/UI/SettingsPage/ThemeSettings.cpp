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
#include "Config.h"
#include "HelioTheme.h"
#include "SerializationKeys.h"
#include "ProjectTreeItem.h"
#include "BinaryData.h"
#include "Workspace.h"
#include "ThemeSettingsItem.h"
#include "ColourScheme.h"
#include "ColourSchemeManager.h"

#define THEME_SETTINGS_ROW_HEIGHT (46)
//[/MiscUserDefs]

ThemeSettings::ThemeSettings()
{
    addAndMakeVisible (themesList = new ListBox());


    //[UserPreSize]
    this->themesList->setModel(this);
    this->themesList->setRowHeight(THEME_SETTINGS_ROW_HEIGHT);
    this->themesList->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->themesList->getViewport()->setScrollBarsShown(true, false);
    //[/UserPreSize]

    setSize (600, 192);

    //[Constructor]
    const int numThemes = ColourSchemeManager::getInstance().getSchemes().size();
    this->setSize(600, 4 + numThemes * THEME_SETTINGS_ROW_HEIGHT);

    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(true);
    this->setOpaque(true);

    ColourSchemeManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

ThemeSettings::~ThemeSettings()
{
    //[Destructor_pre]
    ColourSchemeManager::getInstance().removeChangeListener(this);
    //[/Destructor_pre]

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

    themesList->setBounds (0, 2, getWidth() - 4, getHeight() - 4);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//

void ThemeSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->themesList->updateContent();
    //this->scrollToSelectedLocale();
}

XmlElement *ThemeSettings::serialize() const
{
    return nullptr;
}

void ThemeSettings::deserialize(const XmlElement &xml)
{

}

void ThemeSettings::reset()
{

}


//===----------------------------------------------------------------------===//
// ListBoxModel
//

Component *ThemeSettings::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    const auto currentTheme = ColourSchemeManager::getInstance().getCurrentScheme();
    const auto &themes = ColourSchemeManager::getInstance().getSchemes();

    if (rowNumber >= themes.size()) { return existingComponentToUpdate; }

    const bool isCurrentScheme = (currentTheme == themes[rowNumber]);
    const bool isLastRow = (rowNumber == themes.size() - 1);

    if (existingComponentToUpdate != nullptr)
    {
        if (ThemeSettingsItem *row = dynamic_cast<ThemeSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isCurrentScheme, themes[rowNumber]);
        }
    }
    else
    {
        auto row = new ThemeSettingsItem(*this->themesList);
        row->updateDescription(isLastRow, isCurrentScheme, themes[rowNumber]);
        return row;
    }

    return existingComponentToUpdate;
}

int ThemeSettings::getNumRows()
{
    const auto &themes = ColourSchemeManager::getInstance().getSchemes();
    return themes.size();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ThemeSettings" template="../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="192">
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="themesList" virtualName=""
                    explicitFocusOrder="0" pos="0 2 4M 4M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
