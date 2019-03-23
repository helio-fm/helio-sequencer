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
class PluginScanner;
class OrchestraPitNode;
class MenuItemComponent;

#include "HeadlineItemDataSource.h"

#if HELIO_DESKTOP
#   define PLUGINSLIST_ROW_HEIGHT (64)
#   define PLUGINSLIST_HEADER_HEIGHT (34)
#elif HELIO_MOBILE
#   define PLUGINSLIST_ROW_HEIGHT (90)
#   define PLUGINSLIST_HEADER_HEIGHT (40)
#endif
//[/Headers]

#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFadingReversed.h"

class AudioPluginsListComponent final : public Component,
                                        public TableListBoxModel,
                                        public HeadlineItemDataSource
{
public:

    AudioPluginsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~AudioPluginsListComponent();

    //[UserMethods]

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

    //===------------------------------------------------------------------===//
    // HeadlineItemDataSource
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;
    Image getIcon() const override;
    String getName() const override;
    bool canBeSelectedAsMenuItem() const override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;


private:

    //[UserVariables]
    PluginScanner &pluginScanner;
    OrchestraPitNode &instrumentsRoot;
    //[/UserVariables]

    UniquePointer<TableListBox> pluginsList;
    UniquePointer<MenuItemComponent> initialScanButton;
    UniquePointer<SeparatorHorizontalFading> separator1;
    UniquePointer<SeparatorHorizontalFading> separator2;
    UniquePointer<Label> titleLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> separator3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginsListComponent)
};
