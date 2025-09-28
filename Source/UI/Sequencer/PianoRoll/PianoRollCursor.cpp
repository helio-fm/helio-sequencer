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
#include "PianoRollCursor.h"
#include "PianoRoll.h"
#include "ProjectNode.h"
#include "NoteComponent.h"
#include "SequencerOperations.h"

PianoRollCursor::PianoRollCursor(PianoRoll &roll) :
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

PianoRollCursor::~PianoRollCursor()
{
    this->roll.getSelectionComponent()->removeChangeListener(this);
    this->stopTimer();
}

inline const float PianoRollCursor::getBeat() const noexcept
{
    return this->roll.getTransport().getSeekBeat();
}

void PianoRollCursor::scrollViewportToKeyIfNeeded()
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

void PianoRollCursor::scrollViewportToBeatIfNeeded()
{
    this->roll.scrollToPlayheadPositionIfNeeded();
}

void PianoRollCursor::doBeforeMoveChecks()
{
    if (!this->isVisible() && this->key == 0)
    {
        this->key = this->roll.getAbsKeyByYPosition(this->getRollViewArea().getCentreY());
        this->setVisible(true);
    }

    if (this->isLassoMode())
    {
        this->roll.getSelectionComponent()->endLasso();
    }

    this->showAndRestartBlinking();
}

void PianoRollCursor::doBeforeSelectChecks()
{
    if (!this->isVisible() && this->key == 0)
    {
        this->key = this->roll.getAbsKeyByYPosition(this->getRollViewArea().getCentreY());
        this->setVisible(true);
    }

    if (!this->isLassoMode())
    {
        this->selectionKeyDelta = 0;
        this->selectionBeatDelta = 0.f;
        const auto selectionArea = this->getSelectionArea();
        this->roll.getSelectionComponent()->beginLasso(selectionArea.getTopLeft(), &this->roll);
    }

    this->showAndRestartBlinking();
}

Rectangle<float> PianoRollCursor::getSelectionArea() const
{
    const auto selectionCursorKey = this->key + this->selectionKeyDelta;
    const auto keyRange = Range<int>(jmin(this->key, selectionCursorKey), jmax(this->key, selectionCursorKey));

    const auto beat = this->getBeat();
    const auto selectionCursorBeat = beat + this->selectionBeatDelta;
    const auto beatRange = Range<float>(jmin(beat, selectionCursorBeat), jmax(beat, selectionCursorBeat));

    const auto minBeat = this->roll.getMinVisibleBeatForCurrentZoomLevel();
    const auto rangeFrom = this->roll.getEventBounds(keyRange.getStart(), beatRange.getStart(), minBeat);
    const auto rangeTo = this->roll.getEventBounds(keyRange.getEnd(), beatRange.getEnd(), minBeat);

    return rangeFrom.getUnion(rangeTo).
        constrainedWithin(this->roll.getLocalBounds().toFloat());
}

bool PianoRollCursor::isLassoMode() const
{
    return this->roll.getSelectionComponent()->isDragging();
}

void PianoRollCursor::dragLasso()
{
    // drags selection in a way that it's always anchored at cursor position
    const auto selectionArea = this->getSelectionArea();
    this->roll.getSelectionComponent()->dragLasso(this->selectionKeyDelta <= 0 ?
            (this->selectionBeatDelta >= 0.f ? selectionArea.getBottomRight() : selectionArea.getBottomLeft()) :
            (this->selectionBeatDelta >= 0.f ? selectionArea.getTopRight() : selectionArea.getTopLeft()));
}

Rectangle<int> PianoRollCursor::getRollViewArea() const
{
    return this->roll.getViewport().
        getViewArea().withTrimmedTop(Globals::UI::rollHeaderHeight);
}

void PianoRollCursor::moveLeft()
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

void PianoRollCursor::moveRight()
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

void PianoRollCursor::moveUp()
{
    this->doBeforeMoveChecks();

    this->key = jmin(this->roll.getNumKeys() - 1, this->key + 1);

    this->updateBounds();
    this->scrollViewportToKeyIfNeeded();
}

void PianoRollCursor::moveDown()
{
    this->doBeforeMoveChecks();

    this->key = jmax(0, this->key - 1);

    this->updateBounds();
    this->scrollViewportToKeyIfNeeded();
}

void PianoRollCursor::selectLeft()
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

void PianoRollCursor::selectRight()
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

void PianoRollCursor::selectUp()
{
    this->doBeforeSelectChecks();

    this->selectionKeyDelta =
        jmin(this->roll.getNumKeys() - this->key - 1,
            this->selectionKeyDelta + 1);

    this->dragLasso();
    this->updateBounds();
}

void PianoRollCursor::selectDown()
{
    const bool hadSelection = this->isLassoMode();
    this->doBeforeSelectChecks();

    if (hadSelection)
    {
        this->selectionKeyDelta =
            jmax(-this->key, this->selectionKeyDelta - 1);
    }

    this->dragLasso();
    this->updateBounds();
}

void PianoRollCursor::interact(RollEditMode editMode)
{
    if (!this->isVisible())
    {
        return;
    }

    if (editMode.isMode(RollEditMode::defaultMode))
    {
        if (auto *noteComponent = this->getTargetingNoteComponent())
        {
            if (!noteComponent->isActive())
            {
                this->roll.getProject().setEditableScope(noteComponent->getClip(), false);
            }
            else if (noteComponent->isActiveAndEditable()) // i.e. not a generated note
            {
                this->roll.selectEvent(noteComponent, true);
            }
        }
        else
        {
            this->roll.deselectAll();
        }
    }
    else if (editMode.isMode(RollEditMode::drawMode)) // todo how to do erasing mode?
    {
        this->roll.addNewNote(this->key - this->roll.getActiveClip().getKey(),
            this->getBeat() - this->roll.getActiveClip().getBeat());

        // new note should be available at this point:
        if (auto *noteComponent = this->getTargetingNoteComponent())
        {
            jassert(noteComponent->isActiveAndEditable());
            if (noteComponent->isActiveAndEditable())
            {
                this->roll.selectEvent(noteComponent, true);
            }
        }
    }
    else if (editMode.isMode(RollEditMode::knifeMode)) // todo how to do merging mode?
    {
        if (auto *noteComponent = this->getTargetingNoteComponent())
        {
            if (!noteComponent->isActive())
            {
                this->roll.getProject().setEditableScope(noteComponent->getClip(), false);
            }

            if (noteComponent->isActiveAndEditable())
            {
                const auto relativeBeat = this->getBeat() - noteComponent->getFullBeat();
                const Array<Note> cutEventsToTheRight =
                    SequencerOperations::cutNotes({ noteComponent->getNote() }, { relativeBeat });
                this->roll.selectEvents(cutEventsToTheRight, true);
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

void PianoRollCursor::paint(Graphics &g)
{
    if (this->isLassoMode())
    {
        const auto b = this->getLocalBounds().reduced(3).toFloat();
        const auto markersAxis = this->selectionKeyDelta <= 0 ?
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

void PianoRollCursor::parentSizeChanged()
{
    this->updateBounds();
}

void PianoRollCursor::updateBounds()
{
    if (this->isLassoMode())
    {
        this->setBounds(this->roll.getSelectionComponent()->getBounds());
        return;
    }

    const auto noteBounds =
        this->roll.getEventBounds(this->key, this->getBeat(), 1.f);

    constexpr int cursorWidth = 4;
    const Rectangle<int> cursorBounds(int(noteBounds.getX()) + 2,
        int(noteBounds.getY()) - 1, cursorWidth, int(noteBounds.getHeight()) + 2);

    this->setBounds(cursorBounds);
}

Point<int> PianoRollCursor::getTargetPoint() const
{
    return { this->getX() + 1, this->getBounds().getCentreY() };
}

NoteComponent *PianoRollCursor::getTargetingNoteComponent() const
{
    if (auto *noteComponent = this->roll.findNoteComponentAt(this->getTargetPoint()))
    {
        if (noteComponent->isEnabled())
        {
            return noteComponent;
        }
    }

    // note not found or not supposed to be interacted with
    return nullptr;
}
