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

#include "AutomationEventComponent.h"

//[MiscUserDefs]
#include "AutomationTrackMap.h"
#include "AutomationCurveHelper.h"
#include "AutomationEventsConnector.h"
#include "AutomationSequence.h"
//[/MiscUserDefs]

AutomationEventComponent::AutomationEventComponent(AutomationTrackMap &parent, const AutomationEvent &targetEvent)
    : event(targetEvent),
      anchor(targetEvent),
      editor(parent),
      draggingState(false)
{

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->setMouseCursor(MouseCursor::PointingHandCursor);

    this->setInterceptsMouseClicks(true, false);
    this->setPaintingIsUnclipped(true);
    this->recreateConnector();
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

AutomationEventComponent::~AutomationEventComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void AutomationEventComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x2bfefefe));
    g.fillEllipse (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0));

    g.setColour (Colour (0x3affffff));
    g.fillEllipse (5.0f, 5.0f, static_cast<float> (getWidth() - 10), static_cast<float> (getHeight() - 10));

    //[UserPaint] Add your own custom painting code here..
    if (this->draggingState)
    {
        g.setColour (Colour (0x2bfefefe));
        g.fillEllipse (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0));
    }
    //[/UserPaint]
}

void AutomationEventComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AutomationEventComponent::moved()
{
    //[UserCode_moved] -- Add your code here...
//    this->updateConnector();
//    this->updateHelper();
    //[/UserCode_moved]
}

bool AutomationEventComponent::hitTest (int x, int y)
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

void AutomationEventComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();
        this->dragger.startDraggingComponent(this, e);
        this->startDragging();
    }
    //[/UserCode_mouseDown]
}

void AutomationEventComponent::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            float deltaValue = 0.f;
            float deltaBeat = 0.f;
            const bool eventChanged = this->getDraggingDelta(e, deltaBeat, deltaValue);

            if (eventChanged)
            {
                this->setMouseCursor(MouseCursor::DraggingHandCursor);
                AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getSequence());
                autoLayer->change(this->event, this->continueDragging(deltaBeat, deltaValue), true);
            }
            else
            {
                this->editor.updateTempoComponent(this); // возвращаем на место активное событие
            }
        }
    }
    //[/UserCode_mouseDrag]
}

void AutomationEventComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);

            float deltaValue = 0.f;
            float deltaBeat = 0.f;
            this->getDraggingDelta(e, deltaBeat, deltaValue);
            this->editor.updateTempoComponent(this);
            this->endDragging();
        }

        this->repaint();
    }
    else if (e.mods.isRightButtonDown())
    {
        this->editor.removeEventIfPossible(this->event);
    }
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

void AutomationEventComponent::recreateConnector()
{
    this->connector = new AutomationEventsConnector(this->editor, this, this->nextEventHolder);
    this->editor.addAndMakeVisible(this->connector);
    this->updateConnector();
}

void AutomationEventComponent::recreateHelper()
{
    this->helper = new AutomationCurveHelper(this->editor, this->event, this, this->nextEventHolder);
    this->editor.addAndMakeVisible(this->helper);
    this->updateHelper();
}

void AutomationEventComponent::updateConnector()
{
    this->connector->resizeToFit(this->event.getCurvature());
}

void AutomationEventComponent::updateHelper()
{
    if (this->helper && this->nextEventHolder)
    {
        const float d = this->editor.getHelperDiameter();
        const Point<int> linePos(this->connector->getPosition());
        const Point<float> lineCentre(this->connector->getCentrePoint());
        Rectangle<int> bounds(0, 0, int(d), int(d));
        this->helper->setBounds(bounds.withPosition(lineCentre.toInt()).translated(linePos.getX() - int(d / 2), linePos.getY() - int(d / 2)));
    }
}

void AutomationEventComponent::setNextNeighbour(AutomationEventComponent *next)
{
    if (next == this->nextEventHolder)
    {
        this->updateConnector();
        this->updateHelper();
        return;
    }

    this->nextEventHolder = next;
    this->recreateConnector();

    if (this->nextEventHolder == nullptr)
    {
        this->helper = nullptr;
    }
    else
    {
        this->recreateHelper();
    }
}


//===----------------------------------------------------------------------===//
// Editing
//

void AutomationEventComponent::startDragging()
{
    this->draggingState = true;
    this->anchor = this->event;
}

bool AutomationEventComponent::isDragging() const
{
    return this->draggingState;
}

bool AutomationEventComponent::getDraggingDelta(const MouseEvent &e, float &deltaBeat, float &deltaValue)
{
    this->dragger.dragComponent(this, e, nullptr);

    float newValue = -1;
    float newBeat = -1;

    this->editor.getRowsColsByMousePosition(this->getX(),
                                            this->getY(),
                                            newValue, newBeat);

    deltaValue = (newValue - this->anchor.getControllerValue());
    deltaBeat = (newBeat - this->anchor.getBeat());

    const bool valueChanged = (this->getControllerValue() != newValue);
    const bool beatChanged = (this->getBeat() != newBeat);

    return (valueChanged || beatChanged);
}

AutomationEvent AutomationEventComponent::continueDragging(const float deltaBeat, const float deltaValue)
{
    const float &newValue = this->anchor.getControllerValue() + deltaValue;
    const float &newBeat = jmax(this->anchor.getBeat() + deltaBeat, float(this->editor.rollFirstBeat));
    return this->event.withParameters(newBeat, newValue);
}

void AutomationEventComponent::endDragging()
{
    this->draggingState = false;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AutomationEventComponent"
                 template="../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="AutomationTrackMap &amp;parent, const AutomationEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;anchor(targetEvent),&#10;editor(parent),&#10;draggingState(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="moved()"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="hitTest (int x, int y)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ELLIPSE pos="0 0 0M 0M" fill="solid: 2bfefefe" hasStroke="0"/>
    <ELLIPSE pos="5 5 10M 10M" fill="solid: 3affffff" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
