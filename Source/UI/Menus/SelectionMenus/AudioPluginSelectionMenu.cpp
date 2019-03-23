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
#include "AudioPluginSelectionMenu.h"
#include "OrchestraPitNode.h"
#include "InstrumentNode.h"
#include "PluginScanner.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "CommandIDs.h"
#include "Icons.h"

AudioPluginSelectionMenu::AudioPluginSelectionMenu(const PluginDescription pd,
    OrchestraPitNode &orchestraNode, PluginScanner &scanner) :
    pluginDescription(pd),
    orchestraNode(orchestraNode),
    pluginScanner(scanner)
{
    this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu AudioPluginSelectionMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::create, TRANS("menu::selection::plugin::init"))->withAction([this]()
    {
        App::Workspace().getAudioCore().addInstrument(this->pluginDescription,
            this->pluginDescription.descriptiveName,
            [this](Instrument *instrument)
        {
            this->orchestraNode.addInstrumentTreeItem(instrument);
        });
        this->dismiss();
    }));

    const auto instruments = this->orchestraNode.findChildrenOfType<InstrumentNode>();
    menu.add(MenuItem::item(Icons::audioPlugin, TRANS("menu::selection::plugin::plug"))->
        withSubmenu()->disabledIf(instruments.isEmpty())->withAction([this]()
    {
        this->updateContent(this->createInstrumentsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::remove, TRANS("menu::selection::plugin::remove"))->withAction([this]()
    {
        this->pluginScanner.removeItem(this->pluginDescription);
        this->dismiss();
    }));

    return menu;
}

MenuPanel::Menu AudioPluginSelectionMenu::createInstrumentsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    const auto instruments = this->orchestraNode.findChildrenOfType<InstrumentNode>();
    for (const auto instrumentNode : instruments)
    {
        menu.add(MenuItem::item(Icons::instrument, instrumentNode->getName())->withAction([this, instrumentNode]()
        {
            instrumentNode->getInstrument()->addNodeToFreeSpace(this->pluginDescription,
                [this, instrumentNode](Instrument *instrument)
                {
                    instrumentNode->updateChildrenEditors();
                });

            this->dismiss();
        }));
    }

    return menu;
}
