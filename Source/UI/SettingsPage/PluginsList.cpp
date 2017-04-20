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

#include "PluginsList.h"

//[MiscUserDefs]
#include "App.h"
#include "MainWindow.h"
#include "PluginManager.h"
#include "InstrumentRow.h"

//[/MiscUserDefs]

PluginsList::PluginsList(PluginManager &parentManager)
    : pluginManager(parentManager)
{
    addAndMakeVisible (pluginsList = new ListBox());


    //[UserPreSize]
    this->pluginsList->setModel(this);
    this->pluginsList->setRowHeight(PLUGINSLIST_ROW_HEIGHT);
    this->pluginsList->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->pluginsList->getViewport()->setScrollBarsShown(true, false);
    //[/UserPreSize]

    setSize (600, 160);

    //[Constructor]

    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(true);
    this->setOpaque(false);

    this->pluginManager.addChangeListener(this);

    //[/Constructor]
}

PluginsList::~PluginsList()
{
    //[Destructor_pre]
    this->pluginManager.removeChangeListener(this);
    //[/Destructor_pre]

    pluginsList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void PluginsList::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PluginsList::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    pluginsList->setBounds (10, 10, getWidth() - 20, getHeight() - 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ListBoxModel
//

Component *PluginsList::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                   Component *existingComponentToUpdate)
{
    PluginDescription *pd =
    this->pluginManager.getList().getType(this->getNumRows() - 1 - rowNumber);

    if (!pd) { return existingComponentToUpdate; }

    if (existingComponentToUpdate != nullptr)
    {
        if (InstrumentRow *row = dynamic_cast<InstrumentRow *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->refreshPluginDescription(*pd);
        }
    }
    else
    {
        InstrumentRow *row = new InstrumentRow(*pd);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int PluginsList::getNumRows()
{
    const int numTypes = this->pluginManager.getList().getNumTypes();
    return numTypes;
}


//===----------------------------------------------------------------------===//
// ChangeListener
//

void PluginsList::changeListenerCallback(ChangeBroadcaster *source)
{
    if (PluginManager *scanner = dynamic_cast<PluginManager *>(source))
    {
        this->pluginsList->updateContent();
        this->pluginsList->setSelectedRows(SparseSet<int>());
        this->pluginsList->scrollToEnsureRowIsOnscreen(this->getNumRows() - 1);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginsList" template="../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="PluginManager &amp;parentManager" variableInitialisers="pluginManager(parentManager)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="160">
  <BACKGROUND backgroundColour="4d4d4d"/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="pluginsList" virtualName=""
                    explicitFocusOrder="0" pos="10 10 20M 20M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
