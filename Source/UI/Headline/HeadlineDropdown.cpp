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
#include "Headline.h"
#include "IconComponent.h"
#include "PanelBackgroundB.h"
#include "HeadlineItemDataSource.h"
#include "MenuPanel.h"
#include "RootNode.h"
#include "MainLayout.h"
#include "ColourIDs.h"
#include "TreeNode.h"

static constexpr int getPadding() { return 4; }

//[/MiscUserDefs]

HeadlineDropdown::HeadlineDropdown(WeakReference<HeadlineItemDataSource> targetItem)
    : item(targetItem)
{
    this->content.reset(new Component());
    this->addAndMakeVisible(content.get());

    this->header.reset(new HeadlineItemHighlighter(targetItem));
    this->addAndMakeVisible(header.get());

    //[UserPreSize]
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    //[/UserPreSize]

    this->setSize(150, 34);

    //[Constructor]
    if (this->item != nullptr)
    {
        if (ScopedPointer<Component> menu = this->item->createMenu())
        {
            this->content.reset(menu.release());
            this->addAndMakeVisible(this->content.get());
            this->syncWidthWithContent();
        }
    }

    this->startTimer(175);
    //[/Constructor]
}

HeadlineDropdown::~HeadlineDropdown()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    content = nullptr;
    header = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineDropdown::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill).brighter(0.035f));
    g.fillRect(1, HEADLINE_HEIGHT - 3, this->getWidth() - 3, this->getHeight() - HEADLINE_HEIGHT + 3);

    // Draw a nice border around the menu:
    g.setColour(Colours::black.withAlpha(40.f / 255.f));
    g.drawHorizontalLine(this->getHeight() - 1, 1.f, float(this->getWidth() - 2));
    g.drawVerticalLine(0, HEADLINE_HEIGHT - 1.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 2, HEADLINE_HEIGHT - 1.f, float(this->getHeight() - 1));

    g.setColour(Colours::white.withAlpha(9.f / 255.f));
    g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
    g.drawVerticalLine(1, HEADLINE_HEIGHT - 2.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 3, HEADLINE_HEIGHT - 2.f, float(this->getHeight() - 1));

    //[/UserPaint]
}

void HeadlineDropdown::resized()
{
    //[UserPreResize] Add your own custom resize code here..
#if 0
    //[/UserPreResize]

    content->setBounds(2, getHeight() - 1 - (getHeight() - 34), getWidth() - 4, getHeight() - 34);
    header->setBounds(0, 0, getWidth() - 0, 32);
    //[UserResized] Add your own custom resize handling here..
#endif

    this->content->setBounds(2, HEADLINE_HEIGHT - 1, this->getWidth() - getPadding(), this->getHeight() - HEADLINE_HEIGHT);
    this->header->setBounds(0, 0, this->getWidth() - 0, HEADLINE_HEIGHT);

    //[/UserResized]
}

void HeadlineDropdown::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if (this->item != nullptr) {
        this->item->onSelectedAsMenuItem();
    }
    //[/UserCode_mouseDown]
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
            .animateComponent(this, this->getBounds(), 0.f, 100, true, 0.f, 1.f);
        delete this;
    }
}

void HeadlineDropdown::syncWidthWithContent()
{
    if (this->getWidth() != this->content->getWidth() + getPadding() ||
        this->header->getWidth() != this->content->getWidth() + getPadding() ||
        this->getHeight() != this->content->getHeight() + HEADLINE_HEIGHT)
    {
        const int w = jmax(this->header->getWidth(), this->content->getWidth() + getPadding());
        this->setSize(w, this->content->getHeight() + HEADLINE_HEIGHT);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineDropdown" template="../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="WeakReference&lt;HeadlineItemDataSource&gt; targetItem"
                 variableInitialisers="item(targetItem)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="150"
                 initialHeight="34">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="inputAttemptWhenModal()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="b986fd50e3b5b1c5" memberName="content" virtualName=""
                    explicitFocusOrder="0" pos="2 1Rr 4M 34M" class="Component" params=""/>
  <JUCERCOMP name="" id="3d892173c3bdab59" memberName="header" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 32" sourceFile="HeadlineItemHighlighter.cpp"
             constructorParams="targetItem"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
