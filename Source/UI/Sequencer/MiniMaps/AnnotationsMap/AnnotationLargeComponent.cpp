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
#include "AnnotationsSequence.h"
#include "AnnotationsProjectMap.h"
#include "AnnotationLargeComponent.h"

AnnotationLargeComponent::AnnotationLargeComponent(AnnotationsProjectMap &parent,
    const AnnotationEvent &targetEvent) :
    AnnotationComponent(parent, targetEvent)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->font = Font(Globals::UI::Fonts::S, Font::plain);
}

AnnotationLargeComponent::~AnnotationLargeComponent() = default;

void AnnotationLargeComponent::paint(Graphics &g)
{
    g.setColour(this->fillColour.withMultipliedAlpha(this->fillAlpha));
    g.fillPath(this->internalPath);

    g.setColour(this->event.getColour().
        interpolatedWith(this->baseColour, 0.5f).withAlpha(this->borderAlpha));

    g.fillRect(1.f, 0.f, float(this->getWidth() - 1), 2.f);
    g.fillRect(1.5f, 2.f, float(this->getWidth() - 2), 1.f);

    if (this->event.getDescription().isNotEmpty())
    {
        g.setColour(this->event.getColour().
            interpolatedWith(this->baseColour, 0.6f).withAlpha(0.95f));

        GlyphArrangement arr;
        arr.addFittedText(this->font,
            this->event.getDescription(),
            4.f + this->boundsOffset.getX(),
            1.f,
            float(this->getWidth()) - AnnotationLargeComponent::borderResizingArea - 4.f,
            float(this->getHeight() - 1),
            Justification::centredLeft,
            1,
            0.85f);

        arr.draw(g);
    }
}

void AnnotationLargeComponent::resized()
{
    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());

    constexpr auto skewWidth = 4;
    this->internalPath.clear();
    this->internalPath.startNewSubPath(1.f, 0.f);
    this->internalPath.lineTo(1.f, h);
    this->internalPath.lineTo(w - skewWidth, h);
    this->internalPath.lineTo(w, 0.f);
    this->internalPath.closeSubPath();
}

void AnnotationLargeComponent::mouseMove(const MouseEvent &e)
{
    if (this->canResize() &&
        e.x >= (this->getWidth() - AnnotationLargeComponent::borderResizingArea))
    {
        this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    }
    else
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);
    }
}

void AnnotationLargeComponent::mouseDown(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        if (this->canResize() &&
            e.x >= (this->getWidth() - AnnotationLargeComponent::borderResizingArea))
        {
            this->state = State::ResizingRight;
            this->hadCheckpoint = false;
        }
        else
        {
            this->state = State::Dragging;
            this->dragger.startDraggingComponent(this, e);
            this->hadCheckpoint = false;
        }
    }
    else
    {
        this->editor.alternateActionFor(this);
    }
}

void AnnotationLargeComponent::mouseDrag(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    if (e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        if (this->state == State::ResizingRight)
        {
            const float newLength = jmax(0.f,
                this->editor.getBeatByXPosition(this->getX() + e.x) - this->event.getBeat());

            const bool lengthHasChanged = (this->event.getLength() != newLength);

            if (lengthHasChanged)
            {
                auto *sequence = static_cast<AnnotationsSequence *>(this->event.getSequence());

                if (!this->hadCheckpoint)
                {
                    sequence->checkpoint();
                    this->hadCheckpoint = true;
                }

                //DBG(newLength);
                sequence->change(this->event, this->event.withLength(newLength), true);
            }
        }
        else if (this->state == State::Dragging)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            this->dragger.dragComponent(this, e, nullptr);
            const float newBeat = this->editor.getBeatByXPosition(this->getX());
            const bool beatHasChanged = (this->event.getBeat() != newBeat);

            if (beatHasChanged)
            {
                auto *sequence = static_cast<AnnotationsSequence *>(this->event.getSequence());

                if (!this->hadCheckpoint)
                {
                    sequence->checkpoint();
                    this->hadCheckpoint = true;

                    // drag-and-copy:
                    if (e.mods.isShiftDown())
                    {
                        sequence->insert(this->event.withNewId(), true);
                    }
                }

                sequence->change(this->event, this->event.withBeat(newBeat), true);
            }
            else
            {
                this->editor.alignAnnotationComponent(this);
            }
        }
    }
}

void AnnotationLargeComponent::mouseUp(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (this->state == State::Dragging ||
            this->state == State::ResizingRight)
        {
            this->state = State::None;
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->editor.onAnnotationMoved(this);
        }

        if (e.getDistanceFromDragStart() < 10 &&
            !this->hadCheckpoint)
        {
            this->editor.onAnnotationTapped(this);
        }
    }
}

void AnnotationLargeComponent::mouseEnter(const MouseEvent &e)
{
    this->fillAlpha = AnnotationLargeComponent::fillFocusedAlpha;
    this->borderAlpha = AnnotationLargeComponent::borderFocusedAlpha;
    this->repaint();
}

void AnnotationLargeComponent::mouseExit(const MouseEvent &e)
{
    this->fillAlpha = AnnotationLargeComponent::fillUnfocusedAlpha;
    this->borderAlpha = AnnotationLargeComponent::borderUnfocusedAlpha;
    this->repaint();
}

void AnnotationLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = {
        bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight()
    };

    this->setBounds(intBounds);
}

void AnnotationLargeComponent::updateContent()
{
    if (this->text != this->event.getDescription())
    {
        this->text = this->event.getDescription();
        this->textWidth = float(this->font.getStringWidth(this->event.getDescription()));
    }

    this->repaint();
}

float AnnotationLargeComponent::getTextWidth() const noexcept
{
    return this->textWidth;
}

bool AnnotationLargeComponent::canResize() const noexcept
{
    return this->getWidth() >= (AnnotationLargeComponent::borderResizingArea * 2);
}
