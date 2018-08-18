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
#include "OverlayButton.h"

OverlayButton::OverlayButton() : Button({})
{
    this->setMouseCursor(MouseCursor::PointingHandCursor);
}

void OverlayButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown)
{
    if (isMouseOverButton || isButtonDown)
    {
        g.setColour(Colours::white.withAlpha(isMouseOverButton && isButtonDown ? 0.075f : 0.025f));
        g.fillRect(this->getLocalBounds().reduced(1, 0));
        g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));
        g.drawVerticalLine(this->getWidth() - 1, 1.f, float(this->getHeight() - 1));
    }
}
