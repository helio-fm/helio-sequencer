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
class OrchestraPitTreeItem;

#include "HeadlineItemDataSource.h"
//[/Headers]

#include "../../Themes/FramePanel.h"

class InstrumentsListComponent final : public Component,
                                       public ListBoxModel,
                                       public HeadlineItemDataSource
{
public:

    InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitTreeItem &instrumentsRoot);
    ~InstrumentsListComponent();

    //[UserMethods]

    void updateListContent();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component*) override;
    void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override {}
    void listBoxItemClicked(int rowNumber, const MouseEvent &e) override;

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


private:

    //[UserVariables]
    PluginScanner &pluginScanner;
    OrchestraPitTreeItem &instrumentsRoot;
    //[/UserVariables]

    ScopedPointer<FramePanel> panel;
    ScopedPointer<ListBox> instrumentsList;
    ScopedPointer<Label> titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsListComponent)
};
