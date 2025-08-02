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

    //===------------------------------------------------------------------===//
    // TableListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    void paintRowBackground(Graphics &, int, int, int, bool) override;
    void paintCell(Graphics &, int, int, int, int, bool) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    int getColumnAutoSizeWidth(int columnId) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void cellClicked(int rowNumber, int columnId, const MouseEvent &) override;

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
    static constexpr auto rowHeight = 60;
    static constexpr auto viewportScrollBarWidth = 2;
#elif PLATFORM_MOBILE
    static constexpr auto rowHeight = 64;
    static constexpr auto viewportScrollBarWidth = 32;
#endif

    static constexpr auto tableHeaderHeight = 40;

    enum ColumnIds
    {
        category = 1, // TableListBox needs any number apart from 0
        vendorAndName = 2,
        format = 3
    };

    Array<PluginDescription> pluginDescriptions;
    Array<PluginDescription> filteredDescriptions;
    void updateFilteredDescriptions();

    UniquePointer<TextEditor> searchTextEditor;
    UniquePointer<TableListBox> pluginsList;
    UniquePointer<Label> titleLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> titleSeparator;

    UniquePointer<MenuItemComponent> initialScanButton1;
    UniquePointer<MenuItemComponent> initialScanButton2;
    void showDefaultScanButtonsIf(bool shouldBeVisible);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginsListComponent)
};
