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

#include "ClipRangeIndicator.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ClipRangeIndicator::ClipRangeIndicator()
    : firstBeat(0.f),
      lastBeat(0.f)
{

    //[UserPreSize]
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (4, 4);

    //[Constructor]
    //[/Constructor]
}

ClipRangeIndicator::~ClipRangeIndicator()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ClipRangeIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(this->paintColour);
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
    //g.fillRect(this->getLocalBounds());
    //[/UserPaint]
}

void ClipRangeIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

bool ClipRangeIndicator::updateWith(const Colour &colour, float firstBeat, float lastBeat)
{
    bool updatedRange = false;

    if (this->trackColour != colour)
    {
        this->trackColour = colour;
        this->paintColour = colour.interpolatedWith(Colours::white, 0.5f).withAlpha(0.45f);
        this->repaint();
    }

    if (this->firstBeat != firstBeat)
    {
        updatedRange = true;
        this->firstBeat = firstBeat;
    }

    if (this->lastBeat != lastBeat)
    {
        updatedRange = true;
        this->lastBeat = lastBeat;
    }

    return updatedRange;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ClipRangeIndicator" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="firstBeat(0.f),&#10;lastBeat(0.f)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="4" initialHeight="4">
  <BACKGROUND backgroundColour="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
