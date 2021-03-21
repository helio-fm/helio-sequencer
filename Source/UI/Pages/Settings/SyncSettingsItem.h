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

#pragma once

#if !NO_NETWORK

#include "BaseResource.h"
#include "DraggingListBoxComponent.h"
#include "SeparatorHorizontal.h"

class SyncSettingsItem final : public DraggingListBoxComponent
{
public:

    explicit SyncSettingsItem(ListBox &parentListBox);
    ~SyncSettingsItem();

    void setSelected(bool shouldBeSelected) override;
    void updateDescription(bool isLastRowInList, bool isSynced, const BaseResource::Ptr resource);

    void resized() override;

private:

    Component *createHighlighterComponent() override;
    BaseResource::Ptr resource;

    UniquePointer<SeparatorHorizontal> separator;
    UniquePointer<ToggleButton> toggleButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncSettingsItem)
};

#endif
