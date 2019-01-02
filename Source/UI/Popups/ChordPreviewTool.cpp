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

#include "ChordPreviewTool.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ChordBuilderTool::ChordBuilderTool()
{
    this->newNote.reset(new PopupCustomButton(createLabel("+")));
    this->addAndMakeVisible(newNote.get());
    this->chordsList.reset(new ChordsCommandPanel(this->defaultChords));
    this->addAndMakeVisible(chordsList.get());


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(500, 500);

    //[Constructor]
    //[/Constructor]
}

ChordBuilderTool::~ChordBuilderTool()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    newNote = nullptr;
    chordsList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ChordBuilderTool::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    {
        float x = static_cast<float> ((getWidth() / 2) - (180 / 2)), y = static_cast<float> ((getHeight() / 2) + -28 - (237 / 2)), width = 180.0f, height = 237.0f;
        Colour fillColour = Colour (0x77000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChordBuilderTool::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newNote->setBounds(proportionOfWidth (0.5000f) - (proportionOfWidth (0.1280f) / 2), proportionOfHeight (0.1100f) - (proportionOfHeight (0.1280f) / 2), proportionOfWidth (0.1280f), proportionOfHeight (0.1280f));
    chordsList->setBounds((getWidth() / 2) - (172 / 2), (getHeight() / 2) + -26 - (224 / 2), 172, 224);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChordBuilderTool" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="500" initialHeight="500">
  <BACKGROUND backgroundColour="ff323e44">
    <ROUNDRECT pos="0Cc -28.5Cc 180 237" cornerSize="2.00000000000000000000"
               fill="solid: 77000000" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="6b3cbe21e2061b28" memberName="newNote" virtualName=""
             explicitFocusOrder="0" pos="50%c 11%c 12.8% 12.8%" sourceFile="PopupCustomButton.cpp"
             constructorParams="createLabel(&quot;+&quot;)"/>
  <GENERICCOMPONENT name="" id="5186723628bce1d6" memberName="chordsList" virtualName=""
                    explicitFocusOrder="0" pos="0Cc -26Cc 172 224" class="ChordsCommandPanel"
                    params="this-&gt;defaultChords"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
