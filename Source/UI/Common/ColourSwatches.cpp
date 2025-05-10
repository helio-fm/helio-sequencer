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
#include "ColourSwatches.h"
#include "ColourIDs.h"

ColourSwatches::ColourSwatches(int buttonSize)
{
    for (const auto &colour : ColourIDs::getColoursList())
    {
        auto button = make<ColourButton>(colour, this);
        button->setSize(buttonSize, buttonSize);
        this->addAndMakeVisible(button.get());
        this->buttons.add(button.release());
    }
}

void ColourSwatches::resized()
{
    int x = 0;
    for (const auto &button : this->buttons)
    {
        button->setBounds(x, 0, button->getWidth(), this->getHeight());
        x += button->getWidth();
    }
}

void ColourSwatches::onColourButtonClicked(ColourButton *clickedButton)
{
    for (const auto &button : this->buttons)
    {
        if (button != clickedButton)
        {
            button->deselect();
        }
    }

    if (auto *parentListener = dynamic_cast<ColourButton::Listener *>(this->getParentComponent()))
    {
        this->lastSelectedColour = clickedButton->getColour();
        parentListener->onColourButtonClicked(clickedButton);
    }
}

Colour ColourSwatches::getColour() const noexcept
{
    return this->lastSelectedColour;
}

int ColourSwatches::getNumButtons() const noexcept
{
    return this->buttons.size();
}

void ColourSwatches::setSelectedColour(Colour colour)
{
    this->lastSelectedColour = colour;
    for (const auto &button : this->buttons)
    {
        if (button->getColour() == colour)
        {
            button->select();
        }
        else
        {
            button->deselect();
        }
    }
}
