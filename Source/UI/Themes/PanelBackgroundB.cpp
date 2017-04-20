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
#include "Icons.h"

static const String panelBgKey = "PanelBackgroundB";
//[/MiscUserDefs]

PanelBackgroundB::PanelBackgroundB()
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->setOpaque(true);
    this->setInterceptsMouseClicks(false, false);
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

    g.setGradientFill (ColourGradient (Colour (0xff48358c),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colour (0xff2d3e61),
                                       0.0f, static_cast<float> (proportionOfHeight (0.6500f)),
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

#if PANEL_B_HAS_PRERENDERED_BACKGROUND

    const CachedImage::Ptr prerendered = static_cast<HelioTheme &>(this->getLookAndFeel()).getPanelsBgCache()[panelBgKey];

    if (prerendered != nullptr)
    {
        Icons::drawImageRetinaAware(*prerendered, g, this->getWidth() / 2, this->getHeight() / 2);
    }

#else

    g.setGradientFill (ColourGradient (findColour(PanelBackgroundB::panelFillStartId),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       findColour(PanelBackgroundB::panelFillEndId),
                                       0.0f, static_cast<float> (proportionOfHeight (0.6500f)),
                                       true));

    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

#if HELIO_DESKTOP
    g.setGradientFill (ColourGradient (findColour(PanelBackgroundB::panelShadeStartId),
                                       static_cast<float> (proportionOfWidth (0.8924f)), static_cast<float> (proportionOfHeight (0.9827f)),
                                       findColour(PanelBackgroundB::panelShadeEndId),
                                       80.0f, static_cast<float> (proportionOfHeight (0.0000f)),
                                       false));

    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    HelioTheme::drawNoise(this, g, 0.5f);
#endif

#endif
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
#if PANEL_B_HAS_PRERENDERED_BACKGROUND

    if (theme.getPanelsBgCache()[panelBgKey] != nullptr)
    {
        return;
    }

    const Desktop::Displays::Display &d = Desktop::getInstance().getDisplays().getMainDisplay();
    const int scale = int(d.scale);
    const int w = d.totalArea.getWidth() * scale;
    const int h = d.totalArea.getHeight() * scale;

    Logger::writeToLog("Prerendering background with w:" + String(w) + ", h:" + String(h));

    CachedImage::Ptr render(new CachedImage(Image::ARGB, w, h, true));
    Graphics g(*render);

    g.setGradientFill (ColourGradient (theme.findColour(PanelBackgroundB::panelFillStartId),
                                       static_cast<float> ((w / 2)), 0.0f,
                                       theme.findColour(PanelBackgroundB::panelFillEndId),
                                       0.0f, static_cast<float> (h * (0.6500f)),
                                       true));

    g.fillAll();

    g.setGradientFill (ColourGradient (theme.findColour(PanelBackgroundB::panelShadeStartId),
                                       static_cast<float> (w * (0.8924f)), static_cast<float> (h * (0.9827f)),
                                       theme.findColour(PanelBackgroundB::panelShadeEndId),
                                       80.0f, static_cast<float> (h * (0.0000f)),
                                       false));

    g.fillAll();

    g.setGradientFill (ColourGradient (theme.findColour(PanelBackgroundB::panelShadeStartId),
                                       static_cast<float> (w * (0.8924f)), static_cast<float> (h * (0.9827f)),
                                       theme.findColour(PanelBackgroundB::panelShadeEndId),
                                       80.0f, static_cast<float> (h * (0.0000f)),
                                       false));

    g.fillAll();

    HelioTheme::drawNoise(theme, g, 0.5f);

    theme.getPanelsBgCache().set(panelBgKey, render);

#endif
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
    <RECT pos="0 0 0M 0M" fill=" radial: 0C 0, 0 65%, 0=ff48358c, 1=ff2d3e61"
          hasStroke="0"/>
    <RECT pos="0 0 0M 0M" fill="linear: 89.24% 98.268%, 80 0%, 0=1e48358c, 1=0"
          hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
