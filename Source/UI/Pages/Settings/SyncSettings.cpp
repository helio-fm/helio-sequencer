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
#include "ComponentsList.h"
#include "UserProfile.h"
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
    
    this->reloadConfigsList();
    this->reloadSyncFlags();

    App::Workspace().getUserProfile().addChangeListener(this);

    for (auto configType : App::Config().getAllResources())
    {
        configType.second->addChangeListener(this);
    }

    this->setSize(100, this->resources.size() * SYNC_SETTINGS_ROW_HEIGHT);

    this->resourcesList->setModel(this);
    this->resourcesList->setRowHeight(SYNC_SETTINGS_ROW_HEIGHT);
    this->resourcesList->getViewport()->setScrollBarsShown(true, false);
    //[/Constructor]
}

SyncSettings::~SyncSettings()
{
    //[Destructor_pre]
    for (auto resource : App::Config().getAllResources())
    {
        resource.second->removeChangeListener(this);
    }

    App::Workspace().getUserProfile().removeChangeListener(this);
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
        // for logging-in case:
        if (this->resources.isEmpty() && profile->isLoggedIn())
        {
            this->reloadConfigsList();
        }
        // profile might have updated its user resource list,
        // so we need to find out which ones are present:
        this->reloadSyncFlags();
        // will update checkpoints on synced resources:
        this->resourcesList->updateContent();
        // (worth noting that profile sends change messages quite frequently)
    }
    else if (auto *resourceManager = dynamic_cast<ResourceManager *>(source))
    {
        // re-read all configs (this shouldn't happen too often anyway)
        this->reloadConfigsList();
        this->reloadSyncFlags();
        // then update the list content as well:
        this->resourcesList->updateContent();
    }
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *SyncSettings::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber < this->resources.size())
    {
        const bool isSynced = this->syncFlags[rowNumber];
        const bool isLastRow = (rowNumber == this->resources.size() - 1);
        if (existingComponentToUpdate != nullptr)
        {
            if (auto *row = dynamic_cast<SyncSettingsItem *>(existingComponentToUpdate))
            {
                row->updateDescription(isLastRow, isSynced, this->resources.getUnchecked(rowNumber));
            }
        }
        else
        {
            auto *row = new SyncSettingsItem(*this->resourcesList);
            row->updateDescription(isLastRow, isSynced, this->resources.getUnchecked(rowNumber));
            return row;
        }
    }

    return existingComponentToUpdate;
}

int SyncSettings::getNumRows()
{
    return this->resources.size();
}

static ComponentsList *findParentList(Component *target)
{
    if (target == nullptr)
    {
        return nullptr;
    }

    if (auto *list = dynamic_cast<ComponentsList *>(target->getParentComponent()))
    {
        return list;
    }

    return findParentList(target->getParentComponent());
}

void SyncSettings::reloadConfigsList()
{
    this->resources.clearQuick();
    this->syncFlags.clearQuick();

    if (App::Workspace().getUserProfile().isLoggedIn())
    {
        for (auto configType : App::Config().getAllResources())
        {
            for (const auto config : configType.second->getUserResources())
            {
                this->resources.add(config);
                this->syncFlags.add(false);
            }
        }
    }

    auto *parentList = findParentList(this);
    const bool shouldShow = !this->resources.isEmpty();
    if (parentList != nullptr && !shouldShow && this->isEnabled())
    {
        parentList->hideChild(this);
    }
    else if (parentList != nullptr && shouldShow && !this->isEnabled())
    {
        this->setSize(100, this->resources.size() * SYNC_SETTINGS_ROW_HEIGHT);
        parentList->showChild(this);
    }
}

void SyncSettings::reloadSyncFlags()
{
    const auto &profile = App::Workspace().getUserProfile();
    for (int i = 0; i < this->resources.size(); ++i)
    {
        this->syncFlags.set(i,
            profile.hasSyncedConfiguration(this->resources.getUnchecked(i)->getResourceType(),
                this->resources.getUnchecked(i)->getResourceId()));
    }
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
