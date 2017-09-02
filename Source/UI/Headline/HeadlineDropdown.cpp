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

#include "HeadlineDropdown.h"

//[MiscUserDefs]
#include "TreeItem.h"
#include "CommandIDs.h"
#include "Icons.h"
//[/MiscUserDefs]

HeadlineDropdown::HeadlineDropdown(Array<TreeItem *> treeItems)
    : items(treeItems)
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (256, 128);

    //[Constructor]
    ReferenceCountedArray<CommandItem> cmds;

    for (int i = 0; i < this->items.size(); ++i)
    {
        Logger::writeToLog(this->items[i]->getCaption());

        cmds.add(CommandItem::withParams(this->items[i]->getIcon(),
            CommandIDs::HeadlineSelectSubitem + i,
            this->items[i]->getCaption()));
    }

    this->updateContent(cmds);
    //[/Constructor]
}

HeadlineDropdown::~HeadlineDropdown()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void HeadlineDropdown::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    CommandPanel::paint(g);
    //[/UserPaint]
}

void HeadlineDropdown::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    CommandPanel::resized();
    //[/UserResized]
}

void HeadlineDropdown::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    for (int i = 0; i < this->items.size(); ++i)
    {
        if (commandId == CommandIDs::HeadlineSelectSubitem + i)
        {
            this->items[i]->setSelected(true, true);
            jassert(this->items[i]);
        }
    }

    this->getParentComponent()->exitModalState(0);
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineDropdown" template="../../Template"
                 componentName="" parentClasses="public CommandPanel" constructorParams="Array&lt;TreeItem *&gt; treeItems"
                 variableInitialisers="items(treeItems)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="256"
                 initialHeight="128">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
