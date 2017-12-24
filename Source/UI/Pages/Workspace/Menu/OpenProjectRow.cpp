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

#include "OpenProjectRow.h"

//[MiscUserDefs]
#include "Icons.h"
#include "IconComponent.h"

class OpenProjectHighlighter : public Component
{
public:

    OpenProjectHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.025f));
        g.fillRoundedRectangle (5.0f, 18.0f, static_cast<float> (getWidth() - 10), static_cast<float> (getHeight() - 21), 2.000f);
    }
};
//[/MiscUserDefs]

OpenProjectRow::OpenProjectRow(Component &parentComponent, ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      parent(parentComponent)
{
    addAndMakeVisible (newProjectImage = new IconComponent (Icons::open));

    addAndMakeVisible (openProjectLabel = new Label (String(),
                                                     TRANS("menu::workspace::project::open")));
    openProjectLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    openProjectLabel->setJustificationType (Justification::centredLeft);
    openProjectLabel->setEditable (false, false, false);

    addAndMakeVisible (shadow = new SeparatorHorizontalFading());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (350, 56);

    //[Constructor]
    //[/Constructor]
}

OpenProjectRow::~OpenProjectRow()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    newProjectImage = nullptr;
    openProjectLabel = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void OpenProjectRow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OpenProjectRow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newProjectImage->setBounds (32 - (24 / 2), (getHeight() / 2) + 8 - (24 / 2), 24, 24);
    openProjectLabel->setBounds (54, 23, 261, 24);
    shadow->setBounds (22, 6, getWidth() - 44, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
Component *OpenProjectRow::createHighlighterComponent()
{
    return new OpenProjectHighlighter();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OpenProjectRow" template="../../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="Component &amp;parentComponent, ListBox &amp;parentListBox"
                 variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;parent(parentComponent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="56">
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="" id="79f90a69d0b95011" memberName="newProjectImage" virtualName=""
                    explicitFocusOrder="0" pos="32c 8Cc 24 24" class="IconComponent"
                    params="Icons::open"/>
  <LABEL name="" id="8ebb161d0a976635" memberName="openProjectLabel" virtualName=""
         explicitFocusOrder="0" pos="54 23 261 24" labelText="menu::workspace::project::open"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="33"/>
  <JUCERCOMP name="" id="ee264fb9c050a680" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="22 6 44M 3" sourceFile="../../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
