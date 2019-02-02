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
#include "Headline.h"
#include "IconComponent.h"
#include "PanelBackgroundB.h"
#include "HeadlineDropdown.h"
#include "CachedLabelImage.h"
#include "ColourIDs.h"
#include "MainLayout.h"
//[/MiscUserDefs]

HeadlineItem::HeadlineItem(WeakReference<HeadlineItemDataSource> treeItem, AsyncUpdater &parent)
    : item(treeItem),
      parentHeadline(parent)
{
    this->titleLabel.reset(new Label(String(),
                                      TRANS("Project")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centredLeft);
    titleLabel->setEditable(false, false, false);

    this->icon.reset(new IconComponent(Icons::helio));
    this->addAndMakeVisible(icon.get());

    this->component.reset(new HeadlineItemArrow());
    this->addAndMakeVisible(component.get());

    //[UserPreSize]
    this->titleLabel->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(false);

    this->titleLabel->setBufferedToImage(true);
    this->titleLabel->setCachedComponentImage(new CachedLabelImage(*this->titleLabel));

    this->bgColour = findDefaultColour(ColourIDs::BackgroundA::fill);
    //[/UserPreSize]

    this->setSize(256, 32);

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
    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineItem::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(this->bgColour);
    g.fillRect(0, 0, getWidth() - 11, getHeight());
    //[/UserPaint]
}

void HeadlineItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds(33, (getHeight() / 2) + -1 - (30 / 2), 256, 30);
    icon->setBounds(7, (getHeight() / 2) + -1 - (32 / 2), 32, 32);
    component->setBounds(getWidth() - 16, 0, 16, getHeight() - 0);
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
        if (this->item->canBeSelectedAsMenuItem())
        {
            this->item->onSelectedAsMenuItem();
        }
        else
        {
            this->showMenuIfAny();
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

WeakReference<HeadlineItemDataSource> HeadlineItem::getDataSource() const noexcept
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
        const int maxTextWidth = this->titleLabel->getWidth();
        this->setSize(jmin(textWidth, maxTextWidth) + 64, HEADLINE_HEIGHT - 2);
    }
}

void HeadlineItem::changeListenerCallback(ChangeBroadcaster *source)
{
    this->parentHeadline.triggerAsyncUpdate();
}

void HeadlineItem::timerCallback()
{
    this->stopTimer();
    this->showMenuIfAny();
}

void HeadlineItem::showMenuIfAny()
{
    if (this->item != nullptr && this->item->hasMenu())
    {
        HeadlineDropdown *hd = new HeadlineDropdown(this->item);
        hd->setTopLeftPosition(this->getPosition());
        hd->setAlpha(0.f);
        App::Layout().showModalComponentUnowned(hd);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItem" template="../../Template"
                 componentName="" parentClasses="public Component, private Timer, private ChangeListener"
                 constructorParams="WeakReference&lt;HeadlineItemDataSource&gt; treeItem, AsyncUpdater &amp;parent"
                 variableInitialisers="item(treeItem),&#10;parentHeadline(parent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="256" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="33 -1Cc 256 30" labelText="Project"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="7 -1Cc 32 32" class="IconComponent"
                    params="Icons::helio"/>
  <JUCERCOMP name="" id="6845054f3705e31" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 16 0M" sourceFile="HeadlineItemArrow.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
