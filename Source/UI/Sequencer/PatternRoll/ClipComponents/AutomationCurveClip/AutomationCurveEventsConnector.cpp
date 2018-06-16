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
#include "AutomationEvent.h"
#include "AutomationCurveEventsConnector.h"
#include "AutomationCurveClipComponent.h"

#define CURVE_CONNECTOR_LINE_HEIGHT 2.0f

AutomationCurveEventsConnector::AutomationCurveEventsConnector(
    AutomationCurveEventComponent *c1,
    AutomationCurveEventComponent *c2) :
    component1(c1),
    component2(c2),
    curvature(0.5f),
    xAnchor(0.f)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::UpDownResizeCursor);
}

void AutomationCurveEventsConnector::retargetAndUpdate(AutomationCurveEventComponent *c1,
    AutomationCurveEventComponent *c2)
{
    this->component1 = c1;
    this->component2 = c2;
    this->resizeToFit();
}

void AutomationCurveEventsConnector::resizeToFit(float newCurvature)
{
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    const float x1 = float(this->component1->getBounds().getCentreX());
    const float x2 = float(this->component2->getBounds().getCentreX());
    const Rectangle<int> newBounds(int(jmin(x1, x2)), 0,
        int(fabsf(x1 - x2)), this->getParentHeight());

    this->curvature = newCurvature;
    this->setBounds(newBounds);
    this->rebuildLinePath();
}

Point<float> AutomationCurveEventsConnector::getCentrePoint() const
{
    const auto c1 = this->component1->getBounds().getCentre();
    const auto c2 = this->component2->getBounds().getCentre();
    const auto curve = this->component1->getEvent().getCurvature();
    const auto y1 = jmin(c1.getY(), c2.getY());
    const auto y2 = jmax(c1.getY(), c2.getY());
    return { (c2.getX() - c1.getX()) / 2.f, y1 + (y2 - y1) * (1.f - curve) };
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationCurveEventsConnector::paint(Graphics &g)
{
    for (const auto &p : this->linePath)
    {
        g.fillRect(p.getX() - 1.f, p.getY() - 0.75f, 2.f, 1.5f);
    }
}

void AutomationCurveEventsConnector::resized()
{
    this->rebuildLinePath();
}

/*
bool AutomationCurveEventsConnector::hitTest(int x, int y)
{
    return this->linePath.contains(float(x), float(y) - (CURVE_CONNECTOR_LINE_HEIGHT / 2.f));
}

void AutomationCurveEventsConnector::mouseDown(const MouseEvent &e)
{
    this->xAnchor = e.position.x;

    if (this->component1)
    {
        this->component1->mouseDown(e.getEventRelativeTo(this->component1));
    }

    if (e.mods.isAnyModifierKeyDown())
    {
        if (this->component2)
        {
            this->component2->mouseDown(e.getEventRelativeTo(this->component2));
        }
    }
}

void AutomationCurveEventsConnector::mouseDrag(const MouseEvent &e)
{
    if (this->component1)
    {
        this->component1->mouseDrag(e.withNewPosition(e.position.withX(this->xAnchor)).getEventRelativeTo(this->component1));
    }
    
    if (e.mods.isAnyModifierKeyDown())
    {
        if (this->component2)
        {
            this->component2->mouseDrag(e.withNewPosition(e.position.withX(this->xAnchor)).getEventRelativeTo(this->component2));
        }
    }
}

void AutomationCurveEventsConnector::mouseUp(const MouseEvent &e)
{
    if (this->component1)
    {
        this->component1->mouseUp(e.withNewPosition(e.position.withX(this->xAnchor)).getEventRelativeTo(this->component1));
    }
    
    if (e.mods.isAnyModifierKeyDown())
    {
        if (this->component2)
        {
            this->component2->mouseUp(e.withNewPosition(e.position.withX(this->xAnchor)).getEventRelativeTo(this->component2));
        }
    }
}
*/

void AutomationCurveEventsConnector::rebuildLinePath()
{
    jassert(this->component1);
    jassert(this->component2);

    if (this->getParentWidth() == 0 ||
        this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    const float x1 = float(this->component1->getBounds().getCentreX());
    const float x2 = float(this->component2->getBounds().getCentreX());

    this->linePath.clear();

    const float r = 2.f;
    const auto &e1 = this->component1->getEvent();
    const auto &e2 = this->component2->getEvent();
    float lastAppliedValue = e1.getControllerValue();
    float interpolatedBeat = e1.getBeat(); // + CURVE_INTERPOLATION_STEP_BEAT;
    while (interpolatedBeat < e2.getBeat())
    {
        const float factor = (interpolatedBeat - e1.getBeat()) / (e2.getBeat() - e1.getBeat());
        const float interpolatedValue =
            AutomationEvent::interpolateEvents(e1.getControllerValue(),
                e2.getControllerValue(), factor, e1.getCurvature());

        const float x = ((x2 - x1) * factor);
        const float y = float(this->getParentHeight()) * (1.f - interpolatedValue);

        const float controllerDelta = fabs(interpolatedValue - lastAppliedValue);
        if (controllerDelta > CURVE_INTERPOLATION_THRESHOLD)
        {
            this->linePath.add({ x, y });
            lastAppliedValue = interpolatedValue;
        }

        interpolatedBeat += CURVE_INTERPOLATION_STEP_BEAT;
    }
}
