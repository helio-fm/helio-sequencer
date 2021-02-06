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
#include "CommandPaletteCommonActions.h"
#include "HotkeySchemesManager.h"
#include "MainLayout.h"
#include "Config.h"

static CommandPaletteActionsProvider::Actions buildCommandsListFor(const Component *target)
{
    //DBG("Building command palette actions for " + target->getComponentID());
    CommandPaletteActionsProvider::Actions actions;
    FlatHashSet<Identifier, IdentifierHash> duplicateLookup;

    const auto hotkeys = App::Config().getHotkeySchemes()->getCurrent();
    const auto actionColor = findDefaultColour(Label::textColourId).withMultipliedAlpha(0.8f);

    for (const auto &keyPress : hotkeys->getKeyPresses())
    {
        const auto i18nKey = CommandIDs::getTranslationKeyFor(keyPress.commandId);
        if (i18nKey.isValid() && keyPress.componentId == target->getComponentID())
        {
            const auto action = [keyPress](TextEditor &ed)
            {
                App::Layout().broadcastCommandMessage(keyPress.commandId);
                return true;
            };

            // don't include duplicate commands (may have some due to similar hotkeys, i.e. ctrl+x/cmd+x)
            if (!duplicateLookup.contains(i18nKey))
            {
                duplicateLookup.insert(i18nKey);
                actions.add(CommandPaletteAction::action(TRANS(i18nKey),
                    keyPress.keyPress.getTextDescription(), 0.f)->
                    withColour(actionColor)->withCallback(action));
            }
        }
    }

    return actions;
}

void CommandPaletteCommonActions::setActiveCommandReceivers(const Array<Component *> &receivers)
{
    this->actions.clearQuick();

    for (const auto *receiver : receivers)
    {
        const auto commands = this->commandsCache.find(receiver->getComponentID());
        if (commands != this->commandsCache.end())
        {
            this->actions.addArray(commands->second);
        }
        else
        {
            // fill it up once - commands and hotkeys are never updated in the runtime
            const auto cachedCommandsList = buildCommandsListFor(receiver);
            this->commandsCache[receiver->getComponentID()] = cachedCommandsList;
            this->actions.addArray(cachedCommandsList);
        }
    }
}
