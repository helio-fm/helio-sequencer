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

#include "CreateProjectRow.h"

//[MiscUserDefs]
#include "Icons.h"
#include "IconComponent.h"

class CreateProjectHighlighter : public Component
{
public:

    CreateProjectHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.025f));
        g.fillRoundedRectangle (5.0f, 4.0f, static_cast<float> (getWidth() - 10), static_cast<float> (getHeight() - 20), 2.000f);
    }
};
//[/MiscUserDefs]

CreateProjectRow::CreateProjectRow(Component &parentComponent, ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      parent(parentComponent)
{
    addAndMakeVisible (newProjectImage = new IconComponent (Icons::create));
    newProjectImage->setName ("newProject");

    addAndMakeVisible (newProjectLabel = new Label (String(),
                                                    TRANS("menu::workspace::project::create")));
    newProjectLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    newProjectLabel->setJustificationType (Justification::centredLeft);
    newProjectLabel->setEditable (false, false, false);

    addAndMakeVisible (shadow = new SeparatorHorizontalFading());

    //[UserPreSize]
    this->shadow->setAlpha(0.75f);
    //[/UserPreSize]

    setSize (350, 56);

    //[Constructor]
    //[/Constructor]
}

CreateProjectRow::~CreateProjectRow()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    newProjectImage = nullptr;
    newProjectLabel = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void CreateProjectRow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    // a hack
#if HELIO_DESKTOP
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void CreateProjectRow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newProjectImage->setBounds (32 - (24 / 2), (getHeight() / 2) + -6 - (24 / 2), 24, 24);
    newProjectLabel->setBounds (54, 9, 261, 24);
    shadow->setBounds (22, getHeight() - 4 - 3, getWidth() - 44, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
Component *CreateProjectRow::createHighlighterComponent()
{
    return new CreateProjectHighlighter();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CreateProjectRow" template="../../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="Component &amp;parentComponent, ListBox &amp;parentListBox"
                 variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;parent(parentComponent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="56">
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="newProject" id="79f90a69d0b95011" memberName="newProjectImage"
                    virtualName="" explicitFocusOrder="0" pos="32c -6Cc 24 24" class="IconComponent"
                    params="Icons::create"/>
  <LABEL name="" id="8ebb161d0a976635" memberName="newProjectLabel" virtualName=""
         explicitFocusOrder="0" pos="54 9 261 24" labelText="menu::workspace::project::create"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="33"/>
  <JUCERCOMP name="" id="ee264fb9c050a680" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="22 4Rr 44M 3" sourceFile="../../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
