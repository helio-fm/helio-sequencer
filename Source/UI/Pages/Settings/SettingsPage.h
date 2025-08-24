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

#include "PageBackgroundB.h"

class SettingsPage final : public Component
{
public:

    class SettingsViewport final : public Viewport
    {
    public:

        SettingsViewport() : Viewport()
        {
            this->setPaintingIsUnclipped(true);
            this->setScrollBarsShown(true, false, true, false);
            this->setInterceptsMouseClicks(false, true);
            this->setWantsKeyboardFocus(false);
            this->setFocusContainerType(Component::FocusContainerType::none);
            this->setScrollBarThickness(SettingsViewport::scrollBarWidth);
        }

        bool keyPressed(const KeyPress &) override { return false; }

        #if PLATFORM_DESKTOP
        static constexpr auto scrollBarWidth = 2;
        #elif PLATFORM_MOBILE
        static constexpr auto scrollBarWidth = 32;
        #endif
    };

    explicit SettingsPage(Component *settingsList)
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setFocusContainerType(Component::FocusContainerType::none);

        this->background = make<PageBackgroundB>();
        this->addAndMakeVisible(this->background.get());

        this->viewport = make<SettingsViewport>();
        this->viewport->setViewedComponent(settingsList, false);

        this->addAndMakeVisible(this->viewport.get());
    }

    void resized() override
    {
        this->background->setBounds(this->getLocalBounds());
        this->viewport->setBounds(this->getLocalBounds().reduced(6, 0));
        this->viewport->getViewedComponent()->
            setSize(this->viewport->getWidth() - SettingsViewport::scrollBarWidth,
                this->viewport->getViewedComponent()->getHeight());
    }

private:

    UniquePointer<PageBackgroundB> background;
    UniquePointer<Viewport> viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPage)
};
