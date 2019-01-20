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
#include "PopupCustomButton.h"

PopupCustomButton::PopupCustomButton(Component *newOwnedComponent, ShapeType shapeType, Colour colour) :
    PopupButton(true, shapeType, colour),
    ownedComponent(newOwnedComponent)
{
    this->ownedComponent->setInterceptsMouseClicks(false, false);
    this->addAndMakeVisible(this->ownedComponent);
    this->setSize(48, 48);
}

void PopupCustomButton::resized()
{
    PopupButton::resized();
    this->ownedComponent->
        setTopLeftPosition((this->getWidth() / 2) - (ownedComponent->getWidth() / 2),
                           (this->getHeight() / 2) - (ownedComponent->getHeight() / 2));
}
