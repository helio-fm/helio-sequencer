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

#include "Headline.h"

//[MiscUserDefs]
#include "HeadlineItem.h"
//[/MiscUserDefs]

Headline::Headline()
{
    addAndMakeVisible (bg = new PanelBackgroundB());
    addAndMakeVisible (separator = new SeparatorHorizontal());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 32);

    //[Constructor]
    //[/Constructor]
}

Headline::~Headline()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    bg = nullptr;
    separator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Headline::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Headline::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    separator->setBounds (0, getHeight() - 2, getWidth() - 0, 2);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

void Headline::syncWithTree(WeakReference<TreeItem> root)
{
    // TODO
    // Figure out what components are to disappear
    // And what to be created and placed properly

    //for (int i = 0; i < this->getNumChildComponents(); ++i)
    //{
    //    if (HeadlineItem *item = dynamic_cast<HeadlineItem *>(this->getChildComponent(i)))
    //    {
    //    }
    //}
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Headline" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="e14a947c03465d1b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="e5efefc65cac6ba7" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 0Rr 0M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
