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

#include "MobileComboBox.h"

//[MiscUserDefs]
#include "CommandPanel.h"

//[/MiscUserDefs]

MobileComboBox::MobileComboBox(WeakReference<Component> editor)
    : editor(editor)
{
    addAndMakeVisible (background = new PanelBackgroundC());
    addAndMakeVisible (menu = new CommandPanel());

    addAndMakeVisible (triggerButtton = new MobileComboBox::Trigger());

    addAndMakeVisible (shadow = new LighterShadowDownwards());
    addAndMakeVisible (separator = new SeparatorHorizontalReversed());
    addAndMakeVisible (currentNameLabel = new Label (String(),
                                                     String()));
    currentNameLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    currentNameLabel->setJustificationType (Justification::centredLeft);
    currentNameLabel->setEditable (false, false, false);
    currentNameLabel->setColour (TextEditor::textColourId, Colours::black);
    currentNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    this->setMouseClickGrabsKeyboardFocus(false);
    this->currentNameLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (400, 300);

    //[Constructor]
    //[/Constructor]
}

MobileComboBox::~MobileComboBox()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    menu = nullptr;
    triggerButtton = nullptr;
    shadow = nullptr;
    separator = nullptr;
    currentNameLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void MobileComboBox::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MobileComboBox::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    menu->setBounds (2, 34, getWidth() - 4, getHeight() - 34);
    triggerButtton->setBounds (getWidth() - 32, 0, 32, 32);
    shadow->setBounds (1, 33, getWidth() - 2, 16);
    separator->setBounds (1, 32, getWidth() - 2, 2);
    currentNameLabel->setBounds (0, 0, getWidth() - 0, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MobileComboBox::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    if (this->primer != nullptr)
    {
        this->setBounds(this->primer->getBounds());
    }
    //[/UserCode_parentHierarchyChanged]
}

void MobileComboBox::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    if (this->primer != nullptr)
    {
        this->setBounds(this->primer->getBounds());
    }
    //[/UserCode_parentSizeChanged]
}

void MobileComboBox::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (this->getParentComponent() != nullptr && this->editor != nullptr)
    {
        if (commandId != CommandIDs::ToggleShowHideCombo)
        {
            this->getParentComponent()->postCommandMessage(commandId);
        }

        this->animator.animateComponent(this, this->editor->getBounds(), 0.f, 200, true, 0.0, 1.0);
        this->getParentComponent()->removeChildComponent(this);
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void MobileComboBox::initMenu(CommandPanel::Items menu)
{
    this->menu->updateContent(menu);
}

void MobileComboBox::initText(TextEditor *editor)
{
    this->currentNameLabel->setFont(editor->getFont());
    this->currentNameLabel->setText(editor->getText(), dontSendNotification);
}

MobileComboBox::Primer::Primer()
{
    this->setInterceptsMouseClicks(false, false);
}

MobileComboBox::Primer::~Primer()
{
    this->cleanup();
}

void MobileComboBox::Primer::initWith(WeakReference<Component> editor, CommandPanel::Items menu)
{
    this->toFront(false);
    this->textEditor = editor;
    this->combo = new MobileComboBox(editor);
    this->combo->initMenu(menu);
    this->comboTrigger = new MobileComboBox::Trigger(this);
    if (this->textEditor != nullptr)
    {
        this->textEditor->addAndMakeVisible(this->comboTrigger);
    }
}

void MobileComboBox::Primer::updateMenu(CommandPanel::Items menu)
{
    this->combo->initMenu(menu);
}

void MobileComboBox::Primer::cleanup()
{
    // Need to be called before target text editor is deleted
    this->comboTrigger = nullptr;
}
void MobileComboBox::Primer::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::ToggleShowHideCombo &&
        this->getParentComponent() != nullptr &&
        this->combo != nullptr)
    {
        // Show combo
        this->getParentComponent()->addAndMakeVisible(this->combo);
        this->combo->setBounds(this->textEditor->getBounds());
        this->combo->setAlpha(0.f);
        if (TextEditor *ed = dynamic_cast<TextEditor *>(this->textEditor.get())) { this->combo->initText(ed); }
        this->animator.animateComponent(this->combo, this->getBounds(), 1.f, 200, false, 1.0, 0.0);
    }
}

MobileComboBox::Trigger::Trigger(WeakReference<Component> listener) :
    IconButton(Icons::findByName(Icons::down, 16), CommandIDs::ToggleShowHideCombo, listener)
{}

void MobileComboBox::Trigger::parentHierarchyChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::parentSizeChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::updateBounds()
{
    if (Component *parent = this->getParentComponent())
    {
        const int w = 32;
        const int h = 32;
        const int x = parent->getWidth() - w;
        const int y = 0;
        this->setBounds(x, 0, w, h);
        this->setAlwaysOnTop(true);
        this->toFront(false);
    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MobileComboBox" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="WeakReference&lt;Component&gt; editor"
                 variableInitialisers="editor(editor)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="400"
                 initialHeight="300">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="481c8ae8d7eec9f7" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="2226274fea88e6e8" memberName="menu" virtualName=""
                    explicitFocusOrder="0" pos="2 34 4M 34M" class="CommandPanel"
                    params=""/>
  <GENERICCOMPONENT name="" id="8ec691832b64961b" memberName="triggerButtton" virtualName=""
                    explicitFocusOrder="0" pos="0Rr 0 32 32" class="MobileComboBox::Trigger"
                    params=""/>
  <JUCERCOMP name="" id="cdb9ae0975a381d3" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="1 33 2M 16" sourceFile="../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="a46de88ae4304986" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="1 32 2M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <LABEL name="" id="cc2095775f3aaed2" memberName="currentNameLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0 0M 32" edTextCol="ff000000" edBkgCol="0"
         labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
