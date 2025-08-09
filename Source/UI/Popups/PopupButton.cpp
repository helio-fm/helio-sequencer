/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "PopupButton.h"
#include "Icons.h"

#if PLATFORM_MOBILE
#   define CONFIRMATION_MODE 1
#endif

PopupButton::PopupButton(Shape shapeType, Colour colour) :
    colour(colour),
    shapeType(shapeType)
{
    this->setAccessible(false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
}

void PopupButton::paint(Graphics &g)
{
    static constexpr float outline1 = 1.5f;
    static constexpr float outline2 = 1.f;

    const float a = this->isSelected ? PopupButton::selectedAlpha : this->alpha;
    const auto fillColour = this->colour.withMultipliedAlpha(a);
    const auto outlineColour = Colours::black.withAlpha(a);

    switch (this->shapeType)
    {
    case Shape::Circle:
        g.setColour(outlineColour.contrasting().withMultipliedAlpha(0.1f));
        g.drawEllipse(outline1, outline1,
            float(this->getWidth()) - (outline1 * 2.f),
            float(this->getHeight()) - (outline1 * 2.f),
            outline1);

        g.setColour(outlineColour);
        g.drawEllipse(outline1 + outline2,
            outline1 + outline2,
            float(this->getWidth()) - (outline1 + outline2) * 2.f,
            float(this->getHeight()) - (outline1 + outline2) * 2.f,
            outline2);

        g.setColour(fillColour);
        g.fillEllipse(outline1 + outline2,
            outline1 + outline2,
            float(this->getWidth()) - (outline1 + outline2) * 2.f,
            float(this->getHeight()) - (outline1 + outline2) * 2.f);
        break;
    case Shape::Hex:
        g.setColour(outlineColour);
        g.strokePath(this->shape, PathStrokeType(1.f));

        g.setColour(fillColour);
        g.fillPath(this->shape);

        if (this->isSelected)
        {
            g.setColour(fillColour.darker(0.65f));
            g.strokePath(this->selectionShape, PathStrokeType(1.f));
        }

        break;
    default:
        break;
    }
}

void PopupButton::resized()
{
    if (this->shapeType == Shape::Hex)
    {
        const int w = this->getWidth();
        const int h = this->getHeight();
        const auto q = float((h - 4) / 4);

        this->shape.clear();
        this->shape.startNewSubPath(float(w / 2), 2.f);
        this->shape.lineTo(float(w - 6), float(h / 2) - q);
        this->shape.lineTo(float(w - 6), float(h / 2) + q);
        this->shape.lineTo(float((w / 2)), float(h - 2));
        this->shape.lineTo(6.f, float(h / 2) + q);
        this->shape.lineTo(6.f, float(h / 2) - q);
        this->shape.closeSubPath();

        const auto localBounds = this->getLocalBounds().toFloat();
        this->selectionShape = shape;
        this->selectionShape.applyTransform(this->shape.
            getTransformToScaleToFit(localBounds.reduced(5.f), true));
    }
}

bool PopupButton::hitTest(int x, int y)
{
    if (this->shapeType == Shape::Hex)
    {
        return this->shape.contains(float(x), float(y));
    }
    else if (this->shapeType == Shape::Circle)
    {
        const int cx = (this->getWidth() / 2) - x;
        const int cy = (this->getHeight() / 2) - y;
        const int r = (this->getWidth() + this->getHeight()) / 2;
        return (cx * cx + cy * cy) <= (r * r);
    }

    return Component::hitTest(x, y);
}

void PopupButton::mouseEnter(const MouseEvent &e)
{
#if !CONFIRMATION_MODE

    if (auto *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        if (!this->isSelected)
        {
            owner->onPopupButtonFirstAction(this);
        }

        owner->onPopupsResetState(this);
    }

    this->alpha = PopupButton::highlightedAlpha;
    this->repaint();

#endif
}

void PopupButton::mouseExit(const MouseEvent &e)
{
    this->alpha = PopupButton::defaultAlpha;
    this->repaint();
}

void PopupButton::mouseDown(const MouseEvent &e)
{
    this->anchor = this->getPosition();
    this->dragger.startDraggingComponent(this, e);

    this->alpha = PopupButton::selectedAlpha;
    this->repaint();

    if (auto *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        owner->onPopupButtonStartDragging(this);
    }
}

void PopupButton::mouseDrag(const MouseEvent &e)
{
    if (auto *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (!owner->onPopupButtonDrag(this))
        {
            this->setTopLeftPosition(this->anchor);
        }
    }
}

void PopupButton::mouseUp(const MouseEvent &e)
{
    this->alpha = PopupButton::defaultAlpha;
    this->repaint();

    BailOutChecker checker(this);

    if (auto *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
    {
        this->dragger.dragComponent(this, e, nullptr);

        if (!owner->onPopupButtonDrag(this))
        {
            this->setTopLeftPosition(this->anchor);
        }
    }

    if (checker.shouldBailOut())
    {
        return;
    }

    if (this->hitTest(e.getPosition().getX(), e.getPosition().getY()))
    {
        if (auto *owner = dynamic_cast<PopupButtonOwner *>(this->getParentComponent()))
        {
            owner->onPopupButtonEndDragging(this);

            if (this->isSelected)
            {
                owner->onPopupButtonSecondAction(this);
            }
            else
            {
                owner->onPopupButtonFirstAction(this);
            }

            if (checker.shouldBailOut())
            {
                return;
            }

            owner->onPopupsResetState(this);
        }
    }

    if (checker.shouldBailOut())
    {
        return;
    }
}

void PopupButton::setState(bool clicked)
{
    this->isSelected = clicked;
    this->repaint();
}

void PopupButton::setUserData(const String &data)
{
    this->userData = data;
}

const String &PopupButton::getUserData() const noexcept
{
    return this->userData;
}

Point<int> PopupButton::getDragDelta() const noexcept
{
    return this->getPosition() - this->anchor;
}
