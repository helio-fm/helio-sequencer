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
#include "DraggingListBoxComponent.h"
#include "RevisionItem.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalFading.h"

class RevisionItemComponent final : public DraggingListBoxComponent
{
public:

    RevisionItemComponent(ListBox &parentListBox);
    ~RevisionItemComponent();

    //[UserMethods]

    void updateItemInfo(VCS::RevisionItem::Ptr revisionItem,
        int rowNumber, bool isLastRow, bool isSelectable);

    void select() const;
    void deselect() const;

    VCS::RevisionItem::Ptr getRevisionItem() const noexcept;

    void setSelected(bool shouldBeSelected) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    mutable ComponentAnimator selectionAnimator;

    void invertSelection() const;
    bool isSelected() const;

    ListBox &list;
    int row;

    ScopedPointer<Component> selectionComponent;
    VCS::RevisionItem::Ptr revisionItem;

    //[/UserVariables]

    UniquePointer<Label> itemLabel;
    UniquePointer<Label> deltasLabel;
    UniquePointer<SeparatorHorizontalFading> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RevisionItemComponent)
};
