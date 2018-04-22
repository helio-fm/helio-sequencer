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
#include "Icons.h"
#include "CommandIDs.h"
#include "App.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"

InstrumentMenu::InstrumentMenu(InstrumentTreeItem &parentInstrument) :
    instrument(parentInstrument)
{
    MenuPanel::Menu cmds;
    //cmds.add(MenuItem::item(Icons::reset, CommandIDs::UpdateInstrument, TRANS("menu::instrument::update")));
    cmds.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameInstrument, TRANS("menu::instrument::rename")));
    cmds.add(MenuItem::item(Icons::trash, CommandIDs::DeleteInstrument, TRANS("menu::instrument::delete")));
    this->updateContent(cmds);
}

void InstrumentMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::UpdateInstrument:
            this->instrument.updateChildrenEditors();
            break;

        case CommandIDs::RenameInstrument:
            // todo dialog box!
            //auto dialog = ModalDialogInput::Presets::renameInstrument()
            break;

        case CommandIDs::DeleteInstrument:
            TreeItem::deleteItem(&this->instrument);
            break;
    }

    this->getParentComponent()->exitModalState(0);
}
