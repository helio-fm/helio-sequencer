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
#include "InstrumentNode.h"
#include "Instrument.h"
#include "ModalDialogInput.h"
#include "PluginScanner.h"
#include "MainLayout.h"
#include "CommandIDs.h"
#include "Icons.h"

InstrumentMenu::InstrumentMenu(InstrumentNode &instrumentNode, PluginScanner &scanner) :
    instrumentNode(instrumentNode),
    pluginScanner(scanner)
{
    this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu InstrumentMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    if (!instrumentNode.isSelected())
    {
        menu.add(MenuItem::item(Icons::routing, TRANS("menu::instrument::showeditor"))->withAction([this]()
        {
            instrumentNode.setSelected();
            this->dismiss();
        }));
    }

    menu.add(MenuItem::item(Icons::ellipsis, TRANS("menu::instrument::rename"))->withAction([this]()
    {
        auto dialog = ModalDialogInput::Presets::renameInstrument(this->instrumentNode.getName());
        dialog->onOk = this->instrumentNode.getRenameCallback();
        App::Layout().showModalComponentUnowned(dialog.release());
    }));

    // TODO:
    //menu.add(MenuItem::item(Icons::icon, TRANS("menu::instrument::seticon")));
    //menu.add(MenuItem::item(Icons::colour, TRANS("menu::instrument::setcolour")));

    menu.add(MenuItem::item(Icons::instrument, TRANS("menu::instrument::addinstrument"))->
        withSubmenu()->disabledIf(!this->pluginScanner.hasInstruments())->withAction([this]()
    {
        this->updateContent(this->createInstrumentsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::audioPlugin, TRANS("menu::instrument::addeffect"))->
        withSubmenu()->disabledIf(!this->pluginScanner.hasEffects())->withAction([this]()
    {
        this->updateContent(this->createEffectsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::remove, TRANS("menu::instrument::delete"))->withAction([this]()
    {
        TreeNode::deleteItem(&this->instrumentNode, true);
        this->dismiss();
    }));

    return menu;
}

MenuPanel::Menu InstrumentMenu::createEffectsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto description : this->pluginScanner.getList())
    {
        if (!description->isInstrument)
        {
            menu.add(MenuItem::item(Icons::audioPlugin,
                description->descriptiveName)->withAction([this, description]()
            {
                this->instrumentNode.getInstrument()->addNodeToFreeSpace(*description, [this](Instrument *instrument)
                {
                    this->instrumentNode.updateChildrenEditors();
                    this->instrumentNode.setSelected();
                });
                this->dismiss();
            }));
        }
    }

    return menu;
}

MenuPanel::Menu InstrumentMenu::createInstrumentsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto description : this->pluginScanner.getList())
    {
        if (description->isInstrument)
        {
            menu.add(MenuItem::item(Icons::audioPlugin,
                description->descriptiveName)->withAction([this, description]()
            {
                this->instrumentNode.getInstrument()->addNodeToFreeSpace(*description, [this](Instrument *instrument)
                {
                    this->instrumentNode.updateChildrenEditors();
                    this->instrumentNode.setSelected();
                });
                this->dismiss();
            }));
        }
    }

    return menu;
}
