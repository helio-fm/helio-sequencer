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

#include "SeparatorVerticalSkew.h"

//[MiscUserDefs]
#include "HelioTheme.h"
//[/MiscUserDefs]

SeparatorVerticalSkew::SeparatorVerticalSkew()
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(128, 400);

    //[Constructor]
    //[/Constructor]
}

SeparatorVerticalSkew::~SeparatorVerticalSkew()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void SeparatorVerticalSkew::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0xff474c90);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0xff5156a1);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());

    if (theme.getBgCacheA().isValid())
    {
        g.setTiledImageFill(theme.getBgCacheA(), 0, 0, 1.f);
        g.fillPath(this->internalPath2, {});
    }

    if (theme.getBgCacheB().isValid())
    {
        g.setTiledImageFill(theme.getBgCacheB(), 0, 0, 1.f);
        g.fillPath(this->internalPath1, {});
    }

    g.setColour(Colours::black.withAlpha(45.f / 255.f));
    g.fillPath(this->line1, {});

    g.setColour(Colours::white.withAlpha(15.f / 255.f));
    g.fillPath(this->line2, {});

    //[/UserPaint]
}

void SeparatorVerticalSkew::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    const float h = float(this->getHeight());
    const float w = float(this->getWidth());

    this->line1.clear();
    this->line1.addLineSegment({w, 0.f, 1.f, h}, 0.5f);

    this->line2.clear();
    this->line2.addLineSegment({w - 1.f, 0.f, 0.f, h}, 0.75f);
    //[/UserPreResize]

    internalPath1.clear();
    internalPath1.startNewSubPath (static_cast<float> (getWidth()), 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth()), static_cast<float> (getHeight()));
    internalPath1.lineTo (static_cast<float> (-1), static_cast<float> (getHeight()));
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (static_cast<float> (getWidth() - -1), 0.0f);
    internalPath2.lineTo (0.0f, 0.0f);
    internalPath2.lineTo (0.0f, static_cast<float> (getHeight()));
    internalPath2.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SeparatorVerticalSkew" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="128" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: ff474c90" hasStroke="0" nonZeroWinding="1">s 0R 0 l 0R 0R l -1 0R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: ff5156a1" hasStroke="0" nonZeroWinding="1">s -1R 0 l 0 0 l 0 0R x</PATH>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
