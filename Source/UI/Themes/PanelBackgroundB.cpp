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

#include "PanelBackgroundB.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "Icons.h"
//[/MiscUserDefs]

PanelBackgroundB::PanelBackgroundB()
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->setOpaque(true);
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    //[/Constructor]
}

PanelBackgroundB::~PanelBackgroundB()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void PanelBackgroundB::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour = Colour (0xff474c90);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    if (theme.getBgCacheB().isValid())
    {
        g.setTiledImageFill(theme.getBgCacheB(), 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());
    }
    else
    {
        g.setColour(findDefaultColour(ColourIDs::BackgroundB::fill));
        g.fillRect(this->getLocalBounds());
    }

    //[/UserPaint]
}

void PanelBackgroundB::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

void PanelBackgroundB::updateRender(HelioTheme &theme)
{
    if (theme.getBgCacheB().isValid())
    {
        return;
    }

    //const Desktop::Displays::Display &d = Desktop::getInstance().getDisplays().getMainDisplay();
    //const int scale = int(d.scale);
    const int w = 128; // d.totalArea.getWidth() * scale;
    const int h = 128; // d.totalArea.getHeight() * scale;

    Image render(Image::ARGB, w, h, true);
    Graphics g(render);
    g.setColour(theme.findColour(ColourIDs::BackgroundB::fill));
    g.fillAll();
    HelioTheme::drawNoise(theme, g, 0.5f);
    theme.getBgCacheB() = render;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelBackgroundB" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="0"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: ff474c90" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
