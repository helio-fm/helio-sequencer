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

class PluginManager;
class InstrumentsRootTreeItem;
class CommandItemComponent;

//[/Headers]

#include "../Themes/PanelBackgroundB.h"
#include "../Themes/FramePanel.h"
#include "../Themes/LightShadowDownwards.h"
#include "../Themes/SeparatorHorizontalFading.h"
#include "../Themes/SeparatorHorizontalFading.h"

class InstrumentsPage  : public Component,
                         public ListBoxModel,
                         public ChangeListener,
                         public ButtonListener
{
public:

    InstrumentsPage (PluginManager &scanner, InstrumentsRootTreeItem &instrumentsTreeItem);

    ~InstrumentsPage();

    //[UserMethods]

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
            Component *existingComponentToUpdate) override;

    String getTooltipForRow(int row) override;

    var getDragSourceDescription(const SparseSet<int> &currentlySelectedRows) override;

    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics &g,
                                  int width, int height, bool rowIsSelected) override;


    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    void showGreeting();
    void hideGreeting();

    PluginManager &pluginManager;

    InstrumentsRootTreeItem &instrumentsRoot;

    KnownPluginList knownPlugins;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundB> background;
    ScopedPointer<FramePanel> panel;
    ScopedPointer<ListBox> pluginsList;
    ScopedPointer<TextButton> initButton;
    ScopedPointer<TextButton> removeButton;
    ScopedPointer<TextButton> scanButton;
    ScopedPointer<LightShadowDownwards> shadow;
    ScopedPointer<CommandItemComponent> initialScanButton;
    ScopedPointer<SeparatorHorizontalFading> separator1;
    ScopedPointer<SeparatorHorizontalFading> separator2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentsPage)
};
