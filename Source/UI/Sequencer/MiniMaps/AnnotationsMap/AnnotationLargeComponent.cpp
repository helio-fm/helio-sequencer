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
    const Colour baseColour(findDefaultColour(Label::textColourId));

    g.setColour(this->event.getColour()
        .interpolatedWith(baseColour, 0.5f).withAlpha(0.65f));

    g.fillRect(1.f, 0.f, float(this->getWidth() - 1), 2.f);
    g.fillRect(1.5f, 2.f, float(this->getWidth() - 2), 1.f);

    if (this->event.getDescription().isNotEmpty())
    {
        const Font labelFont(16.f, Font::plain);
        g.setColour(this->event.getColour().interpolatedWith(baseColour, 0.55f).withAlpha(0.9f));

        GlyphArrangement arr;
        arr.addFittedText(labelFont,
            this->event.getDescription(),
            2.f + this->boundsOffset.getX(),
            0.f,
            float(this->getWidth()) - 16.f,
            float(this->getHeight()) - 8.f,
            Justification::centredLeft,
            1,
            0.85f);

        arr.draw(g);
    }
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
    if (this->editor.rollHasMultiTouch(e))
    {
        return;
    }

    this->mouseDownWasTriggered = true;

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
    if (this->editor.rollHasMultiTouch(e))
    {
        return;
    }

    if (e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 3)
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
                const bool firstChangeIsToCome = !this->hadCheckpoint;
                auto *sequence = static_cast<AnnotationsSequence *>(this->event.getSequence());

                if (! this->hadCheckpoint)
                {
                    sequence->checkpoint();
                    this->hadCheckpoint = true;
                }

                // Drag-and-copy logic:
                if (firstChangeIsToCome && e.mods.isAnyModifierKeyDown())
                {
                    sequence->insert(this->event.withNewId(), true);
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
            this->mouseDownWasTriggered &&
            !this->hadCheckpoint)
        {
            this->editor.onAnnotationTapped(this);
        }
    }

    this->mouseDownWasTriggered = false;
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
