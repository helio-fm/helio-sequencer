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

#include "ColourButton.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class ColourButtonFrame final : public Component
{
public:

    void paint(Graphics &g) override
    {
        const int y1 = 2;
        const int y2 = this->getHeight() - 2;
        const int x1 = 2;
        const int x2 = this->getWidth() - 2;

        g.setColour(findDefaultColour(ColourIDs::ColourButton::outline).withAlpha(0.5f));
        g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
        g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
        g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
        g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    }
};
//[/MiscUserDefs]

ColourButton::ColourButton(Colour c, ColourButtonListener *listener)
    : index(0),
      colour(c),
      owner(listener)
{

    //[UserPreSize]
    this->setMouseClickGrabsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

ColourButton::~ColourButton()
{
    //[Destructor_pre]
    this->checkMark = nullptr;
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ColourButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    const int y1 = 2;
    const int y2 = this->getHeight() - 2;
    const int x1 = 2;
    const int x2 = this->getWidth() - 2;

    const Colour base(findDefaultColour(ColourIDs::ColourButton::outline));

    // To avoid smoothed rectangles:
    g.setColour(this->colour.interpolatedWith(base, 0.25f).withAlpha(0.9f));
    //g.fillRect(x1, y2 - 4, x2 - x1 + 1, 5);
    g.fillRect(x1, y1, x2 - x1 + 1, 5);

    g.setColour(this->colour.interpolatedWith(base, 0.5f).withAlpha(0.1f));
    g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
    g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
    g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
    g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    //[/UserPaint]
}

void ColourButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    if (this->checkMark != nullptr)
    {
        const int s = jmin(this->getHeight(), this->getWidth()) - 4;
        this->checkMark->setSize(s, s);
        const auto c = this->getLocalBounds().getCentre();
        this->checkMark->setCentrePosition(c.x, c.y + 3);
        //this->checkMark->setBounds(this->getLocalBounds().reduced(8));
    }
    //[/UserResized]
}

void ColourButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->checkMark == nullptr)
    {
        this->select();
        this->owner->onColourButtonClicked(this);
    }
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
Component *ColourButton::createHighlighterComponent()
{
    return new ColourButtonFrame();
}

void ColourButton::select()
{
    if (this->checkMark == nullptr)
    {
        this->checkMark = new IconComponent(Icons::apply);
        this->addChildComponent(this->checkMark);
        this->resized();
        this->fader.fadeIn(this->checkMark, 100);
    }
}

void ColourButton::deselect()
{
    if (this->checkMark != nullptr)
    {
        this->fader.fadeOutSnapshot(this->checkMark, 200);
        this->checkMark = nullptr;
    }
}

int ColourButton::getButtonIndex() const noexcept
{
    return this->index;
}

void ColourButton::setButtonIndex(int val)
{
    this->index = val;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ColourButton" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="Colour c, ColourButtonListener *listener"
                 variableInitialisers="index(0),&#10;colour(c),&#10;owner(listener)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
