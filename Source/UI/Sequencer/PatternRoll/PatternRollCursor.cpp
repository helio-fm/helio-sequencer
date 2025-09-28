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
#include "PatternRollCursor.h"
#include "PatternRoll.h"
#include "ProjectNode.h"
#include "ClipComponent.h"
#include "PatternOperations.h"

PatternRollCursor::PatternRollCursor(PatternRoll &roll) :
    KeyboardCursor(roll.getTransport()),
    roll(roll)
{
    this->setAccessible(false);
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setFocusContainerType(Component::FocusContainerType::none);

    this->roll.getSelectionComponent()->addChangeListener(this);
}

PatternRollCursor::~PatternRollCursor()
{
    this->roll.getSelectionComponent()->removeChangeListener(this);
    this->stopTimer();
}

inline const float PatternRollCursor::getBeat() const noexcept
{
    return this->roll.getTransport().getSeekBeat();
}

void PatternRollCursor::scrollViewportToRowIfNeeded()
{
    constexpr auto margin = 2;
    const auto viewArea = this->getRollViewArea();
    if (this->getY() < viewArea.getY())
    {
        this->roll.startPanning({ 0.f,
            float(this->getY() - viewArea.getY() - margin) });
    }
    else if (this->getBottom() > viewArea.getBottom())
    {
        this->roll.startPanning({ 0.f,
            float(this->getBottom() - viewArea.getBottom() + margin) });
    }
}

void PatternRollCursor::scrollViewportToBeatIfNeeded()
{
    this->roll.scrollToPlayheadPositionIfNeeded();
}

void PatternRollCursor::doBeforeMoveChecks()
{
    if (!this->isVisible() && this->row == 0)
    {
        this->row = this->roll.getRowByYPosition(this->getRollViewArea().getCentreY());
        this->setVisible(true);
    }

    if (this->isLassoMode())
    {
        this->roll.getSelectionComponent()->endLasso();
    }

    this->showAndRestartBlinking();
}

void PatternRollCursor::doBeforeSelectChecks()
{
    if (!this->isVisible() && this->row == 0)
    {
        this->row = this->roll.getRowByYPosition(this->getRollViewArea().getCentreY());
        this->setVisible(true);
    }

    if (!this->isLassoMode())
    {
        this->selectionRowDelta = 0;
        this->selectionBeatDelta = 0.f;
        const auto selectionArea = this->getSelectionArea();
        this->roll.getSelectionComponent()->beginLasso(selectionArea.getTopLeft(), &this->roll);
    }

    this->showAndRestartBlinking();
}

Rectangle<float> PatternRollCursor::getSelectionArea() const
{
    const auto selectionCursorRow = this->row + this->selectionRowDelta;
    const auto rowRange = Range<int>(jmin(this->row, selectionCursorRow), jmax(this->row, selectionCursorRow));

    const auto beat = this->getBeat();
    const auto selectionCursorBeat = beat + this->selectionBeatDelta;
    const auto beatRange = Range<float>(jmin(beat, selectionCursorBeat), jmax(beat, selectionCursorBeat));

    const auto minBeat = this->roll.getMinVisibleBeatForCurrentZoomLevel();
    const auto rangeFrom = this->roll.getEventBounds(rowRange.getStart(), beatRange.getStart(), minBeat);
    const auto rangeTo = this->roll.getEventBounds(rowRange.getEnd(), beatRange.getEnd(), minBeat);

    return rangeFrom.getUnion(rangeTo).
        constrainedWithin(this->roll.getLocalBounds().toFloat());
}

bool PatternRollCursor::isLassoMode() const
{
    return this->roll.getSelectionComponent()->isDragging();
}

void PatternRollCursor::dragLasso()
{
    // drags selection in a way that it's always anchored at cursor position
    const auto selectionArea = this->getSelectionArea();
    const auto draggingCorner = this->selectionRowDelta >= 0 ?
        (this->selectionBeatDelta >= 0.f ? selectionArea.getBottomRight() : selectionArea.getBottomLeft()) :
        (this->selectionBeatDelta >= 0.f ? selectionArea.getTopRight() : selectionArea.getTopLeft());

    this->roll.getSelectionComponent()->dragLasso(draggingCorner);
}

Rectangle<int> PatternRollCursor::getRollViewArea() const
{
    return this->roll.getViewport().
        getViewArea().withTrimmedTop(Globals::UI::rollHeaderHeight);
}

void PatternRollCursor::moveLeft()
{
    this->doBeforeMoveChecks();

    const auto seekBeat = this->getBeat();
    const auto beatStepSize = this->roll.getMinVisibleBeatForCurrentZoomLevel();
    const auto x = this->roll.getXPositionByBeat(seekBeat);
    const auto floorSnapBeat = this->roll.getFloorBeatSnapByXPosition(x);
    const auto targetBeat = (seekBeat != floorSnapBeat) ?
        floorSnapBeat : seekBeat - beatStepSize;
    this->roll.getTransport().seekToBeat(jmax(this->roll.getFirstBeat(), targetBeat));

    this->updateBounds();
    this->scrollViewportToBeatIfNeeded();
}

void PatternRollCursor::moveRight()
{
    this->doBeforeMoveChecks();

    const auto seekBeat = this->getBeat();
    const auto beatStepSize = this->roll.getMinVisibleBeatForCurrentZoomLevel();
    const auto x = this->roll.getXPositionByBeat(seekBeat);
    const auto ceilSnapBeat = this->roll.getCeilBeatSnapByXPosition(x);
    const auto targetBeat = (seekBeat != ceilSnapBeat) ? ceilSnapBeat : seekBeat + beatStepSize;
    this->roll.getTransport().seekToBeat(jmin(this->roll.getLastBeat() - beatStepSize, targetBeat));

    this->updateBounds();
    this->scrollViewportToBeatIfNeeded();
}

void PatternRollCursor::moveUp()
{
    this->doBeforeMoveChecks();

    this->row = jmax(0, this->row - 1);

    this->updateBounds();
    this->scrollViewportToRowIfNeeded();
}

void PatternRollCursor::moveDown()
{
    this->doBeforeMoveChecks();

    this->row = jmin(this->roll.getNumRows(), this->row + 1);

    this->updateBounds();
    this->scrollViewportToRowIfNeeded();
}

void PatternRollCursor::selectLeft()
{
    this->doBeforeSelectChecks();

    const auto deltaBeat =
        this->roll.getMinVisibleBeatForCurrentZoomLevel();

    this->selectionBeatDelta =
        jmax(this->selectionBeatDelta - deltaBeat,
            this->roll.getFirstBeat() - this->getBeat());

    this->dragLasso();
    this->updateBounds();
}

void PatternRollCursor::selectRight()
{
    const bool hadSelection = this->isLassoMode();
    this->doBeforeSelectChecks();

    const auto deltaBeat =
        this->roll.getMinVisibleBeatForCurrentZoomLevel();

    if (hadSelection)
    {
        this->selectionBeatDelta =
            jmin(this->selectionBeatDelta + deltaBeat,
                this->roll.getLastBeat() - this->getBeat() - deltaBeat);
    }

    this->dragLasso();
    this->updateBounds();
}

void PatternRollCursor::selectUp()
{
    this->doBeforeSelectChecks();

    this->selectionRowDelta =
        jmax(-this->row, this->selectionRowDelta - 1);

    this->dragLasso();
    this->updateBounds();
}

void PatternRollCursor::selectDown()
{
    const bool hadSelection = this->isLassoMode();
    this->doBeforeSelectChecks();

    if (hadSelection)
    {
        this->selectionRowDelta =
            jmin(this->roll.getNumRows() - this->row - 1,
                this->selectionRowDelta + 1);
    }

    this->dragLasso();
    this->updateBounds();
}

void PatternRollCursor::interact(RollEditMode editMode)
{
    if (!this->isVisible())
    {
        return;
    }

    if (editMode.isMode(RollEditMode::defaultMode))
    {
        if (auto *clipComponent = this->getTargetingClipComponent())
        {
            if (clipComponent->isSelected())
            {
                this->roll.getProject().setEditableScope(clipComponent->getClip(), true);
            }
            else if (clipComponent->isActiveAndEditable())
            {
                this->roll.selectEvent(clipComponent, true);
            }
        }
        else
        {
            this->roll.deselectAll();
        }
    }
    else if (editMode.isMode(RollEditMode::drawMode)) // todo how to do erasing mode?
    {
        this->roll.addNewClip(this->row, this->getBeat());

        // new clip should be available at this point:
        if (auto *clipComponent = this->getTargetingClipComponent())
        {
            jassert(clipComponent->isActiveAndEditable());
            if (clipComponent->isActiveAndEditable())
            {
                this->roll.selectEvent(clipComponent, true);
            }
        }
    }
    else if (editMode.isMode(RollEditMode::knifeMode)) // todo how to do merging mode?
    {
        if (auto *clipComponent = this->getTargetingClipComponent())
        {
            if (clipComponent->isActiveAndEditable())
            {
                const auto absBeat = this->getBeat();
                PatternOperations::cutClip(this->roll.getProject(),
                    clipComponent->getClip(), absBeat, false);

                if (auto *cutClipComponent = this->getTargetingClipComponent())
                {
                    jassert(cutClipComponent->isActiveAndEditable());
                    if (cutClipComponent->isActiveAndEditable())
                    {
                        this->roll.selectEvent(cutClipComponent, true);
                    }
                }
            }
        }
    }
#if DEBUG
    else
    {
        jassertfalse;
    }
#endif
}

void PatternRollCursor::paint(Graphics &g)
{
    if (this->isLassoMode())
    {
        const auto b = this->getLocalBounds().reduced(3).toFloat();
        const auto markersAxis = this->selectionRowDelta >= 0 ?
            this->selectionBeatDelta >= 0.f : this->selectionBeatDelta < 0.f;
        KeyboardCursor::paintSelectionMarkers(g, b, markersAxis);
        return;
    }

    const auto b = this->getLocalBounds();

    g.setColour(this->shadowColour.withMultipliedAlpha(this->blinkAlpha));
    g.fillRect(b);

    g.setColour(this->fillColour.withMultipliedAlpha(this->blinkAlpha));
    g.fillRect(b.reduced(1, 0));
}

void PatternRollCursor::parentSizeChanged()
{
    this->updateBounds();
}

void PatternRollCursor::updateBounds()
{
    if (this->isLassoMode())
    {
        this->setBounds(this->roll.getSelectionComponent()->getBounds());
        return;
    }

    const auto clipBounds =
        this->roll.getEventBounds(this->row, this->getBeat(), 1.f);

    constexpr int cursorWidth = 4;
    const Rectangle<int> cursorBounds(int(clipBounds.getX()) + 1,
        int(clipBounds.getY()), cursorWidth, int(clipBounds.getHeight()));

    this->setBounds(cursorBounds);
}

Point<int> PatternRollCursor::getTargetPoint() const
{
    return { this->getX() + 1, this->getBounds().getCentreY() };
}

ClipComponent *PatternRollCursor::getTargetingClipComponent() const
{
    if (auto *clipComponent = this->roll.findClipComponentAt(this->getTargetPoint()))
    {
        if (clipComponent->isEnabled())
        {
            return clipComponent;
        }
    }

    // clip not found or not supposed to be interacted with
    return nullptr;
}
