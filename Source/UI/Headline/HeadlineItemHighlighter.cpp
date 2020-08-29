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

#include "HeadlineItemHighlighter.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "ColourIDs.h"
//[/MiscUserDefs]

HeadlineItemHighlighter::HeadlineItemHighlighter(WeakReference<HeadlineItemDataSource> targetItem)
    : item(targetItem)
{
    this->titleLabel.reset(new Label(String(),
                                            String()));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (18.00f, Font::plain));
    titleLabel->setJustificationType(Justification::centredLeft);
    titleLabel->setEditable(false, false, false);

    this->icon.reset(new IconComponent(Icons::helio));
    this->addAndMakeVisible(icon.get());

    this->arrow.reset(new HeadlineItemArrow());
    this->addAndMakeVisible(arrow.get());

    //[UserPreSize]
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->titleLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(150, 34);

    //[Constructor]
    if (this->item != nullptr)
    {
        this->icon->setIconImage(this->item->getIcon());
        this->titleLabel->setText(this->item->getName(), dontSendNotification);
        const int textWidth = this->titleLabel->getFont()
            .getStringWidth(this->titleLabel->getText());
        this->setSize(textWidth + 64, Globals::UI::headlineHeight);
    }
    //[/Constructor]
}

HeadlineItemHighlighter::~HeadlineItemHighlighter()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    titleLabel = nullptr;
    icon = nullptr;
    arrow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineItemHighlighter::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x15ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::BackgroundA::fill).brighter(0.035f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineItemHighlighter::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds(33, (getHeight() / 2) - (30 / 2), getWidth() - 44, 30);
    icon->setBounds(11, (getHeight() / 2) - (26 / 2), 26, 26);
    arrow->setBounds(getWidth() - 16, 0, 16, getHeight() - 0);
    internalPath1.clear();
    internalPath1.startNewSubPath (2.0f, 1.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 16), 1.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 2), static_cast<float> (getHeight() - 2));
    internalPath1.lineTo (1.0f, static_cast<float> (getHeight() - 1));
    internalPath1.lineTo (2.0f, static_cast<float> (getHeight() - 2));
    internalPath1.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItemHighlighter"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="WeakReference&lt;HeadlineItemDataSource&gt; targetItem"
                 variableInitialisers="item(targetItem)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="150"
                 initialHeight="34">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 15ffffff" hasStroke="0" nonZeroWinding="1">s 2 1 l 16R 1 l 2R 2R l 1 1R l 2 2R x</PATH>
  </BACKGROUND>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="33 0Cc 44M 30" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="11 0Cc 26 26" class="IconComponent"
                    params="Icons::helio"/>
  <JUCERCOMP name="" id="6845054f3705e31" memberName="arrow" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 16 0M" sourceFile="HeadlineItemArrow.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



