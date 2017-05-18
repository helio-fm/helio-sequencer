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

#include "CommandIDs.h"

class DialogBackground : public Component, private Timer
{
public:
    
    DialogBackground() : appearMode(true)
    {
        this->setAlpha(0.f);
        this->startTimer(17);
    }
    
    void handleCommandMessage(int commandId) override
    {
        if (commandId == CommandIDs::HideDialog)
        {
            this->appearMode = false;
            this->startTimer(17);
        }
    }

    void parentHierarchyChanged() override
    { this->setSize(this->getParentWidth(), this->getParentHeight()); }

    void parentSizeChanged() override
    { this->setSize(this->getParentWidth(), this->getParentHeight()); }

    void paint(Graphics &g) override
    { g.fillAll(Colours::black.withAlpha(0.1f)); }

private:

    void timerCallback() override
    {
        if (this->appearMode)
        {
            this->setAlpha(this->getAlpha() + 0.2);
    
            if (this->getAlpha() == 1.f)
            {
                this->stopTimer();
            }
        }
    else
    {
        this->setAlpha(this->getAlpha() - 0.2);
    
        if (this->getAlpha() == 0.f)
        {
            delete this;
        }
    }
    }

    bool appearMode;
};
