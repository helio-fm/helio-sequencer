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
#include "InstrumentsCommandPanel.h"
#include "InstrumentsRootTreeItem.h"
#include "Icons.h"
#include "CommandIDs.h"
#include "App.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Document.h"
#include "PluginManager.h"
#include "Workspace.h"
#include "App.h"

InstrumentsCommandPanel::InstrumentsCommandPanel(InstrumentsRootTreeItem &parentOrchestra) :
    instrumentsRoot(parentOrchestra)
{
    CommandPanel::Items cmds;

    const bool pluginsAreCurrentlyScanning = App::Workspace().getPluginManager().isWorking();

    if (!pluginsAreCurrentlyScanning)
    {
        cmds.add(CommandItem::withParams(Icons::reset, CommandIDs::ScanAllPlugins, TRANS("menu::instruments::reload")));
    }
    
    cmds.add(CommandItem::withParams(Icons::open, CommandIDs::ScanPluginsFolder, TRANS("menu::instruments::scanfolder")));
    
    const KnownPluginList &info = App::Workspace().getPluginManager().getList();
    
    for (int i = 0; i < info.getNumTypes(); ++i)
    {
        const PluginDescription *pd = info.getType(i);
        cmds.add(CommandItem::withParams(Icons::create, CommandIDs::CreateInstrument + i, TRANS("menu::instruments::add") + " " + pd->descriptiveName));
    }
    
    this->updateContent(cmds);
}

InstrumentsCommandPanel::~InstrumentsCommandPanel()
{
}

void InstrumentsCommandPanel::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::ScanAllPlugins:
            App::Workspace().getPluginManager().runInitialScan();
            break;

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

    const KnownPluginList &info = App::Workspace().getPluginManager().getList();
    
    for (int i = 0; i < info.getNumTypes(); ++i)
    {
        if (commandId == CommandIDs::CreateInstrument + i)
        {
            const PluginDescription pluginDescription(*info.getType(i));
            
            App::Workspace().getAudioCore().
                addInstrument(pluginDescription, pluginDescription.descriptiveName, [this](Instrument *instrument)
            {
                this->instrumentsRoot.addInstrumentTreeItem(instrument);
            });
        }
    }
    
    this->getParentComponent()->exitModalState(0);
}
