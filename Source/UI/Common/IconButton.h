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

#include "IconComponent.h"
#include "HighlightedComponent.h"
#include "CommandIDs.h"

class IconButton : public IconComponent, public HighlightedComponent
{
public:
    
    explicit IconButton(Icons::Id iconId,
        int commandId = CommandIDs::IconButtonPressed,
        WeakReference<Component> listener = nullptr) :
        IconComponent(iconId),
        commandId(commandId),
        listener(listener)
    {
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
    }
    
    explicit IconButton(Image targetImage,
        int commandId = CommandIDs::IconButtonPressed,
        WeakReference<Component> listener = nullptr) :
        IconComponent(targetImage),
        commandId(commandId),
        listener(listener)
    {
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
    }

    void mouseDown(const MouseEvent &e) override
    {
        if (this->listener != nullptr)
        {
            this->listener->postCommandMessage(this->commandId);
        }
        else if (this->getParentComponent() != nullptr)
        {
            this->getParentComponent()->postCommandMessage(this->commandId);
        }
    }
    
    // Silence useless VC C4250 warnings:

    void resized() override
    { IconComponent::resized(); }

    void paint(Graphics &g) override
    { IconComponent::paint(g); }

    void mouseEnter(const MouseEvent &event) override
    { HighlightedComponent::mouseEnter(event); }

    void mouseExit(const MouseEvent &event) override
    { HighlightedComponent::mouseExit(event); }

private:

    int commandId;
    
    WeakReference<Component> listener;

    Component *createHighlighterComponent() override
    {
        return this->image.isNull() ?
            new IconButton(this->iconId) :
            new IconButton(this->image);
    }
    
};
