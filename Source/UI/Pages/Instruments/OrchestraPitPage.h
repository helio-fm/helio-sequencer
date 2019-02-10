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
//[/Headers]

#include "../../Themes/SeparatorVerticalSkew.h"
#include "../../Themes/PanelBackgroundA.h"
#include "../../Themes/PanelBackgroundB.h"
#include "AudioPluginsListComponent.h"
#include "InstrumentsListComponent.h"

class OrchestraPitPage final : public Component,
                               public ChangeListener
{
public:

    OrchestraPitPage(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~OrchestraPitPage();

    //[UserMethods]
    void onStageSelectionChanged();
    void onPluginsSelectionChanged();

    void changeListenerCallback(ChangeBroadcaster *source) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    PluginScanner &pluginScanner;
    OrchestraPitNode &instrumentsRoot;
    //[/UserVariables]

    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<PanelBackgroundA> backgroundA;
    UniquePointer<PanelBackgroundB> backgroundB;
    UniquePointer<AudioPluginsListComponent> pluginsList;
    UniquePointer<InstrumentsListComponent> instrumentsList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrchestraPitPage)
};
