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

#include "AudioPluginsListComponent.h"
#include "InstrumentsListComponent.h"
#include "SeparatorVerticalSkew.h"
#include "PageBackgroundA.h"
#include "PageBackgroundB.h"

class OrchestraPitPage final : public Component,  public ChangeListener
{
public:

    OrchestraPitPage(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot);
    ~OrchestraPitPage();

    void onStageSelectionChanged();
    void onPluginsSelectionChanged();

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void resized() override;
    void handleCommandMessage(int commandId) override;

private:

    PluginScanner &pluginScanner;
    OrchestraPitNode &instrumentsRoot;

    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<PageBackgroundA> backgroundA;
    UniquePointer<PageBackgroundB> backgroundB;
    UniquePointer<AudioPluginsListComponent> pluginsList;
    UniquePointer<InstrumentsListComponent> instrumentsList;

    UniquePointer<FileChooser> scanFolderFileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrchestraPitPage)
};
