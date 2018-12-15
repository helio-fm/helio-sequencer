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

#include "ProgressTooltip.h"

//[MiscUserDefs]
#include "ComponentIDs.h"
#include "CommandIDs.h"

#define PROGRESS_TOOLTIP_FADEOUT_TIMS_MS 350
//[/MiscUserDefs]

ProgressTooltip::ProgressTooltip(bool cancellable)
    : isCancellable(cancellable)
{
    this->progressIndicator.reset(new ProgressIndicator());
    this->addAndMakeVisible(progressIndicator.get());


    //[UserPreSize]
    this->setComponentID(ComponentIDs::progressTooltipId);
    this->progressIndicator->startAnimating();
    //[/UserPreSize]

    this->setSize(96, 96);

    //[Constructor]
    //[/Constructor]
}

ProgressTooltip::~ProgressTooltip()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    progressIndicator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ProgressTooltip::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = static_cast<float> ((getWidth() / 2) - (96 / 2)), y = static_cast<float> ((getHeight() / 2) - (96 / 2)), width = 96.0f, height = 96.0f;
        Colour fillColour = Colour (0xa0000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 15.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ProgressTooltip::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    progressIndicator->setBounds((getWidth() / 2) - (64 / 2), (getHeight() / 2) - (64 / 2), 64, 64);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ProgressTooltip::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
    //[/UserCode_parentHierarchyChanged]
}

void ProgressTooltip::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancel();
    }
    //[/UserCode_handleCommandMessage]
}

bool ProgressTooltip::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancel();
        return true;
    }

    return false;
    //[/UserCode_keyPressed]
}

void ProgressTooltip::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]
void ProgressTooltip::cancel()
{
    if (!this->isCancellable)
    {
        return;
    }

    if (this->onCancel != nullptr)
    {
        this->onCancel();
    }

    this->dismiss();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ProgressTooltip" template="../../Template"
                 componentName="" parentClasses="public CenteredTooltipComponent"
                 constructorParams="bool cancellable" variableInitialisers="isCancellable(cancellable)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="96" initialHeight="96">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0Cc 0Cc 96 96" cornerSize="15.00000000000000000000" fill="solid: a0000000"
               hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="c8b225da767e9a4d" memberName="progressIndicator"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 64 64" class="ProgressIndicator"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
