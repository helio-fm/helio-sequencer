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

#include "MenuItemComponentMarker.h"

//[MiscUserDefs]
//[/MiscUserDefs]

MenuItemComponentMarker::MenuItemComponentMarker()
{

    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(64, 64);

    //[Constructor]
    //[/Constructor]
}

MenuItemComponentMarker::~MenuItemComponentMarker()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void MenuItemComponentMarker::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    if (this->getWidth() > 64)
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour = Colour (0x0bffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(Label::textColourId).withAlpha(0.1f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    else
    {
        g.setColour(findDefaultColour(Label::textColourId).withAlpha(0.125f));
        g.fillRect(x, y - 1, w, 1);
        g.fillRect(x, y + h, w, 1);
        g.fillRect(x - 1, y, 1, h);
        g.fillRect(x + w, y, 1, h);
    }
    //[/UserPaint]
}

void MenuItemComponentMarker::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    const int minSize = jmin(this->getWidth(), this->getHeight());
    const auto squareMarker = Rectangle<int>(0, 0, minSize, minSize)
        .reduced(5).withCentre(this->getLocalBounds().getCentre());
    this->x = squareMarker.getX();
    this->y = squareMarker.getY();
    this->w = squareMarker.getWidth();
    this->h = squareMarker.getHeight();
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MenuItemComponentMarker"
                 template="../../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="64"
                 initialHeight="64">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: bffffff" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



