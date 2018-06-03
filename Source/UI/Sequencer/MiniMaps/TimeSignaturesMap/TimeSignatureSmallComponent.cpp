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
#include "CachedLabelImage.h"
//[/Headers]

#include "TimeSignatureSmallComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

TimeSignatureSmallComponent::TimeSignatureSmallComponent(TimeSignaturesProjectMap<TimeSignatureSmallComponent> &parent, const TimeSignatureEvent &targetEvent)
    : event(targetEvent),
      editor(parent)
{
    addAndMakeVisible (signatureLabel = new Label (String(),
                                                   TRANS("...")));
    signatureLabel->setFont (Font (14.00f, Font::plain).withTypefaceStyle ("Regular"));
    signatureLabel->setJustificationType (Justification::centredLeft);
    signatureLabel->setEditable (false, false, false);

    signatureLabel->setBounds (0, 4, 48, 16);

    addAndMakeVisible (component = new SeparatorVertical());

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->signatureLabel->setInterceptsMouseClicks(false, false);

    this->signatureLabel->setBufferedToImage(true);
    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));
    //[/UserPreSize]

    setSize (128, 32);

    //[Constructor]
    //[/Constructor]
}

TimeSignatureSmallComponent::~TimeSignatureSmallComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    signatureLabel = nullptr;
    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TimeSignatureSmallComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TimeSignatureSmallComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (0, 0, 2, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TimeSignatureSmallComponent::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->setSize(this->getWidth(), this->getParentHeight());
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]

const TimeSignatureEvent &TimeSignatureSmallComponent::getEvent() const
{
    return this->event;
}

void TimeSignatureSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
                                          bounds.getY(),
                                          bounds.getWidth() - float(intBounds.getWidth()),
                                          bounds.getHeight());

    this->setBounds(intBounds);
}

float TimeSignatureSmallComponent::getBeat() const
{
    return this->event.getBeat();
}

void TimeSignatureSmallComponent::updateContent()
{
    this->signatureLabel->setText(this->event.toString(), dontSendNotification);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TimeSignatureSmallComponent"
                 template="../../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="TimeSignaturesProjectMap&lt;TimeSignatureSmallComponent&gt; &amp;parent, const TimeSignatureEvent &amp;targetEvent"
                 variableInitialisers="event(targetEvent),&#10;editor(parent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="3dbd8cef4b61c2fe" memberName="signatureLabel" virtualName=""
         explicitFocusOrder="0" pos="0 4 48 16" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="14.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="1e5a57ee127ef53d" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0 0 2 0M" sourceFile="../../../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
