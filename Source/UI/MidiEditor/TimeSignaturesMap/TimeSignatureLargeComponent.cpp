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
#include "TimeSignaturesLayer.h"
//[/Headers]

#include "TimeSignatureLargeComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

TimeSignatureLargeComponent::TimeSignatureLargeComponent(TimeSignaturesTrackMap<TimeSignatureLargeComponent> &parent, const TimeSignatureEvent &targetEvent)
    : event(targetEvent),
      anchor(targetEvent),
      editor(parent),
      mouseDownWasTriggered(false)
{
    addAndMakeVisible (numeratorLabel = new Label (String(),
                                                   TRANS("16")));
    numeratorLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    numeratorLabel->setJustificationType (Justification::centredRight);
    numeratorLabel->setEditable (false, false, false);
    numeratorLabel->setColour (Label::textColourId, Colour (0x99ffffff));
    numeratorLabel->setColour (TextEditor::textColourId, Colours::black);
    numeratorLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (divLabel = new Label (String(),
                                             TRANS("/")));
    divLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    divLabel->setJustificationType (Justification::centredLeft);
    divLabel->setEditable (false, false, false);
    divLabel->setColour (Label::textColourId, Colour (0x99ffffff));
    divLabel->setColour (TextEditor::textColourId, Colours::black);
    divLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (denominatorLabel = new Label (String(),
                                                     TRANS("16")));
    denominatorLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    denominatorLabel->setJustificationType (Justification::centredLeft);
    denominatorLabel->setEditable (false, false, false);
    denominatorLabel->setColour (Label::textColourId, Colour (0x99ffffff));
    denominatorLabel->setColour (TextEditor::textColourId, Colours::black);
    denominatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    this->setOpaque(false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->divLabel->setInterceptsMouseClicks(false, false);
    this->numeratorLabel->setInterceptsMouseClicks(false, false);
    this->denominatorLabel->setInterceptsMouseClicks(false, false);
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
    divLabel = nullptr;
    denominatorLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TimeSignatureLargeComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    g.setColour (Colour (0x20ffffff));
    g.fillRect (0, 0, 2, getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void TimeSignatureLargeComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    numeratorLabel->setBounds (-14, 0, 42, 24);
    divLabel->setBounds (20, 0, 24, 32);
    denominatorLabel->setBounds (29, 8, 32, 24);
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
    //Logger::writeToLog("TimeSignatureEventComponent::mouseDown");
    this->mouseDownWasTriggered = true;

    if (e.mods.isLeftButtonDown())
    {
        // don't checkpoint right here, but only before the actual change
        //this->event.getLayer()->checkpoint();

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
                    this->event.getLayer()->checkpoint();
                    this->draggingHadCheckpoint = true;
                }

                Array<TimeSignatureEvent> groupDragBefore, groupDragAfter;
                groupDragBefore.add(this->event);
                groupDragAfter.add(this->event.withBeat(newBeat));
                TimeSignaturesLayer *autoLayer = static_cast<TimeSignaturesLayer *>(this->event.getLayer());
                autoLayer->changeGroup(groupDragBefore, groupDragAfter, true);
            }

            this->editor.alignTimeSignatureComponent(this);
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
    //Logger::writeToLog("TimeSignatureLargeComponent::mouseDoubleClick");
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
    this->numeratorLabel->setText(String(this->event.getNumerator()), dontSendNotification);
    this->denominatorLabel->setText(String(this->event.getDenominator()), dontSendNotification);
    Logger::writeToLog("TimeSignatureLargeComponent::updateContent");
    this->repaint();
}

float TimeSignatureLargeComponent::getTextWidth() const
{
    return 32;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TimeSignatureLargeComponent"
                 template="../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="TimeSignaturesTrackMap&lt;TimeSignatureLargeComponent&gt; &amp;parent, const TimeSignatureEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;anchor(targetEvent),&#10;editor(parent),&#10;mouseDownWasTriggered(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="mouseMove (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDoubleClick (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 2 0M" fill="solid: 20ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="numeratorLabel" virtualName=""
         explicitFocusOrder="0" pos="-14 0 42 24" textCol="99ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="16" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="5710d8ba7f669fd0" memberName="divLabel" virtualName=""
         explicitFocusOrder="0" pos="20 0 24 32" textCol="99ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="/" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="971a291c01db0330" memberName="denominatorLabel" virtualName=""
         explicitFocusOrder="0" pos="29 8 32 24" textCol="99ffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="16" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
