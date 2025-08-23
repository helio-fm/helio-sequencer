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

#include "IconComponent.h"
#include "HighlightedComponent.h"
#include "CommandIDs.h"

class IconButton : public IconComponent, public HighlightedComponent
{
public:
    
    explicit IconButton(Icons::Id iconId,
        int commandId = CommandIDs::IconButtonPressed,
        WeakReference<Component> listener = nullptr,
        Optional<int> iconSize = {}) :
        IconComponent(iconId, 1.f, iconSize),
        commandId(commandId),
        listener(listener)
    {
        this->setWantsKeyboardFocus(false);
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
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
    }

    // on desktop platforms buttons react on mouse down (feels a bit more responsive I guess),
    // but on mobile platforms they react on mouse up to be able to check if this was a tap, not dragging
#if PLATFORM_DESKTOP
    void mouseDown(const MouseEvent &e) override
    {
        if (this->isEnabled())
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

        HighlightedComponent::mouseExit(e);
    }
#elif PLATFORM_MOBILE
    void mouseUp(const MouseEvent &e) override
    {
        if (this->isEnabled() && e.getDistanceFromDragStart() < IconButton::dragStartThreshold)
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

        HighlightedComponent::mouseExit(e);
    }
#endif

    void enablementChanged() override
    {
        this->setAlpha(this->isEnabled() ? 1.0f : 0.4f);
    }

    void resized() override
    {
        IconComponent::resized();
        HighlightedComponent::resized();
    }

    void paint(Graphics &g) override
    {
        IconComponent::paint(g);
    }

    void mouseEnter(const MouseEvent &event) override
    {
        HighlightedComponent::mouseEnter(event);
    }

    void mouseExit(const MouseEvent &event) override
    {
        HighlightedComponent::mouseExit(event);
    }

protected:

    const int commandId = 0;
    
    WeakReference<Component> listener;

    Component *createHighlighterComponent() override
    {
        return this->image.isNull() ?
            new IconButton(this->iconId, this->commandId, nullptr, this->iconSize) :
            new IconButton(this->image);
    }

    static constexpr auto dragStartThreshold = 5;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconButton)
};
