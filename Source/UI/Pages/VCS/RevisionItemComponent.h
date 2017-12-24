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
#include "Head.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontal.h"

class RevisionItemComponent  : public DraggingListBoxComponent
{
public:

    RevisionItemComponent (ListBox &parentListBox, VCS::Head &owner);

    ~RevisionItemComponent();

    //[UserMethods]

    void updateItemInfo(int rowNumber, bool isLastRow, VCS::RevisionItem::Ptr revisionItemInfo);

    void select() const;

    void deselect() const;

    VCS::RevisionItem::Ptr getRevisionItem()
    {
        return this->revisionItem;
    }

    void setSelected(bool shouldBeSelected) override
    {
        this->invertSelection();
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

//    virtual Component *createHighlighterComponent() override;

    mutable ComponentAnimator selectionAnimator;

    void invertSelection() const;

    bool isSelected() const;

    ListBox &list;

    int row;

    ScopedPointer<Component> selectionComponent;


    VCS::RevisionItem::Ptr revisionItem;

    VCS::Head &head;


    WeakReference<RevisionItemComponent>::Master masterReference;

    friend class WeakReference<RevisionItemComponent>;

    //[/UserVariables]

    ScopedPointer<Label> itemLabel;
    ScopedPointer<Label> deltasLabel;
    ScopedPointer<SeparatorHorizontal> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RevisionItemComponent)
};
