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
#include "SerializationKeys.h"
#include "ProjectNode.h"
#include "BinaryData.h"
#include "ThemeSettingsItem.h"
#include "MenuPanel.h"
#include "ModalDialogConfirmation.h"
#include "Config.h"
#include "HelioTheme.h"
#include "Workspace.h"
#include "MainLayout.h"

#define THEME_SETTINGS_ROW_HEIGHT (46)
//[/MiscUserDefs]

UserInterfaceSettings::UserInterfaceSettings()
{
    this->fontComboPrimer.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(fontComboPrimer.get());

    this->openGLRendererButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(openGLRendererButton.get());
    openGLRendererButton->setButtonText(TRANS("settings::renderer::opengl"));
    openGLRendererButton->setRadioGroupId(1);
    openGLRendererButton->addListener(this);

    this->defaultRendererButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(defaultRendererButton.get());
    defaultRendererButton->setButtonText(TRANS("settings::renderer::default"));
    defaultRendererButton->setRadioGroupId(1);
    defaultRendererButton->addListener(this);
    defaultRendererButton->setToggleState (true, dontSendNotification);

    this->separator2.reset(new SeparatorHorizontalFading());
    this->addAndMakeVisible(separator2.get());
    this->fontEditor.reset(new TextEditor(String()));
    this->addAndMakeVisible(fontEditor.get());
    fontEditor->setMultiLine (false);
    fontEditor->setReturnKeyStartsNewLine (false);
    fontEditor->setReadOnly (true);
    fontEditor->setScrollbarsShown (false);
    fontEditor->setCaretVisible (false);
    fontEditor->setPopupMenuEnabled (false);
    fontEditor->setText (String());


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
    //[/UserPreSize]

    this->setSize(600, 146);

    //[Constructor]
    MenuPanel::Menu fontsMenu;
    Font::findFonts(this->systemFonts);
    const String lastUsedFontName = App::Config().getProperty(Serialization::Config::lastUsedFont);

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
    //[/Constructor]
}

UserInterfaceSettings::~UserInterfaceSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    fontComboPrimer = nullptr;
    openGLRendererButton = nullptr;
    defaultRendererButton = nullptr;
    separator2 = nullptr;
    fontEditor = nullptr;

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

    fontComboPrimer->setBounds(4, 4, getWidth() - 8, getHeight() - 8);
    openGLRendererButton->setBounds(16, ((16 + 32 - -16) + 6) + 32, getWidth() - 32, 32);
    defaultRendererButton->setBounds(16, (16 + 32 - -16) + 6, getWidth() - 32, 32);
    separator2->setBounds(16, 16 + 32 - -16, getWidth() - 32, 4);
    fontEditor->setBounds(16, 16, getWidth() - 33, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UserInterfaceSettings::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == openGLRendererButton.get())
    {
        //[UserButtonCode_openGLRendererButton] -- add your button handler code here..
        if (this->openGLRendererButton->getToggleState())
        {
            this->openGLRendererButton->setToggleState(false, dontSendNotification);

            auto dialog = ModalDialogConfirmation::Presets::confirmOpenGL();

            dialog->onOk = [this]()
            {
                App::setOpenGLRendererEnabled(true);
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
    else if (buttonThatWasClicked == defaultRendererButton.get())
    {
        //[UserButtonCode_defaultRendererButton] -- add your button handler code here..
        App::setOpenGLRendererEnabled(false);
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
        if (auto *theme = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            theme->updateFont(this->systemFonts[fontIndex]);
            SafePointer<Component> window = this->getTopLevelComponent();
            App::recreateLayout();
            if (window != nullptr)
            {
                window->resized();
                window->repaint();
            }
        }
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void UserInterfaceSettings::updateButtons()
{
    const bool openGLEnabled = App::isOpenGLRendererEnabled();
    this->defaultRendererButton->setToggleState(!openGLEnabled, dontSendNotification);
    this->openGLRendererButton->setToggleState(openGLEnabled, dontSendNotification);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserInterfaceSettings" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="146">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="40404"/>
  <GENERICCOMPONENT name="" id="1b5648cb76a38566" memberName="fontComboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="4 4 8M 8M" class="MobileComboBox::Primer"
                    params=""/>
  <TOGGLEBUTTON name="" id="42fbb3993c5b4950" memberName="openGLRendererButton"
                virtualName="" explicitFocusOrder="0" pos="16 32 32M 32" posRelativeY="1f025eebf3951095"
                buttonText="settings::renderer::opengl" connectedEdges="0" needsCallback="1"
                radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="" id="1f025eebf3951095" memberName="defaultRendererButton"
                virtualName="" explicitFocusOrder="0" pos="16 6 32M 32" posRelativeY="68d81e5e36696154"
                buttonText="settings::renderer::default" connectedEdges="0" needsCallback="1"
                radioGroupId="1" state="1"/>
  <JUCERCOMP name="" id="68d81e5e36696154" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="16 -16R 32M 4" posRelativeY="4fd07309a20b15b6"
             sourceFile="../../Themes/SeparatorHorizontalFading.cpp" constructorParams=""/>
  <TEXTEDITOR name="" id="4fd07309a20b15b6" memberName="fontEditor" virtualName=""
              explicitFocusOrder="0" pos="16 16 33M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
