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

#include "TreePanelPhone.h"

//[MiscUserDefs]

#include "SizeSwitcherComponent.h"
#include "LongTapController.h"
#include "MainLayout.h"
#include "TreeItem.h"
#include "IconComponent.h"
#include "Icons.h"
#include "SerializationKeys.h"

#include "SpectralLogo.h"
#include "SpectrumComponent.h"
#include "VolumeComponent.h"
#include "App.h"
#include "AudioCore.h"

#include "RootTreeItemPanelDefault.h"
#include "RootTreeItemPanelCompact.h"

#include "RolloverHeaderLeft.h"
#include "RolloverHeaderRight.h"
#include "RolloverContainer.h"

//[/MiscUserDefs]

TreePanelPhone::TreePanelPhone()
{
    addAndMakeVisible (background = new PanelBackgroundC());
    addAndMakeVisible (tree = new TreeView());

    addAndMakeVisible (shadow = new LighterShadowUpwards());
    addAndMakeVisible (gradient = new GradientVertical());
    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (gradient1 = new GradientVerticalReversed());
    addAndMakeVisible (spectrometer = new SpectrumComponent (nullptr));

    addAndMakeVisible (separator = new SeparatorHorizontal());
    addAndMakeVisible (rootTreeItemPanel = new Component());

    addAndMakeVisible (vuMeterLeft = new VolumeComponent (nullptr, 0, VolumeComponent::Left));

    addAndMakeVisible (vuMeterRight = new VolumeComponent (nullptr, 1, VolumeComponent::Right));


    //[UserPreSize]

    this->tree->setRootItemVisible(true);
    this->tree->setDefaultOpenness(true);
    this->tree->setFocusContainer(false);
    this->tree->setWantsKeyboardFocus(false);
    this->tree->getViewport()->setWantsKeyboardFocus(false);
    this->tree->setOpenCloseButtonsVisible(false);

    //[/UserPreSize]

    setSize (200, 640);

    //[Constructor]
    //[/Constructor]
}

TreePanelPhone::~TreePanelPhone()
{
    //[Destructor_pre]
    tree->setRootItem(nullptr);
    //[/Destructor_pre]

    background = nullptr;
    tree = nullptr;
    shadow = nullptr;
    gradient = nullptr;
    headLine = nullptr;
    headShadow = nullptr;
    gradient1 = nullptr;
    spectrometer = nullptr;
    separator = nullptr;
    rootTreeItemPanel = nullptr;
    vuMeterLeft = nullptr;
    vuMeterRight = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TreePanelPhone::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TreePanelPhone::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    const bool widthChanged = (this->tree->getWidth() != this->getWidth());

    if (widthChanged)
    {
        if (this->isCompactMode())
        {
            this->rootTreeItemPanel = new RootTreeItemPanelCompact();
        }
        else
        {
            this->rootTreeItemPanel = new RootTreeItemPanelDefault();
        }

        this->addAndMakeVisible(this->rootTreeItemPanel);
        this->rootTreeItemPanel->postCommandMessage(CommandIDs::UpdateRootItemPanel);
    }
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    tree->setBounds (0, 49, getWidth() - 0, getHeight() - 112);
    shadow->setBounds (0, getHeight() - 63 - 6, getWidth() - 0, 6);
    gradient->setBounds (-50, getHeight() - 62, getWidth() - -100, 62);
    headLine->setBounds (0, 47, getWidth() - 0, 2);
    headShadow->setBounds (0, 48, getWidth() - 0, 6);
    gradient1->setBounds (-50, 0, getWidth() - -100, 47);
    spectrometer->setBounds (0, getHeight() - 62, getWidth() - 0, 62);
    separator->setBounds (0, getHeight() - 62 - 2, getWidth() - 0, 2);
    rootTreeItemPanel->setBounds (0, 0, getWidth() - 0, 48);
    vuMeterLeft->setBounds (0, getHeight() - 62, 8, 62);
    vuMeterRight->setBounds (getWidth() - 8, getHeight() - 62, 8, 62);
    //[UserResized] Add your own custom resize handling here..

    if (widthChanged)
    {
        this->tree->setIndentSize(this->isCompactMode() ? 0 : 7);

        // Scrollbars on the tree are evil, as they mess up the whole UI
        this->tree->getViewport()->setScrollBarsShown(false, false);
        // Force reload components:
		if (this->root != nullptr)
		{
			this->tree->setRootItem(nullptr);
			this->tree->setRootItem(this->root);
		}
    }

    if (this->currentRollover != nullptr &&
        ! this->rolloverFader.isAnimating())
    {
        this->currentRollover->setBounds(0, 0, this->getWidth(), this->tree->getBottom());
    }
    //[/UserResized]
}


//[MiscUserCode]
void TreePanelPhone::setRoot(TreeItem *rootItem)
{
    this->root = rootItem;
    this->tree->setRootItem(rootItem);

    if (rootItem != nullptr)
    {
        this->tree->getRootItem()->setOpen(true);
        this->tree->setRootItemVisible(true);
    }
}

void TreePanelPhone::setRootItemPanelSelected(bool shouldBeSelected)
{
    this->rootTreeItemPanel->postCommandMessage(shouldBeSelected ?
                                                CommandIDs::SelectRootItemPanel :
                                                CommandIDs::DeselectRootItemPanel);
}

void TreePanelPhone::setAudioMonitor(AudioMonitor *audioMonitor)
{
    this->spectrometer->setTargetAnalyzer(audioMonitor);
    this->vuMeterLeft->setTargetAnalyzer(audioMonitor);
    this->vuMeterRight->setTargetAnalyzer(audioMonitor);
}

Rectangle<int> TreePanelPhone::getWorkingArea()
{
    return this->tree->getBounds();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TreePanelPhone" template="../../Template"
                 componentName="" parentClasses="public TreePanel" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="200" initialHeight="640">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="dab1f297d3cc7439" memberName="tree" virtualName=""
                    explicitFocusOrder="0" pos="0 49 0M 112M" class="TreeView" params=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 63Rr 0M 6" sourceFile="../Themes/LighterShadowUpwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="15cecf620af99284" memberName="gradient" virtualName=""
             explicitFocusOrder="0" pos="-50 0Rr -100M 62" sourceFile="../Themes/GradientVertical.cpp"
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
  <GENERICCOMPONENT name="" id="1c5204139a3bea83" memberName="spectrometer" virtualName=""
                    explicitFocusOrder="0" pos="0 0Rr 0M 62" class="SpectrumComponent"
                    params="nullptr"/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 62Rr 0M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="faec82bf5da2e1" memberName="rootTreeItemPanel" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 48" class="Component" params=""/>
  <GENERICCOMPONENT name="" id="f36c1cf11c793cec" memberName="vuMeterLeft" virtualName=""
                    explicitFocusOrder="0" pos="0 0Rr 8 62" class="VolumeComponent"
                    params="nullptr, 0, VolumeComponent::Left"/>
  <GENERICCOMPONENT name="" id="e90d6ee8a79197a7" memberName="vuMeterRight" virtualName=""
                    explicitFocusOrder="0" pos="0Rr 0Rr 8 62" class="VolumeComponent"
                    params="nullptr, 1, VolumeComponent::Right"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_TreePanelPhone_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,19,1,
0,154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,101,7,0,0,
0,12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* TreePanelPhone::gray1x1_png = (const char*) resource_TreePanelPhone_gray1x1_png;
const int TreePanelPhone::gray1x1_pngSize = 150;
