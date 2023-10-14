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

class PluginScanner;
class OrchestraPitNode;
class HeadlineContextMenuController;

#include "HeadlineItemDataSource.h"
#include "InstrumentNode.h"
#include "SeparatorHorizontalFadingReversed.h"

class InstrumentsListComponent final : public Component,
                                       public ListBoxModel,
                                       public HeadlineItemDataSource
{
public:

    InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~InstrumentsListComponent();

    void clearSelection();
    void updateListContent();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemClicked(int row, const MouseEvent&) override;
    void listBoxItemDoubleClicked(int row, const MouseEvent&) override;

    //===------------------------------------------------------------------===//
    // HeadlineItemDataSource
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;
    Image getIcon() const override;
    String getName() const override;
    bool canBeSelectedAsMenuItem() const override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void parentHierarchyChanged() override;

private:

    PluginScanner &pluginScanner;
    OrchestraPitNode &instrumentsRoot;

    UniquePointer<HeadlineContextMenuController> contextMenuController;

    Array<WeakReference<InstrumentNode>> instruments;
    Image instrumentIcon;

#if PLATFORM_DESKTOP
    static constexpr auto rowHeight = 48;
#elif PLATFORM_MOBILE
    static constexpr auto rowHeight = 56;
#endif

    static constexpr auto iconSize = rowHeight * 0.65f;

    UniquePointer<ListBox> instrumentsList;
    UniquePointer<Label> titleLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsListComponent)
};
