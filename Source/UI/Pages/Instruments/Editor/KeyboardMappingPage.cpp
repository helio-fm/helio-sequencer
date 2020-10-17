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

#include "Common.h"
#include "KeyboardMappingPage.h"
#include "PanelBackgroundB.h"
#include "FramePanel.h"

KeyboardMappingPage::KeyboardMappingPage()
{
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);

    this->background = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->background.get());

    this->panel = make<FramePanel>();
    this->addAndMakeVisible(this->panel.get());
}

void KeyboardMappingPage::resized()
{
    this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
    this->panel->setBounds(20, 20, (this->getWidth() - 40), (this->getHeight() - 40));
}
