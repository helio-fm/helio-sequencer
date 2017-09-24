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

#include "AutomationsCommandPanel.h"

//[MiscUserDefs]
#include "CommandItemComponent.h"
#include "HybridRollCommandPanel.h"
//[/MiscUserDefs]

AutomationsCommandPanel::AutomationsCommandPanel()
{
    addAndMakeVisible (component = new PanelBackgroundC());
    addAndMakeVisible (rightwardsShadow = new ShadowRightwards());
    addAndMakeVisible (listBox = new ListBox());


    //[UserPreSize]
    this->recreateCommandDescriptions();

    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->listBox->setRowHeight(HYBRID_ROLL_COMMANDPANEL_ROWHEIGHT);
    //[/UserPreSize]

    setSize (64, 640);

    //[Constructor]
    this->setSize(HYBRID_ROLL_COMMANDPANEL_WIDTH, 640);

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

AutomationsCommandPanel::~AutomationsCommandPanel()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;
    rightwardsShadow = nullptr;
    listBox = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AutomationsCommandPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AutomationsCommandPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    rightwardsShadow->setBounds (-3, getHeight() - (getHeight() - 0), 20, getHeight() - 0);
    listBox->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AutomationsCommandPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
        default:
            break;
    }
    //[/UserCode_handleCommandMessage]
}

void AutomationsCommandPanel::childrenChanged()
{
    //[UserCode_childrenChanged] -- Add your code here...
    //[/UserCode_childrenChanged]
}

void AutomationsCommandPanel::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}


//[MiscUserCode]

void AutomationsCommandPanel::recreateCommandDescriptions()
{
    this->commandDescriptions.clear();
    // todo
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//

int AutomationsCommandPanel::getNumRows()
{
    return this->commandDescriptions.size();
}

void AutomationsCommandPanel::paintListBoxItem(int rowNumber,
                                            Graphics &g,
                                            int width, int height,
                                            bool rowIsSelected)
{
    //
}

Component *AutomationsCommandPanel::refreshComponentForRow(int rowNumber, bool isRowSelected,
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

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AutomationsCommandPanel"
                 template="../../Template" componentName="" parentClasses="public Component, private ListBoxModel"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="64"
                 initialHeight="640">
  <METHODS>
    <METHOD name="childrenChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="c47e649dc80172d4" memberName="rightwardsShadow" virtualName=""
             explicitFocusOrder="0" pos="-3 0Rr 20 0M" sourceFile="../Themes/ShadowRightwards.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: gray1x1_png, 150, "../../../../MainLayout/~icons/gray1x1.png"
static const unsigned char resource_AutomationsCommandPanel_gray1x1_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,1,0,0,0,1,8,2,0,0,0,144,119,83,222,0,0,0,9,112,72,89,115,0,0,11,19,0,0,
11,19,1,0,154,156,24,0,0,0,7,116,73,77,69,7,222,4,19,5,8,9,228,2,121,9,0,0,0,29,105,84,88,116,67,111,109,109,101,110,116,0,0,0,0,0,67,114,101,97,116,101,100,32,119,105,116,104,32,71,73,77,80,100,46,101,
7,0,0,0,12,73,68,65,84,8,215,99,136,138,138,2,0,2,32,1,15,53,60,95,243,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* AutomationsCommandPanel::gray1x1_png = (const char*) resource_AutomationsCommandPanel_gray1x1_png;
const int AutomationsCommandPanel::gray1x1_pngSize = 150;
