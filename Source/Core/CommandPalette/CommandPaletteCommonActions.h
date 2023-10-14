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

#pragma once

#include "CommandPaletteActionsProvider.h"

class CommandPaletteCommonActions final : public CommandPaletteActionsProvider
{
public:

    CommandPaletteCommonActions() = default;

    void setActiveCommandReceivers(const Array<Component *> &receivers);

protected:

    const Actions &getActions() const override
    {
        return this->actions;
    }

    Actions actions;

    FlatHashMap<String, Actions, StringHash> commandsCache;

};
