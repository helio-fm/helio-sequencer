/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "AudioPluginSelectionMenu.h"
#include "OrchestraPitNode.h"
#include "PluginScanner.h"
#include "Workspace.h"
#include "AudioCore.h"

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

    menu.add(MenuItem::item(Icons::create, TRANS(I18n::Menu::Selection::pluginInit))->
        closesMenu()->
        withAction([this]()
    {
        App::Workspace().getAudioCore().addInstrument(this->pluginDescription,
            this->pluginDescription.descriptiveName, {});
    }));

    const auto instruments = this->orchestraNode.findChildrenOfType<InstrumentNode>();
    menu.add(MenuItem::item(Icons::audioPlugin, TRANS(I18n::Menu::Selection::pluginPlug))->
        withSubmenu()->
        disabledIf(instruments.isEmpty())->
        closesMenu()->
        withAction([this]()
    {
        this->updateContent(this->createInstrumentsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::remove, TRANS(I18n::Menu::Selection::pluginRemove))->withAction([this]()
    {
        this->pluginScanner.removePlugin(this->pluginDescription);
    }));

    return menu;
}

MenuPanel::Menu AudioPluginSelectionMenu::createInstrumentsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->
        withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    const auto instruments = this->orchestraNode.findChildrenOfType<InstrumentNode>();
    for (auto *instrumentNode : instruments)
    {
        menu.add(MenuItem::item(Icons::instrument, instrumentNode->getName())->
            closesMenu()->
            withAction([this, instrumentNode = WeakReference<InstrumentNode>(instrumentNode)]()
        {
            jassert(instrumentNode != nullptr);
            instrumentNode->getInstrument()->addNodeToFreeSpace(this->pluginDescription,
                [instrumentNode](Instrument *instrument)
                {
                    if (instrumentNode == nullptr)
                    {
                        jassertfalse;
                        return;
                    }

                    instrumentNode->recreateChildrenEditors();
                    instrumentNode->notifyOrchestraChanged(); // will update the pit page
                });
        }));
    }

    return menu;
}
