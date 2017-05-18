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

#include "UpdateDialog.h"

//[MiscUserDefs]
#include "DocumentOwner.h"
#include "App.h"
#include "MainLayout.h"
#include "AuthorizationManager.h"
#include "ProjectTreeItem.h"
#include "PlayerThread.h"
#include "ProgressIndicator.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "UpdateManager.h"
//[/MiscUserDefs]

UpdateDialog::UpdateDialog()
{
    addAndMakeVisible (background = new PanelC());
    addAndMakeVisible (updateButton = new TextButton (String()));
    updateButton->setButtonText (TRANS("dialog::update::proceed"));
    updateButton->setConnectedEdges (Button::ConnectedOnTop);
    updateButton->addListener (this);

    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("dialog::update::minor")));
    titleLabel->setFont (Font (Font::getDefaultSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (true, true, false);
    titleLabel->setColour (Label::textColourId, Colours::white);
    titleLabel->setColour (TextEditor::textColourId, Colours::black);
    titleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    titleLabel->addListener (this);

    addAndMakeVisible (cancelButton = new TextButton (String()));
    cancelButton->setButtonText (TRANS("dialog::update::cancel"));
    cancelButton->setConnectedEdges (Button::ConnectedOnTop);
    cancelButton->addListener (this);

    addAndMakeVisible (descriptionLabel1 = new Label (String(),
                                                      TRANS("dialog::update::version::installed")));
    descriptionLabel1->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    descriptionLabel1->setJustificationType (Justification::centredLeft);
    descriptionLabel1->setEditable (false, false, false);
    descriptionLabel1->setColour (Label::textColourId, Colour (0x77ffffff));
    descriptionLabel1->setColour (TextEditor::textColourId, Colours::black);
    descriptionLabel1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (descriptionLabel2 = new Label (String(),
                                                      TRANS("dialog::update::version::available")));
    descriptionLabel2->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    descriptionLabel2->setJustificationType (Justification::centredLeft);
    descriptionLabel2->setEditable (false, false, false);
    descriptionLabel2->setColour (Label::textColourId, Colour (0x99ffffff));
    descriptionLabel2->setColour (TextEditor::textColourId, Colours::black);
    descriptionLabel2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (installedVersionLabel = new Label (String(),
                                                          TRANS("...")));
    installedVersionLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    installedVersionLabel->setJustificationType (Justification::centredRight);
    installedVersionLabel->setEditable (false, false, false);
    installedVersionLabel->setColour (Label::textColourId, Colour (0x55ffffff));
    installedVersionLabel->setColour (TextEditor::textColourId, Colours::black);
    installedVersionLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (availableVersionLabel = new Label (String(),
                                                          TRANS("...")));
    availableVersionLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    availableVersionLabel->setJustificationType (Justification::centredRight);
    availableVersionLabel->setEditable (false, false, false);
    availableVersionLabel->setColour (Label::textColourId, Colour (0x77ffffff));
    availableVersionLabel->setColour (TextEditor::textColourId, Colours::black);
    availableVersionLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (forceUpdateButton = new TextButton (String()));
    forceUpdateButton->setButtonText (TRANS("dialog::update::proceed"));
    forceUpdateButton->setConnectedEdges (Button::ConnectedOnTop);
    forceUpdateButton->addListener (this);

    addAndMakeVisible (separatorH = new SeparatorHorizontal());

    //[UserPreSize]
    UpdateManager *updater = App::Helio()->getUpdateManager();
    this->installedVersionLabel->setText(App::getAppReadableVersion(), dontSendNotification);
    this->availableVersionLabel->setText(updater->getLatestVersion(), dontSendNotification);

    if (updater->hasMajorUpdate())
    {
        this->titleLabel->setText(TRANS("dialog::update::major"), dontSendNotification);
        this->cancelButton->setVisible(false);
        this->updateButton->setVisible(false);
        this->forceUpdateButton->setVisible(true);
    }
    else
    {
        this->cancelButton->setVisible(true);
        this->updateButton->setVisible(true);
        this->forceUpdateButton->setVisible(false);
    }
	
	this->separatorH->setAlphaMultiplier(2.5f);
	//[/UserPreSize]

    setSize (500, 190);

    //[Constructor]
    this->rebound();
    //[/Constructor]
}

UpdateDialog::~UpdateDialog()
{
    //[Destructor_pre]
    FadingDialog::fadeOut();
    //[/Destructor_pre]

    background = nullptr;
    updateButton = nullptr;
    titleLabel = nullptr;
    cancelButton = nullptr;
    descriptionLabel1 = nullptr;
    descriptionLabel2 = nullptr;
    installedVersionLabel = nullptr;
    availableVersionLabel = nullptr;
    forceUpdateButton = nullptr;
    separatorH = nullptr;

    //[Destructor]
    //[/Destructor]
}

void UpdateDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour = Colour (0x59000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UpdateDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    updateButton->setBounds ((getWidth() / 2) + 76 - (283 / 2), getHeight(), 283, 42);
    titleLabel->setBounds ((getWidth() / 2) - (464 / 2), 4 + 15, 464, 40);
    cancelButton->setBounds ((getWidth() / 2) + -150 - (136 / 2), getHeight(), 136, 42);
    descriptionLabel1->setBounds ((getWidth() / 2) + -114 - (208 / 2), 4 + 59, 208, 24);
    descriptionLabel2->setBounds ((getWidth() / 2) + -114 - (208 / 2), 4 + 87, 208, 24);
    installedVersionLabel->setBounds ((getWidth() / 2) + 106 - (216 / 2), 4 + 63, 216, 24);
    availableVersionLabel->setBounds ((getWidth() / 2) + 106 - (216 / 2), 4 + 91, 216, 24);
    forceUpdateButton->setBounds (4, getHeight() - 4 - 48, getWidth() - 8, 48);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UpdateDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == updateButton)
    {
        //[UserButtonCode_updateButton] -- add your button handler code here..
        UpdateManager *updater = App::Helio()->getUpdateManager();
        URL updateUrl(updater->getUpdateUrl());
        updateUrl.launchInDefaultBrowser();
        delete this;
        //[/UserButtonCode_updateButton]
    }
    else if (buttonThatWasClicked == cancelButton)
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        delete this;
        //[/UserButtonCode_cancelButton]
    }
    else if (buttonThatWasClicked == forceUpdateButton)
    {
        //[UserButtonCode_forceUpdateButton] -- add your button handler code here..
        UpdateManager *updater = App::Helio()->getUpdateManager();
        URL updateUrl(updater->getUpdateUrl());
        updateUrl.launchInDefaultBrowser();
        //[/UserButtonCode_forceUpdateButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void UpdateDialog::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == titleLabel)
    {
        //[UserLabelCode_titleLabel] -- add your label text handling code here..
        //[/UserLabelCode_titleLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void UpdateDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentHierarchyChanged]
}

void UpdateDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentSizeChanged]
}

bool UpdateDialog::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    return true;
    //[/UserCode_keyPressed]
}

void UpdateDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    // dont do anything.
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UpdateDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="500" initialHeight="190">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="inputAttemptWhenModal()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: 59000000" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/PanelC.cpp" constructorParams=""/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="updateButton" virtualName=""
              explicitFocusOrder="0" pos="75.5Cc 0R 283 42" posRelativeY="fee11f38ba63ec9"
              buttonText="dialog::update::proceed" connectedEdges="4" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="" id="9c63b5388edfe183" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 15 464 40" posRelativeY="e96b77baef792d3a"
         textCol="ffffffff" edTextCol="ff000000" edBkgCol="0" labelText="dialog::update::minor"
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="28" kerning="0" bold="0"
         italic="0" justification="33"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="-150Cc 0R 136 42" posRelativeY="fee11f38ba63ec9"
              buttonText="dialog::update::cancel" connectedEdges="4" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="descriptionLabel1"
         virtualName="" explicitFocusOrder="0" pos="-114Cc 59 208 24"
         posRelativeY="e96b77baef792d3a" textCol="77ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="dialog::update::version::installed" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="45eb7fb0b9758df1" memberName="descriptionLabel2"
         virtualName="" explicitFocusOrder="0" pos="-114Cc 87 208 24"
         posRelativeY="e96b77baef792d3a" textCol="99ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="dialog::update::version::available" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="9ee14541a0070623" memberName="installedVersionLabel"
         virtualName="" explicitFocusOrder="0" pos="106Cc 63 216 24" posRelativeY="e96b77baef792d3a"
         textCol="55ffffff" edTextCol="ff000000" edBkgCol="0" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="34"/>
  <LABEL name="" id="c4486b9eacc8fa89" memberName="availableVersionLabel"
         virtualName="" explicitFocusOrder="0" pos="106Cc 91 216 24" posRelativeY="e96b77baef792d3a"
         textCol="77ffffff" edTextCol="ff000000" edBkgCol="0" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="34"/>
  <TEXTBUTTON name="" id="29470b579dfe9508" memberName="forceUpdateButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 8M 48" buttonText="dialog::update::proceed"
              connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
