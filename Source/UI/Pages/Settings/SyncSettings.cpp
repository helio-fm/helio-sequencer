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

#include "SyncSettings.h"

//[MiscUserDefs]
#include "Workspace.h"
#include "SyncSettingsItem.h"
#include "Config.h"

#if HELIO_DESKTOP
#   define SYNC_SETTINGS_ROW_HEIGHT (32)
#elif HELIO_MOBILE
#   define SYNC_SETTINGS_ROW_HEIGHT (48)
#endif
//[/MiscUserDefs]

SyncSettings::SyncSettings()
{
    this->resourcesList.reset(new ListBox());
    this->addAndMakeVisible(resourcesList.get());


    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    this->setSize(600, 265);

    //[Constructor]

    // how to collect and update references to all resources? problems:
    //  - user profile does not store remotely-stored resources info
    // App::Workspace().getUserProfile().get

    for (auto resource : App::Config().getAllResources())
    {
        const auto resourceType = resource.first;
        const auto configs = resource.second->getUserResources<BaseResource>();
        for (const auto config : configs)
        {
            this->keys.add(resourceType + "/" + config->getResourceId());
            this->resources.add(config);
        }

        resource.second->addChangeListener(this);
    }

    App::Workspace().getUserProfile().addChangeListener(this);

    this->setSize(600, this->keys.size() * SYNC_SETTINGS_ROW_HEIGHT);

    this->resourcesList->setModel(this);
    this->resourcesList->setRowHeight(SYNC_SETTINGS_ROW_HEIGHT);
    this->resourcesList->getViewport()->setScrollBarsShown(true, false);
    //[/Constructor]
}

SyncSettings::~SyncSettings()
{
    //[Destructor_pre]
    App::Workspace().getUserProfile().removeChangeListener(this);

    for (auto resource : App::Config().getAllResources())
    {
        resource.second->removeChangeListener(this);
    }
    //[/Destructor_pre]

    resourcesList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SyncSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SyncSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    resourcesList->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void SyncSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    if (auto *profile = dynamic_cast<UserProfile *>(source))
    {
        // will update checkpoints on synced resources:
        this->resourcesList->updateContent();
    }
    else if (auto *resourceManager = dynamic_cast<ResourceManager *>(source))
    {
        // FIXME! re-read all resources? kinda overkill?
        // then update the list content as well:
        //this->resourcesList->updateContent();
    }
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *SyncSettings::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber < this->keys.size())
    {
        const bool isSynced = false; // TODO;
        const bool isLastRow = (rowNumber == this->keys.size() - 1);
        if (existingComponentToUpdate != nullptr)
        {
            if (auto *row = dynamic_cast<SyncSettingsItem *>(existingComponentToUpdate))
            {
                row->updateDescription(isLastRow, isSynced, this->keys.getReference(rowNumber));
            }
        }
        else
        {
            auto *row = new SyncSettingsItem(*this->resourcesList);
            row->updateDescription(isLastRow, isSynced, this->keys.getReference(rowNumber));
            return row;
        }
    }

    return existingComponentToUpdate;
}

int SyncSettings::getNumRows()
{
    return this->keys.size();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SyncSettings" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="265">
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="resourcesList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
