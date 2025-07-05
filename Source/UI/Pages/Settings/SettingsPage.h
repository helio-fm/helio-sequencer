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

    explicit SettingsPage(Component *settingsList)
    {
        this->background = make<PageBackgroundB>();
        this->addAndMakeVisible(this->background.get());

        this->viewport = make<Viewport>();
        this->addAndMakeVisible(this->viewport.get());
        this->viewport->setScrollBarsShown(true, false);
        this->viewport->setScrollBarThickness(SettingsPage::viewportScrollBarWidth);
        this->viewport->setViewedComponent(settingsList, false);

        this->setFocusContainerType(Component::FocusContainerType::keyboardFocusContainer);
        this->setWantsKeyboardFocus(true);
        this->setPaintingIsUnclipped(true);
    }

    void resized() override
    {
        this->background->setBounds(this->getLocalBounds());
        this->viewport->setBounds(this->getLocalBounds().reduced(6));
        this->viewport->getViewedComponent()->
            setSize(this->viewport->getMaximumVisibleWidth(),
                this->viewport->getViewedComponent()->getHeight());
    }

#if PLATFORM_DESKTOP
    static constexpr auto viewportScrollBarWidth = 2;
#elif PLATFORM_MOBILE
    static constexpr auto viewportScrollBarWidth = 32;
#endif

private:

    UniquePointer<PageBackgroundB> background;
    UniquePointer<Viewport> viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPage)
};
