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
#include "CommandIDs.h"
#include "Icons.h"

static MenuPanel::Menu createDefaultMenu()
{
    MenuPanel::Menu cmds;

    // TODO
    // Create new instrument
    // Add to instrument >
    // Remove from list
    cmds.add(MenuItem::item(Icons::create, TRANS("menu::selection::plugin::init")));
    cmds.add(MenuItem::item(Icons::audioPlugin, TRANS("menu::selection::plugin::plug"))->withSubmenu()->withTimer());
    cmds.add(MenuItem::item(Icons::remove, TRANS("menu::selection::plugin::remove")));

    return cmds;
}

AudioPluginSelectionMenu::AudioPluginSelectionMenu()
{
    this->updateContent(createDefaultMenu(), MenuPanel::SlideRight);
}

void AudioPluginSelectionMenu::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::Back)
    {
        this->updateContent(createDefaultMenu(), MenuPanel::SlideRight);
        return;
    }
    else if (commandId == CommandIDs::CopyEvents)
    {
        this->dismiss();
        return;
    }
}
