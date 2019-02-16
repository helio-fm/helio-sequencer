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

#include "RadioButton.h"

//[MiscUserDefs]
#include "ComponentFader.h"

class RadioButtonFrame final : public Component
{
public:

    RadioButtonFrame(float alpha) : alpha(alpha)
    {
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const int y1 = 2;
        const int y2 = this->getHeight() - 2;
        const int x1 = 2;
        const int x2 = this->getWidth() - 2;

        const Colour baseColour(findDefaultColour(Label::textColourId));

        g.setColour(baseColour.withAlpha(this->alpha * 0.75f));
        g.fillRect(x1, y1, x2 - x1 + 1, 5);

        g.setColour(baseColour.withAlpha(this->alpha * 0.25f));
        g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
        g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
        g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
        g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    }

private:

    float alpha;
};
//[/MiscUserDefs]

RadioButton::RadioButton(const String &text, Colour c, RadioButtonListener *listener)
    : index(0),
      colour(c),
      owner(listener)
{
    addAndMakeVisible (label = new Label (String(),
                                          TRANS("C#")));
    label->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centred);
    label->setEditable (false, false, false);


    //[UserPreSize]
    this->label->setText(text, dontSendNotification);
    this->label->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

RadioButton::~RadioButton()
{
    //[Destructor_pre]
    this->checkMark = nullptr;
    //[/Destructor_pre]

    label = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RadioButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    const int y1 = 2;
    const int y2 = this->getHeight() - 2;
    const int x1 = 2;
    const int x2 = this->getWidth() - 2;

    const Colour baseColour(findDefaultColour(Label::textColourId));

    // To avoid smoothed rectangles:
    g.setColour(this->colour.interpolatedWith(baseColour, 0.25f).withAlpha(0.1f));
    //g.fillRect(x1, y2 - 4, x2 - x1 + 1, 5);
    g.fillRect(x1, y1, x2 - x1 + 1, 5);

    g.setColour(this->colour.interpolatedWith(baseColour, 0.5f).withAlpha(0.1f));
    g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
    g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
    g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
    g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    //[/UserPaint]
}

void RadioButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    label->setBounds (0, 3, getWidth() - 0, getHeight() - 3);
    //[UserResized] Add your own custom resize handling here..
    if (this->checkMark != nullptr)
    {
        this->checkMark->setBounds(this->getLocalBounds());
    }
    //[/UserResized]
}

void RadioButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->isSelected())
    { this->deselect(); }
    else
    { this->select(); }

    this->owner->onRadioButtonClicked(this);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
Component *RadioButton::createHighlighterComponent()
{
    return new RadioButtonFrame(0.5f);
}

void RadioButton::select()
{
    if (this->checkMark == nullptr)
    {
        this->checkMark = new RadioButtonFrame(1.f);
        this->addChildComponent(this->checkMark);
        this->resized();
        this->fader.fadeIn(this->checkMark, 100);
    }
}

void RadioButton::deselect()
{
    if (this->checkMark != nullptr)
    {
        this->fader.fadeOutSnapshot(this->checkMark, 200);
        this->checkMark = nullptr;
    }
}

int RadioButton::getButtonIndex() const noexcept
{
    return this->index;
}

void RadioButton::setButtonIndex(int val)
{
    this->index = val;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RadioButton" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="const String &amp;text, Colour c, RadioButtonListener *listener"
                 variableInitialisers="index(0),&#10;colour(c),&#10;owner(listener)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="ff14851992cbe505" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="0 3 0M 3M" labelText="C#" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18" kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
