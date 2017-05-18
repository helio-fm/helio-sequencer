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

#include "ColourSwatches.h"

//[MiscUserDefs]
#include "CommandPanel.h"
//[/MiscUserDefs]

ColourSwatches::ColourSwatches()
{

    //[UserPreSize]
	const StringPairArray colours(CommandPanel::getColoursList());
	for (const auto c : colours.getAllValues())
	{
		const Colour colour(Colour::fromString(c));
		ScopedPointer<ColourButton> button(new ColourButton(colour, this));
		this->addAndMakeVisible(button);
		this->buttons.add(button.release());
	}
    //[/UserPreSize]

    setSize (384, 42);

    //[Constructor]
    //[/Constructor]
}

ColourSwatches::~ColourSwatches()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ColourSwatches::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        float x = 1.0f, y = 1.0f, width = 40.0f, height = 40.0f;
        Colour fillColour = Colour (0xffa52a90);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
    }

    {
        float x = 42.0f, y = 1.0f, width = 40.0f, height = 40.0f;
        Colour fillColour = Colour (0xff5f6cff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
    }

    {
        float x = 343.0f, y = 1.0f, width = 40.0f, height = 40.0f;
        Colour fillColour = Colour (0xffff5f5f);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
    }

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void ColourSwatches::resized()
{
    //[UserPreResize] Add your own custom resize code here..
	int x = 0;
	for (const auto &button : this->buttons)
	{
		const int w = this->getWidth() / this->buttons.size();
		button->setBounds(x, 0, w, this->getHeight());
		x += w;
	}
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void ColourSwatches::onColourButtonClicked(ColourButton *clickedButton)
{
	for (const auto &button : this->buttons)
	{
		if (button != clickedButton)
		{
			button->deselect();
		}
	}

	if (ColourButtonListener *parentListener =
		dynamic_cast<ColourButtonListener *>(this->getParentComponent()))
	{
		parentListener->onColourButtonClicked(clickedButton);
	}
}

void ColourSwatches::setSelectedColour(Colour colour)
{
	for (const auto &button : this->buttons)
	{
		if (button->getColour() == colour)
		{ button->select(); }
		else
		{ button->deselect(); }
	}
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ColourSwatches" template="../../Template"
                 componentName="" parentClasses="public Component, public ColourButtonListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="384"
                 initialHeight="42">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="1 1 40 40" cornerSize="2" fill="solid: ffa52a90" hasStroke="0"/>
    <ROUNDRECT pos="42 1 40 40" cornerSize="2" fill="solid: ff5f6cff" hasStroke="0"/>
    <ROUNDRECT pos="343 1 40 40" cornerSize="2" fill="solid: ffff5f5f" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
