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

#include "OpenGLSettings.h"

//[MiscUserDefs]
#include "App.h"
#include "MainWindow.h"
#include "MainLayout.h"
#include "ModalDialogConfirmation.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

OpenGLSettings::OpenGLSettings()
{
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


    //[UserPreSize]
#if JUCE_MAC
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::coregraphics"));
#elif JUCE_WINDOWS
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::direct2d"));
#elif JUCE_LINUX
    this->defaultRendererButton->setButtonText(TRANS("settings::renderer::native"));
#endif
    //[/UserPreSize]

    setSize (600, 64);

    //[Constructor]
    //[/Constructor]
}

OpenGLSettings::~OpenGLSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    openGLRendererButton = nullptr;
    defaultRendererButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void OpenGLSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OpenGLSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    openGLRendererButton->setBounds (8, (getHeight() / 2) + 16 - (32 / 2), getWidth() - 16, 32);
    defaultRendererButton->setBounds (8, (getHeight() / 2) + -16 - (32 / 2), getWidth() - 16, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void OpenGLSettings::buttonClicked (Button* buttonThatWasClicked)
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

void OpenGLSettings::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isVisible())
    {
        this->updateButtons();
    }
    //[/UserCode_visibilityChanged]
}

void OpenGLSettings::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void OpenGLSettings::updateButtons()
{
    const bool openGLEnabled = MainWindow::isOpenGLRendererEnabled();
    this->defaultRendererButton->setToggleState(!openGLEnabled, dontSendNotification);
    this->openGLRendererButton->setToggleState(openGLEnabled, dontSendNotification);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OpenGLSettings" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="64">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="4d4d4d"/>
  <TOGGLEBUTTON name="" id="42fbb3993c5b4950" memberName="openGLRendererButton"
                virtualName="" explicitFocusOrder="0" pos="8 16Cc 16M 32" txtcol="bcffffff"
                buttonText="settings::renderer::opengl" connectedEdges="0" needsCallback="1"
                radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="" id="1f025eebf3951095" memberName="defaultRendererButton"
                virtualName="" explicitFocusOrder="0" pos="8 -16Cc 16M 32" txtcol="bcffffff"
                buttonText="settings::renderer::default" connectedEdges="0" needsCallback="1"
                radioGroupId="1" state="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
