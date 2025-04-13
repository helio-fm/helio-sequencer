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

#include "CommandIDs.h"

class DialogBackground final : public Component
{
public:
    
    DialogBackground()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
    }
    
    void handleCommandMessage(int commandId) override
    {
        if (commandId == CommandIDs::DismissDialog)
        {
            UniquePointer<Component> deleter(this);
        }
    }

    void parentHierarchyChanged() override
    {
        this->setSize(this->getParentWidth(), this->getParentHeight());
    }

    void parentSizeChanged() override
    {
        this->setSize(this->getParentWidth(), this->getParentHeight());
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::black.withAlpha(0.025f));
        g.fillRect(this->getLocalBounds()); 
    }

};
