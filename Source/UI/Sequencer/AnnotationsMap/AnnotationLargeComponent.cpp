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
#include "AnnotationsSequence.h"
//[/Headers]

#include "AnnotationLargeComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

AnnotationLargeComponent::AnnotationLargeComponent(AnnotationsTrackMap<AnnotationLargeComponent> &parent, const AnnotationEvent &targetEvent)
    : event(targetEvent),
      anchor(targetEvent),
      editor(parent),
      mouseDownWasTriggered(false),
      textWidth(0.f)
{
    addAndMakeVisible (annotationLabel = new Label ("annotationLabel",
                                                    String()));
    annotationLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    annotationLabel->setJustificationType (Justification::centredLeft);
    annotationLabel->setEditable (true, true, false);
    annotationLabel->setColour (Label::textColourId, Colour (0x99ffffff));
    annotationLabel->setColour (TextEditor::textColourId, Colours::black);
    annotationLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    annotationLabel->addListener (this);


    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    this->annotationLabel->setInterceptsMouseClicks(false, false);
    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->annotationLabel->setVisible(false);
    //this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    setSize (128, 32);

    //[Constructor]
    //[/Constructor]
}

AnnotationLargeComponent::~AnnotationLargeComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    annotationLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AnnotationLargeComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    g.setColour (Colour (0x88ffffff));
    g.setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawText (TRANS("..."),
                4, 4, getWidth() - 6, getHeight() - 8,
                Justification::centredLeft, true);

    g.setColour (Colour (0x20ffffff));
    g.fillRect (0, 0, getWidth() - 0, 3);

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setColour(this->event.getColour().interpolatedWith(Colours::white, 0.5f).withAlpha(75.f / 255.f));
    g.fillRect(0, 2, this->getWidth(), 3);

    if (this->event.getDescription().isNotEmpty())
    {
        const Font labelFont(16.00f, Font::plain);
        g.setColour(this->event.getColour().interpolatedWith(Colours::white, 0.55f).withAlpha(200.f / 255.f));

        GlyphArrangement arr;
        arr.addFittedText(labelFont,
                          this->event.getDescription(),
                          4.f + this->boundsOffset.getX(),
                          2.f,
                          float(this->getWidth()) - 16.f,
                          float(this->getHeight()) - 8.f,
                          Justification::centredLeft,
                          2,
                          0.85f);
        arr.draw(g);
    }
    //[/UserPaint]
}

void AnnotationLargeComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    annotationLabel->setBounds (4, -40, getWidth() - 6, getHeight() - 8);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AnnotationLargeComponent::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == annotationLabel)
    {
        //[UserLabelCode_annotationLabel] -- add your label text handling code here..
        //[/UserLabelCode_annotationLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void AnnotationLargeComponent::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}

void AnnotationLargeComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    //Logger::writeToLog("AnnotationEventComponent::mouseDown");
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

void AnnotationLargeComponent::mouseDrag (const MouseEvent& e)
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

                Array<AnnotationEvent> groupDragBefore, groupDragAfter;
                groupDragBefore.add(this->event);
                groupDragAfter.add(this->event.withBeat(newBeat));
                AnnotationsSequence *sequence = static_cast<AnnotationsSequence *>(this->event.getSequence());
                sequence->changeGroup(groupDragBefore, groupDragAfter, true);
            }
            else
            {
                this->editor.alignAnnotationComponent(this);
            }
        }
    }
    //[/UserCode_mouseDrag]
}

void AnnotationLargeComponent::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->draggingState = false;
            this->editor.onAnnotationMoved(this);
        }

        if (e.getDistanceFromDragStart() < 10 &&
            this->mouseDownWasTriggered &&
            !this->draggingHadCheckpoint)
        {
            this->editor.onAnnotationTapped(this);
        }
    }

    this->mouseDownWasTriggered = false;
    //[/UserCode_mouseUp]
}

void AnnotationLargeComponent::mouseDoubleClick (const MouseEvent& e)
{
    //[UserCode_mouseDoubleClick] -- Add your code here...
    //Logger::writeToLog("AnnotationLargeComponent::mouseDoubleClick");
    //[/UserCode_mouseDoubleClick]
}


//[MiscUserCode]

const AnnotationEvent &AnnotationLargeComponent::getEvent() const
{
    return this->event;
}

void AnnotationLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
                                          bounds.getY(),
                                          bounds.getWidth() - float(intBounds.getWidth()),
                                          bounds.getHeight());

    this->setBounds(intBounds);
}

float AnnotationLargeComponent::getBeat() const
{
    return this->event.getBeat();
}

void AnnotationLargeComponent::updateContent()
{
    if (this->annotationLabel->getText() != this->event.getDescription())
    {
        this->annotationLabel->setText(this->event.getDescription(), dontSendNotification);
        this->textWidth = float(this->annotationLabel->getFont().getStringWidth(this->event.getDescription()));
        //Logger::writeToLog("AnnotationLargeComponent::updateContent " + String(this->textWidth));
    }

    this->repaint();
}

float AnnotationLargeComponent::getTextWidth() const
{
    return this->textWidth;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AnnotationLargeComponent"
                 template="../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="AnnotationsTrackMap&lt;AnnotationLargeComponent&gt; &amp;parent, const AnnotationEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;anchor(targetEvent),&#10;editor(parent),&#10;mouseDownWasTriggered(false),&#10;textWidth(0.f)"
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
    <TEXT pos="4 4 6M 8M" fill="solid: 88ffffff" hasStroke="0" text="..."
          fontname="Default font" fontsize="16" kerning="0" bold="0" italic="0"
          justification="33"/>
    <RECT pos="0 0 0M 3" fill="solid: 20ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="annotationLabel" id="3dbd8cef4b61c2fe" memberName="annotationLabel"
         virtualName="" explicitFocusOrder="0" pos="4 -40 6M 8M" textCol="99ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
