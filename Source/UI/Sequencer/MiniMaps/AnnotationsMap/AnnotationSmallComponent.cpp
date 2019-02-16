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
#include "CachedLabelImage.h"
//[/Headers]

#include "AnnotationSmallComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

AnnotationSmallComponent::AnnotationSmallComponent(AnnotationsProjectMap<AnnotationSmallComponent> &parent, const AnnotationEvent &targetEvent)
    : event(targetEvent),
      editor(parent),
      textWidth(0.f)
{
    addAndMakeVisible (annotationLabel = new Label (String(),
                                                    TRANS("...")));
    annotationLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    annotationLabel->setJustificationType (Justification::centredLeft);
    annotationLabel->setEditable (false, false, false);


    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->annotationLabel->setInterceptsMouseClicks(false, false);

    // Label is small and fixed size, do not redraw it all the time:
    this->annotationLabel->setBufferedToImage(true);
    this->annotationLabel->setCachedComponentImage(new CachedLabelImage(*this->annotationLabel));
    //[/UserPreSize]

    setSize (128, 32);

    //[Constructor]
    //[/Constructor]
}

AnnotationSmallComponent::~AnnotationSmallComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    annotationLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AnnotationSmallComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    const Colour baseColour(findDefaultColour(Label::textColourId));
    g.setColour(this->event.getTrackColour().interpolatedWith(baseColour, 0.55f).withAlpha(0.2f));
    g.drawHorizontalLine(this->getHeight() - 4, 0.f, float(this->getWidth() - 4));
    //[/UserPaint]
}

void AnnotationSmallComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    annotationLabel->setBounds (-2, getHeight() - 4 - 16, 160, 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AnnotationSmallComponent::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->setSize(this->getWidth(), this->getParentHeight());
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]

const AnnotationEvent &AnnotationSmallComponent::getEvent() const
{
    return this->event;
}

void AnnotationSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
                                          bounds.getY(),
                                          bounds.getWidth() - float(intBounds.getWidth()),
                                          bounds.getHeight());

    this->setBounds(intBounds);
}

float AnnotationSmallComponent::getBeat() const
{
    return this->event.getBeat();
}

void AnnotationSmallComponent::updateContent()
{
    if (this->annotationLabel->getText() != this->event.getDescription() ||
        this->lastColour != this->event.getTrackColour())
    {
        this->lastColour = this->event.getTrackColour();
        this->annotationLabel->setText(this->event.getDescription(), dontSendNotification);
        this->annotationLabel->setColour(Label::textColourId, this->lastColour.interpolatedWith(Colours::white, 0.55f));
        this->textWidth = float(this->annotationLabel->getFont().getStringWidth(this->event.getDescription()));
    }

    this->repaint();
}

float AnnotationSmallComponent::getTextWidth() const
{
    return this->textWidth;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AnnotationSmallComponent"
                 template="../../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="AnnotationsProjectMap&lt;AnnotationSmallComponent&gt; &amp;parent, const AnnotationEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;editor(parent),&#10;textWidth(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="annotationLabel" virtualName=""
         explicitFocusOrder="0" pos="-2 4Rr 160 16" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
