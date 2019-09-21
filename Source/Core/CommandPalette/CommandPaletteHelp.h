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

#pragma once

#include "CommandPaletteActionsProvider.h"

class CommandPaletteHelp final : public CommandPaletteActionsProvider
{
public:

    bool usesPrefix(const Prefix prefix) const noexcept override
    {
        return prefix == '?';
    }

protected:

    const Actions &getActions() const override
    {
        if (this->help.isEmpty())
        {
            for (const auto &tmp : this->temp)
            {
                this->help.add(new CommandPaletteAction(tmp));
            }
        }

        return this->help;
    }

    mutable Actions help;
    
    StringArray temp =
    {
        "/ projects list",
        "@ timeline events",
        "# chords list",
        "$ chord inline constructor",
        "! version control"
    };
};
