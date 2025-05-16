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
#include "AutomationStepEventComponent.h"
#include "AutomationStepEventsConnector.h"
#include "AutomationSequence.h"

AutomationStepEventComponent::AutomationStepEventComponent(AutomationEditorBase &editor,
    const AutomationEvent &event, const Clip &clip) :
    editor(editor),
    event(event),
    clip(clip)
{
    this->setWantsKeyboardFocus(false);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->setAccessible(false);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->updateColour();
}

void AutomationStepEventComponent::updateColour()
{
    this->dotColour = this->editor.getColour(this->event)
        .withMultipliedSaturation(this->isEditable ? 1.f : 0.4f)
        .withMultipliedAlpha(this->isEditable ? 1.f : 0.2f);
    this->lineColour = this->dotColour
        .withMultipliedAlpha(0.75f);
}

void AutomationStepEventComponent::paint(Graphics &g)
{
    constexpr auto marginTop = AutomationStepEventComponent::marginTop;
    constexpr auto marginBottom = AutomationStepEventComponent::marginBottom;
    constexpr auto r = AutomationStepEventComponent::pointRadius;
    constexpr auto d = r * 2.f;

    const float left = this->floatLocalBounds.getX() + r;
    const float right = jmax(left, this->floatLocalBounds.getWidth() - r);
    const float top = r + marginTop;
    const float h = this->floatLocalBounds.getHeight() - d - marginTop - marginBottom;
    const float y = top + (h * this->event.getControllerValue());
    const float previousY = top + (h * (this->prevEventHolder != nullptr ?
        this->prevEventHolder->getEvent().getControllerValue() :
        Globals::Defaults::onOffControllerState));

    g.setColour(this->dotColour);
    g.fillRect(right - r, y - r + 1.f, d, d - 2.f);
    g.fillRect(right - r + 1.f, y - r, d - 2.f, d);

    g.setColour(this->lineColour);
    if (y > previousY)
    {
        // _
        //  |
        const bool compactMode =
            this->prevEventHolder != nullptr && (this->getX() - this->prevEventHolder->getRight()) <= 0;

        const auto offset = compactMode ? d : 0.f;
        g.fillRect(right, previousY + offset, 1.f, y - previousY - d - offset);
        g.drawHorizontalLine(roundToInt(previousY), left, compactMode ? 0.f : right);
    }
    else if (previousY > y)
    {
        // _|
        const bool compactMode =
            (this->prevEventHolder != nullptr && (this->getX() - this->prevEventHolder->getRight()) <= 0) ||
            (this->nextEventHolder != nullptr && (this->nextEventHolder->getX() - this->getRight()) <= 0);

        g.fillRect(right, y + d, 1.f, previousY - y - d - (compactMode ? d : 0.f));
        g.drawHorizontalLine(roundToInt(previousY), left, compactMode ? right - r : right);
    }
    else
    {
        g.drawHorizontalLine(roundToInt(y), left, right - d);
    }

    if (this->isHighlighted)
    {
        g.fillRect(0, this->getHeight() - 6, this->getWidth(), 4);
    }
}

void AutomationStepEventComponent::moved()
{
    this->updateChildrenBounds();
}

bool AutomationStepEventComponent::hitTest(int x, int y)
{
    if (!this->isEditable)
    {
        return false;
    }

    return y > AutomationStepEventComponent::marginTop;
}

void AutomationStepEventComponent::parentHierarchyChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->recreateConnector();
    }
}

void AutomationStepEventComponent::mouseDown(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    jassert(this->isEditable);
    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();
        this->dragger.startDraggingComponent(this, e);
        this->isDragging = true;
        this->anyChangeDone = false;
    }
}

void AutomationStepEventComponent::mouseDrag(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    jassert(this->isEditable);
    if (this->isDragging)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->dragger.dragComponent(this, e, nullptr);

        // aligned by right side:
        const float newBeat = this->editor.getBeatByPosition(
            this->getX() + this->getWidth(), this->clip);

        this->drag(newBeat);
    }
}

void AutomationStepEventComponent::mouseUp(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    if (this->isDragging)
    {
        this->isDragging = false;
    }

    const auto isPenMode = this->editor.hasEditMode(RollEditMode::drawMode);

    if (e.mods.isRightButtonDown() ||
        (e.source.isTouch() && isPenMode && !this->anyChangeDone))
    {
        auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());
        if (sequence->size() > 1) // no empty automation tracks please
        {
            sequence->checkpoint();
            sequence->remove(this->event, true);
        }
    }
}

void AutomationStepEventComponent::mouseEnter(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    jassert(this->isEditable);
    this->isHighlighted = true;
    this->repaint();
}

void AutomationStepEventComponent::mouseExit(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->isHighlighted = false;
    this->repaint();
}

void AutomationStepEventComponent::drag(float targetBeat)
{
    float newRoundBeat = targetBeat;
    auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());

    // ограничим перемещение отрезком между двумя соседними компонентами
    const int myIndex = sequence->indexOfSorted(&this->event);
    const bool hasPreviousEvent = myIndex > 0;
    const bool hasNextEvent = myIndex < (sequence->size() - 1);

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
        this->anyChangeDone = true;
    }
    else
    {
        this->setFloatBounds(this->editor.getEventBounds(this->event, this->clip));
        this->updateChildrenBounds();
    }
}

void AutomationStepEventComponent::dragByDelta(float deltaBeat)
{
    this->drag(this->getEvent().getBeat() + deltaBeat);
}

void AutomationStepEventComponent::recreateConnector()
{
    if (this->nextEventHolder)
    {
        this->connector = make<AutomationStepEventsConnector>(this,
            this->nextEventHolder, this->event.getControllerValue());

        jassert(this->getParentComponent() != nullptr);
        this->getParentComponent()->addAndMakeVisible(this->connector.get());
        this->updateConnector();
    }
}

void AutomationStepEventComponent::updateConnector()
{
    if (this->connector)
    {
        this->connector->setEditable(this->isEditable);
        this->connector->resizeToFit(this->event.getControllerValue());
        this->connector->repaint();
    }
}

//===----------------------------------------------------------------------===//
// EventComponentBase
//===----------------------------------------------------------------------===//

void AutomationStepEventComponent::setNextNeighbour(EventComponentBase *next)
{
    if (next != nullptr)
    {
        next->repaint();
    }

    if (next == this->nextEventHolder)
    {
        this->updateChildrenBounds();
        return;
    }

    this->nextEventHolder = next;

    if (this->nextEventHolder == nullptr)
    {
        this->connector = nullptr;
    }
    else
    {
        this->recreateConnector();
    }
}

void AutomationStepEventComponent::setPreviousNeighbour(EventComponentBase *prev)
{
    if (prev == this->prevEventHolder)
    {
        return;
    }

    this->prevEventHolder = prev;
}

void AutomationStepEventComponent::setEditable(bool shouldBeEditable)
{
    if (this->isEditable == shouldBeEditable)
    {
        return;
    }

    this->isEditable = shouldBeEditable;

    this->updateColour();
    this->updateConnector();

    if (this->isEditable)
    {
        this->toFront(false);
    }
}
