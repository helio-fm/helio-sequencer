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
#include "Icons.h"

static void drawPanel(Graphics& g, HelioTheme &theme);
//[/MiscUserDefs]

PanelBackgroundC::PanelBackgroundC()
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

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

    g.fillAll (Colour (0xff182135));

    g.setGradientFill (ColourGradient (Colour (0xff28478b),
                                       static_cast<float> (proportionOfWidth (1.0052f)), static_cast<float> (proportionOfHeight (0.0697f)),
                                       Colour (0xff1e2a51),
                                       static_cast<float> (proportionOfWidth (-0.1087f)), static_cast<float> (proportionOfHeight (0.8561f)),
                                       true));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    g.setGradientFill (ColourGradient (Colour (0x705f009a),
                                       static_cast<float> (proportionOfWidth (0.0010f)), static_cast<float> (proportionOfHeight (0.3576f)),
                                       Colour (0x3f220e7a),
                                       static_cast<float> (proportionOfWidth (1.2686f)), static_cast<float> (proportionOfHeight (0.8279f)),
                                       true));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    g.setGradientFill (ColourGradient (Colour (0x1e48358c),
                                       static_cast<float> (proportionOfWidth (0.8924f)), static_cast<float> (proportionOfHeight (0.9827f)),
                                       Colour (0x00000000),
                                       80.0f, static_cast<float> (proportionOfHeight (0.0000f)),
                                       false));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
#endif

    // TODO remove clipping and opaque everywhere possible
    // this->setPaintingIsUnclipped(true);
    // this->setOpaque(true);

    // TrackMapNoteComponent::paint 

    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    if (theme.getBgCache3().isValid())
    {
        g.setTiledImageFill(theme.getBgCache3(), 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());
    }
    else
    {
        drawPanel(g, static_cast<HelioTheme &>(this->getLookAndFeel()));
    }

    //[/UserPaint]
}

void PanelBackgroundC::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

static void drawPanel(Graphics& g, HelioTheme &theme)
{
    g.setColour(theme.findColour(PanelBackgroundC::panelFillId));
    g.fillAll();
    HelioTheme::drawNoise(theme, g);
}

void PanelBackgroundC::updateRender(HelioTheme &theme)
{
    if (theme.getBgCache3().isValid())
    {
        return;
    }

    //const Desktop::Displays::Display &d = Desktop::getInstance().getDisplays().getMainDisplay();
    const int w = 512; // d.totalArea.getWidth() * int(d.scale);
    const int h = 512; // d.totalArea.getHeight() * int(d.scale);
    //Logger::writeToLog("Rendering background with w:" + String(w) + ", h:" + String(h));

    Image render(Image::ARGB, w, h, true);
    Graphics g(render);
    drawPanel(g, theme);
    theme.getBgCache3() = render;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelBackgroundC" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff182135">
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
