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
#include "AutomationEvent.h"
#include "AutomationCurveEventsConnector.h"

AutomationCurveEventsConnector::AutomationCurveEventsConnector(
    AutomationEditorBase &editor,
    AutomationEditorBase::EventComponentBase *c1,
    AutomationEditorBase::EventComponentBase *c2) :
    editor(editor),
    component1(c1),
    component2(c2)
{
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setAccessible(false);
    this->setFocusContainerType(Component::FocusContainerType::none);
}

void AutomationCurveEventsConnector::retargetAndUpdate(AutomationEditorBase::EventComponentBase *c1,
    AutomationEditorBase::EventComponentBase *c2)
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

    const float x1 = this->component1->getFloatBounds().getCentreX();
    const float x2 = this->component2->getFloatBounds().getCentreX();
    const Rectangle<int> newBounds(int(jmin(x1, x2)), 0,
        int(fabsf(x1 - x2)), this->getParentHeight());

    this->curvature = newCurvature;
    this->setBounds(newBounds);
    this->rebuildLinePath();
}

Point<float> AutomationCurveEventsConnector::getCentrePoint() const
{
    const auto c1 = this->component1->getFloatBounds().getCentre();
    const auto c2 = this->component2->getFloatBounds().getCentre();
    const auto curve = this->component1->getEvent().getCurvature();
    const auto y1 = jmin(c1.getY(), c2.getY());
    const auto y2 = jmax(c1.getY(), c2.getY());
    return { (c2.getX() - c1.getX()) / 2.f,
        y1 + (y2 - y1) * (1.f - curve) };
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationCurveEventsConnector::paint(Graphics &g)
{
    if (this->component1 != nullptr)
    {
        g.setColour(this->component1->getColour());
    }

    for (const auto &p : this->linePath)
    {
        g.fillRect(p.getX() - 1.f, p.getY() - 0.75f, 2.f, 1.5f);
    }
}

void AutomationCurveEventsConnector::resized()
{
    this->rebuildLinePath();
}

void AutomationCurveEventsConnector::rebuildLinePath()
{
    jassert(this->component1);
    jassert(this->component2);

    if (this->getParentWidth() == 0 ||
        this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    const float x1 = this->component1->getFloatBounds().getCentreX();
    const float x2 = this->component2->getFloatBounds().getCentreX();

    this->linePath.clearQuick();

    const auto &e1 = this->component1->getEvent();
    const auto &e2 = this->component2->getEvent();
    float lastAppliedValue = e1.getControllerValue();
    float interpolatedBeat = e1.getBeat(); // + AutomationEvent::curveInterpolationStepBeat;
    while (interpolatedBeat < e2.getBeat())
    {
        const float factor = (interpolatedBeat - e1.getBeat()) / (e2.getBeat() - e1.getBeat());
        const float interpolatedValue =
            AutomationEvent::interpolateEvents(e1.getControllerValue(),
                e2.getControllerValue(), factor, e1.getCurvature());

        const float x = (x2 - x1) * factor;
        const float y = float(this->getParentHeight()) * (1.f - interpolatedValue);

        const float controllerDelta = fabs(interpolatedValue - lastAppliedValue);
        if (controllerDelta > AutomationEvent::curveInterpolationThreshold)
        {
            this->linePath.add({ x, y });
            lastAppliedValue = interpolatedValue;
        }

        interpolatedBeat += AutomationEvent::curveInterpolationStepBeat;
    }
}
