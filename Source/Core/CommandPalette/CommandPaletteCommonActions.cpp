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
#include "CommandPaletteCommonActions.h"
#include "HotkeySchemesCollection.h"
#include "MainLayout.h"
#include "RollBase.h"
#include "Config.h"

void CommandPaletteCommonActions::setActiveCommandReceivers(const Array<WeakReference<Component>> &receivers)
{
    this->currentReceivers = receivers;
}

const CommandPaletteActionsProvider::Actions &CommandPaletteCommonActions::getActions() const
{
    this->actions.clearQuick();

    for (auto &receiver : this->currentReceivers)
    {
        if (receiver.get() == nullptr)
        {
            jassertfalse;
            continue;
        }

        const auto *roll = dynamic_cast<RollBase *>(receiver.get());

        float order = 10.f;

        CommandPaletteActionsProvider::Actions commandsList;
        FlatHashSet<I18n::Key> duplicatesLookup;

        const auto hotkeys = App::Config().getHotkeySchemes()->getCurrent();

        for (const auto &keyPress : hotkeys->getKeyPresses())
        {
            if (keyPress.first.componentId != receiver->getComponentID())
            {
                continue;
            }

            const auto commandId = keyPress.second;
            const auto i18nKey = CommandIDs::getTranslationKeyFor(commandId);
            // don't include duplicate commands (may have some due to similar hotkeys, i.e. ctrl+x/cmd+x)
            if (i18nKey <= 0 || duplicatesLookup.contains(i18nKey))
            {
                continue;
            }

            duplicatesLookup.insert(i18nKey);

            String actionName;
            if (roll != nullptr)
            {
                if (!roll->canHandleCommand(commandId))
                {
                    continue;
                }

                actionName = roll->getTranslatedCommandWithContext(commandId, i18nKey);
            }
            else
            {
                actionName = TRANS(i18nKey);
            }

            actions.add(CommandPaletteAction::action(actionName,
                keyPress.first.keyPress.getTextDescription(), order++)->
                withColour(this->actionColour)->
                withCallback([commandId](TextEditor &ed)
            {
                App::Layout().broadcastCommandMessage(commandId);
                return true;
            }));
        }

        this->actions.addArray(commandsList);
    }

    return this->actions;
}
