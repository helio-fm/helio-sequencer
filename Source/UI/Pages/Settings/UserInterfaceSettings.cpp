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
//[/MiscUserDefs]

UserInterfaceSettings::UserInterfaceSettings()
{
    this->fontComboPrimer.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(fontComboPrimer.get());

    this->openGLRendererButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(openGLRendererButton.get());
    openGLRendererButton->setRadioGroupId(1);
    openGLRendererButton->addListener(this);

    this->defaultRendererButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(defaultRendererButton.get());
    defaultRendererButton->setRadioGroupId(1);
    defaultRendererButton->addListener(this);
    defaultRendererButton->setToggleState (true, dontSendNotification);

    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());
    this->fontEditor.reset(new TextEditor(String()));
    this->addAndMakeVisible(fontEditor.get());
    fontEditor->setMultiLine (false);
    fontEditor->setReturnKeyStartsNewLine (false);
    fontEditor->setReadOnly (true);
    fontEditor->setScrollbarsShown (false);
    fontEditor->setCaretVisible (false);
    fontEditor->setPopupMenuEnabled (false);
    fontEditor->setText (String());

    this->separator2.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator2.get());
    this->nativeTitleBarButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(nativeTitleBarButton.get());
    nativeTitleBarButton->addListener(this);


    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->nativeTitleBarButton->setButtonText(TRANS(I18n::Settings::nativeTitleBar));
    this->nativeTitleBarButton->setToggleState(App::isUsingNativeTitleBar(), dontSendNotification);

    this->openGLRendererButton->setButtonText(TRANS(I18n::Settings::rendererOpengl));
    this->defaultRendererButton->setButtonText(TRANS(I18n::Settings::rendererDefault));

#if JUCE_MAC
    this->nativeTitleBarButton->setEnabled(false);
    this->defaultRendererButton->setButtonText(TRANS(I18n::Settings::rendererCoreGraphics));
#elif JUCE_WINDOWS
    this->defaultRendererButton->setButtonText(TRANS(I18n::Settings::rendererDirect2d));
#elif JUCE_LINUX
    this->defaultRendererButton->setButtonText(TRANS(I18n::Settings::rendererNative));
#endif
    //[/UserPreSize]

    this->setSize(600, 200);

    //[Constructor]

    // fixme defer this menu initialization

    const auto lastUsedFontName = App::Config().getProperty(Serialization::Config::lastUsedFont);

    const auto fontsMenuProvider = [this, lastUsedFontName]()
    {
        MenuPanel::Menu fontsMenu;

        this->systemFonts.clearQuick();
        Font::findFonts(this->systemFonts);

        for (int i = 0; i < this->systemFonts.size(); ++i)
        {
            const auto &typefaceName = this->systemFonts.getReference(i).getTypefaceName();
            const bool isSelected = typefaceName == lastUsedFontName;
            fontsMenu.add(MenuItem::item(isSelected ? Icons::apply : Icons::empty,
                CommandIDs::SelectFont + i, typefaceName));
        }

        return fontsMenu;
    };

    this->fontEditor->setInterceptsMouseClicks(false, true);
    this->fontEditor->setFont(18.f);
    this->fontEditor->setText(TRANS(I18n::Settings::uiFont) + ": " + lastUsedFontName);
    this->fontComboPrimer->initWith(this->fontEditor.get(), fontsMenuProvider);
    //[/Constructor]
}

UserInterfaceSettings::~UserInterfaceSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    fontComboPrimer = nullptr;
    openGLRendererButton = nullptr;
    defaultRendererButton = nullptr;
    separator = nullptr;
    fontEditor = nullptr;
    separator2 = nullptr;
    nativeTitleBarButton = nullptr;

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
    separator->setBounds(16, 16 + 32 - -16, getWidth() - 32, 4);
    fontEditor->setBounds(16, 16, getWidth() - 33, 32);
    separator2->setBounds(16, 16 + 32 - -100, getWidth() - 32, 4);
    nativeTitleBarButton->setBounds(16, (16 + 32 - -100) + 6, getWidth() - 32, 32);
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
                App::Config().getUiFlags()->setOpenGlRendererEnabled(true);
                this->updateButtons();
            };

            dialog->onCancel = [this]()
            {
                this->updateButtons();
            };

            App::showModalComponent(move(dialog));
        }
        //[/UserButtonCode_openGLRendererButton]
    }
    else if (buttonThatWasClicked == defaultRendererButton.get())
    {
        //[UserButtonCode_defaultRendererButton] -- add your button handler code here..
        App::Config().getUiFlags()->setOpenGlRendererEnabled(false);
        this->updateButtons();
        //[/UserButtonCode_defaultRendererButton]
    }
    else if (buttonThatWasClicked == nativeTitleBarButton.get())
    {
        //[UserButtonCode_nativeTitleBarButton] -- add your button handler code here..
        // Will reload the layout:
        App::Config().getUiFlags()->setNativeTitleBarEnabled(this->nativeTitleBarButton->getToggleState());
        //[/UserButtonCode_nativeTitleBarButton]
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
        auto &theme = static_cast<HelioTheme &>(LookAndFeel::getDefaultLookAndFeel());
        theme.updateFont(this->systemFonts[fontIndex]);
        SafePointer<Component> window = this->getTopLevelComponent();
        App::recreateLayout();
        if (window != nullptr)
        {
            window->resized();
            window->repaint();
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
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="200">
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
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="1"
                state="0"/>
  <TOGGLEBUTTON name="" id="1f025eebf3951095" memberName="defaultRendererButton"
                virtualName="" explicitFocusOrder="0" pos="16 6 32M 32" posRelativeY="68d81e5e36696154"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="1"
                state="1"/>
  <JUCERCOMP name="" id="68d81e5e36696154" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="16 -16R 32M 4" posRelativeY="4fd07309a20b15b6"
             sourceFile="../../Themes/SeparatorHorizontal.cpp" constructorParams=""/>
  <TEXTEDITOR name="" id="4fd07309a20b15b6" memberName="fontEditor" virtualName=""
              explicitFocusOrder="0" pos="16 16 33M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <JUCERCOMP name="" id="677f8d2bd5f611b7" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="16 -100R 32M 4" posRelativeY="4fd07309a20b15b6"
             sourceFile="../../Themes/SeparatorHorizontal.cpp" constructorParams=""/>
  <TOGGLEBUTTON name="" id="de9d85da72f61a3d" memberName="nativeTitleBarButton"
                virtualName="" explicitFocusOrder="0" pos="16 6 32M 32" posRelativeY="677f8d2bd5f611b7"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



