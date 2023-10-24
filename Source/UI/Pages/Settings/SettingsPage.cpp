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
#include "SettingsPage.h"
#include "PageBackgroundB.h"

SettingsPage::SettingsPage(Component *settingsList)
{
    this->background = make<PageBackgroundB>();
    this->addAndMakeVisible(this->background.get());
    this->viewport = make<Viewport>();
    this->addAndMakeVisible(this->viewport.get());
    this->viewport->setScrollBarsShown(true, false);

#if PLATFORM_DESKTOP
    this->viewport->setScrollBarThickness(2);
#elif PLATFORM_MOBILE
    this->viewport->setScrollBarThickness(35);
#endif

    this->viewport->setViewedComponent(settingsList, false);

    this->setFocusContainerType(Component::FocusContainerType::keyboardFocusContainer);
    this->setWantsKeyboardFocus(true);
    this->setPaintingIsUnclipped(true);

    this->setSize(600, 400);
}

SettingsPage::~SettingsPage() = default;

void SettingsPage::resized()
{
    this->background->setBounds(this->getLocalBounds());
    this->viewport->setBounds(this->getLocalBounds().reduced(16, 0));
    
    this->viewport->getViewedComponent()->
        setSize(this->viewport->getMaximumVisibleWidth(),
            this->viewport->getViewedComponent()->getHeight());
}
