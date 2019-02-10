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

#include "RenderDialog.h"

//[MiscUserDefs]
#include "DocumentOwner.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "PlayerThread.h"
#include "ProgressIndicator.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "MenuItemComponent.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

RenderDialog::RenderDialog(ProjectNode &parentProject, const File &renderTo, const String &formatExtension)
    : project(parentProject),
      extension(formatExtension.toLowerCase()),
      shouldRenderAfterDialogCompletes(false)
{
    addAndMakeVisible (background = new DialogPanel());
    addAndMakeVisible (renderButton = new TextButton (String()));
    renderButton->setButtonText (TRANS("dialog::render::proceed"));
    renderButton->setConnectedEdges (Button::ConnectedOnTop);
    renderButton->addListener (this);

    addAndMakeVisible (filenameEditor = new Label (String(),
                                                   TRANS("...")));
    filenameEditor->setFont (Font (Font::getDefaultSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Regular"));
    filenameEditor->setJustificationType (Justification::topLeft);
    filenameEditor->setEditable (true, true, false);
    filenameEditor->addListener (this);

    addAndMakeVisible (filenameLabel = new Label (String(),
                                                  TRANS("dialog::render::caption")));
    filenameLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    filenameLabel->setJustificationType (Justification::centredLeft);
    filenameLabel->setEditable (false, false, false);

    addAndMakeVisible (cancelButton = new TextButton (String()));
    cancelButton->setButtonText (TRANS("dialog::render::close"));
    cancelButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    cancelButton->addListener (this);

    addAndMakeVisible (slider = new Slider (String()));
    slider->setRange (0, 1000, 0);
    slider->setSliderStyle (Slider::LinearBar);
    slider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    slider->addListener (this);

    addAndMakeVisible (indicator = new ProgressIndicator());

    addAndMakeVisible (browseButton = new MenuItemComponent (this, nullptr, MenuItem::item(Icons::browse, CommandIDs::Browse)));

    addAndMakeVisible (pathEditor = new Label (String(),
                                               TRANS("...")));
    pathEditor->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    pathEditor->setJustificationType (Justification::centredLeft);
    pathEditor->setEditable (false, false, false);

    addAndMakeVisible (component3 = new SeparatorHorizontalFading());
    component3->setBounds (32, 121, 456, 8);

    addAndMakeVisible (separatorH = new SeparatorHorizontal());

    //[UserPreSize]
    // just in case..
    this->project.getTransport().stopPlayback();

    this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);
    this->indicator->setVisible(false);
    this->slider->setEnabled(false);
    this->slider->setRange(0.0, 1.0, 0.01);

    this->separatorH->setAlphaMultiplier(2.5f);

    this->pathEditor->setText(renderTo.getParentDirectory().getFullPathName(), dontSendNotification);
    this->filenameEditor->setText(renderTo.getFileName(), dontSendNotification);

#if JUCE_MAC
    this->filenameEditor->setEditable(false);
#endif
    //[/UserPreSize]

    setSize (520, 224);

    //[Constructor]
    this->updatePosition();
    //[/Constructor]
}

RenderDialog::~RenderDialog()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    renderButton = nullptr;
    filenameEditor = nullptr;
    filenameLabel = nullptr;
    cancelButton = nullptr;
    slider = nullptr;
    indicator = nullptr;
    browseButton = nullptr;
    pathEditor = nullptr;
    component3 = nullptr;
    separatorH = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RenderDialog::paint (Graphics& g)
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

void RenderDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    renderButton->setBounds (getWidth() - 4 - (getWidth() - 8), getHeight() - 4 - 48, getWidth() - 8, 48);
    filenameEditor->setBounds ((getWidth() / 2) + 25 - (406 / 2), 4 + 71, 406, 32);
    filenameLabel->setBounds ((getWidth() / 2) + 29 - (414 / 2), 4 + 16, 414, 22);
    cancelButton->setBounds (0, getHeight() - -74 - 48, 255, 48);
    slider->setBounds ((getWidth() / 2) + 24 - (392 / 2), 139, 392, 12);
    indicator->setBounds ((getWidth() / 2) + -212 - (32 / 2), 139 + 12 / 2 + -2 - (32 / 2), 32, 32);
    browseButton->setBounds (getWidth() - 448 - 48, 59, 48, 48);
    pathEditor->setBounds ((getWidth() / 2) + 25 - (406 / 2), 4 + 48, 406, 24);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RenderDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == renderButton)
    {
        //[UserButtonCode_renderButton] -- add your button handler code here..
        this->startOrAbortRender();
        //[/UserButtonCode_renderButton]
    }
    else if (buttonThatWasClicked == cancelButton)
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        this->stopRender();
        this->dismiss();
        //[/UserButtonCode_cancelButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void RenderDialog::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == filenameEditor)
    {
        //[UserLabelCode_filenameEditor] -- add your label text handling code here..
        //[/UserLabelCode_filenameEditor]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void RenderDialog::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider)
    {
        //[UserSliderCode_slider] -- add your slider handling code here..
        //[/UserSliderCode_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void RenderDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void RenderDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void RenderDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::HideDialog)
    {
        Transport &transport = this->project.getTransport();
        if (! transport.isRendering())
        {
            this->dismiss();
        }
    }
    else if (commandId == CommandIDs::Browse)
    {
#if HELIO_DESKTOP
        FileChooser fc(TRANS("dialog::render::selectfile"),
                       File(this->getFileName()), ("*." + this->extension), true);

        if (fc.browseForFileToSave(true))
        {
            this->pathEditor->setText(fc.getResult().getParentDirectory().getFullPathName(), dontSendNotification);
            this->filenameEditor->setText(fc.getResult().getFileName(), dontSendNotification);

            if (this->shouldRenderAfterDialogCompletes)
            {
                this->startOrAbortRender();
            }
        }

        this->shouldRenderAfterDialogCompletes = false;
#endif
    }
    //[/UserCode_handleCommandMessage]
}

bool RenderDialog::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}

void RenderDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::HideDialog);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void RenderDialog::startOrAbortRender()
{
    Transport &transport = this->project.getTransport();

    if (! transport.isRendering())
    {
        transport.startRender(this->getFileName());
        this->startTrackingProgress();
    }
    else
    {
        transport.stopRender();
        this->stopTrackingProgress();
        App::Layout().showModalComponentUnowned(new FailTooltip());
    }
}

void RenderDialog::stopRender()
{
    Transport &transport = this->project.getTransport();

    if (transport.isRendering())
    {
        transport.stopRender();
        this->stopTrackingProgress();
    }
}

String RenderDialog::getFileName() const
{
    const String safeRenderName = File::createLegalFileName(this->filenameEditor->getText(true));
    const String absolutePath = this->pathEditor->getText();
    return File(absolutePath).getChildFile(safeRenderName).getFullPathName();
}

void RenderDialog::timerCallback()
{
    Transport &transport = this->project.getTransport();

    if (transport.isRendering())
    {
        const float percentsDone = transport.getRenderingPercentsComplete();
        this->slider->setValue(percentsDone, dontSendNotification);
    }
    else
    {
        this->stopTrackingProgress();
        App::Layout().showModalComponentUnowned(new SuccessTooltip());
        transport.stopRender();
    }
}

void RenderDialog::startTrackingProgress()
{
    this->startTimerHz(60);
    this->indicator->startAnimating();
    this->animator.fadeIn(this->indicator, 250);
    this->renderButton->setButtonText(TRANS("dialog::render::abort"));
}

void RenderDialog::stopTrackingProgress()
{
    this->stopTimer();

    Transport &transport = this->project.getTransport();
    const float percentsDone = transport.getRenderingPercentsComplete();
    this->slider->setValue(percentsDone, dontSendNotification);

    this->animator.fadeOut(this->indicator, 250);
    this->indicator->stopAnimating();
    this->renderButton->setButtonText(TRANS("dialog::render::proceed"));
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RenderDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, private Timer"
                 constructorParams="ProjectNode &amp;parentProject, const File &amp;renderTo, const String &amp;formatExtension"
                 variableInitialisers="project(parentProject),&#10;extension(formatExtension.toLowerCase()),&#10;shouldRenderAfterDialogCompletes(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="520" initialHeight="224">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10.00000000000000000000" fill="solid: 59000000"
               hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="renderButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 8M 48" buttonText="dialog::render::proceed"
              connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="9c63b5388edfe183" memberName="filenameEditor" virtualName=""
         explicitFocusOrder="0" pos="25Cc 71 406 32" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="28.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="filenameLabel" virtualName=""
         explicitFocusOrder="0" pos="29Cc 16 414 22" posRelativeY="e96b77baef792d3a"
         labelText="dialog::render::caption" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="0 -74Rr 255 48" buttonText="dialog::render::close"
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="" id="53d73eae72d7741b" memberName="slider" virtualName=""
          explicitFocusOrder="0" pos="24Cc 139 392 12" min="0.00000000000000000000"
          max="1000.00000000000000000000" int="0.00000000000000000000"
          style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.00000000000000000000"
          needsCallback="1"/>
  <GENERICCOMPONENT name="" id="92641fd94a728225" memberName="indicator" virtualName=""
                    explicitFocusOrder="0" pos="-212Cc -2Cc 32 32" posRelativeY="53d73eae72d7741b"
                    class="ProgressIndicator" params=""/>
  <GENERICCOMPONENT name="" id="62a5bd7c1a3ec2" memberName="browseButton" virtualName=""
                    explicitFocusOrder="0" pos="448Rr 59 48 48" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::browse, CommandIDs::Browse)"/>
  <LABEL name="" id="2310f57af9b4eefb" memberName="pathEditor" virtualName=""
         explicitFocusOrder="0" pos="25Cc 48 406 24" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="16.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="ab3833b58a212645" memberName="component3" virtualName=""
             explicitFocusOrder="0" pos="32 121 456 8" sourceFile="../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
