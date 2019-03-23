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
#include "AutomationStepEventComponent.h"
#include "AutomationStepsClipComponent.h"
#include "AutomationStepEventsConnector.h"
#include "AutomationSequence.h"

AutomationStepEventComponent::AutomationStepEventComponent(AutomationStepsClipComponent &parent,
    const AutomationEvent &targetEvent) :
    event(targetEvent),
    editor(parent),
    isDragging(false),
    isHighlighted(false)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->recreateConnector();
}

void AutomationStepEventComponent::paint(Graphics &g)
{
    const bool prevDownState = this->prevEventHolder ?
        this->prevEventHolder->isPedalDownEvent() : DEFAULT_ON_OFF_EVENT_STATE;

    const float threshold = STEP_EVENT_MIN_LENGTH_IN_BEATS * 3.f;

    const bool isCloseToPrevious = this->prevEventHolder ?
        (this->getBeat() - this->prevEventHolder->getBeat()) <= threshold : false;

    const bool isCloseToNext = this->nextEventHolder ?
        (this->nextEventHolder->getBeat() - this->getBeat()) <= threshold : false;

    g.setColour(this->editor.getEventColour());
    const float r = STEP_EVENT_POINT_OFFSET;
    const float d = r * 2.f;
    const float bottom = this->realBounds.getHeight() - r - STEP_EVENT_MARGIN_BOTTOM;
    const float left = this->realBounds.getX() - float(this->getX());
    const float right = jmax(left + 0.5f, this->realBounds.getWidth() - r);
    const float top = r + STEP_EVENT_MARGIN_TOP;

    if (this->event.isPedalDownEvent() && !prevDownState)
    {
        const bool compact = isCloseToPrevious && this->hasCompactMode();
        if (!compact)
        {
            g.drawLine(right + 0.5f, top, right + 0.5f, bottom - d + 1.f);
            g.drawHorizontalLine(int(top), left, right + 0.5f);
#if STEP_EVENT_THICK_LINES
            g.drawLine(right - 0.5f, top + 1.f, right - 0.5f, bottom - d + 1.f);
            g.drawHorizontalLine(int(top) + 1, left - 1.f, right - 0.5f);
#endif
        }
        g.fillEllipse(right - r + 0.5f, bottom - r, d, d);
    }
    else if (this->event.isPedalUpEvent() && prevDownState)
    {
        const bool compact = isCloseToNext && this->hasCompactMode();
        g.drawLine(right, top + d, right, compact ? bottom - d + 1.f : bottom);
        g.drawHorizontalLine(int(bottom), left, compact ? right - d : right + 0.5f);
#if STEP_EVENT_THICK_LINES
        g.drawLine(right + 1.f, top + d, right + 1.f, compact ? bottom - d + 1.f : bottom + 1.f);
        g.drawHorizontalLine(int(bottom) + 1, left - 1.f, compact ? right - d : right + 1.5f);
#endif
        g.fillEllipse(right - r, top - r, d, d);
    }
    else if (this->event.isPedalDownEvent() && prevDownState)
    {
        g.drawHorizontalLine(int(bottom), left, right - d);
#if STEP_EVENT_THICK_LINES
        g.drawHorizontalLine(int(bottom) + 1, left - 1.f, right - d);
#endif
        g.fillEllipse(right - r + 0.5f, bottom - r, d, d);
    }
    else if (this->event.isPedalUpEvent() && !prevDownState)
    {
        g.drawHorizontalLine(int(top), left, right - d);
#if STEP_EVENT_THICK_LINES
        g.drawHorizontalLine(int(top) + 1, left - 1.f, right - d);
#endif
        g.fillEllipse(right - r, top - r, d, d);
    }

    if (this->isHighlighted)
    {
        g.fillRect(0, this->getHeight() - 6, this->getWidth(), 4);
    }
}

void AutomationStepEventComponent::moved()
{
    this->updateConnector();
}

void AutomationStepEventComponent::mouseDown(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();
        this->dragger.startDraggingComponent(this, e);
        this->isDragging = true;
    }
}

void AutomationStepEventComponent::mouseDrag(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown() && this->isDragging)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->dragger.dragComponent(this, e, nullptr);
        float newRoundBeat = this->editor.getBeatByXPosition(this->getX() + this->getWidth());
        this->drag(newRoundBeat);
    }
}

void AutomationStepEventComponent::mouseUp(const MouseEvent &e)
{
    this->setMouseCursor(MouseCursor::NormalCursor);

    if (e.mods.isLeftButtonDown())
    {
        if (this->isDragging)
        {
            this->isDragging = false;
        }

        if (e.getDistanceFromDragStart() < 2)
        {
#if 0
            AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getLayer());

            const int myIndex = autoLayer->indexOfSorted(&this->event);
            const bool hasPreviousEvent = (myIndex > 0);
            const bool hasNextEvent = (myIndex < (autoLayer->size() - 1));
            bool prevEventIsFarEnough = false;
            bool nextEventIsFarEnough = false;
            bool prevControllerValuesWillMatch = false;
            bool nextControllerValuesWillMatch = false;
            bool justInsertedTriggerEvent = false;

            if (hasPreviousEvent)
            {
                AutomationEvent *prevEvent = static_cast<AutomationEvent *>(autoLayer->getUnchecked(myIndex - 1));

                const float beatDelta = this->event.getBeat() - prevEvent->getBeat();
                const float cvDelta = fabs(this->event.getControllerValue() - prevEvent->getControllerValue());

                prevEventIsFarEnough = (beatDelta > 4.f);
                prevControllerValuesWillMatch = ((1.f - cvDelta) < 0.01f);

                //if (prevEventIsFarEnough && prevControllerValuesWillMatch)
                //{
                //    // turned off, it feels wierd
                //    justInsertedTriggerEvent = true;
                //    autoLayer->insert(this->event.withDeltaBeat(-0.5f).copyWithNewId(), true);
                //}
            }

            //const bool canInvert = !hasPreviousEvent || justInsertedTriggerEvent || (hasPreviousEvent && !prevControllerValuesWillMatch);

            // пусть переключать можно будет только крайние события
            const bool canInvert = !hasPreviousEvent || !hasNextEvent;

            if (canInvert)
            {
                autoLayer->change(this->event, this->event.withInvertedControllerValue(), true);
            }
#endif
        }
    }
    else if (e.mods.isRightButtonDown())
    {
        this->editor.removeEventIfPossible(this->event);
    }
}

void AutomationStepEventComponent::mouseEnter(const MouseEvent &e)
{
    this->isHighlighted = true;
    this->repaint();
}

void AutomationStepEventComponent::mouseExit(const MouseEvent &e)
{
    this->isHighlighted = false;
    this->repaint();
}

void AutomationStepEventComponent::drag(float targetBeat)
{
    float newRoundBeat = targetBeat;
    AutomationSequence *autoLayer = static_cast<AutomationSequence *>(this->event.getSequence());

    // ограничим перемещение отрезком между двумя соседними компонентами
    const int myIndex = autoLayer->indexOfSorted(&this->event);
    const bool hasPreviousEvent = (myIndex > 0);
    const bool hasNextEvent = (myIndex < (autoLayer->size() - 1));

    if (hasPreviousEvent)
    {
        newRoundBeat = jmax(autoLayer->getUnchecked(myIndex - 1)->getBeat() + STEP_EVENT_MIN_LENGTH_IN_BEATS, newRoundBeat);
    }

    if (hasNextEvent)
    {
        newRoundBeat = jmin(autoLayer->getUnchecked(myIndex + 1)->getBeat() - STEP_EVENT_MIN_LENGTH_IN_BEATS, newRoundBeat);
    }

    const float newRoundDeltaBeat = (newRoundBeat - this->event.getBeat());


    if (fabs(newRoundDeltaBeat) > 0.01)
    {
        autoLayer->change(this->event, this->event.withBeat(newRoundBeat), true);
    }
    else
    {
        this->editor.updateEventComponent(this);
    }
}

void AutomationStepEventComponent::dragByDelta(float deltaBeat)
{
    this->drag(this->getBeat() + deltaBeat);
}

void AutomationStepEventComponent::recreateConnector()
{
    this->connector = new AutomationStepEventsConnector(this, this->nextEventHolder, this->isPedalDownEvent());
    this->editor.addAndMakeVisible(this->connector);
    this->updateConnector();
}

void AutomationStepEventComponent::updateConnector()
{
    this->connector->resizeToFit(this->isPedalDownEvent());
}

void AutomationStepEventComponent::setNextNeighbour(AutomationStepEventComponent *next)
{
    if (next != nullptr)
    {
        next->repaint();
    }

    if (next == this->nextEventHolder)
    {
        this->updateConnector();
        return;
    }

    this->nextEventHolder = next;
    this->recreateConnector();
}

void AutomationStepEventComponent::setPreviousNeighbour(AutomationStepEventComponent *prev)
{
    if (prev == this->prevEventHolder)
    {
        return;
    }

    // todo logic
    this->prevEventHolder = prev;
}

bool AutomationStepEventComponent::isPedalDownEvent() const noexcept
{
    return this->event.isPedalDownEvent();
}

float AutomationStepEventComponent::getBeat() const noexcept
{
    return this->event.getBeat();
}

void AutomationStepEventComponent::setRealBounds(const Rectangle<float> bounds)
{
    this->realBounds = bounds;
    this->setBounds(bounds.toType<int>());
}

Rectangle<float> AutomationStepEventComponent::getRealBounds() const noexcept
{
    return this->realBounds;
}
