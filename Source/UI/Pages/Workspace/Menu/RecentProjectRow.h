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
class WorkspaceMenu;
class IconComponent;

#include "RecentFilesList.h"
#include "DraggingListBoxComponent.h"
//[/Headers]


class RecentProjectRow  : public DraggingListBoxComponent
{
public:

    RecentProjectRow (WorkspaceMenu &parent, ListBox &parentListBox);

    ~RecentProjectRow();

    //[UserMethods]

    void setSelected(bool shouldBeSelected) override;

    void updateDescription(bool isLastRow, const RecentFileDescription::Ptr file);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    Component *createHighlighterComponent() override;

    WorkspaceMenu &parentList;
    RecentFileDescription::Ptr targetFile;
    bool isSelected;

    //[/UserVariables]

    ScopedPointer<Label> titleLabel;
    ScopedPointer<Label> dateLabel;
    ScopedPointer<IconComponent> activenessImage;
    ScopedPointer<IconComponent> remoteIndicatorImage;
    ScopedPointer<IconComponent> localIndicatorImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecentProjectRow)
};
