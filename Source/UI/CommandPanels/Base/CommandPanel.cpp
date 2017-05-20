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

#include "CommandPanel.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "ProjectTreeItem.h"
#include "PlayerThread.h"
#include "Icons.h"
#include "MidiRoll.h"
#include "MidiLayer.h"
#include "InternalClipboard.h"
#include "CommandItemComponent.h"
#include "App.h"
#include "MainWindow.h"
//[/MiscUserDefs]

CommandPanel::CommandPanel()
{
    addAndMakeVisible (component = new PanelBackgroundC());
    addAndMakeVisible (listBox = new ListBox());


    //[UserPreSize]
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    //[/UserPreSize]

    setSize (220, 300);

    //[Constructor]
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

CommandPanel::~CommandPanel()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;
    listBox = nullptr;

    //[Destructor]
    //[/Destructor]
}

void CommandPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CommandPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    listBox->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CommandPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

class ColourSorter
{
public:
    static int compareElements(const String &a, const String &b)
    {
        const Colour ca = Colour::fromString(a);
        const Colour cb = Colour::fromString(b);

        if (a < b) {
            return -1;
        } else if (a > b) {
            return 1;
        } else { // if a == b
            return 0;
        }
    }
};



// Hardcoded for now
StringPairArray CommandPanel::getColoursList()
{
    StringPairArray c;
    //c.set(TRANS("colours::none"),           Colours::transparentWhite.toString());
    //c.set(TRANS("colours::black"),          Colours::black.toString());
    c.set(TRANS("colours::white"),          Colours::white.toString());
    c.set(TRANS("colours::red"),            Colours::red.toString());
    //c.set(TRANS("colours::crimson"),        Colours::crimson.toString());
    c.set(TRANS("colours::deeppink"),       Colours::deeppink.toString());
    c.set(TRANS("colours::fuchsia"),        Colours::fuchsia.toString());
    //c.set(TRANS("colours::darkviolet"),     Colours::darkviolet.toString());
    //c.set(TRANS("colours::blueviolet"),     Colours::blueviolet.toString());
    c.set(TRANS("colours::blue"),           Colours::blue.toString());
    c.set(TRANS("colours::royalblue"),      Colours::royalblue.toString());
    c.set(TRANS("colours::aqua"),           Colours::aqua.toString());
    //c.set(TRANS("colours::springgreen"),    Colours::springgreen.toString());
    c.set(TRANS("colours::lime"),           Colours::lime.toString());
    //c.set(TRANS("colours::chartreuse"),     Colours::chartreuse.toString());
    c.set(TRANS("colours::greenyellow"),    Colours::greenyellow.toString());
    c.set(TRANS("colours::gold"),           Colours::gold.toString());
    //c.set(TRANS("colours::darkorange"),     Colours::darkorange.toString());
    c.set(TRANS("colours::tomato"),         Colours::tomato.toString());
    c.set(TRANS("colours::orangered"),      Colours::orangered.toString());
    return c;
}


#define ANIM_TIME_MS 150
#define FADE_ALPHA 0.5f
#define TOPLEVEL_HEIGHT_MARGINS 170

void CommandPanel::updateContent(ReferenceCountedArray<CommandItem> commands, AnimationType animationType)
{
    const bool firstTimeUpdate = (this->commandDescriptions.size() == 0);

    this->commandDescriptions = commands;
    const int listBoxHeightOffset = this->getHeight() - this->listBox->getHeight();
    const int newHeight = firstTimeUpdate ? (commands.size() * COMMAND_PANEL_BUTTON_HEIGHT + listBoxHeightOffset) : this->getHeight();

    int maxHeight = newHeight;

    //Logger::writeToLog(String(topLevelComp->getHeight()));
    maxHeight = App::Helio()->getWindow()->getHeight() - TOPLEVEL_HEIGHT_MARGINS;

    int estimatedWidth = 0;
    ScopedPointer<CommandItemComponent> tempItem(new CommandItemComponent(nullptr, nullptr, CommandItem::empty()));
    Font stringFont(tempItem->getFont());

    for (auto && command : commands)
    {
        const int stringWidth = stringFont.getStringWidth(command->commandText) +
                                stringFont.getStringWidth(command->subText);

        if (estimatedWidth < stringWidth)
        {
            estimatedWidth = stringWidth;
        }
    }

    const int newWidth = estimatedWidth + int(COMMAND_PANEL_BUTTON_HEIGHT * 2.5f);
    this->setSize(jmax(newWidth, this->getWidth()), jmin(newHeight, maxHeight));


    if (this->listBox)
    {
        if (animationType == Fading)
        {
            this->animator.fadeOut(this->listBox, ANIM_TIME_MS);
        }
        else if (animationType == SlideLeft)
        {
            const Rectangle<int> fb(this->listBox->getBounds().translated(-this->listBox->getWidth(), 0));
            this->animator.animateComponent(this->listBox, fb, FADE_ALPHA, ANIM_TIME_MS, true, 1.0, 0.0);
        }
        else if (animationType == SlideRight)
        {
            const Rectangle<int> fb(this->listBox->getBounds().translated(this->listBox->getWidth(), 0));
            this->animator.animateComponent(this->listBox, fb, FADE_ALPHA, ANIM_TIME_MS, true, 1.0, 0.0);
        }
        else if (animationType == SlideUp)
        {
            const Rectangle<int> fb(this->listBox->getBounds().translated(0, -this->listBox->getHeight()));
            this->animator.animateComponent(this->listBox, fb, FADE_ALPHA, 200, true, 1.0, 0.0);
        }
        else if (animationType == SlideDown)
        {
            const Rectangle<int> fb(this->listBox->getBounds().translated(0, this->listBox->getHeight()));
            this->animator.animateComponent(this->listBox, fb, FADE_ALPHA, 200, true, 1.0, 0.0);
        }

        this->removeChildComponent(this->listBox);
        this->listBox = nullptr;
    }

    this->listBox = new ListBox();
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->listBox->setRowHeight(COMMAND_PANEL_BUTTON_HEIGHT);
    this->listBox->updateContent();

    this->addAndMakeVisible(this->listBox);


    if (animationType == Fading)
    {
        this->listBox->setBounds(this->getLocalBounds());
        this->animator.fadeIn(this->listBox, ANIM_TIME_MS);
    }
    else if (animationType == SlideLeft)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getLocalBounds().translated(this->getWidth(), 0));
        this->animator.animateComponent(this->listBox, this->getLocalBounds(), 1.f, ANIM_TIME_MS, true, 1.0, 0.0);
    }
    else if (animationType == SlideRight)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getLocalBounds().translated(-this->getWidth(), 0));
        this->animator.animateComponent(this->listBox, this->getLocalBounds(), 1.f, ANIM_TIME_MS, true, 1.0, 0.0);
    }
    else if (animationType == SlideUp)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getLocalBounds().translated(0, this->getHeight()));
        this->animator.animateComponent(this->listBox, this->getLocalBounds(), 1.f, ANIM_TIME_MS, true, 1.0, 0.0);
    }
    else if (animationType == SlideDown)
    {
        this->listBox->setAlpha(FADE_ALPHA);
        this->listBox->setBounds(this->getLocalBounds().translated(0, -this->getHeight()));
        this->animator.animateComponent(this->listBox, this->getLocalBounds(), 1.f, ANIM_TIME_MS, true, 1.0, 0.0);
    }
    else
    {
        this->listBox->setBounds(this->getLocalBounds());
    }
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int CommandPanel::getNumRows()
{
    return this->commandDescriptions.size();
}

void CommandPanel::paintListBoxItem(int rowNumber,
                                    Graphics &g,
                                    int width, int height,
                                    bool rowIsSelected)
{
}

Component *CommandPanel::refreshComponentForRow(int rowNumber, bool isRowSelected,
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

void CommandPanel::listWasScrolled()
{

}


//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CommandPanel" template="../../../Template"
                 componentName="" parentClasses="public Component, private ListBoxModel"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="220"
                 initialHeight="300">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
