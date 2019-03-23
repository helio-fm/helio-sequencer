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

#include "Common.h"
#include "OrchestraPitMenu.h"
#include "OrchestraPitNode.h"
#include "Icons.h"
#include "CommandIDs.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Document.h"
#include "PluginScanner.h"
#include "Workspace.h"

OrchestraPitMenu::OrchestraPitMenu(OrchestraPitNode &parentOrchestra) :
    instrumentsRoot(parentOrchestra)
{
    MenuPanel::Menu cmds;

    const bool pluginsAreCurrentlyScanning = App::Workspace().getPluginManager().isWorking();

    if (!pluginsAreCurrentlyScanning)
    {
        cmds.add(MenuItem::item(Icons::reset, CommandIDs::ScanAllPlugins, TRANS("menu::instruments::reload")));
    }
    
    cmds.add(MenuItem::item(Icons::browse, CommandIDs::ScanPluginsFolder, TRANS("menu::instruments::scanfolder")));
        
    this->updateContent(cmds, MenuPanel::SlideRight);
}

void OrchestraPitMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::ScanPluginsFolder:
            
#if HELIO_DESKTOP
            FileChooser fc(TRANS("dialog::scanfolder::caption"),
                           File::getCurrentWorkingDirectory(), ("*.*"), true);
            
            if (fc.browseForDirectory())
            {
                App::Workspace().getPluginManager().scanFolderAndAddResults(fc.getResult());
            }
#endif
            
            break;
    }
}
