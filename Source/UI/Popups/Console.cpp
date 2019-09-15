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

#include "Console.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "Headline.h"
//[/MiscUserDefs]

Console::Console()
{
    this->bg.reset(new PanelBackgroundB());
    this->addAndMakeVisible(bg.get());
    this->shadowDn.reset(new ShadowDownwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowDn.get());
    this->shadowL.reset(new ShadowLeftwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowL.get());
    this->shadowR.reset(new ShadowRightwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowR.get());
    this->textEditor.reset(new ConsoleTextEditor());
    this->addAndMakeVisible(textEditor.get());

    this->actionsList.reset(new ListBox());
    this->addAndMakeVisible(actionsList.get());


    //[UserPreSize]
    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setPopupMenuEnabled(false);
    this->textEditor->setScrollbarsShown(false);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setMultiLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setFont(21.f);

    this->textEditor->addListener(this);
    //[/UserPreSize]

    this->setSize(500, 400);

    //[Constructor]
    //[/Constructor]
}

Console::~Console()
{
    //[Destructor_pre]
    this->textEditor->removeListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    shadowDn = nullptr;
    shadowL = nullptr;
    shadowR = nullptr;
    textEditor = nullptr;
    actionsList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Console::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Console::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds(8, 0, getWidth() - 16, 200);
    shadowDn->setBounds(8 + 0, 0 + 200, (getWidth() - 16) - 0, 8);
    shadowL->setBounds(0, 0, 8, 160 - -40);
    shadowR->setBounds(getWidth() - 8, 0, 8, 160 - -40);
    textEditor->setBounds(12, 4, getWidth() - 24, 32);
    actionsList->setBounds(12 + 0, 4 + 32, (getWidth() - 24) - 0, 160);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Console::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void Console::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDismiss();
    }
    //[/UserCode_handleCommandMessage]
}

bool Console::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancelAndDismiss();
        return true;
    }

    return false;
    //[/UserCode_keyPressed]
}

void Console::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void Console::textEditorTextChanged(TextEditor &ed)
{

    // todo
}

void Console::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->dismiss();
}

void Console::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelAndDismiss();
}

void Console::textEditorFocusLost(TextEditor&)
{
    //const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty()
        //&& focusedComponent != this->okButton.get()
        )
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void Console::dismiss()
{
    this->fadeOut();
    delete this;
}

void Console::fadeOut()
{
    const int fadeoutTime = 200;
    auto &animator = Desktop::getInstance().getAnimator();
    if (App::isOpenGLRendererEnabled())
    {
        animator.animateComponent(this, this->getBounds().reduced(30).translated(0, -30), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
    else
    {
        animator.animateComponent(this, this->getBounds(), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
}

void Console::updatePosition()
{
    this->setTopLeftPosition(this->getParentWidth() / 2 - this->getWidth() / 2, HEADLINE_HEIGHT + 1);
}

void Console::cancelAndDismiss()
{
    // todo cancel
    this->dismiss();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Console" template="../../Template"
                 componentName="" parentClasses="public Component, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="500"
                 initialHeight="400">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <JUCERCOMP name="" id="80e2df40dfc6307e" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="8 0 16M 200" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="5d11a023e4905d74" memberName="shadowDn" virtualName=""
             explicitFocusOrder="0" pos="0 0R 0M 8" posRelativeX="80e2df40dfc6307e"
             posRelativeY="80e2df40dfc6307e" posRelativeW="80e2df40dfc6307e"
             sourceFile="../Themes/ShadowDownwards.cpp" constructorParams="ShadowType::Normal"/>
  <JUCERCOMP name="" id="72514a37e5bc1299" memberName="shadowL" virtualName=""
             explicitFocusOrder="0" pos="0 0 8 -40M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowLeftwards.cpp" constructorParams="ShadowType::Normal"/>
  <JUCERCOMP name="" id="3cd9e82f6261eff1" memberName="shadowR" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 8 -40M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowRightwards.cpp" constructorParams="ShadowType::Normal"/>
  <GENERICCOMPONENT name="" id="3abf9d1982e1c63b" memberName="textEditor" virtualName=""
                    explicitFocusOrder="0" pos="12 4 24M 32" class="ConsoleTextEditor"
                    params=""/>
  <GENERICCOMPONENT name="" id="2362db9f8826e90f" memberName="actionsList" virtualName=""
                    explicitFocusOrder="0" pos="0 0R 0M 160" posRelativeX="3abf9d1982e1c63b"
                    posRelativeY="3abf9d1982e1c63b" posRelativeW="3abf9d1982e1c63b"
                    class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



