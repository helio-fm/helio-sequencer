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

class PluginScanner;
class OrchestraPitNode;
class MenuItemComponent;
class HeadlineContextMenuController;

#include "HeadlineItemDataSource.h"
#include "SeparatorHorizontalFading.h"
#include "SeparatorHorizontalFading.h"
#include "SeparatorHorizontalFadingReversed.h"

class AudioPluginsListComponent final : public Component,
                                        public TableListBoxModel,
                                        public HeadlineItemDataSource
{
public:

    AudioPluginsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~AudioPluginsListComponent();

    void clearSelection();
    void updateListContent();
    void showScanButtonIf(bool hasNoPlugins);

    //===------------------------------------------------------------------===//
    // TableListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    var getDragSourceDescription(const SparseSet<int> &currentlySelectedRows) override;
    void paintRowBackground(Graphics &, int, int, int, bool) override;
    void paintCell(Graphics &, int, int, int, int, bool) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    int getColumnAutoSizeWidth(int columnId) override;
    String getCellTooltip(int rowNumber, int columnId) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void cellClicked(int rowNumber, int columnId, const MouseEvent&) override;

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

#if PLATFORM_DESKTOP
    static constexpr auto rowHeight = 64;
    static constexpr auto tableHeaderHeight = 34;
#elif PLATFORM_MOBILE
    static constexpr auto rowHeight = 72;
    static constexpr auto tableHeaderHeight = 40;
#endif

    enum ColumnIds
    {
        vendorAndName = 1, // TableListBox needs any number apart from 0
        category = 2,
        format = 3
    };

    UniquePointer<TableListBox> pluginsList;
    UniquePointer<MenuItemComponent> initialScanButton;
    UniquePointer<Label> titleLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> titleSeparator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginsListComponent)
};
