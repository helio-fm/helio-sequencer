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

//[Headers]
#include "Common.h"
//[/Headers]

#include "AutomationCurveHelper.h"

//[MiscUserDefs]
#include "AutomationCurveSequenceMap.h"
#include "ComponentConnectorCurve.h"
#include "AutomationSequence.h"
//[/MiscUserDefs]

AutomationCurveHelper::AutomationCurveHelper(AutomationCurveSequenceMap &parent, const AutomationEvent &targetEvent, Component *target1, Component *target2)
    : editor(parent),
      event(targetEvent),
      component1(target1),
      component2(target2),
      draggingState(false)
{

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->setMouseCursor(MouseCursor::UpDownResizeCursor);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

AutomationCurveHelper::~AutomationCurveHelper()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void AutomationCurveHelper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x2bfefefe));
    g.fillEllipse (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0));

    //[UserPaint] Add your own custom painting code here..
    if (this->draggingState)
    {
        g.setColour (Colour (0x2bfefefe));
        g.fillEllipse (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0));
    }
    //[/UserPaint]
}

void AutomationCurveHelper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

bool AutomationCurveHelper::hitTest (int x, int y)
{
    //[UserCode_hitTest] -- Add your code here...
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
    //[/UserCode_hitTest]
}

void AutomationCurveHelper::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();
        this->dragger.startDraggingComponent(this, e);
        this->anchor = this->getBounds().getCentre();
        this->curveAnchor = this->getCurvature();
        this->draggingState = true;
        this->repaint();
    }
    //[/UserCode_mouseDown]
}

void AutomationCurveHelper::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->dragger.dragComponent(this, e, nullptr);
            const float newCurvature = this->constrainPosition();

            //const bool xReverse = e.getMouseDownPosition().getX() < e.getPosition().getX();
            //const bool yReverse = e.getMouseDownPosition().getY() > e.getPosition().getY();
            //const float newCurvature = this->curveAnchor + ((float(e.getDistanceFromDragStart()) / 512.f) * ((xReverse || yReverse) ? 1.f : -1.f));

            //const float dragDistance = 0.05f; //float(e.getDistanceFromDragStart()) / 512.f;
            //const float flattenCurvature = (newCurvature < this->getCurvature()) ? this->getCurvature() - dragDistance : this->getCurvature() + dragDistance;

            //Logger::writeToLog(String(newCurvature));

            // todo! right helper dragging
            AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getSequence());
            autoLayer->change(this->event, this->event.withCurvature(newCurvature), true);
        }
    }
    //[/UserCode_mouseDrag]
}

void AutomationCurveHelper::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            //this->constrainPosition();
            this->draggingState = false;
        }

        this->repaint();
    }
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

float AutomationCurveHelper::getCurvature() const
{
    return this->event.getCurvature();
}

float AutomationCurveHelper::constrainPosition()
{
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return 0.5f;
    }

    const int yCentreAnchor(this->anchor.getY());
    const int yCentreMy(this->getBounds().getCentre().getY());
    const int yCentre1(this->component1->getBounds().getCentre().getY());
    const int yCentre2(this->component2->getBounds().getCentre().getY());

    const int yMin = jmin(yCentre1, yCentre2);
    const int yMax = jmax(yCentre1, yCentre2);
    const int yLength = yMax - yMin;

    const int newCenterY = jmin(yMax, jmax(yMin, yCentreMy));


    const float upperAnchorLength = float(yCentreAnchor - yMin);
    const float lowerAnchorLength = float(yMax - yCentreAnchor);

    const float upperCurveDelta = float(1.f - this->curveAnchor);
    const float lowerCurveDelta = float(this->curveAnchor);

    const float upperAnchorDragDelta = float(newCenterY - yCentreAnchor);
    const float lowerAnchorDragDelta = float(yCentreAnchor - newCenterY);

    const float upperCurveDragDelta = (upperAnchorLength > 0.f) ? (upperCurveDelta * (upperAnchorDragDelta / upperAnchorLength)) : 0.f;
    const float lowerCurveDragDelta = (lowerAnchorLength > 0.f) ? (lowerCurveDelta * (lowerAnchorDragDelta / lowerAnchorLength)) : 0.f;

    //Logger::writeToLog("upperCurveDragDelta: " + String(upperCurveDragDelta));
    //Logger::writeToLog("lowerCurveDragDelta: " + String(lowerCurveDragDelta));
    //this->setCentrePosition(this->anchor.getX(), newCenterY);

    float newCurviness = this->curveAnchor - upperCurveDragDelta + lowerCurveDragDelta;

    return newCurviness;
    //return 1.f - (float(newCenterY - yMin) / float(yLength));
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AutomationCurveHelper" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="AutomationCurveSequenceMap &amp;parent, const AutomationEvent &amp;targetEvent, Component *target1, Component *target2"
                 variableInitialisers="editor(parent),&#10;event(targetEvent),&#10;component1(target1),&#10;component2(target2),&#10;draggingState(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="hitTest (int x, int y)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ELLIPSE pos="0 0 0M 0M" fill="solid: 2bfefefe" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
