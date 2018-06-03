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
#include "AutomationCurveEventsConnector.h"
#include "AutomationCurveClipComponent.h"

AutomationCurveEventsConnector::AutomationCurveEventsConnector(Component *c1, Component *c2) :
    ComponentConnectorCurve(c1, c2),
    xAnchor(0.f)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::UpDownResizeCursor);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

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
