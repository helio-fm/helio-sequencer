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
#include "CutPointMark.h"

CutPointMark::CutPointMark(SafePointer<Component> targetComponent, float absPosX) :
    targetComponent(targetComponent),
    absPosX(absPosX)
{
    this->setAlpha(0.f);
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

CutPointMark::~CutPointMark()
{
    Desktop::getInstance().getAnimator()
        .animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 0.f);
}

void CutPointMark::paint(Graphics &g)
{
    g.setColour(Colours::black);
    g.fillRect(this->getLocalBounds());
    g.setColour(Colours::white);
    g.fillRect(this->getLocalBounds().reduced(1));
}

void CutPointMark::fadeIn()
{
    Desktop::getInstance().getAnimator()
        .animateComponent(this, this->getBounds(), 1.f, 150, false, 0.f, 0.f);
}

void CutPointMark::rebound()
{
    if (this->targetComponent != nullptr)
    {
        const float wt = float(this->targetComponent->getWidth());
        const int ht = this->targetComponent->getHeight();
        const int x = this->targetComponent->getX() + int(wt * this->absPosX);
        const int y = this->targetComponent->getY();
        this->setBounds(x - 2, y - 3, 4, ht + 6);
    }
}

void CutPointMark::updatePosition(float pos)
{
    this->absPosX = pos;
    this->rebound();
}
