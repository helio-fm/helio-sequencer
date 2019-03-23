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

#include "SyncSettingsItem.h"

//[MiscUserDefs]
#include "SettingsListItemHighlighter.h"
#include "SettingsListItemSelection.h"
#include "ResourceSyncService.h"
#include "Network.h"
#include "Icons.h"
//[/MiscUserDefs]

SyncSettingsItem::SyncSettingsItem(ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport())
{
    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());
    this->toggleButton.reset(new ToggleButton(String()));
    this->addAndMakeVisible(toggleButton.get());
    toggleButton->setButtonText(TRANS("..."));
    toggleButton->addListener(this);


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(350, 32);

    //[Constructor]
    //[/Constructor]
}

SyncSettingsItem::~SyncSettingsItem()
{
    //[Destructor_pre]
    //this->selectionComponent = nullptr;
    //[/Destructor_pre]

    separator = nullptr;
    toggleButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SyncSettingsItem::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SyncSettingsItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    separator->setBounds(40, getHeight() - 2, getWidth() - 46, 2);
    toggleButton->setBounds(8, (getHeight() / 2) + -1 - (24 / 2), getWidth() - 14, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SyncSettingsItem::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == toggleButton.get())
    {
        //[UserButtonCode_toggleButton] -- add your button handler code here..
        //[/UserButtonCode_toggleButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]

void SyncSettingsItem::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        const bool newSyncState = !this->toggleButton->getToggleState();
        auto *syncService = App::Network().getResourceSyncService();
        if (newSyncState)
        {
            syncService->queueSync(this->resource);
        }
        else
        {
            syncService->queueDelete(this->resource);
        }
        this->toggleButton->setToggleState(newSyncState, dontSendNotification);
    }
}

void SyncSettingsItem::updateDescription(bool isLastRowInList, bool isSynced, const BaseResource::Ptr resource)
{
    this->resource = resource;
    this->separator->setVisible(!isLastRowInList);
    this->toggleButton->setButtonText(resource->getResourceType().toString() + "/" + resource->getResourceId());
    this->toggleButton->setToggleState(isSynced, dontSendNotification);
}

Component *SyncSettingsItem::createHighlighterComponent()
{
    return new SettingsListItemHighlighter();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SyncSettingsItem" template="../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="ListBox &amp;parentListBox" variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport())"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="6f5a73e394d91c2a" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="40 0Rr 46M 2" sourceFile="../../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <TOGGLEBUTTON name="" id="d15a0d8489a53bdd" memberName="toggleButton" virtualName=""
                explicitFocusOrder="0" pos="8 -1Cc 14M 24" buttonText="..." connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
