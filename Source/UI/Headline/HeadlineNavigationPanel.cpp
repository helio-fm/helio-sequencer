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

#include "HeadlineNavigationPanel.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "PanelBackgroundB.h"
#include "HeadlineDropdown.h"
#include "HelioCallout.h"
#include "Workspace.h"
#include "App.h"
//[/MiscUserDefs]

HeadlineNavigationPanel::HeadlineNavigationPanel()
{
    addAndMakeVisible (navigatePrevious = new IconButton (Icons::findByName(Icons::left, 20), CommandIDs::ShowPreviousPage));

    addAndMakeVisible (navigateNext = new IconButton (Icons::findByName(Icons::right, 20), CommandIDs::ShowNextPage));


    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (80, 32);

    //[Constructor]
    //[/Constructor]
}

HeadlineNavigationPanel::~HeadlineNavigationPanel()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    navigatePrevious = nullptr;
    navigateNext = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineNavigationPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x0dffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = this->findColour(PanelBackgroundB::panelFillStartId).brighter(0.03f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x77000000), strokeColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 9) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 16) - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath2, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x55ffffff), strokeColour2 = Colour (0x00ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 10) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 17) - 0.0f + x,
                                       5.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath3, PathStrokeType (0.500f), AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineNavigationPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    navigatePrevious->setBounds (5, (getHeight() / 2) + -1 - (32 / 2), 28, 32);
    navigateNext->setBounds (32, (getHeight() / 2) + -1 - (32 / 2), 28, 32);
    internalPath1.clear();
    internalPath1.startNewSubPath (0.0f, 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 9), 16.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 16), 32.0f);
    internalPath1.lineTo (0.0f, 32.0f);
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (static_cast<float> (getWidth() - 32), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 9), 16.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 16), 32.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 32), 32.0f);
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (static_cast<float> (getWidth() - 32), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 17), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 10), 16.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 17), 32.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 32), 32.0f);
    internalPath3.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HeadlineNavigationPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
    case CommandIDs::ShowPreviousPage:
        App::Workspace().navigateBackwardIfPossible();
        break;
    case CommandIDs::ShowNextPage:
        App::Workspace().navigateForwardIfPossible();
        break;
    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void HeadlineNavigationPanel::updateState(bool canGoPrevious, bool canGoNext)
{
    this->navigatePrevious->setInterceptsMouseClicks(canGoPrevious, false);
    this->navigatePrevious->setAlpha(canGoPrevious ? 0.45f : 0.2f);
    this->navigateNext->setInterceptsMouseClicks(canGoNext, false);
    this->navigateNext->setAlpha(canGoNext ? 0.45f : 0.2f);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineNavigationPanel"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="80"
                 initialHeight="32">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: dffffff" hasStroke="0" nonZeroWinding="1">s 0 0 l 16R 0 l 9R 16 l 16R 32 l 0 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 9R 16, 16R 2, 0=77000000, 1=0" nonZeroWinding="1">s 32R 0 l 16R 0 l 9R 16 l 16R 32 l 32R 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 10R 16, 17R 5, 0=55ffffff, 1=ffffff" nonZeroWinding="1">s 32R 0 l 17R 0 l 10R 16 l 17R 32 l 32R 32 x</PATH>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="88e1e92c7548ba42" memberName="navigatePrevious" virtualName=""
                    explicitFocusOrder="0" pos="5 -1Cc 28 32" class="IconButton"
                    params="Icons::findByName(Icons::left, 20), CommandIDs::ShowPreviousPage"/>
  <GENERICCOMPONENT name="" id="900658e63c264259" memberName="navigateNext" virtualName=""
                    explicitFocusOrder="0" pos="32 -1Cc 28 32" class="IconButton"
                    params="Icons::findByName(Icons::right, 20), CommandIDs::ShowNextPage"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
