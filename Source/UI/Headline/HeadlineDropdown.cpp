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

#include "HeadlineDropdown.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "PanelBackgroundB.h"
#include "HeadlineDropdown.h"
#include "CommandPanel.h"
#include "ColourIDs.h"
#include "MainLayout.h"
#include "App.h"
//[/MiscUserDefs]

HeadlineDropdown::HeadlineDropdown(WeakReference<TreeItem> targetItem)
    : item(targetItem)
{
    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("Project")));
    titleLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);

    addAndMakeVisible (icon = new IconComponent (Icons::workspace));

    addAndMakeVisible (content = new Component());

    internalPath1.startNewSubPath (0.0f, 0.0f);
    internalPath1.lineTo (40.0f, 0.0f);
    internalPath1.lineTo (40.0f, 32.0f);
    internalPath1.lineTo (0.0f, 32.0f);
    internalPath1.lineTo (8.0f, 16.0f);
    internalPath1.closeSubPath();


    //[UserPreSize]
    this->titleLabel->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (150, 34);

    //[Constructor]
    if (this->item != nullptr)
    {
        this->icon->setIconImage(this->item->getIcon());
        this->titleLabel->setText(this->item->getName(), dontSendNotification);
        const int textWidth = this->titleLabel->getFont()
            .getStringWidth(this->titleLabel->getText());
        this->setSize(textWidth + 64, this->getHeight());

        // Create tree panel for the root,
        // and generic menu for the rest:
        if (this->item->getParentItem() == nullptr)
        {
            ScopedPointer<TreeView> treeView(new TreeView());
            treeView->setFocusContainer(false);
            treeView->setWantsKeyboardFocus(false);
            treeView->setRootItem(this->item);
            treeView->getRootItem()->setOpen(true);
            treeView->setRootItemVisible(true);
            treeView->setDefaultOpenness(true);
            treeView->getViewport()->setWantsKeyboardFocus(false);
            treeView->getViewport()->setScrollBarsShown(false, false);
            treeView->setOpenCloseButtonsVisible(false);
            treeView->setIndentSize(4);
            const auto treeContentBounds =
                treeView->getViewport()->getViewedComponent()->getLocalBounds();
            const auto w = treeContentBounds.getWidth();
            const auto h = jmin(treeContentBounds.getHeight(),
                App::Layout().getHeight() - 180);
            treeView->setSize(w, h);
            this->content = treeView.release();
            this->addAndMakeVisible(this->content);
            this->syncWidthWithContent();
        }
        else if (ScopedPointer<Component> menu = this->item->createItemMenu())
        {
            this->content = menu.release();
            this->addAndMakeVisible(this->content);
            this->syncWidthWithContent();
        }
        else
        {
            // TODO dismiss immediately?
        }
    }

    this->startTimer(200);
    //[/Constructor]
}

HeadlineDropdown::~HeadlineDropdown()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    titleLabel = nullptr;
    icon = nullptr;
    content = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineDropdown::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour1 = Colour (0x33000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       16.0f - 0.0f + y,
                                       fillColour2,
                                       16.0f - 0.0f + x,
                                       14.0f - 0.0f + y,
                                       true));
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x15ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = this->findColour(ColourIDs::BackgroundB::fill).brighter(0.035f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x66000000), strokeColour2 = Colour (0x11000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 2) - 0.0f + x,
                                       32.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 16) - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath3, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x27ffffff), strokeColour2 = Colour (0x0bffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 3) - 0.0f + x,
                                       32.0f - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 17) - 0.0f + x,
                                       5.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath4, PathStrokeType (0.500f), AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..

    // Draw a nice border around the menu:
    g.setColour(Colours::black.withAlpha(40.f / 255.f));
    g.drawHorizontalLine(this->getHeight() - 1, 1.f, float(this->getWidth() - 2));
    g.drawVerticalLine(0, 33.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 2, 33.f, float(this->getHeight() - 1));

    g.setColour(Colours::white.withAlpha(9.f / 255.f));
    g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
    g.drawVerticalLine(1, 33.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 3, 33.f, float(this->getHeight() - 1));

    //[/UserPaint]
}

void HeadlineDropdown::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds (34, 0, getWidth() - 44, 31);
    icon->setBounds (8, 16 - (32 / 2), 32, 32);
    content->setBounds (2, 33, getWidth() - 4, getHeight() - 34);
    internalPath2.clear();
    internalPath2.startNewSubPath (0.0f, 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 2), 32.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 2), static_cast<float> (getHeight() - 1));
    internalPath2.lineTo (1.0f, static_cast<float> (getHeight() - 1));
    internalPath2.lineTo (1.0f, 32.0f);
    internalPath2.lineTo (8.0f, 16.0f);
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (static_cast<float> (getWidth() - -40), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 9), 16.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 2), 32.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - -40), 32.0f);
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath (static_cast<float> (getWidth() - -24), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 17), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 10), 16.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 3), 32.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - -24), 32.0f);
    internalPath4.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HeadlineDropdown::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    //[/UserCode_mouseExit]
}

void HeadlineDropdown::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->item != nullptr) {
        this->item->setSelected(true, true);
    }
    //[/UserCode_mouseDown]
}

void HeadlineDropdown::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    //[/UserCode_mouseUp]
}

void HeadlineDropdown::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->exitModalState(0);
    delete this;
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

template<typename T>
T *findParent(Component *target)
{
    Component *c = target;

    while (c != nullptr)
    {
        if (T *cast = dynamic_cast<T *>(c))
        {
            return cast;
        }

        c = c->getParentComponent();
    }

    return nullptr;
}

void HeadlineDropdown::childBoundsChanged(Component *child)
{
    this->syncWidthWithContent();
}

void HeadlineDropdown::timerCallback()
{
    Component *componentUnderMouse =
        Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();

    HeadlineDropdown *root =
        findParent<HeadlineDropdown>(componentUnderMouse);

    if (componentUnderMouse != nullptr && root != this)
    {
        this->stopTimer();
        this->exitModalState(0);
        Desktop::getInstance().getAnimator()
            .animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 1.f);
        delete this;
    }
}

void HeadlineDropdown::syncWidthWithContent()
{
    if (this->getWidth() != this->content->getWidth() + 4 ||
        this->getHeight() != this->content->getHeight() + 34)
    {
        const int w = jmax(this->getWidth(), this->content->getWidth() + 4);
        this->setSize(w, this->content->getHeight() + 34);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineDropdown" template="../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="WeakReference&lt;TreeItem&gt; targetItem"
                 variableInitialisers="item(targetItem)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="150"
                 initialHeight="34">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill=" radial: 0 16, 16 14, 0=33000000, 1=0"
          hasStroke="0" nonZeroWinding="1">s 0 0 l 40 0 l 40 32 l 0 32 l 8 16 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 15ffffff" hasStroke="0" nonZeroWinding="1">s 0 0 l 16R 0 l 2R 32 l 2R 1R l 1 1R l 1 32 l 8 16 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 2R 32, 16R 2, 0=66000000, 1=11000000"
          nonZeroWinding="1">s -40R 0 l 16R 0 l 9R 16 l 2R 32 l -40R 32 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 3R 32, 17R 5, 0=27ffffff, 1=bffffff" nonZeroWinding="1">s -24R 0 l 17R 0 l 10R 16 l 3R 32 l -24R 32 x</PATH>
  </BACKGROUND>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="34 0 44M 31" labelText="Project"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="8 16c 32 32" class="IconComponent"
                    params="Icons::workspace"/>
  <GENERICCOMPONENT name="" id="b986fd50e3b5b1c5" memberName="content" virtualName=""
                    explicitFocusOrder="0" pos="2 33 4M 34M" class="Component" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
