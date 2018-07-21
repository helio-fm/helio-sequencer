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

static inline ComponentAnimator &rootAnimator()
{
    return Desktop::getInstance().getAnimator();
}

CutPointMark::CutPointMark(SafePointer<Component> targetComponent, float absPosX) :
    targetComponent(targetComponent),
    absPosX(absPosX),
    initialized(false)
{
    this->setAlpha(0.f);
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

CutPointMark::~CutPointMark()
{
    rootAnimator().cancelAnimation(this, false);
    if (this->initialized)
    {
        rootAnimator().animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 0.f);
    }
}

void CutPointMark::paint(Graphics &g)
{
    const float w = float(this->getWidth());
    const float h = float(this->getHeight());

    g.setColour(Colours::orangered.darker(0.75f));
    g.fillRect(w / 2.f - 0.25f, 1.f, .5f, h - 2.f);

    g.setColour(Colours::black);

    Path p;
    p.addTriangle(0.f, 0.f, w, 0.f, w / 2.f, 7.f);
    g.fillPath(p);

    p.clear();
    p.addTriangle(0, h, w, h, w / 2.f, h - 7.f);
    g.fillPath(p);

    g.setColour(Colours::orangered);

    p.clear();
    p.addTriangle(1.f, 1.f, w - 1.f, 1.f, w / 2.f, 6);
    g.fillPath(p);

    p.clear();
    p.addTriangle(1.f, h - 1.f, w - 1.f, h - 1.f, w / 2.f, h - 6.f);
    g.fillPath(p);
}

void CutPointMark::fadeIn()
{
    rootAnimator().animateComponent(this, this->getBounds(), 1.f, 150, false, 0.f, 0.f);
}

void CutPointMark::updateBounds(bool forceNoAnimation)
{
    rootAnimator().cancelAnimation(this, false);

    if (this->absPosX <= 0.f || this->absPosX >= 1.f)
    {
        if (this->initialized)
        {
            this->setVisible(false);
            this->initialized = false;
        }
    }
    else if (this->targetComponent != nullptr)
    {
        const float wt = float(this->targetComponent->getWidth());
        const int ht = this->targetComponent->getHeight();
        const int x = this->targetComponent->getX() + int(wt * this->absPosX);
        const int y = this->targetComponent->getY();
        const Rectangle<int> newBounds(x - 4, y - 3, 9, ht + 6);

        if (!this->initialized || forceNoAnimation)
        {
            this->initialized = true;
            this->setVisible(true);
            this->setBounds(newBounds);
        }
        else
        {
            rootAnimator().animateComponent(this, newBounds, 1.f, 50, false, 1.f, .5f);
        }
    }
}

void CutPointMark::updatePosition(float pos)
{
    this->absPosX = pos;
    this->updateBounds();
}

void CutPointMark::updatePositionFromMouseEvent(int mouseX, int mouseY)
{
    if (mouseY < this->targetComponent->getY() ||
        mouseY > this->targetComponent->getBottom())
    {
        this->absPosX = -1.f;
    }
    else
    {
        this->absPosX = jlimit(0.f, 1.f,
            float(mouseX - this->targetComponent->getX()) /
            float(this->targetComponent->getWidth()));
    }

    this->updateBounds();
}

Component *CutPointMark::getComponent() const noexcept
{
    return this->targetComponent.getComponent();
}

float CutPointMark::getCutPosition() const noexcept
{
    return this->absPosX;
}
