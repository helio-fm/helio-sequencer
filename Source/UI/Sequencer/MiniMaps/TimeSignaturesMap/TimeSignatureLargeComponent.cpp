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
#include "TimeSignaturesSequence.h"
#include "HybridRoll.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
//[/Headers]

#include "TimeSignatureLargeComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

TimeSignatureLargeComponent::TimeSignatureLargeComponent(TimeSignaturesProjectMap<TimeSignatureLargeComponent> &parent, const TimeSignatureEvent &targetEvent)
    : event(targetEvent),
      editor(parent),
      anchor(targetEvent),
      numerator(0),
      denominator(0),
      mouseDownWasTriggered(false)
{
    addAndMakeVisible (numeratorLabel = new Label (String(),
                                                   String()));
    numeratorLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    numeratorLabel->setJustificationType (Justification::centredLeft);
    numeratorLabel->setEditable (false, false, false);

    numeratorLabel->setBounds (-2, 4, 32, 14);

    addAndMakeVisible (denominatorLabel = new Label (String(),
                                                     String()));
    denominatorLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    denominatorLabel->setJustificationType (Justification::centredLeft);
    denominatorLabel->setEditable (false, false, false);

    denominatorLabel->setBounds (-2, 17, 32, 14);


    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->numeratorLabel->setInterceptsMouseClicks(false, false);
    this->denominatorLabel->setInterceptsMouseClicks(false, false);
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    this->numeratorLabel->setBufferedToImage(true);
    this->numeratorLabel->setCachedComponentImage(new CachedLabelImage(*this->numeratorLabel));

    this->denominatorLabel->setBufferedToImage(true);
    this->denominatorLabel->setCachedComponentImage(new CachedLabelImage(*this->denominatorLabel));
    //[/UserPreSize]

    setSize (128, 32);

    //[Constructor]
    //[/Constructor]
}

TimeSignatureLargeComponent::~TimeSignatureLargeComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    numeratorLabel = nullptr;
    denominatorLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TimeSignatureLargeComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    const Colour lineCol(findDefaultColour(ColourIDs::Roll::headerSnaps));
    g.setColour(lineCol);
    g.fillRect(0.f, 1.f, float(this->getWidth()), 2.f);
    //[/UserPaint]
}

void TimeSignatureLargeComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TimeSignatureLargeComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}

void TimeSignatureLargeComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->mouseDownWasTriggered = true;

    if (e.mods.isLeftButtonDown())
    {
        // don't checkpoint right here, but only before the actual change
        //this->event.getSequence()->checkpoint();

        this->dragger.startDraggingComponent(this, e);
        this->draggingHadCheckpoint = false;
        this->draggingState = true;
        this->anchor = this->event;
    }
    else
    {
        this->editor.alternateActionFor(this);
        //this->editor.showContextMenuFor(this);
    }
    //[/UserCode_mouseDown]
}

void TimeSignatureLargeComponent::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            this->dragger.dragComponent(this, e, nullptr);
            const float newBeat = this->editor.getBeatByXPosition(this->getX());
            const bool beatHasChanged = (this->event.getBeat() != newBeat);

            if (beatHasChanged)
            {
                if (! this->draggingHadCheckpoint)
                {
                    this->event.getSequence()->checkpoint();
                    this->draggingHadCheckpoint = true;
                }

                Array<TimeSignatureEvent> groupDragBefore, groupDragAfter;
                groupDragBefore.add(this->event);
                groupDragAfter.add(this->event.withBeat(newBeat));
                TimeSignaturesSequence *sequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
                sequence->changeGroup(groupDragBefore, groupDragAfter, true);
            }
            else
            {
                this->editor.alignTimeSignatureComponent(this);
            }
        }
    }
    //[/UserCode_mouseDrag]
}

void TimeSignatureLargeComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->draggingState = false;
            this->editor.onTimeSignatureMoved(this);
        }

        if (e.getDistanceFromDragStart() < 10 &&
            this->mouseDownWasTriggered &&
            !this->draggingHadCheckpoint)
        {
            this->editor.onTimeSignatureTapped(this);
        }
    }

    this->mouseDownWasTriggered = false;
    //[/UserCode_mouseUp]
}

void TimeSignatureLargeComponent::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    //[/UserCode_mouseDoubleClick]
}


//[MiscUserCode]

const TimeSignatureEvent &TimeSignatureLargeComponent::getEvent() const
{
    return this->event;
}

void TimeSignatureLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
                                          bounds.getY(),
                                          bounds.getWidth() - float(intBounds.getWidth()),
                                          bounds.getHeight());

    this->setBounds(intBounds);
}

float TimeSignatureLargeComponent::getBeat() const
{
    return this->event.getBeat();
}

void TimeSignatureLargeComponent::updateContent()
{
    if (this->numerator != this->event.getNumerator() ||
        this->denominator != this->event.getDenominator())
    {
        this->numerator = this->event.getNumerator();
        this->denominator = this->event.getDenominator();
        this->numeratorLabel->setText(String(this->numerator), dontSendNotification);
        this->denominatorLabel->setText(String(this->denominator), dontSendNotification);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TimeSignatureLargeComponent"
                 template="../../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="TimeSignaturesProjectMap&lt;TimeSignatureLargeComponent&gt; &amp;parent, const TimeSignatureEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;editor(parent),&#10;anchor(targetEvent),&#10;numerator(0),&#10;denominator(0),&#10;mouseDownWasTriggered(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="numeratorLabel" virtualName=""
         explicitFocusOrder="0" pos="-2 4 32 14" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <LABEL name="" id="48b6c750cc766a42" memberName="denominatorLabel" virtualName=""
         explicitFocusOrder="0" pos="-2 17 32 14" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
