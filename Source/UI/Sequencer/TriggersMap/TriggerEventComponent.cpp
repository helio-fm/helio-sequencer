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

#include "TriggerEventComponent.h"

//[MiscUserDefs]
#include "TriggersTrackMap.h"
#include "TriggerEventConnector.h"
#include "AutomationSequence.h"

// a hack
//#define proportionOfWidth(x) ((this->getWidth() * x) + (this->getWidth() - this->realBounds.getWidth()))
#define proportionOfWidth(x) (this->realBounds.getWidth() * (x))

//[/MiscUserDefs]

TriggerEventComponent::TriggerEventComponent(TriggersTrackMap &parent, const AutomationEvent &targetEvent)
    : event(targetEvent),
      editor(parent)
{

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->setOpaque(false);
//    this->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, false);
    this->setPaintingIsUnclipped(true);
    this->recreateConnector();
    this->setMouseCursor(MouseCursor::PointingHandCursor);
    //[/UserPreSize]

    setSize (64, 32);

    //[Constructor]
    //[/Constructor]
}

TriggerEventComponent::~TriggerEventComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void TriggerEventComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    g.setColour (Colours::black);
    g.strokePath (internalPath1, PathStrokeType (1.000f));

    g.setColour (Colour (0xff002dff));
    g.strokePath (internalPath2, PathStrokeType (1.000f));

    g.setColour (Colours::red);
    g.strokePath (internalPath3, PathStrokeType (1.000f));

    g.setColour (Colour (0xff00ff03));
    g.strokePath (internalPath4, PathStrokeType (1.000f));

    //[UserPaint] Add your own custom painting code here..
#endif

    const bool prevDownState = (this->prevEventHolder) ? this->prevEventHolder->isPedalDownEvent() : DEFAULT_TRIGGER_AUTOMATION_EVENT_STATE;

    g.setColour(Colours::white.withAlpha(0.15f));

    if (this->event.isPedalDownEvent() && !prevDownState)
    {
        g.fillPath(internalPath2);
        //g.strokePath (internalPath2, PathStrokeType (2.0f));
    }
    else if (this->event.isPedalUpEvent() && prevDownState)
    {
        g.fillPath(internalPath1);
        //g.strokePath (internalPath1, PathStrokeType (2.0f));
    }
    else if (this->event.isPedalDownEvent() && prevDownState)
    {
        g.fillPath(internalPath4);
        //g.strokePath (internalPath4, PathStrokeType (2.0f));
    }
    else if (this->event.isPedalUpEvent() && !prevDownState)
    {
        g.fillPath(internalPath3);
        //g.strokePath (internalPath3, PathStrokeType (2.0f));
    }

    //[/UserPaint]
}

void TriggerEventComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    internalPath1.clear();
    internalPath1.startNewSubPath (static_cast<float> (proportionOfWidth (0.0000f)), static_cast<float> (getHeight()));
    internalPath1.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), 1.0f);
    internalPath1.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), static_cast<float> (getHeight()));
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (static_cast<float> (proportionOfWidth (0.0000f)), 1.0f);
    internalPath2.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), static_cast<float> (getHeight()));
    internalPath2.lineTo (static_cast<float> (proportionOfWidth (0.0000f)), static_cast<float> (getHeight()));
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (static_cast<float> (proportionOfWidth (0.0000f)), 1.0f);
    internalPath3.lineTo (static_cast<float> (proportionOfWidth (0.5000f)), static_cast<float> (getHeight() - 1));
    internalPath3.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), 1.0f);
    internalPath3.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), static_cast<float> (getHeight()));
    internalPath3.lineTo (static_cast<float> (proportionOfWidth (0.0000f)), static_cast<float> (getHeight()));
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath (static_cast<float> (proportionOfWidth (0.0000f)), static_cast<float> (getHeight()));
    internalPath4.lineTo (static_cast<float> (proportionOfWidth (0.5000f)), 1.0f);
    internalPath4.lineTo (static_cast<float> (proportionOfWidth (1.0000f)), static_cast<float> (getHeight()));
    internalPath4.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TriggerEventComponent::moved()
{
    //[UserCode_moved] -- Add your code here...
    this->updateConnector();
    //[/UserCode_moved]
}

void TriggerEventComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        this->event.getLayer()->checkpoint();
        this->dragger.startDraggingComponent(this, e);
        this->draggingState = true;
    }
    //[/UserCode_mouseDown]
}

void TriggerEventComponent::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            this->dragger.dragComponent(this, e, nullptr);
            float newRoundBeat = this->editor.getBeatByXPosition(this->getX() + int(this->getAnchor() * this->getWidth()));
            this->drag(newRoundBeat);
        }
    }
    //[/UserCode_mouseDrag]
}

void TriggerEventComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->draggingState = false;
        }

        if (e.getDistanceFromDragStart() < 2)
        {
#if 0
            AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getLayer());

            const int myIndex = autoLayer->indexOfSorted(&this->event);
            const bool hasPreviousEvent = (myIndex > 0);
            const bool hasNextEvent = (myIndex < (autoLayer->size() - 1));
            bool prevEventIsFarEnough = false;
            bool nextEventIsFarEnough = false;
            bool prevControllerValuesWillMatch = false;
            bool nextControllerValuesWillMatch = false;
            bool justInsertedTriggerEvent = false;

            if (hasPreviousEvent)
            {
                AutomationEvent *prevEvent = static_cast<AutomationEvent *>(autoLayer->getUnchecked(myIndex - 1));

                const float beatDelta = this->event.getBeat() - prevEvent->getBeat();
                const float cvDelta = fabs(this->event.getControllerValue() - prevEvent->getControllerValue());

                prevEventIsFarEnough = (beatDelta > 4.f);
                prevControllerValuesWillMatch = ((1.f - cvDelta) < 0.01f);

                //if (prevEventIsFarEnough && prevControllerValuesWillMatch)
                //{
                //    // turned off, it feels wierd
                //    justInsertedTriggerEvent = true;
                //    autoLayer->insert(this->event.withDeltaBeat(-0.5f).copyWithNewId(), true);
                //}
            }

            //const bool canInvert = !hasPreviousEvent || justInsertedTriggerEvent || (hasPreviousEvent && !prevControllerValuesWillMatch);

            // пусть переключать можно будет только крайние события
            const bool canInvert = !hasPreviousEvent || !hasNextEvent;

            if (canInvert)
            {
                autoLayer->change(this->event, this->event.withInvertedControllerValue(), true);
            }
#endif
        }
    }
    else if (e.mods.isRightButtonDown())
    {
        this->editor.removeEventIfPossible(this->event);
    }
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

void TriggerEventComponent::drag(float targetBeat)
{
    float newRoundBeat = targetBeat;
    AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getLayer());

    // ограничим перемещение отрезком между двумя соседними компонентами
    const int myIndex = autoLayer->indexOfSorted(&this->event);
    const bool hasPreviousEvent = (myIndex > 0);
    const bool hasNextEvent = (myIndex < (autoLayer->size() - 1));
    const float minBeatOffset = 0.5f;


    if (hasPreviousEvent)
    {
        newRoundBeat = jmax(autoLayer->getUnchecked(myIndex - 1)->getBeat() + minBeatOffset, newRoundBeat);
    }

    if (hasNextEvent)
    {
        newRoundBeat = jmin(autoLayer->getUnchecked(myIndex + 1)->getBeat() - minBeatOffset, newRoundBeat);
    }

    const float newRoundDeltaBeat = (newRoundBeat - this->event.getBeat());


    if (fabs(newRoundDeltaBeat) > 0.01)
    {
        autoLayer->change(this->event, this->event.withBeat(newRoundBeat), true);
    }
    else
    {
        this->editor.updateEventComponent(this);
    }
}

void TriggerEventComponent::dragByDelta(float deltaBeat)
{
    this->drag(this->getBeat() + deltaBeat);
}

void TriggerEventComponent::recreateConnector()
{
    this->connector = new TriggerEventConnector(this, this->nextEventHolder, this->isPedalDownEvent());
    this->editor.addAndMakeVisible(this->connector);
    this->updateConnector();
}

void TriggerEventComponent::updateConnector()
{
    this->connector->resizeToFit(this->isPedalDownEvent());
}

void TriggerEventComponent::setNextNeighbour(TriggerEventComponent *next)
{
    if (next != nullptr)
    {
        next->repaint();
    }

    if (next == this->nextEventHolder)
    {
        this->updateConnector();
        return;
    }

    this->nextEventHolder = next;
    this->recreateConnector();
}

void TriggerEventComponent::setPreviousNeighbour(TriggerEventComponent *prev)
{
    if (prev == this->prevEventHolder)
    {
        //this->updateConnector();
        return;
    }

    // todo logic
    this->prevEventHolder = prev;
    //this->recreateConnector();
}

float TriggerEventComponent::getAnchor()
{
    return 1.0f;
}

bool TriggerEventComponent::isPedalDownEvent() const
{
    return this->event.isPedalDownEvent();
}

float TriggerEventComponent::getBeat() const
{
    return this->event.getBeat();
}

void TriggerEventComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->realBounds = bounds;
    this->setBounds(intBounds);
}

Rectangle<float> TriggerEventComponent::getRealBounds() const
{
    return this->realBounds;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TriggerEventComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="TriggersTrackMap &amp;parent, const AutomationEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;editor(parent)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="64" initialHeight="32">
  <METHODS>
    <METHOD name="moved()"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff000000" nonZeroWinding="1">s 0% 0R l 100% 1 l 100% 0R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff002dff" nonZeroWinding="1">s 0% 1 l 100% 0R l 0% 0R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ffff0000" nonZeroWinding="1">s 0% 1 l 50% 1R l 100% 1 l 100% 0R l 0% 0R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour="solid: ff00ff03" nonZeroWinding="1">s 0% 0R l 50% 1 l 100% 0R x</PATH>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
