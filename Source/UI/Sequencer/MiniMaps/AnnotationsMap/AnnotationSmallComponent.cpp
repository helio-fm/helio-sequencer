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
#include "AnnotationsProjectMap.h"
#include "CachedLabelImage.h"
//[/Headers]

#include "AnnotationSmallComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

AnnotationSmallComponent::AnnotationSmallComponent(AnnotationsProjectMap &parent, const AnnotationEvent &targetEvent)
    : AnnotationComponent(parent, targetEvent),
      textWidth(0.f)
{
    this->annotationLabel.reset(new Label(String(),
                                           String()));
    this->addAndMakeVisible(annotationLabel.get());
    this->annotationLabel->setFont(Font (12.00f, Font::plain));
    annotationLabel->setJustificationType(Justification::centredLeft);
    annotationLabel->setEditable(false, false, false);


    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->annotationLabel->setInterceptsMouseClicks(false, false);

    // Label is small and fixed size, do not redraw it all the time:
    this->annotationLabel->setBufferedToImage(true);
    this->annotationLabel->setCachedComponentImage(new CachedLabelImage(*this->annotationLabel));
    //[/UserPreSize]

    this->setSize(128, 32);

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
    g.fillRect(0, this->getHeight() - 2, this->getWidth() - 4, 2);
    //[/UserPaint]
}

void AnnotationSmallComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    annotationLabel->setBounds(-2, getHeight() - 2 - 16, 160, 16);
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

void AnnotationSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = {
        bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight() };

    this->setBounds(intBounds);
}

void AnnotationSmallComponent::updateContent()
{
    if (this->annotationLabel->getText() != this->event.getDescription() ||
        this->lastColour != this->event.getTrackColour())
    {
        this->lastColour = this->event.getTrackColour();
        this->annotationLabel->setText(this->event.getDescription(), dontSendNotification);
        const auto fgColour = findDefaultColour(Label::textColourId);
        this->annotationLabel->setColour(Label::textColourId, this->lastColour.interpolatedWith(fgColour, 0.55f));
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
                 template="../../../../Template" componentName="" parentClasses="public AnnotationComponent"
                 constructorParams="AnnotationsProjectMap &amp;parent, const AnnotationEvent &amp;targetEvent"
                 variableInitialisers="AnnotationComponent(parent, targetEvent),&#10;textWidth(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="annotationLabel" virtualName=""
         explicitFocusOrder="0" pos="-2 2Rr 160 16" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12" kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



