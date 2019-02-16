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
#include "KeySelector.h"
#include "MenuPanel.h"
#include "ColourIDs.h"
#include "Scale.h"

KeySelector::KeySelector()
{
    const Colour base(findDefaultColour(ColourIDs::ColourButton::outline).withAlpha(0.5f));

    const StringArray keys(Scale::getKeyNames());
    for (int i = 0; i < keys.size(); ++i)
    {
        ScopedPointer<RadioButton> button(new RadioButton(keys[i], base, this));
        button->setButtonIndex(i);
        this->addAndMakeVisible(button);
        this->buttons.add(button.release());
    }

    this->buttons.getFirst()->select();
}

void KeySelector::resized()
{
    int x = 0;
    for (const auto &button : this->buttons)
    {
        const int w = this->getWidth() / this->buttons.size();
        button->setBounds(x, 0, w, this->getHeight());
        x += w;
    }
}

void KeySelector::onRadioButtonClicked(RadioButton *clickedButton)
{
    for (const auto &button : this->buttons)
    {
        if (button != clickedButton)
        {
            button->deselect();
        }
    }

    clickedButton->select();

    if (KeySelector::Listener *parentListener =
        dynamic_cast<KeySelector::Listener *>(this->getParentComponent()))
    {
        parentListener->onKeyChanged(clickedButton->getButtonIndex());
    }
}


void KeySelector::setSelectedKey(int key)
{
    for (const auto &button : this->buttons)
    {
        if (button->getButtonIndex() == key)
        {
            button->select();
        }
        else
        {
            button->deselect();
        }
    }
}
