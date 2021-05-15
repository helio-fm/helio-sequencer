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
    editor(parent)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->recreateConnector();
}

void AutomationStepEventComponent::paint(Graphics &g)
{
    const bool prevDownState = this->prevEventHolder != nullptr ?
        this->prevEventHolder->isPedalDownEvent() :
        Globals::Defaults::onOffControllerState;

    constexpr auto threshold = AutomationStepEventComponent::minLengthInBeats * 3.f;

    const bool isCloseToPrevious = this->prevEventHolder != nullptr &&
        (this->getBeat() - this->prevEventHolder->getBeat()) <= threshold;

    const bool isCloseToNext = this->nextEventHolder != nullptr &&
        (this->nextEventHolder->getBeat() - this->getBeat()) <= threshold;

    constexpr auto r = AutomationStepEventComponent::pointOffset;
    constexpr auto d = r * 2.f;
    constexpr auto top = r + AutomationStepEventComponent::marginTop;
    const float bottom = this->realBounds.getHeight() - r - AutomationStepEventComponent::marginBottom;
    const float left = this->realBounds.getX() - float(this->getX());
    const float right = jmax(left + 0.5f, this->realBounds.getWidth() - r);

    const auto lineColour = this->editor.getLineColour();
    const auto pointColour = this->editor.getEventColour();
    g.setColour(pointColour);

    if (this->event.isPedalDownEvent() && !prevDownState)
    {
        g.fillEllipse(right - r + 0.5f, bottom - r, d, d);

        g.setColour(lineColour);
        const bool compact = isCloseToPrevious && this->hasCompactMode();
        if (!compact)
        {
            g.drawLine(right + 0.5f, top, right + 0.5f, bottom - d + 1.f);
            g.drawHorizontalLine(int(top), left, right + 0.5f);
        }
    }
    else if (this->event.isPedalUpEvent() && prevDownState)
    {
        g.fillEllipse(right - r, top - r, d, d);

        g.setColour(lineColour);
        const bool compact = isCloseToNext && this->hasCompactMode();
        g.drawLine(right, top + d, right, compact ? bottom - d + 1.f : bottom);
        g.drawHorizontalLine(int(bottom), left, compact ? right - d : right + 0.5f);
    }
    else if (this->event.isPedalDownEvent() && prevDownState)
    {
        g.fillEllipse(right - r + 0.5f, bottom - r, d, d);

        g.setColour(lineColour);
        g.drawHorizontalLine(int(bottom), left, right - d);
    }
    else if (this->event.isPedalUpEvent() && !prevDownState)
    {
        g.fillEllipse(right - r, top - r, d, d);

        g.setColour(lineColour);
        g.drawHorizontalLine(int(top), left, right - d);
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
    auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());

    // ограничим перемещение отрезком между двумя соседними компонентами
    const int myIndex = sequence->indexOfSorted(&this->event);
    const bool hasPreviousEvent = (myIndex > 0);
    const bool hasNextEvent = (myIndex < (sequence->size() - 1));

    if (hasPreviousEvent)
    {
        newRoundBeat = jmax(sequence->getUnchecked(myIndex - 1)->getBeat() +
            AutomationStepEventComponent::minLengthInBeats, newRoundBeat);
    }

    if (hasNextEvent)
    {
        newRoundBeat = jmin(sequence->getUnchecked(myIndex + 1)->getBeat() -
            AutomationStepEventComponent::minLengthInBeats, newRoundBeat);
    }

    const float newRoundDeltaBeat = (newRoundBeat - this->event.getBeat());


    if (fabs(newRoundDeltaBeat) > 0.01)
    {
        sequence->change(this->event, this->event.withBeat(newRoundBeat), true);
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
    this->connector = make<AutomationStepEventsConnector>(this,
        this->nextEventHolder, this->isPedalDownEvent());

    this->editor.addAndMakeVisible(this->connector.get());
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
