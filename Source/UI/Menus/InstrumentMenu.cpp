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
#include "InstrumentMenu.h"
#include "InstrumentTreeItem.h"
#include "Instrument.h"
#include "ModalDialogInput.h"
#include "PluginScanner.h"
#include "MainLayout.h"
#include "CommandIDs.h"
#include "Icons.h"
#include "App.h"

InstrumentMenu::InstrumentMenu(InstrumentTreeItem &instrumentNode, PluginScanner &scanner) :
    instrumentNode(instrumentNode),
    pluginScanner(scanner)
{
    this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu InstrumentMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    // TODO:
    //menu.add(MenuItem::item(Icons::reset, TRANS("menu::instrument::update")));
    //menu.add(MenuItem::item(Icons::?current, TRANS("menu::instrument::seticon")));
    //menu.add(MenuItem::item(Icons::colour, TRANS("menu::instrument::setcolour")));

    menu.add(MenuItem::item(Icons::ellipsis, TRANS("menu::instrument::rename"))->withAction([this]()
    {
        auto dialog = ModalDialogInput::Presets::renameInstrument(this->instrumentNode.getName());
        dialog->onOk = this->instrumentNode.getRenameCallback();
        App::Layout().showModalComponentUnowned(dialog.release());
        this->dismiss();
    }));

    menu.add(MenuItem::item(Icons::trash, TRANS("menu::instrument::delete"))->withAction([this]()
    {
        TreeItem::deleteItem(&this->instrumentNode);
        this->dismiss();
    }));

    bool hasEffects = false;
    for (const auto description : this->pluginScanner.getList())
    {
        if (!description->isInstrument)
        {
            hasEffects = true;
            break;
        }
    }

    menu.add(MenuItem::item(Icons::instrumentGraph, TRANS("menu::instrument::addeffect"))->
        withSubmenu()->withTimer()->disabledIf(!hasEffects)->withAction([this]()
    {
        this->updateContent(this->createEffectsMenu(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu InstrumentMenu::createEffectsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::left, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto description : this->pluginScanner.getList())
    {
        if (!description->isInstrument)
        {
            menu.add(MenuItem::item(Icons::instrumentGraph,
                description->descriptiveName)->withAction([this, description]()
            {
                this->instrumentNode.getInstrument()->addNodeToFreeSpace(*description, [this](Instrument *instrument)
                {
                    this->instrumentNode.updateChildrenEditors();
                });
                this->dismiss();
            }));
        }
    }

    return menu;
}
