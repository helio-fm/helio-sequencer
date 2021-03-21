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

#include "SyncSettingsItem.h"
#include "ResourceSyncService.h"
#include "ColourIDs.h"

class SyncSettingsItemHighlighter final : public Component
{
public:

    SyncSettingsItemHighlighter()
    {
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
        g.fillRoundedRectangle(32.f, 2.f, float(this->getWidth() - 37), float(this->getHeight() - 5), 2.f);
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncSettingsItemHighlighter)
};

SyncSettingsItem::SyncSettingsItem(ListBox &parentListBox) :
    DraggingListBoxComponent(parentListBox.getViewport())
{
    this->setPaintingIsUnclipped(true);

    this->separator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->separator.get());

    this->toggleButton = make<ToggleButton>();
    this->addAndMakeVisible(this->toggleButton.get());
}

SyncSettingsItem::~SyncSettingsItem() = default;

void SyncSettingsItem::resized()
{
    constexpr auto toggleSize = 24;
    this->toggleButton->setBounds(8, (this->getHeight() / 2) - (toggleSize / 2) - 1, this->getWidth() - 14, toggleSize);
    this->separator->setBounds(40, this->getHeight() - 2, this->getWidth() - 46, 2);
}

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
    return new SyncSettingsItemHighlighter();
}

#endif
