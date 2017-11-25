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

#include "NavigationSidebar.h"

//[MiscUserDefs]

#include "SizeSwitcherComponent.h"
#include "MainLayout.h"
#include "TreeItem.h"
#include "IconComponent.h"
#include "Icons.h"
#include "SerializationKeys.h"
#include "GenericAudioMonitorComponent.h"
#include "WaveformAudioMonitorComponent.h"
#include "SpectrogramAudioMonitorComponent.h"
#include "ModeIndicatorComponent.h"
#include "App.h"
#include "AudioCore.h"
#include "HelioTheme.h"
#include "RolloverHeaderLeft.h"
#include "RolloverHeaderRight.h"
#include "RolloverContainer.h"

#define AUDIO_MONITOR_HEIGHT 126

//[/MiscUserDefs]

NavigationSidebar::NavigationSidebar()
{
    addAndMakeVisible (background = new PanelBackgroundC());
    addAndMakeVisible (shadow = new LighterShadowUpwards());
    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (gradient1 = new GradientVerticalReversed());
    addAndMakeVisible (separator = new SeparatorHorizontal());
    addAndMakeVisible (modeIndicatorSelector = new ModeIndicatorTrigger());

    addAndMakeVisible (modeIndicator = new ModeIndicatorComponent (3));


    //[UserPreSize]
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->genericMonitor = new GenericAudioMonitorComponent(nullptr);
    this->waveformMonitor = new WaveformAudioMonitorComponent(nullptr);
    this->spectrogramMonitor = new SpectrogramAudioMonitorComponent(nullptr);

    this->addChildComponent(this->genericMonitor);
    this->addChildComponent(this->waveformMonitor);
    this->addChildComponent(this->spectrogramMonitor);

    this->genericMonitor->setVisible(true);
    //[/UserPreSize]

    setSize (72, 640);

    //[Constructor]
    //[/Constructor]
}

NavigationSidebar::~NavigationSidebar()
{
    //[Destructor_pre]
    this->spectrogramMonitor = nullptr;
    this->waveformMonitor = nullptr;
    this->genericMonitor = nullptr;

    //tree->setRootItem(nullptr);
    //[/Destructor_pre]

    background = nullptr;
    shadow = nullptr;
    headLine = nullptr;
    headShadow = nullptr;
    gradient1 = nullptr;
    separator = nullptr;
    modeIndicatorSelector = nullptr;
    modeIndicator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NavigationSidebar::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NavigationSidebar::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    this->genericMonitor->setSize(this->getWidth(), AUDIO_MONITOR_HEIGHT);
    this->waveformMonitor->setSize(this->getWidth(), AUDIO_MONITOR_HEIGHT);
    this->spectrogramMonitor->setSize(this->getWidth(), AUDIO_MONITOR_HEIGHT);

    this->genericMonitor->setTopLeftPosition(0, this->getHeight() - AUDIO_MONITOR_HEIGHT);
    this->waveformMonitor->setTopLeftPosition(0, this->getHeight() - AUDIO_MONITOR_HEIGHT);
    this->spectrogramMonitor->setTopLeftPosition(0, this->getHeight() - AUDIO_MONITOR_HEIGHT);
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    shadow->setBounds (0, getHeight() - 127 - 6, getWidth() - 0, 6);
    headLine->setBounds (0, 47, getWidth() - 0, 2);
    headShadow->setBounds (0, 48, getWidth() - 0, 6);
    gradient1->setBounds (-50, 0, getWidth() - -100, 47);
    separator->setBounds (0, getHeight() - 126 - 2, getWidth() - 0, 2);
    modeIndicatorSelector->setBounds (0, getHeight() - 126, getWidth() - 0, 126);
    modeIndicator->setBounds (0, getHeight() - 4 - 5, getWidth() - 0, 5);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void NavigationSidebar::paintOverChildren(Graphics& g)
{
    g.setColour(findColour(HelioTheme::resizerLineColourId));
    g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
}

void NavigationSidebar::setAudioMonitor(AudioMonitor *audioMonitor)
{
    this->spectrogramMonitor->setTargetAnalyzer(audioMonitor);
    this->waveformMonitor->setTargetAnalyzer(audioMonitor);
    this->genericMonitor->setTargetAnalyzer(audioMonitor);
}

void NavigationSidebar::handleChangeMode()
{
    switch (this->modeIndicator->scrollToNextMode())
    {
    case 0:
        this->switchMonitorsAnimated(this->waveformMonitor, this->genericMonitor);
        break;
    case 1:
        this->switchMonitorsAnimated(this->genericMonitor, this->spectrogramMonitor);
        break;
    case 2:
        this->switchMonitorsAnimated(this->spectrogramMonitor, this->waveformMonitor);
        break;

    default:
        break;
    }
}

void NavigationSidebar::switchMonitorsAnimated(Component *oldOne, Component *newOne)
{
    const int w = this->getWidth();
    const int y = this->getHeight() - AUDIO_MONITOR_HEIGHT;
    this->animator.animateComponent(oldOne, oldOne->getBounds().translated(-w, 0), 0.f, 200, true, 0.f, 1.f);
    newOne->setAlpha(0.f);
    newOne->setVisible(true);
    newOne->setTopLeftPosition(w, y);
    this->animator.animateComponent(newOne, newOne->getBounds().translated(-w, 0), 1.f, 200, false, 1.f, 0.f);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NavigationSidebar" template="../../Template"
                 componentName="" parentClasses="public ModeIndicatorOwnerComponent"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="72"
                 initialHeight="640">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 127Rr 0M 6" sourceFile="../Themes/LighterShadowUpwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 47 0M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 6" sourceFile="../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f09d886c97d1c017" memberName="gradient1" virtualName=""
             explicitFocusOrder="0" pos="-50 0 -100M 47" sourceFile="../Themes/GradientVerticalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 126Rr 0M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="9e1622013601218a" memberName="modeIndicatorSelector"
                    virtualName="" explicitFocusOrder="0" pos="0 0Rr 0M 126" class="ModeIndicatorTrigger"
                    params=""/>
  <GENERICCOMPONENT name="" id="4b6240e11495d88b" memberName="modeIndicator" virtualName=""
                    explicitFocusOrder="0" pos="0 4Rr 0M 5" class="ModeIndicatorComponent"
                    params="3"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_NavigationSidebar_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,19,
1,0,154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,101,7,0,
0,0,12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* NavigationSidebar::gray1x1_png = (const char*) resource_NavigationSidebar_gray1x1_png;
const int NavigationSidebar::gray1x1_pngSize = 150;
