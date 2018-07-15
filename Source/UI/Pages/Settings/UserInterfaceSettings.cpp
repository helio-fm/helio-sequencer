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

#include "UserInterfaceSettings.h"

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

UserInterfaceSettings::UserInterfaceSettings()
{
    addAndMakeVisible (fontComboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible (themesList = new ListBox());

    addAndMakeVisible (openGLRendererButton = new ToggleButton (String()));
    openGLRendererButton->setButtonText (TRANS("settings::renderer::opengl"));
    openGLRendererButton->setRadioGroupId (1);
    openGLRendererButton->addListener (this);
    openGLRendererButton->setColour (ToggleButton::textColourId, Colour (0xbcffffff));

    addAndMakeVisible (defaultRendererButton = new ToggleButton (String()));
    defaultRendererButton->setButtonText (TRANS("settings::renderer::default"));
    defaultRendererButton->setRadioGroupId (1);
    defaultRendererButton->addListener (this);
    defaultRendererButton->setToggleState (true, dontSendNotification);
    defaultRendererButton->setColour (ToggleButton::textColourId, Colour (0xbcffffff));

    addAndMakeVisible (separator2 = new SeparatorHorizontalFading());
    addAndMakeVisible (fontEditor = new TextEditor (String()));
    fontEditor->setMultiLine (false);
    fontEditor->setReturnKeyStartsNewLine (false);
    fontEditor->setReadOnly (true);
    fontEditor->setScrollbarsShown (false);
    fontEditor->setCaretVisible (false);
    fontEditor->setPopupMenuEnabled (false);
    fontEditor->setText (String());

    addAndMakeVisible (separator3 = new SeparatorHorizontalFading());

    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

#if JUCE_MAC
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::coregraphics"));
#elif JUCE_WINDOWS
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::direct2d"));
#elif JUCE_LINUX
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::native"));
#endif

    this->currentScheme = ColourSchemesManager::getInstance().getCurrentScheme();

    this->themesList->setModel(this);
    this->themesList->setRowHeight(THEME_SETTINGS_ROW_HEIGHT);
    this->themesList->getViewport()->setScrollBarsShown(true, false);
    //[/UserPreSize]

    setSize (600, 350);

    //[Constructor]
    const int numSchemes = ColourSchemesManager::getInstance().getSchemes().size();
    this->setSize(600, 142 + numSchemes * THEME_SETTINGS_ROW_HEIGHT);

    MenuPanel::Menu fontsMenu;
    Font::findFonts(this->systemFonts);
    const String lastUsedFontName = Config::get(Serialization::Config::lastUsedFont);

    for (int i = 0; i < this->systemFonts.size(); ++i)
    {
        const String &typeName = this->systemFonts.getReference(i).getTypeface()->getName();
        const bool isSelected = typeName == lastUsedFontName;
        fontsMenu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
            CommandIDs::SelectFont + i, typeName));
    }

    this->fontEditor->setInterceptsMouseClicks(false, true);
    this->fontEditor->setFont(18.f);
    this->fontEditor->setText(TRANS("settings::ui::font") + ": " + lastUsedFontName);
    this->fontComboPrimer->initWith(this->fontEditor.get(), fontsMenu);

    ColourSchemesManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

UserInterfaceSettings::~UserInterfaceSettings()
{
    //[Destructor_pre]
    ColourSchemesManager::getInstance().removeChangeListener(this);
    //[/Destructor_pre]

    fontComboPrimer = nullptr;
    themesList = nullptr;
    openGLRendererButton = nullptr;
    defaultRendererButton = nullptr;
    separator2 = nullptr;
    fontEditor = nullptr;
    separator3 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void UserInterfaceSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UserInterfaceSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fontComboPrimer->setBounds (4, 4, getWidth() - 8, getHeight() - 8);
    themesList->setBounds (8, 8, getWidth() - 24, getHeight() - 142);
    openGLRendererButton->setBounds (48, ((((8 + (getHeight() - 142) - -8) + 10) + 32 - -8) + 6) + 32, getWidth() - 64, 32);
    defaultRendererButton->setBounds (48, (((8 + (getHeight() - 142) - -8) + 10) + 32 - -8) + 6, getWidth() - 64, 32);
    separator2->setBounds (48, ((8 + (getHeight() - 142) - -8) + 10) + 32 - -8, getWidth() - 64, 4);
    fontEditor->setBounds (47, (8 + (getHeight() - 142) - -8) + 10, getWidth() - 64, 32);
    separator3->setBounds (48, 8 + (getHeight() - 142) - -8, getWidth() - 64, 4);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UserInterfaceSettings::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == openGLRendererButton)
    {
        //[UserButtonCode_openGLRendererButton] -- add your button handler code here..
        if (this->openGLRendererButton->getToggleState())
        {
            this->openGLRendererButton->setToggleState(false, dontSendNotification);

            auto dialog = ModalDialogConfirmation::Presets::confirmOpenGL();

            dialog->onOk = [this]()
            {
                App::Window().setOpenGLRendererEnabled(true);
                this->updateButtons();
            };

            dialog->onCancel = [this]()
            {
                this->updateButtons();
            };

            App::Layout().showModalComponentUnowned(dialog.release());
        }
        //[/UserButtonCode_openGLRendererButton]
    }
    else if (buttonThatWasClicked == defaultRendererButton)
    {
        //[UserButtonCode_defaultRendererButton] -- add your button handler code here..
        App::Window().setOpenGLRendererEnabled(false);
        this->updateButtons();
        //[/UserButtonCode_defaultRendererButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void UserInterfaceSettings::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isVisible())
    {
        this->updateButtons();
    }
    //[/UserCode_visibilityChanged]
}

void UserInterfaceSettings::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId >= CommandIDs::SelectFont &&
        commandId <= (CommandIDs::SelectFont + this->systemFonts.size()))
    {
        const int fontIndex = commandId - CommandIDs::SelectFont;
        if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            ht->updateFont(this->systemFonts[fontIndex]);
            App::Helio().recreateLayout();
            if (this->getTopLevelComponent() != nullptr)
            {
                this->getTopLevelComponent()->resized();
                this->getTopLevelComponent()->repaint();
            }
        }
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void UserInterfaceSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->themesList->updateContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *UserInterfaceSettings::refreshComponentForRow(int rowNumber, bool isRowSelected,
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

int UserInterfaceSettings::getNumRows()
{
    const auto &themes = ColourSchemesManager::getInstance().getSchemes();
    return themes.size();
}

void UserInterfaceSettings::updateButtons()
{
    const bool openGLEnabled = MainWindow::isOpenGLRendererEnabled();
    this->defaultRendererButton->setToggleState(!openGLEnabled, dontSendNotification);
    this->openGLRendererButton->setToggleState(openGLEnabled, dontSendNotification);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserInterfaceSettings" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="350">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="1b5648cb76a38566" memberName="fontComboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="themesList" virtualName=""
                    explicitFocusOrder="0" pos="8 8 24M 142M" class="ListBox" params=""/>
  <TOGGLEBUTTON name="" id="42fbb3993c5b4950" memberName="openGLRendererButton"
                virtualName="" explicitFocusOrder="0" pos="48 32 64M 32" posRelativeY="1f025eebf3951095"
                txtcol="bcffffff" buttonText="settings::renderer::opengl" connectedEdges="0"
                needsCallback="1" radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="" id="1f025eebf3951095" memberName="defaultRendererButton"
                virtualName="" explicitFocusOrder="0" pos="48 6 64M 32" posRelativeY="68d81e5e36696154"
                txtcol="bcffffff" buttonText="settings::renderer::default" connectedEdges="0"
                needsCallback="1" radioGroupId="1" state="1"/>
  <JUCERCOMP name="" id="68d81e5e36696154" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="48 -8R 64M 4" posRelativeY="4fd07309a20b15b6"
             sourceFile="../../Themes/SeparatorHorizontalFading.cpp" constructorParams=""/>
  <TEXTEDITOR name="" id="4fd07309a20b15b6" memberName="fontEditor" virtualName=""
              explicitFocusOrder="0" pos="47 10 64M 32" posRelativeY="33d31e3c1e0ee070"
              initialText="" multiline="0" retKeyStartsLine="0" readonly="1"
              scrollbars="0" caret="0" popupmenu="0"/>
  <JUCERCOMP name="" id="33d31e3c1e0ee070" memberName="separator3" virtualName=""
             explicitFocusOrder="0" pos="48 -8R 64M 4" posRelativeY="5005ba29a3a1bbc6"
             sourceFile="../../Themes/SeparatorHorizontalFading.cpp" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
