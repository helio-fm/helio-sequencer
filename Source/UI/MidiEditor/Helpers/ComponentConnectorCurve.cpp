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
#include "ComponentConnectorCurve.h"

#define CONNECTOR_LINE_HEIGHT 5.0f

ComponentConnectorCurve::ComponentConnectorCurve(Component *c1, Component *c2) :
    component1(c1),
    component2(c2),
    curvature(0.5f)
{
    this->setInterceptsMouseClicks(false, false);
    //this->resizeToFit();
}

void ComponentConnectorCurve::getPoints(float &x1, float &y1, float &x2, float &y2) const
{
    jassert(this->component1 || this->component2);

    if (this->component1 == nullptr)
    {
        x1 = 0;
        y1 = float(this->component2->getBounds().getCentreY());
    }
    else
    {
        x1 = float(this->component1->getBounds().getCentreX());
        y1 = float(this->component1->getBounds().getCentreY());
    }
    
    if (this->component2 == nullptr)
    {
        if (this->component1->getParentComponent())
        {
            x2 = float(this->component1->getParentWidth());
            y2 = y1;
        }
    }
    else
    {
        x2 = float(this->component2->getBounds().getCentreX());
        y2 = float(this->component2->getBounds().getCentreY());
    }
}

void ComponentConnectorCurve::retargetAndUpdate(Component *c1, Component *c2)
{
    this->component1 = c1;
    this->component2 = c2;
    this->resizeToFit();
}

void ComponentConnectorCurve::resizeToFit(float newCurvature)
{
    if (this->component1 == nullptr && this->component2 == nullptr)
    {
        return;
    }
    
    this->toBack();
    
    float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    this->getPoints(x1, y1, x2, y2);

    const Rectangle<int> newBounds(int(jmin(x1, x2) - 4),
                                   int(jmin(y1, y2) - 4),
                                   int(fabsf(x1 - x2) + 8),
                                   int(fabsf(y1 - y2) + 8));

    if (newBounds != this->getBounds())
    {
        this->curvature = newCurvature;
        this->setBounds(newBounds);
    }
    else
    {
        if (this->curvature != newCurvature)
        {
            this->curvature = newCurvature;
            this->resized();
            this->repaint();
        }
    }
}

Point<float> ComponentConnectorCurve::getCentrePoint() const
{
    return this->linePath.getPointAlongPath(this->linePath.getLength() / 4.f).
        translated(0, CONNECTOR_LINE_HEIGHT / 2.f);
//    return this->linePath.getPointAlongPath(this->linePath.getLength() / 4.f).translated(0, CONNECTOR_LINE_HEIGHT / 2.f).withX(this->getWidth() / 2.f);
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ComponentConnectorCurve::paint(Graphics &g)
{
    //g.setColour(this->findColour(HistoryComponent::connectorColourId));
    g.setColour(Colours::white.withAlpha(0.15f));
    g.fillPath(this->linePath);
}

void ComponentConnectorCurve::resized()
{
    if (this->getParentWidth() > 0)
    {
        float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        this->getPoints(x1, y1, x2, y2);
        
        x1 -= this->getX();
        y1 -= this->getY();
        x2 -= this->getX();
        y2 -= this->getY();
        
        const float dy = (y2 - y1);
        const float dx = (x2 - x1);
        
        this->linePath.clear();
        this->linePath.startNewSubPath(x1, y1);
        
        float c = 0.f;
        float rc = 0.f;
        
        if (y1 > y2)
        {
            c = this->curvature;
            rc = (1.f - this->curvature);
        }
        else
        {
            rc = this->curvature;
            c = (1.f - this->curvature);
        }
        
        this->linePath.cubicTo(x1 + dx * rc, y1 + dy * c,
                               x1 + dx * rc, y1 + dy * c,
                               x2, y2);
        
        PathStrokeType stroke(CONNECTOR_LINE_HEIGHT, PathStrokeType::beveled, PathStrokeType::butt);
        stroke.createStrokedPath(linePath, linePath);
        
        this->linePath.setUsingNonZeroWinding(false);
    }
}

bool ComponentConnectorCurve::hitTest(int x, int y)
{
    return this->linePath.contains(float(x), float(y) - (CONNECTOR_LINE_HEIGHT / 2));
}
