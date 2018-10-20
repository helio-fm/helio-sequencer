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

//[Headers]
class DashboardMenu;
class IconComponent;

#include "RecentProjectInfo.h"
#include "DraggingListBoxComponent.h"
//[/Headers]


class RecentProjectRow final : public DraggingListBoxComponent
{
public:

    RecentProjectRow(DashboardMenu &parent, ListBox &parentListBox);
    ~RecentProjectRow();

    //[UserMethods]
    void setSelected(bool shouldBeSelected) override;
    void updateDescription(const RecentProjectInfo::Ptr file, bool isLoaded, bool isLastRow);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    Component *createHighlighterComponent() override;

    DashboardMenu &parentList;
    RecentProjectInfo::Ptr targetFile;
    bool isFileLoaded;
    bool isSelected;

    //[/UserVariables]

    UniquePointer<Label> titleLabel;
    UniquePointer<Label> dateLabel;
    UniquePointer<IconComponent> activenessImage;
    UniquePointer<IconComponent> remoteIndicatorImage;
    UniquePointer<IconComponent> localIndicatorImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecentProjectRow)
};
