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
#include "ColourSwatches.h"
#include "MenuPanel.h"

ColourSwatches::ColourSwatches()
{
    const StringPairArray colours(MenuPanel::getColoursList());
    for (const auto &c : colours.getAllValues())
    {
        const Colour colour(Colour::fromString(c));
        ScopedPointer<ColourButton> button(new ColourButton(colour, this));
        this->addAndMakeVisible(button);
        this->buttons.add(button.release());
    }
}

void ColourSwatches::resized()
{
    int x = 0;
    for (const auto &button : this->buttons)
    {
        const int w = this->getWidth() / this->buttons.size();
        button->setBounds(x, 0, w, this->getHeight());
        x += w;
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

    if (ColourButtonListener *parentListener =
        dynamic_cast<ColourButtonListener *>(this->getParentComponent()))
    {
        this->lastSelectedColour = clickedButton->getColour();
        parentListener->onColourButtonClicked(clickedButton);
    }
}

Colour ColourSwatches::getColour() const noexcept
{
    return this->lastSelectedColour;
}

void ColourSwatches::setSelectedColour(Colour colour)
{
    this->lastSelectedColour = colour;
    for (const auto &button : this->buttons)
    {
        if (button->getColour() == colour)
        { button->select(); }
        else
        { button->deselect(); }
    }
}
