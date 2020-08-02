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

class DialogBackground final : public Component, private Timer
{
public:
    
    DialogBackground()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
        this->startTimerHz(60);
    }
    
    void handleCommandMessage(int commandId) override
    {
        if (commandId == CommandIDs::HideDialog)
        {
            this->appearMode = false;
            this->startTimerHz(60);
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
        g.setColour(Colours::black.withAlpha(this->alpha));
        g.fillRect(this->getLocalBounds()); 
    }

private:

    static constexpr auto alphaStep = 0.025f;
    static constexpr auto maxAlpha = 0.2f;

    void timerCallback() override
    {
        if (this->appearMode)
        {
            this->alpha += alphaStep;
            this->repaint();

            if (this->alpha >= maxAlpha)
            {
                this->stopTimer();
            }
        }
        else
        {
            this->alpha -= alphaStep;
            this->repaint();

            if (this->alpha <= alphaStep)
            {
                delete this;
            }
        }
    }

    bool appearMode = true;
    float alpha = 0.f;
};
