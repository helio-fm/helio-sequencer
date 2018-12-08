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

#include "PanelBackgroundC.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "Icons.h"

static void drawPanel(Graphics& g, HelioTheme &theme);
//[/MiscUserDefs]

PanelBackgroundC::PanelBackgroundC()
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

PanelBackgroundC::~PanelBackgroundC()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void PanelBackgroundC::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour1 = Colour (0xff28478b), fillColour2 = Colour (0xff1e2a51);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (1.0052f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.0697f)) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (proportionOfWidth (-0.1087f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.8561f)) - 0.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour1 = Colour (0x705f009a), fillColour2 = Colour (0x3f220e7a);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.0010f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.3576f)) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (proportionOfWidth (1.2686f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.8279f)) - 0.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour1 = Colour (0x1e48358c), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.8924f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.9827f)) - 0.0f + y,
                                       fillColour2,
                                       80.0f - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.0000f)) - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setFillType(this->fillType);
    g.fillRect(this->getLocalBounds());

    //[/UserPaint]
}

void PanelBackgroundC::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    this->bgCache = theme.getBgCacheC();
    this->fillType = FillType(this->bgCache, {});
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

static void drawPanel(Graphics& g, HelioTheme &theme)
{
    g.setColour(theme.findColour(ColourIDs::BackgroundC::fill));
    g.fillAll();
    HelioTheme::drawNoise(theme, g);
}

void PanelBackgroundC::updateRender(HelioTheme &theme)
{
    if (theme.getBgCacheC().isValid())
    {
        return;
    }

    //const Desktop::Displays::Display &d = Desktop::getInstance().getDisplays().getMainDisplay();
    const int w = 128; // d.totalArea.getWidth() * int(d.scale);
    const int h = 128; // d.totalArea.getHeight() * int(d.scale);

    Image render(Image::ARGB, w, h, true);
    Graphics g(render);
    drawPanel(g, theme);
    theme.getBgCacheC() = render;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelBackgroundC" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill=" radial: 100.522% 6.973%, -10.867% 85.608%, 0=ff28478b, 1=ff1e2a51"
          hasStroke="0"/>
    <RECT pos="0 0 0M 0M" fill=" radial: 0.104% 35.757%, 126.855% 82.789%, 0=705f009a, 1=3f220e7a"
          hasStroke="0"/>
    <RECT pos="0 0 0M 0M" fill="linear: 89.24% 98.268%, 80 0%, 0=1e48358c, 1=0"
          hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
