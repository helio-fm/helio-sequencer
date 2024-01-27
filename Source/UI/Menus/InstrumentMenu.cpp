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
#include "InstrumentMenu.h"
#include "InstrumentNode.h"
#include "Instrument.h"
#include "ModalDialogInput.h"
#include "PluginScanner.h"
#include "OrchestraPit.h"
#include "PluginWindow.h"

InstrumentMenu::InstrumentMenu(InstrumentNode &instrumentNode,
    PluginScanner &scanner, OrchestraPit &pit) :
    instrumentNode(instrumentNode),
    pluginScanner(scanner),
    pit(pit)
{
    this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu InstrumentMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    const auto instrument = this->instrumentNode.getInstrument();

    if (auto mainNode = instrument->findMainPluginNode())
    {
        const auto hasEditor = mainNode->getProcessor()->hasEditor();
        menu.add(MenuItem::item(Icons::instrument,
            TRANS(I18n::Menu::instrumentShowWindow))->
            disabledIf(!hasEditor)->
            closesMenu()->
            withAction([instrument]()
            {
                PluginWindow::showWindowFor(instrument->getIdAndHash());
            }));
    }

    if (!this->instrumentNode.isSelected()) // isSelectedOrHasSelectedChild() ?
    {
        menu.add(MenuItem::item(Icons::routing,
            TRANS(I18n::Menu::instrumentShowEditor))->
            disabledIf(!instrument->isValid())->
            closesMenu()->
            withAction([this]()
            {
                this->instrumentNode.setSelected();
            }));
    }

    if (!App::isRunningOnPhone() && // no idea how to make this page usable on phones yet(
        !this->instrumentNode.hasSelectedChildOfType<KeyboardMappingNode>())
    {
        menu.add(MenuItem::item(Icons::piano,
            TRANS(I18n::Menu::keyboardMappingEdit))->
            disabledIf(!instrument->isValid())->
            closesMenu()->
            withAction([this]()
        {
            this->instrumentNode.recreateChildrenEditors();
            auto *kbmNode = this->instrumentNode.findChildOfType<KeyboardMappingNode>();
            jassert(kbmNode != nullptr);
            kbmNode->setSelected();
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

    const auto isRequiredInstrument =
        (this->instrumentNode.getInstrument() == this->pit.getDefaultInstrument()) ||
        (this->instrumentNode.getInstrument() == this->pit.getMetronomeInstrument()) ||
        (this->instrumentNode.getInstrument() == this->pit.getMidiOutputInstrument());

    menu.add(MenuItem::item(Icons::remove,
        TRANS(I18n::Menu::delete_))->
        disabledIf(isRequiredInstrument)->
        closesMenu()->
        withAction([this]()
        {
            this->pit.removeInstrument(this->instrumentNode.getInstrument());
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
                        this->instrumentNode.recreateChildrenEditors();
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
                        this->instrumentNode.recreateChildrenEditors();
                        this->instrumentNode.setSelected();
                    });
                }));
        }
    }

    return menu;
}
