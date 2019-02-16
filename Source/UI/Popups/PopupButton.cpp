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

#include "Common.h"
#include "PopupButton.h"
#include "Icons.h"

#define RADUIS_START   (0.f)
#define RADUIS_END     (1.f)
#define RADUIS_STEP    (0.075f)

#if HELIO_MOBILE
#define CONFIRMATION_MODE
#endif


PopupButtonHighlighter::PopupButtonHighlighter(const PopupButton &parent) :
    button(parent)
{
    this->setInterceptsMouseClicks(false, false);
}

void PopupButtonHighlighter::paint(Graphics &g)
{
    const float r = this->button.getRadiusDelta();
    static const float d = 6.f;

    g.setColour(this->button.colour);

    switch (this->button.shapeType)
    {
    case PopupButton::Circle:
        g.fillEllipse(d + r, d + r,
            float(getWidth()) - (d * 2.f) - (r * 2.f),
            float(getHeight()) - (d * 2.f) - (r * 2.f));
    case PopupButton::Hex:
        g.fillPath(this->button.shape);
    default:
        break;
    }
}


PopupButtonConfirmation::PopupButtonConfirmation(const PopupButton &parent) : button(parent)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, false);
    this->clickConfirmImage = Icons::getPathByName(Icons::apply);
}

void PopupButtonConfirmation::paint(Graphics &g)
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

PopupButton::PopupButton(bool shouldShowConfirmImage, ShapeType shapeType, Colour colour) :
    alpha(0.5f),
    firstClickDone(false),
    raduisDelta(RADUIS_START),
    showConfirmImage(shouldShowConfirmImage),
    colour(colour),
    shapeType(shapeType)
{
    this->mouseOverHighlighter.reset(new PopupButtonHighlighter(*this));
    this->addAndMakeVisible(mouseOverHighlighter.get());

    this->mouseDownHighlighter.reset(new PopupButtonHighlighter(*this));
    this->addAndMakeVisible(mouseDownHighlighter.get());

    this->confirmationMark.reset(new PopupButtonConfirmation(*this));
    this->addAndMakeVisible(confirmationMark.get());

    this->mouseOverHighlighter->setAlpha(0.f);
    this->mouseDownHighlighter->setAlpha(0.f);
    this->confirmationMark->setAlpha(0.f);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->setSize(48, 48);

    if (this->shapeType == Circle)
    {
        this->startTimerHz(60);
    }
}

void PopupButton::paint (Graphics& g)
{
    const float r = this->getRadiusDelta();
    static const float outline1 = 4.5f;
    static const float outline2 = 2.f;

    switch (this->shapeType)
    {
    case Circle:
        g.setColour(Colours::white.withAlpha(0.085f));
        g.drawEllipse(outline1 + r, outline1 + r,
            float(this->getWidth()) - (outline1 + r) * 2.f,
            float(this->getHeight()) - (outline1 + r) * 2.f,
            outline1);

        g.setColour(Colours::black.withAlpha(0.9f));
        g.drawEllipse(outline1 + outline2 + r,
            outline1 + outline2 + r,
            float(this->getWidth()) - (outline1 + outline2 + r) * 2.f,
            float(this->getHeight()) - (outline1 + outline2 + r) * 2.f,
            outline2);

        g.setColour(this->colour);
        g.fillEllipse(outline1 + outline2 + r,
            outline1 + outline2 + r,
            float(this->getWidth()) - (outline1 + outline2 + r) * 2.f,
            float(this->getHeight()) - (outline1 + outline2 + r) * 2.f);

    case Hex:
        g.setColour(Colours::black.withAlpha(0.9f));
        g.strokePath(this->shape, PathStrokeType(outline2));

        g.setColour(this->colour);
        g.fillPath(this->shape);

    default:
        break;
    }
}

void PopupButton::resized()
{
    const int w = this->getWidth();
    const int h = this->getHeight();

    this->mouseOverHighlighter->setBounds(this->getLocalBounds());
    this->mouseDownHighlighter->setBounds(this->getLocalBounds());
    this->confirmationMark->setBounds(this->getLocalBounds());

    if (this->shapeType == Hex)
    {
        const auto q = float((h - 4) / 4);
        this->shape.clear();
        this->shape.startNewSubPath(float(w / 2), 2.0f);
        this->shape.lineTo(float(w - 6), float(h / 2) - q);
        this->shape.lineTo(float(w - 6), float(h / 2) + q);
        this->shape.lineTo(float((w / 2)), float(h - 2));
        this->shape.lineTo(6.0f, float(h / 2) + q);
        this->shape.lineTo(6.0f, float(h / 2) - q);
        this->shape.closeSubPath();
    }
}

bool PopupButton::hitTest (int x, int y)
{
    const int cX = (this->getWidth() / 2) - x;
    const int cY = (this->getHeight() / 2) - y;
    const int r = (this->getWidth() + this->getHeight()) / 2;
    return (cX * cX + cY * cY) <= (r * r);
}

void PopupButton::mouseEnter (const MouseEvent& e)
{
    this->fader.fadeIn(this->mouseOverHighlighter.get(), 100);

#if ! CONFIRMATION_MODE

    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        if (! this->firstClickDone)
        {
            owner->onPopupButtonFirstAction(this);
        }

        owner->onPopupsResetState(this);
    }

#endif
}

void PopupButton::mouseExit (const MouseEvent& e)
{
    this->fader.fadeOut(this->mouseOverHighlighter.get(), 100);
}

void PopupButton::mouseDown (const MouseEvent& e)
{
    this->anchor = this->getPosition();
    this->dragger.startDraggingComponent(this, e);

    this->mouseDownHighlighter->repaint();
    this->fader.fadeIn(this->mouseDownHighlighter.get(), 100);

    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        owner->onPopupButtonStartDragging(this);
    }
}

void PopupButton::mouseDrag (const MouseEvent& e)
{
    if (PopupButtonOwner *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (! owner->onPopupButtonDrag(this))
        {
            this->setTopLeftPosition(this->anchor);
        }
    }
}

void PopupButton::mouseUp (const MouseEvent& e)
{
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
    this->fader.fadeOut(this->mouseDownHighlighter.get(), 100);
}

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

#if CONFIRMATION_MODE
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

void PopupButton::setUserData(const String &data)
{
    this->userData = data;
}

const String &PopupButton::getUserData() const noexcept
{
    return this->userData;
}

float PopupButton::getRadiusDelta() const noexcept
{
    return RADUIS_END - this->raduisDelta;
}

Point<int> PopupButton::getDragDelta() const noexcept
{
    return this->getPosition() - this->anchor;
}

void PopupButton::updateChildren() {}
