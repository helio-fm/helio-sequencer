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

#include "HeadlineItemDataSource.h"
#include "InstrumentNode.h"

#if HELIO_DESKTOP
#   define INSTRUMENTSLIST_ROW_HEIGHT (48)
#elif HELIO_MOBILE
#   define INSTRUMENTSLIST_ROW_HEIGHT (72)
#endif
//[/Headers]

#include "../../Themes/SeparatorHorizontalFadingReversed.h"

class InstrumentsListComponent final : public Component,
                                       public ListBoxModel,
                                       public HeadlineItemDataSource
{
public:

    InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~InstrumentsListComponent();

    //[UserMethods]

    void clearSelection();
    void updateListContent();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemDoubleClicked(int row, const MouseEvent&) override;

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

    Array<WeakReference<InstrumentNode>> instruments;
    Image instrumentIcon;
    //[/UserVariables]

    UniquePointer<ListBox> instrumentsList;
    UniquePointer<Label> titleLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> separator1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsListComponent)
};
