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

#include "FineTuningValueIndicator.h"

//[MiscUserDefs]
//[/MiscUserDefs]

FineTuningValueIndicator::FineTuningValueIndicator(float initialValue, String suffix)
    : value(initialValue),
      suffix(suffix)
{
    this->valueLabel.reset(new Label(String(),
                                      TRANS("0.0")));
    this->addAndMakeVisible(valueLabel.get());
    this->valueLabel->setFont(Font (14.00f, Font::plain).withTypefaceStyle ("Regular"));
    valueLabel->setJustificationType(Justification::centred);
    valueLabel->setEditable(false, false, false);


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(64, 64);

    //[Constructor]
    //[/Constructor]
}

FineTuningValueIndicator::~FineTuningValueIndicator()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    valueLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void FineTuningValueIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    const float startAngleRadians = MathConstants<float>::pi * 1.5f;
    const float endAngleRadians = MathConstants<float>::pi * 2.5f;
    this->getLookAndFeel().drawRotarySlider(g, 0, 0,
        this->getWidth(), this->getHeight(), this->value,
        startAngleRadians, endAngleRadians, this->dummySlider);
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FineTuningValueIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    valueLabel->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), getHeight() - 2 - 24, getWidth() - 0, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

#define NUM_DECIMAL_PLACES (3)

void FineTuningValueIndicator::setValue(float newValue, float valueView)
{
    if (this->value != newValue)
    {
        this->value = newValue;
        this->valueLabel->setText(String(valueView, NUM_DECIMAL_PLACES) + this->suffix, dontSendNotification);
        this->repaint();
    }
}

void FineTuningValueIndicator::setValue(float newValue, int valueView)
{
    if (this->value != newValue)
    {
        this->value = newValue;
        this->valueLabel->setText(String(valueView) + this->suffix, dontSendNotification);
        this->repaint();
    }
}

void FineTuningValueIndicator::repositionToTargetAt(Component *component, Point<int> offset)
{
    this->setTopLeftPosition(offset + component->getBounds().getCentre() - this->getLocalBounds().getCentre());
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FineTuningValueIndicator"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="float initialValue, String suffix" variableInitialisers="value(initialValue)&#10;suffix(suffix)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="64" initialHeight="64">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="546fff7dc132314d" memberName="valueLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 2Rr 0M 24" labelText="0.0" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="14.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
