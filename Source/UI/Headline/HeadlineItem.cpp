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
#include "PanelBackgroundB.h"
#include "HeadlineDropdown.h"
#include "HelioCallout.h"
#include "MainLayout.h"
#include "App.h"

class HeadlineItemHighlighter : public Component
{
public:

    explicit HeadlineItemHighlighter(const Path &targetPath) :
        path(targetPath) {}

    void paint(Graphics &g) override
    {
        Colour fillColour = Colour(0x09ffffff);
        g.setColour(fillColour);
        g.fillPath(this->path);
    }

private:

    Path path;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemHighlighter)
};

//[/MiscUserDefs]

HeadlineItem::HeadlineItem(WeakReference<TreeItem> treeItem, AsyncUpdater &parent)
    : item(treeItem),
      parentHeadline(parent)
{
    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("Project")));
    titleLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (Label::textColourId, Colours::white);
    titleLabel->setColour (TextEditor::textColourId, Colours::black);
    titleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (icon = new IconComponent (Icons::workspace));


    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->titleLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (256, 32);

    //[Constructor]
    if (this->item != nullptr)
    {
        this->item->addChangeListener(this);
    }
    //[/Constructor]
}

HeadlineItem::~HeadlineItem()
{
    //[Destructor_pre]
    if (this->item != nullptr)
    {
        this->item->removeChangeListener(this);
    }
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
        Colour fillColour = Colour (0x0cffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = this->findColour(PanelBackgroundB::panelFillStartId).brighter(0.025f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour1 = Colour (0x22000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..

        // A hack - don't draw a shadow for the first item in chain
        if (this->getPosition().getX() <= 0)
        { fillColour1 = Colours::transparentBlack; }

        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       fillColour2,
                                       16.0f - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       true));
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x77000000), strokeColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 9) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 16) - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath3, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x55ffffff), strokeColour2 = Colour (0x00ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 10) - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 17) - 0.0f + x,
                                       5.0f - 0.0f + y,
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

    titleLabel->setBounds (34, 0, 512, 31);
    icon->setBounds (8, (getHeight() / 2) - (32 / 2), 32, 32);
    internalPath1.clear();
    internalPath1.startNewSubPath (0.0f, 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 9), 16.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 16), 32.0f);
    internalPath1.lineTo (0.0f, 32.0f);
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (0.0f, 0.0f);
    internalPath2.lineTo (40.0f, 0.0f);
    internalPath2.lineTo (40.0f, 32.0f);
    internalPath2.lineTo (0.0f, 32.0f);
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (static_cast<float> (getWidth() - 32), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 9), 16.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 16), 32.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 32), 32.0f);
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath (static_cast<float> (getWidth() - 32), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 17), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 10), 16.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 17), 32.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 32), 32.0f);
    internalPath4.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HeadlineItem::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    // A hacky way to prevent re-opening the menu again after the new page is shown.
    // Showhow comparing current mouse screen position to e.getMouseDownScreenPosition()
    // won't work (maybe a JUCE bug), so take this from getMainMouseSource:
    const auto lastMouseDown =
        Desktop::getInstance().getMainMouseSource().getLastMouseDownPosition().toInt();
    if (lastMouseDown != e.getScreenPosition())
    {
        this->startTimer(200);
    }
    //[/UserCode_mouseEnter]
}

void HeadlineItem::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->stopTimer();
    //[/UserCode_mouseExit]
}

void HeadlineItem::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->item != nullptr)
    {
        this->stopTimer();
        if (this->item->isSelected())
        {
            this->showMenu();
        }
        else
        {
            this->item->setSelected(true, true);
        }
    }
    //[/UserCode_mouseDown]
}

void HeadlineItem::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    this->stopTimer();
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

TreeItem *HeadlineItem::getTreeItem() const noexcept
{
    return this->item;
}

void HeadlineItem::updateContent()
{
    if (this->item != nullptr)
    {
        this->icon->setIconImage(this->item->getIcon());
        this->titleLabel->setText(this->item->getName(), dontSendNotification);
        const int textWidth = this->titleLabel->getFont()
            .getStringWidth(this->titleLabel->getText());
        this->setSize(textWidth + 64, this->getHeight());
    }
}

void HeadlineItem::changeListenerCallback(ChangeBroadcaster* source)
{
    this->parentHeadline.triggerAsyncUpdate();
}

void HeadlineItem::timerCallback()
{
    this->stopTimer();
    this->showMenu();
}

void HeadlineItem::showMenu()
{
    // FIXME If has menu:
    if (this->item != nullptr)
    {
        HeadlineDropdown *hd = new HeadlineDropdown(this->item);
        hd->setTopLeftPosition(this->getPosition());
        hd->setAlpha(0.f);
        App::Layout().showModalNonOwnedDialog(hd);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItem" template="../../Template"
                 componentName="" parentClasses="public Component, private Timer, private ChangeListener"
                 constructorParams="WeakReference&lt;TreeItem&gt; treeItem, AsyncUpdater &amp;parent"
                 variableInitialisers="item(treeItem),&#10;parentHeadline(parent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="256" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: cffffff" hasStroke="0" nonZeroWinding="1">s 0 0 l 16R 0 l 9R 16 l 16R 32 l 0 32 x</PATH>
    <PATH pos="0 0 100 100" fill=" radial: 0 16, 16 16, 0=22000000, 1=0"
          hasStroke="0" nonZeroWinding="1">s 0 0 l 40 0 l 40 32 l 0 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 9R 16, 16R 2, 0=77000000, 1=0" nonZeroWinding="1">s 32R 0 l 16R 0 l 9R 16 l 16R 32 l 32R 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 10R 16, 17R 5, 0=55ffffff, 1=ffffff" nonZeroWinding="1">s 32R 0 l 17R 0 l 10R 16 l 17R 32 l 32R 32 x</PATH>
  </BACKGROUND>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="34 0 512 31" textCol="ffffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="Project" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="18"
         kerning="0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="8 0Cc 32 32" class="IconComponent"
                    params="Icons::workspace"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
