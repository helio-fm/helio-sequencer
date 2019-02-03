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

#include "InitScreen.h"

//[MiscUserDefs]
#include "LogoFader.h"
#include "HelioTheme.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "CommandIDs.h"
#include "ColourIDs.h"
//[/MiscUserDefs]

InitScreen::InitScreen()
{
    this->headLine.reset(new SeparatorHorizontalReversed());
    this->addAndMakeVisible(headLine.get());
    this->headShadow.reset(new ShadowDownwards(Light));
    this->addAndMakeVisible(headShadow.get());
    this->gradient1.reset(new PanelBackgroundA());
    this->addAndMakeVisible(gradient1.get());

    //[UserPreSize]
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);
    this->setOpaque(true);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    // Debug png logos generator
    //const auto fader = new LogoFader(false);
    //this->addAndMakeVisible(fader);
    //fader->setBounds(0, 0, 400, 400);
    //[/Constructor]
}

InitScreen::~InitScreen()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().fadeOut(this, SHORT_FADE_TIME);
    //[/Destructor_pre]

    headLine = nullptr;
    headShadow = nullptr;
    gradient1 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void InitScreen::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour = Colour (0xff48358c);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::BackgroundA::fill);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    HelioTheme::drawNoiseWithin(this->getLocalBounds().toFloat(), this, g, 1.5f);
    //[/UserPaint]
}

void InitScreen::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    headLine->setBounds(0, 32, getWidth() - 0, 2);
    headShadow->setBounds(0, 33, getWidth() - 0, 6);
    gradient1->setBounds(-50, 0, getWidth() - -100, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void InitScreen::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isVisible())
    {
        // Debug png logos generator
        //this->startTimer(5000);
        this->postCommandMessage(CommandIDs::InitWorkspace);
    }
    //[/UserCode_visibilityChanged]
}

void InitScreen::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::InitWorkspace)
    {
        if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            ht->updateBackgroundRenders();
        }

        App::Workspace().init();
        App::Layout().show();
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void InitScreen::timerCallback()
{
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    if (! isShiftPressed)
    {
        this->stopTimer();
        this->postCommandMessage(CommandIDs::InitWorkspace);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InitScreen" template="../../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="visibilityChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: ff48358c" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 32 0M 2" sourceFile="../../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 33 0M 6" sourceFile="../../Themes/ShadowDownwards.cpp"
             constructorParams="Light"/>
  <JUCERCOMP name="" id="f09d886c97d1c017" memberName="gradient1" virtualName=""
             explicitFocusOrder="0" pos="-50 0 -100M 32" sourceFile="../../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
