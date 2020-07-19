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

    const auto instrument = this->instrumentNode.getInstrument();

    if (!this->instrumentNode.isSelected())
    {
        menu.add(MenuItem::item(Icons::routing,
            TRANS(I18n::Menu::instrumentShowEditor))->
            disabledIf(!instrument->isValid())->
            closesMenu()->
            withAction([this]()
            {
                instrumentNode.setSelected();
            }));
    }

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::instrumentRename))->
        disabledIf(!instrument->isValid())->
        withAction([this]()
        {
            auto dialog = ModalDialogInput::Presets::renameInstrument(this->instrumentNode.getName());
            dialog->onOk = this->instrumentNode.getRenameCallback();
            App::showModalComponent(move(dialog));
        }));

    // TODO:
    //menu.add(MenuItem::item(Icons::icon, TRANS(I18n::Menu::instrument::seticon)));
    //menu.add(MenuItem::item(Icons::colour, TRANS(I18n::Menu::instrument::setcolour)));

    menu.add(MenuItem::item(Icons::instrument,
        TRANS(I18n::Menu::instrumentAdd))->
        withSubmenu()->
        disabledIf(!this->pluginScanner.hasInstruments())->
        withAction([this]()
        {
            this->updateContent(this->createInstrumentsMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::audioPlugin,
        TRANS(I18n::Menu::instrumentAddEffect))->
        withSubmenu()->
        disabledIf(!this->pluginScanner.hasEffects() || !instrument->isValid())->
        withAction([this]()
        {
            this->updateContent(this->createEffectsMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::remove,
        TRANS(I18n::Menu::instrumentDelete))->
        closesMenu()->
        withAction([this]()
        {
            this->instrumentNode.removeFromOrchestraAndDelete();
        }));

    return menu;
}

MenuPanel::Menu InstrumentMenu::createEffectsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &description : this->pluginScanner.getPlugins())
    {
        if (!description.isInstrument)
        {
            menu.add(MenuItem::item(Icons::audioPlugin,
                description.descriptiveName)->
                closesMenu()->
                withAction([this, description]()
                {
                    this->instrumentNode.getInstrument()->addNodeToFreeSpace(description, [this](Instrument *instrument)
                    {
                        this->instrumentNode.updateChildrenEditors();
                        this->instrumentNode.setSelected();
                    });
                }));
        }
    }

    return menu;
}

MenuPanel::Menu InstrumentMenu::createInstrumentsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &description : this->pluginScanner.getPlugins())
    {
        if (description.isInstrument)
        {
            menu.add(MenuItem::item(Icons::audioPlugin,
                description.descriptiveName)->
                closesMenu()->
                withAction([this, description]()
                {
                    this->instrumentNode.getInstrument()->addNodeToFreeSpace(description, [this](Instrument *instrument)
                    {
                        this->instrumentNode.updateChildrenEditors();
                        this->instrumentNode.setSelected();
                    });
                }));
        }
    }

    return menu;
}
