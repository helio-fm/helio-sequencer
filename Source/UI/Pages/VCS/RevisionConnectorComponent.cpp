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
#include "RevisionConnectorComponent.h"
#include "ColourIDs.h"

RevisionConnectorComponent::RevisionConnectorComponent(Component *c1, Component *c2) :
    component1(c1),
    component2(c2)
{
    this->setInterceptsMouseClicks(false, false);
    this->resizeToFit();
}

void RevisionConnectorComponent::getPoints(float &x1, float &y1, float &x2, float &y2) const
{
    x1 = float(this->component1->getBounds().getCentreX());
    y1 = float(this->component1->getY());
    x2 = float(this->component2->getBounds().getCentreX());
    y2 = float(this->component2->getBottom());
}

void RevisionConnectorComponent::resizeToFit()
{
    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    const Rectangle<int> newBounds(int(jmin(x1, x2) - 4),
        int(jmin(y1, y2) - 4),
        int(fabsf(x1 - x2) + 8),
        int(fabsf(y1 - y2) + 8));

    if (newBounds != this->getBounds())
    {
        this->setBounds(newBounds);
    }
    else
    {
        this->resized();
    }
}

void RevisionConnectorComponent::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::VersionControl::connector));
    g.fillPath(this->linePath);
}

void RevisionConnectorComponent::resized()
{
    if (!this->component1 || !this->component2)
    {
        delete this;
    }

    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    x1 -= this->getX();
    y1 -= this->getY();
    x2 -= this->getX();
    y2 -= this->getY();

    const float dy = (y2 - y1);
    const float dx = (x2 - x1);

    this->linePath.clear();
    this->linePath.startNewSubPath(x1, y1);

    const float curvinessX = (1.f - (fabs(dx) / float(this->getParentWidth()))) * 1.5f;
    const float curvinessY = (fabs(dy) / float(this->getParentHeight())) * 1.5f;
    const float curviness = (curvinessX + curvinessY) / 2.f;

    this->linePath.cubicTo(x1, y1 + dy * (curviness),
        x2, y1 + dy * (1.f - curviness),
        x2, y2);

    PathStrokeType stroke(1.0f, PathStrokeType::beveled, PathStrokeType::butt);
    stroke.createStrokedPath(linePath, linePath);

    this->linePath.setUsingNonZeroWinding(false);
}
