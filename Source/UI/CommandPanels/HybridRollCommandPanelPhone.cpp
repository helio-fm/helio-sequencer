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

#include "HybridRollCommandPanelPhone.h"

//[MiscUserDefs]
#include "Transport.h"
#include "HelioCallout.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

HybridRollCommandPanelPhone::HybridRollCommandPanelPhone(ProjectTreeItem &parent)
    : HybridRollCommandPanel(parent)
{
    addAndMakeVisible (headBg = new PanelBackgroundC());
    addAndMakeVisible (bodyBg = new PanelBackgroundC());
    addAndMakeVisible (listBox = new ListBox());

    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (shadow = new LighterShadowUpwards());
    addAndMakeVisible (gradient2 = new GradientVertical());
    addAndMakeVisible (separator = new SeparatorHorizontal());
    addAndMakeVisible (totalTime = new Label (String(),
                                              TRANS("...")));
    totalTime->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    totalTime->setJustificationType (Justification::centred);
    totalTime->setEditable (false, false, false);
    totalTime->setColour (Label::textColourId, Colour (0x77ffffff));
    totalTime->setColour (TextEditor::textColourId, Colours::black);
    totalTime->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (currentTime = new Label (String(),
                                                TRANS("...")));
    currentTime->setFont (Font (Font::getDefaultSansSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Regular"));
    currentTime->setJustificationType (Justification::centred);
    currentTime->setEditable (false, false, false);
    currentTime->setColour (Label::textColourId, Colour (0x77ffffff));
    currentTime->setColour (TextEditor::textColourId, Colours::black);
    currentTime->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (playButton = new PlayButton());
    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (gradient = new GradientVerticalReversed());
    addAndMakeVisible (annotationsButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::menu, CommandIDs::ShowAnnotations)));


    //[UserPreSize]
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->listBox->setRowHeight(HYBRID_ROLL_COMMANDPANEL_ROWHEIGHT);
    //[/UserPreSize]

    setSize (64, 640);

    //[Constructor]
    this->setSize(HYBRID_ROLL_COMMANDPANEL_WIDTH, 320);

    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *c = this->getChildComponent(i);
        c->setFocusContainer(false);
        c->setWantsKeyboardFocus(false);
        c->setMouseClickGrabsKeyboardFocus(false);
    }
    //[/Constructor]
}

HybridRollCommandPanelPhone::~HybridRollCommandPanelPhone()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    headBg = nullptr;
    bodyBg = nullptr;
    listBox = nullptr;
    headLine = nullptr;
    shadow = nullptr;
    gradient2 = nullptr;
    separator = nullptr;
    totalTime = nullptr;
    currentTime = nullptr;
    playButton = nullptr;
    headShadow = nullptr;
    gradient = nullptr;
    annotationsButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HybridRollCommandPanelPhone::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HybridRollCommandPanelPhone::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    headBg->setBounds (0, 0, getWidth() - 0, 48);
    bodyBg->setBounds (0, 48, getWidth() - 0, getHeight() - 48);
    listBox->setBounds (0, 49, getWidth() - 0, getHeight() - 112);
    headLine->setBounds (0, 47, getWidth() - 0, 2);
    shadow->setBounds (0, getHeight() - 63 - 6, getWidth() - 0, 6);
    gradient2->setBounds (0, getHeight() - 63, getWidth() - 0, 63);
    separator->setBounds (0, getHeight() - 62 - 2, getWidth() - 0, 2);
    totalTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - -46 - (28 / 2), 72, 28);
    currentTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - -18 - (32 / 2), 72, 32);
    playButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), getHeight() - 32 - (64 / 2), getWidth() - 0, 64);
    headShadow->setBounds (0, 48, getWidth() - 0, 6);
    gradient->setBounds (0, 0, getWidth() - 0, 47);
    annotationsButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 45);
    //[UserResized] Add your own custom resize handling here..
    //Logger::writeToLog("HybridRollCommandPanel updateContent");
    // a hack for themes changing
    this->listBox->updateContent();
    this->annotationsButton->resized();

#if HELIO_DESKTOP
    playButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), getHeight() - 88 - (64 / 2), getWidth() - 0, 48);
    totalTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 28 - (28 / 2), 72, 28);
    currentTime->setBounds ((getWidth() / 2) - (72 / 2), getHeight() - 54 - (32 / 2), 72, 32);
    annotationsButton->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 28 - (36 / 2), getWidth() - 0, 36);
#endif

    //[/UserResized]
}

void HybridRollCommandPanelPhone::childrenChanged()
{
    //[UserCode_childrenChanged] -- Add your code here...
    //this->updateButtonsImages();
    //[/UserCode_childrenChanged]
}

void HybridRollCommandPanelPhone::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ListBoxModel
//

Component
*HybridRollCommandPanelPhone::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                     Component *existingComponentToUpdate)
{
    if (rowNumber >= this->commandDescriptions.size())
    {
        return existingComponentToUpdate;
    }

    const CommandItem::Ptr itemDescription = this->commandDescriptions[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (CommandItemComponent *row = dynamic_cast<CommandItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        CommandItemComponent *row = new CommandItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}


//===----------------------------------------------------------------------===//
// AsyncUpdater
//

void HybridRollCommandPanelPhone::handleAsyncUpdate()
{
    if (this->isTimerRunning())
    {
        const double systemTimeOffset = (Time::getMillisecondCounter() - this->timerStartSystemTime);
        const double currentTimeMs(this->timerStartSeekTime + systemTimeOffset);
        this->currentTime->setText(Transport::getTimeString(currentTimeMs), dontSendNotification);
    }
    else
    {
        this->currentTime->setText(Transport::getTimeString(this->lastSeekTime), dontSendNotification);
    }

    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime), dontSendNotification);
}

//===----------------------------------------------------------------------===//
// TransportListener
//

void HybridRollCommandPanelPhone::onTotalTimeChanged(const double timeMs)
{
    HybridRollCommandPanel::onTotalTimeChanged(timeMs);
    this->totalTime->setText(Transport::getTimeString(this->lastTotalTime), dontSendNotification);
}

void HybridRollCommandPanelPhone::onPlay()
{
    HybridRollCommandPanel::onPlay();
    this->playButton->setPlaying(true);
}

void HybridRollCommandPanelPhone::onStop()
{
    HybridRollCommandPanel::onStop();
    this->playButton->setPlaying(false);
}

//===--------------------------------------------------------------------------===//
// HybridRollCommandPanel
//

void HybridRollCommandPanelPhone::updateModeButtons()
{
    this->recreateCommandDescriptions();
    this->listBox->updateContent();
}

void HybridRollCommandPanelPhone::emitAnnotationsCallout(Component *newAnnotationsMenu)
{
    HelioCallout::emit(newAnnotationsMenu, this->annotationsButton);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MidiRollCommandPanelPhone"
                 template="../../Template" componentName="" parentClasses="public HybridRollCommandPanel"
                 constructorParams="ProjectTreeItem &amp;parent" variableInitialisers="HybridRollCommandPanel(parent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="64" initialHeight="640">
  <METHODS>
    <METHOD name="childrenChanged()"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="8f95c631b98e644b" memberName="headBg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 48" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="bodyBg" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 48M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 49 0M 112M" class="ListBox" params=""/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 47 0M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 63Rr 0M 6" sourceFile="../Themes/LighterShadowUpwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="15cecf620af99284" memberName="gradient2" virtualName=""
             explicitFocusOrder="0" pos="0 0Rr 0M 63" sourceFile="../Themes/GradientVertical.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 62Rr 0M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <LABEL name="" id="700073f74a17c931" memberName="totalTime" virtualName=""
         explicitFocusOrder="0" pos="0Cc -46Rc 72 28" textCol="77ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="b9e867ece7f52ad8" memberName="currentTime" virtualName=""
         explicitFocusOrder="0" pos="0Cc -18Rc 72 32" textCol="77ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="28" kerning="0" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc 32Rc 0M 64" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 6" sourceFile="../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f09d886c97d1c017" memberName="gradient" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 47" sourceFile="../Themes/GradientVerticalReversed.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="annotationsButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 45" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::menu, CommandIDs::ShowAnnotations)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_MidiRollCommandPanelPhone_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,
0,11,19,1,0,154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,
101,7,0,0,0,12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* HybridRollCommandPanelPhone::gray1x1_png = (const char*) resource_MidiRollCommandPanelPhone_gray1x1_png;
const int HybridRollCommandPanelPhone::gray1x1_pngSize = 150;
