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

#include "LogComponent.h"

//[MiscUserDefs]
#include "App.h"
//[/MiscUserDefs]

LogComponent::LogComponent()
{
    addAndMakeVisible (logText = new TextEditor (String()));
    logText->setMultiLine (true);
    logText->setReturnKeyStartsNewLine (false);
    logText->setReadOnly (true);
    logText->setScrollbarsShown (true);
    logText->setCaretVisible (false);
    logText->setPopupMenuEnabled (false);
    logText->setColour (TextEditor::textColourId, Colours::white);
    logText->setColour (TextEditor::backgroundColourId, Colour (0xa9000000));
    logText->setText (String());


    //[UserPreSize]
    this->logText->setWantsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (600, 220);

    //[Constructor]
    if (DebugLogger *hl = dynamic_cast<DebugLogger *>(Logger::getCurrentLogger()))
    {
        hl->addChangeListener(this);
    }
    //[/Constructor]
}

LogComponent::~LogComponent()
{
    //[Destructor_pre]
    if (DebugLogger *hl = dynamic_cast<DebugLogger *>(Logger::getCurrentLogger()))
    {
        hl->removeChangeListener(this);
    }
    //[/Destructor_pre]

    logText = nullptr;

    //[Destructor]
    //[/Destructor]
}

void LogComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LogComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    logText->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LogComponent::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    this->syncWithLog();
    //[/UserCode_visibilityChanged]
}

void LogComponent::broughtToFront()
{
    //[UserCode_broughtToFront] -- Add your code here...
    this->syncWithLog();
    //[/UserCode_broughtToFront]
}

void LogComponent::focusOfChildComponentChanged (FocusChangeType cause)
{
    //[UserCode_focusOfChildComponentChanged] -- Add your code here...
    this->syncWithLog();
    //[/UserCode_focusOfChildComponentChanged]
}


//[MiscUserCode]
void LogComponent::changeListenerCallback(ChangeBroadcaster *source)
{
    this->syncWithLog();
}

void LogComponent::syncWithLog()
{
    if (DebugLogger *hl = dynamic_cast<DebugLogger *>(Logger::getCurrentLogger()))
    {
        this->logText->setText(hl->getText());
        this->logText->moveCaretToEnd();
    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LogComponent" template="../../Template"
                 componentName="" parentClasses="public Component, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="220">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="broughtToFront()"/>
    <METHOD name="focusOfChildComponentChanged (FocusChangeType cause)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <TEXTEDITOR name="" id="4cd59a9fefba81ab" memberName="logText" virtualName=""
              explicitFocusOrder="0" pos="0 0 0M 0M" textcol="ffffffff" bkgcol="a9000000"
              initialText="" multiline="1" retKeyStartsLine="0" readonly="1"
              scrollbars="1" caret="0" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
