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

#include "PanelBackgroundA.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "Icons.h"
//[/MiscUserDefs]

PanelBackgroundA::PanelBackgroundA()
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

PanelBackgroundA::~PanelBackgroundA()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void PanelBackgroundA::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour = Colour (0xff5156a1);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::BackgroundA::fill);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    if (theme.getBgCacheA().isValid())
    {
        g.setTiledImageFill(theme.getBgCacheA(), 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());
    }
    else
    {
        g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill));
        g.fillRect(this->getLocalBounds());
    }
    //[/UserPaint]
}

void PanelBackgroundA::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void PanelBackgroundA::updateRender(HelioTheme &theme)
{
    if (theme.getBgCacheA().isValid())
    {
        return;
    }

    const int w = 128; // d.totalArea.getWidth() * scale;
    const int h = 128; // d.totalArea.getHeight() * scale;

    Image render(Image::ARGB, w, h, true);
    Graphics g(render);
    g.setColour(theme.findColour(ColourIDs::BackgroundA::fill));
    g.fillAll();
    HelioTheme::drawNoise(theme, g, 0.5f);
    theme.getBgCacheA() = render;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelBackgroundA" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="0"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: ff5156a1" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
