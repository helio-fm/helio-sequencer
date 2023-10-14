/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class HeadlineContextMenuController;

#include "RevisionItem.h"
#include "DraggingListBoxComponent.h"
#include "SeparatorHorizontal.h"

class RevisionItemComponent final : public DraggingListBoxComponent
{
public:

    explicit RevisionItemComponent(ListBox &parentListBox);

    ~RevisionItemComponent();

    void updateItemInfo(VCS::RevisionItem::Ptr revisionItem,
        int rowNumber, bool isLastRow, bool isSelectable);

    void select() const;
    void deselect() const;

    VCS::RevisionItem::Ptr getRevisionItem() const noexcept;

    void setSelected(bool shouldBeSelected) override;

    void mouseUp(const MouseEvent &event) override;
    void resized() override;

private:

    mutable ComponentAnimator selectionAnimator;

    void invertSelection() const;
    bool isSelected() const;

    ListBox &list;
    int row = 0;

    UniquePointer<HeadlineContextMenuController> contextMenuController;
    UniquePointer<Component> selectionComponent;
    VCS::RevisionItem::Ptr revisionItem;

    UniquePointer<Label> itemLabel;
    UniquePointer<Label> deltasLabel;
    UniquePointer<SeparatorHorizontal> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionItemComponent)
};
