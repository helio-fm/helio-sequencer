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

#include "Revision.h"
#include "RevisionItem.h"
#include "SeparatorHorizontal.h"

class RevisionTooltipComponent final : public Component, public ListBoxModel
{
public:

    explicit RevisionTooltipComponent(const VCS::Revision::Ptr revision);

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int row, bool isRowSelected, Component *c) override;

    void listBoxItemClicked(int, const MouseEvent &) override {}
    void listBoxItemDoubleClicked(int, const MouseEvent &) override {}
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}

    void resized() override;
    void inputAttemptWhenModal() override;

private:

    void hide();

#if PLATFORM_DESKTOP
    static constexpr auto numRowsOnScreen = 5;
    static constexpr auto rowHeight = 65;
#elif PLATFORM_MOBILE
    static constexpr auto numRowsOnScreen = 4;
    static constexpr auto rowHeight = 50;
#endif

    const VCS::Revision::Ptr revision;
    ReferenceCountedArray<VCS::RevisionItem> revisionItemsOnly;

    UniquePointer<ListBox> changesList;
    UniquePointer<SeparatorHorizontal> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionTooltipComponent)
};
