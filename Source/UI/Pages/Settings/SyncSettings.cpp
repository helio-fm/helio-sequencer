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

#include "Common.h"

#if !NO_NETWORK

#include "SyncSettings.h"
#include "Workspace.h"
#include "SyncSettingsItem.h"
#include "ComponentsList.h"
#include "Config.h"

SyncSettings::SyncSettings()
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->resourcesList = make<ListBox>();
    this->addAndMakeVisible(this->resourcesList.get());
    
    this->reloadConfigsList();
    this->reloadSyncFlags();

    App::Workspace().getUserProfile().addChangeListener(this);

    for (auto configType : App::Config().getAllResources())
    {
        configType.second->addChangeListener(this);
    }

    this->setSize(100, this->resources.size() * SyncSettings::rowHeight);

    this->resourcesList->setModel(this);
    this->resourcesList->setRowHeight(SyncSettings::rowHeight);
    this->resourcesList->getViewport()->setScrollBarsShown(true, false);
}

SyncSettings::~SyncSettings()
{
    for (auto resource : App::Config().getAllResources())
    {
        resource.second->removeChangeListener(this);
    }

    App::Workspace().getUserProfile().removeChangeListener(this);
}

void SyncSettings::resized()
{
    this->resourcesList->setBounds(this->getLocalBounds());
}

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
    else if (dynamic_cast<ConfigurationResourceCollection *>(source))
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
            for (const auto &config : configType.second->getUserResources())
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
        this->setSize(100, this->resources.size() * SyncSettings::rowHeight);
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

// Common network settings
// todo in future: set custom api url here?

NetworkSettings::NetworkSettings()
{
    this->checkForUpdatesButton = make<ToggleButton>(TRANS(I18n::Settings::checkForUpdates));
    this->addAndMakeVisible(this->checkForUpdatesButton.get());

    this->checkForUpdatesButton->setToggleState(App::Config().isUpdatesCheckEnabled(), dontSendNotification);

    this->checkForUpdatesButton->onStateChange = [this]
    {
        App::Config().setUpdatesCheckEnabled(this->checkForUpdatesButton->getToggleState());
    };

    this->setSize(40, 40);
}

void NetworkSettings::resized()
{
    this->checkForUpdatesButton->setBounds(this->getLocalBounds().reduced(12, 0));
}

#endif
