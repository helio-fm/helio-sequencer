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

#include "HeadlineItem.h"

//[MiscUserDefs]
#include "IconComponent.h"

class HeadlineItemShadow : public Component
{
public:

    void paint(Graphics &g) override
    {
        // TODO
    }

private:

    Path internalPath1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemShadow)
};

//[/MiscUserDefs]

HeadlineItem::HeadlineItem(WeakReference<TreeItem> treeItem)
    : item(treeItem)
{
    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("...")));
    titleLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (Label::textColourId, Colours::black);
    titleLabel->setColour (TextEditor::textColourId, Colours::black);
    titleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (icon = new IconComponent (Icons::workspace));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (256, 32);

    //[Constructor]
    //[/Constructor]
}

HeadlineItem::~HeadlineItem()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    titleLabel = nullptr;
    icon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineItem::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour1 = Colour (0x55000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       fillColour2,
                                       16.0f - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       true));
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x20ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colours::black, strokeColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 1) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 9) - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath3, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colours::white, strokeColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 2) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 10) - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath4, PathStrokeType (0.500f), AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds (36, 0, getWidth() - 44, getHeight() - 2);
    icon->setBounds (10, (getHeight() / 2) - (28 / 2), 28, 28);
    internalPath1.clear();
    internalPath1.startNewSubPath (0.0f, 0.0f);
    internalPath1.lineTo (40.0f, 0.0f);
    internalPath1.lineTo (40.0f, 32.0f);
    internalPath1.lineTo (0.0f, 32.0f);
    internalPath1.lineTo (8.0f, 16.0f);
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (0.0f, 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 10), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 2), 16.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 10), 32.0f);
    internalPath2.lineTo (0.0f, 32.0f);
    internalPath2.lineTo (8.0f, 16.0f);
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (224.0f, 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 9), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 1), 16.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 9), 32.0f);
    internalPath3.lineTo (224.0f, 32.0f);
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath (224.0f, 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 10), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 1), 16.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 10), 32.0f);
    internalPath4.lineTo (224.0f, 32.0f);
    internalPath4.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

TreeItem *HeadlineItem::getTreeItem() const noexcept
{
    return this->item;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItem" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="WeakReference&lt;TreeItem&gt; treeItem"
                 variableInitialisers="item(treeItem)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="256"
                 initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill=" radial: 0 16, 16 16, 0=55000000, 1=0"
          hasStroke="0" nonZeroWinding="1">s 0 0 l 40 0 l 40 32 l 0 32 l 8 16 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 20ffffff" hasStroke="0" nonZeroWinding="1">s 0 0 l 10R 0 l 2R 16 l 10R 32 l 0 32 l 8 16 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 1R 16, 9R 0, 0=ff000000, 1=0" nonZeroWinding="1">s 224 0 l 9R 0 l 1R 16 l 9R 32 l 224 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 2R 16, 10R 2, 0=ffffffff, 1=0" nonZeroWinding="1">s 224 0 l 10R 0 l 1R 16 l 10R 32 l 224 32 x</PATH>
  </BACKGROUND>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="36 0 44M 2M" textCol="ff000000" edTextCol="ff000000"
         edBkgCol="0" labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="18"
         kerning="0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="10 0Cc 28 28" class="IconComponent"
                    params="Icons::workspace"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
