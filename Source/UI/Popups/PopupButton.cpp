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

#include "PopupButton.h"

//[MiscUserDefs]
#include "Icons.h"
#define RADUIS_START 0.f
#define RADUIS_END 1.f
#define RADUIS_STEP 0.075f

class PopupButtonHighlighter : public Component
{
public:

    explicit PopupButtonHighlighter(PopupButton &parent) : button(parent)
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const float r = this->button.getRadiusDelta();
        const float d = 1.5f;

        g.setColour(Colours::white.withAlpha(0.2f));
        g.fillEllipse(d + r,
                      d + r,
                      float(getWidth()) - (d * 2.f) - (r * 2.f),
                      float(getHeight()) - (d * 2.f) - (r * 2.f));
    }

private:

    PopupButton &button;
};

class PopupButtonConfirmation : public Component
{
public:

    explicit PopupButtonConfirmation(PopupButton &parent) : button(parent)
    {
        this->setAlwaysOnTop(true);
        this->setInterceptsMouseClicks(false, false);
        this->clickConfirmImage = Icons::getPathByName(Icons::apply);
    }

    void paint(Graphics &g) override
    {
        const float r = this->button.getRadiusDelta();
        const float d = 1.5f;

        g.setColour(Colours::white.withAlpha(0.2f));
        g.fillEllipse(d + r,
                      d + r,
                      float(getWidth()) - (d * 2.f) - (r * 2.f),
                      float(getHeight()) - (d * 2.f) - (r * 2.f));

        AffineTransform pathScaleTransform =
        this->clickConfirmImage.getTransformToScaleToFit(this->getLocalBounds().toFloat().reduced(12), true);

        g.setColour(Colours::white.withAlpha(0.3f));
        g.fillPath(this->clickConfirmImage, pathScaleTransform);

        g.setColour(Colours::black.withAlpha(0.65f));
        g.strokePath(this->clickConfirmImage,
                     PathStrokeType(1.5f),
                     pathScaleTransform);
    }

private:

    PopupButton &button;
    Path clickConfirmImage;
};

//[/MiscUserDefs]

PopupButton::PopupButton(bool shouldShowConfirmImage)
    : alpha(0.5f),
      firstClickDone(false),
      raduisDelta(RADUIS_START),
      showConfirmImage(shouldShowConfirmImage)
{
    addAndMakeVisible (mouseOverHighlighter = new PopupButtonHighlighter (*this));

    addAndMakeVisible (mouseDownHighlighter = new PopupButtonHighlighter (*this));

    addAndMakeVisible (confirmationMark = new PopupButtonConfirmation (*this));


    //[UserPreSize]
    this->mouseOverHighlighter->setAlpha(0.f);
    this->mouseDownHighlighter->setAlpha(0.f);
    this->confirmationMark->setAlpha(0.f);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (48, 48);

    //[Constructor]
    this->startTimerHz(60);
    //[/Constructor]
}

PopupButton::~PopupButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    mouseOverHighlighter = nullptr;
    mouseDownHighlighter = nullptr;
    confirmationMark = nullptr;

    //[Destructor]
    //[/Destructor]
}

void PopupButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..


#if 0
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x6effffff),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colours::white,
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (-55),
                                       true));
    g.drawEllipse (3.0f, 3.0f, static_cast<float> (getWidth() - 6), static_cast<float> (getHeight() - 6), 1.500f);

    //[UserPaint] Add your own custom painting code here..
#endif

    const float d = 1.5f;
    const float r = this->getRadiusDelta();
    g.setColour(this->findColour(Label::textColourId));
    g.drawEllipse(d + r,
                  d + r,
                  float(getWidth()) - (d * 2.f) - (r * 2.f),
                  float(getHeight()) - (d * 2.f) - (r * 2.f),
                  d);

    //[/UserPaint]
}

void PopupButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    mouseOverHighlighter->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    mouseDownHighlighter->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    confirmationMark->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

bool PopupButton::hitTest (int x, int y)
{
    //[UserCode_hitTest] -- Add your code here...
    const int cX = (this->getWidth() / 2) - x;
    const int cY = (this->getHeight() / 2) - y;
    const int r = (this->getWidth() + this->getHeight()) / 2;
    return (cX * cX + cY * cY) <= (r * r);
    //[/UserCode_hitTest]
}

void PopupButton::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->fader.fadeIn(this->mouseOverHighlighter, 100);

#if HELIO_DESKTOP

    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        if (! this->firstClickDone)
        {
            owner->onPopupButtonFirstAction(this);
        }

        owner->onPopupsResetState(this);
    }

#endif

    //[/UserCode_mouseEnter]
}

void PopupButton::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->fader.fadeOut(this->mouseOverHighlighter, 100);
    //[/UserCode_mouseExit]
}

void PopupButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->anchor = this->getPosition();
    this->dragger.startDraggingComponent(this, e);

    this->mouseDownHighlighter->repaint();
    this->fader.fadeIn(this->mouseDownHighlighter, 100);

    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        owner->onPopupButtonStartDragging(this);
    }
    //[/UserCode_mouseDown]
}

void PopupButton::mouseDrag (const MouseEvent& e)
{
    //[UserCode_mouseDrag] -- Add your code here...
    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (! owner->onPopupButtonDrag(this))
        {
            this->setTopLeftPosition(this->anchor);
        }
    }
    //[/UserCode_mouseDrag]
}

void PopupButton::mouseUp (const MouseEvent& e)
{
    //[UserCode_mouseUp] -- Add your code here...
    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (! owner->onPopupButtonDrag(this))
        {
            this->setTopLeftPosition(this->anchor);
        }
    }

    if (this->hitTest(e.getPosition().getX(), e.getPosition().getY()))
    {
        if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
        {
            owner->onPopupButtonEndDragging(this);

            if (this->firstClickDone)
            {
                owner->onPopupButtonSecondAction(this);
            }
            else
            {
                owner->onPopupButtonFirstAction(this);
            }

            owner->onPopupsResetState(this);
        }
    }

    this->updateChildren();
    this->fader.fadeOut(this->mouseDownHighlighter, 100);
    //[/UserCode_mouseUp]
}


//[MiscUserCode]

void PopupButton::timerCallback()
{
    this->raduisDelta += RADUIS_STEP;

    if (this->raduisDelta >= RADUIS_END)
    {
        this->raduisDelta = RADUIS_END;
        this->stopTimer();
    }

    this->repaint();
}

void PopupButton::setState(bool clicked)
{
    this->firstClickDone = clicked;

#if HELIO_MOBILE
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (! dynamic_cast<PopupButtonHighlighter *>(this->getChildComponent(i)) &&
            ! dynamic_cast<PopupButtonConfirmation *>(this->getChildComponent(i)))
        {
            if (clicked)
            {
                this->fader.fadeOut(this->getChildComponent(i), 100);
            }
            else
            {
                this->fader.fadeIn(this->getChildComponent(i), 100);
            }
        }
    }

    if (this->showConfirmImage && !this->firstClickDone)
    {
        this->fader.fadeOut(this->confirmationMark, 100);
    }

    if (this->showConfirmImage && this->firstClickDone)
    {
        this->fader.fadeIn(this->confirmationMark, 100);
    }
#endif

    this->updateChildren();
}

float PopupButton::getRadiusDelta() const
{
    return RADUIS_END - this->raduisDelta;
}

Point<int> PopupButton::getDragDelta() const
{
    return this->getPosition() - this->anchor;
}

void PopupButton::updateChildren()
{

}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PopupButton" template="../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="bool shouldShowConfirmImage" variableInitialisers="alpha(0.5f),&#10;firstClickDone(false),&#10;raduisDelta(RADUIS_START)&#10;showConfirmImage(shouldShowConfirmImage)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="48" initialHeight="48">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="hitTest (int x, int y)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseDrag (const MouseEvent&amp; e)"/>
    <METHOD name="mouseUp (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="1f3677">
    <ELLIPSE pos="3 3 6M 6M" fill=" radial: 0C 0R, 0C -55R, 0=ffffff, 1=ffffff"
             hasStroke="1" stroke="1.5, mitered, butt" strokeColour=" radial: 0C 0, 0C -55, 0=6effffff, 1=ffffffff"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="d95c32db5ada1835" memberName="mouseOverHighlighter"
                    virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" class="PopupButtonHighlighter"
                    params="*this"/>
  <GENERICCOMPONENT name="" id="cbb41a4b3770a7e" memberName="mouseDownHighlighter"
                    virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" class="PopupButtonHighlighter"
                    params="*this"/>
  <GENERICCOMPONENT name="" id="fea9118d2ea95030" memberName="confirmationMark" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="PopupButtonConfirmation"
                    params="*this"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
