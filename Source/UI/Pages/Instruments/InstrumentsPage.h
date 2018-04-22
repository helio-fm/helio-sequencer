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

struct PluginDescriptionWrapper : ReferenceCountedObject
{
    PluginDescription pluginDescription;
    typedef ReferenceCountedObjectPtr<PluginDescriptionWrapper> Ptr;
};

class PluginScanner;
class InstrumentsRootTreeItem;
class MenuItemComponent;

#if HELIO_DESKTOP
#    define PLUGINSLIST_ROW_HEIGHT (65)
#elif HELIO_MOBILE
#    define PLUGINSLIST_ROW_HEIGHT (90)
#endif

//[/Headers]

#include "../../Themes/PanelBackgroundB.h"
#include "../../Themes/FramePanel.h"
#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFading.h"

class InstrumentsPage final : public Component,
                              public TableListBoxModel,
                              public ChangeListener
{
public:

    InstrumentsPage(PluginScanner &scanner, InstrumentsRootTreeItem &instrumentsTreeItem);
    ~InstrumentsPage();

    //[UserMethods]

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

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    void showGreeting();
    void hideGreeting();

    PluginScanner &pluginScanner;

    InstrumentsRootTreeItem &instrumentsRoot;

    KnownPluginList knownPlugins;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundB> background;
    ScopedPointer<FramePanel> panel;
    ScopedPointer<TableListBox> pluginsList;
    ScopedPointer<MenuItemComponent> initialScanButton;
    ScopedPointer<SeparatorHorizontalFading> separator1;
    ScopedPointer<SeparatorHorizontalFading> separator2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsPage)
};
