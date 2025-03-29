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
#include "ClipComponent.h"
#include "MidiEvent.h"
#include "PatternRoll.h"
#include "Pattern.h"
#include "PatternOperations.h"
#include "HelioTheme.h"
#include "CommandIDs.h"
#include "ColourIDs.h"
#include "Icons.h"

ClipComponent::ClipComponent(RollBase &editor, const Clip &clip) :
    RollChildComponentBase(editor),
    clip(clip),
    anchor(clip)
{
    jassert(clip.isValid());
    this->updateColours();
    this->toFront(false);
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setFloatBounds(this->getRoll().getEventBounds(this));
}

const Clip &ClipComponent::getClip() const noexcept
{
    jassert(clip.isValid());
    return this->clip;
}

PatternRoll &ClipComponent::getRoll() const noexcept
{
    return static_cast<PatternRoll &>(this->roll);
}

void ClipComponent::updateColours()
{
    jassert(this->clip.isValid());

    this->fillColour = this->flags.isRecordingTarget ?
        Colours::black.interpolatedWith(findDefaultColour(ColourIDs::Roll::headerRecording), 0.5f) :
        findDefaultColour(ColourIDs::Roll::clipFill);

    const auto foregroundColour = findDefaultColour(ColourIDs::Roll::clipForeground);
    this->frameColour = foregroundColour
        .interpolatedWith(this->getClip().getTrackColour(), this->flags.isSelected ? 0.45f : 0.5f)
        .withAlpha(this->flags.isGhost ? 0.2f : (this->flags.isSelected ? 0.85f : 0.75f))
        .darker(this->flags.isInstanceOfSelected ? 0.25f : 0.f);

    this->frameBorderColour = this->frameColour.withAlpha(0.45f);
    this->frameCornerColour = this->frameColour.withAlpha(0.6f);

    this->eventColour = foregroundColour
        .interpolatedWith(this->getClip().getTrackColour(), this->flags.isSelected ? 0.6f : 0.7f)
        .withAlpha(0.2f + 0.5f * this->clip.getVelocity());

    this->eventMutedColour = eventColour.withMultipliedAlpha(0.3f);
}

//===----------------------------------------------------------------------===//
// MidiEventComponent
//===----------------------------------------------------------------------===//

float ClipComponent::getBeat() const noexcept
{
    return this->clip.getBeat();
}

const String &ClipComponent::getSelectionGroupId() const noexcept
{
    return this->clip.getPattern()->getTrackId();
}

Rectangle<float> ClipComponent::getTextArea() const noexcept
{
    return Rectangle<float>(4.f, 4.f,
        jmax(0.f, float(this->getWidth() - 8)),
        jmax(0.f, float(this->getHeight() - 7)));
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

// "if ... static_cast" is here only so the macro can use the if's scope
#define forEachSelectedClip(lasso, child) \
    for (int _i = 0; _i < lasso.getNumSelected(); _i++) \
        if (auto *child = static_cast<ClipComponent *>(lasso.getSelectedItem(_i)))

void ClipComponent::mouseDoubleClick(const MouseEvent &e)
{
    // signal to switch to piano roll and focus on a clip area
    this->getRoll().postCommandMessage(CommandIDs::ZoomEntireClip);
}

void ClipComponent::mouseDown(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (e.mods.isRightButtonDown() &&
        (this->roll.getEditMode().isMode(RollEditMode::defaultMode) ||
         this->roll.getEditMode().isMode(RollEditMode::drawMode)))
    {
        // see the comment above PatternRoll::startErasingEvents for
        // the explanation of how erasing events works and why:
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    RollChildComponentBase::mouseDown(e);

    const auto &selection = this->roll.getLassoSelection();
    if (e.mods.isLeftButtonDown())
    {
        this->dragger.startDraggingComponent(this, e);

        forEachSelectedClip(selection, clipComponent)
        {
            clipComponent->startDragging();
        }
    }
    else if (e.mods.isMiddleButtonDown())
    {
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
        forEachSelectedClip(selection, clipComponent)
        {
            clipComponent->startTuning();
        }
    }
}

void ClipComponent::mouseDrag(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (!this->isActiveAndEditable())
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(RollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(RollEditMode::eraseMode))
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    const auto &selection = this->roll.getLassoSelection();

    if (this->state == State::Dragging)
    {
        float deltaBeat = 0.f;
        const bool eventChanged = this->getDraggingDelta(e, deltaBeat);
        this->setFloatBounds(this->getRoll().getEventBounds(this));

        if (eventChanged)
        {
            const bool firstChangeIsToCome = !this->firstChangeDone;

            this->checkpointIfNeeded();

            // Drag-and-copy logic, same as for notes (see the comment in NoteComponent):
            if (firstChangeIsToCome && e.mods.isShiftDown())
            {
                PatternOperations::duplicateSelection(this->getRoll().getLassoSelection(), false);

                forEachSelectedClip(selection, clipComponent)
                {
                    clipComponent->toFront(false);
                }

                this->getRoll().updateHighlightedInstances();
            }

            forEachSelectedClip(selection, clipComponent)
            {
                clipComponent->getClip().getPattern()->change(clipComponent->getClip(),
                    clipComponent->continueDragging(deltaBeat), true);
            }
        }
    }
    else if (this->state == State::Tuning)
    {
        this->checkpointIfNeeded();

        forEachSelectedClip(selection, clipComponent)
        {
            clipComponent->getClip().getPattern()->change(clipComponent->getClip(),
                clipComponent->continueTuning(e), true);
        }
    }
}

void ClipComponent::mouseUp(const MouseEvent &e)
{
    // no multi-touch check here, need to exit the editing mode (if any) even in multi-touch
    //if (this->roll.isMultiTouchEvent(e)) { return; }

    if (!this->isActiveAndEditable())
    {
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        (this->roll.getEditMode().isMode(RollEditMode::defaultMode) ||
         this->roll.getEditMode().isMode(RollEditMode::eraseMode)))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    Lasso &selection = this->roll.getLassoSelection();

    if (this->state == State::Dragging)
    {
        this->setFloatBounds(this->getRoll().getEventBounds(this));

        forEachSelectedClip(selection, clipComponent)
        {
            clipComponent->endDragging();
        }
    }
    else if (this->state == State::Tuning)
    {
        forEachSelectedClip(selection, clipComponent)
        {
            clipComponent->endTuning();
        }

        this->setMouseCursor(MouseCursor::NormalCursor);
    }
}

void ClipComponent::paint(Graphics &g)
{
    const float w = float(this->getWidth());
    const float h = float(this->getHeight());
    const float v = this->clip.getVelocity();

    g.setColour(this->fillColour);
    g.fillRect(1.f, 0.f, jmax(0.f, w - 2.f), h);

    const auto textBounds = this->getTextArea();

    g.setColour(this->frameColour);

    if (this->flags.isSelected)
    {
        // top and bottom lines
        g.fillRect(1.f, 0.f, jmax(0.f, w - 2.f), 3.f);
        g.fillRect(1, int(h - 1), jmax(0, int(w - 2)), 1);

        // volume multiplier indicator
        const float fs = (w - 4.f) - ((w - 4.f) * v);
        g.fillRect(2.f + (fs / 2.f), 4.f, jmax(0.f, w - fs - 4.f), 2.f);
        HelioTheme::drawText(g,
            this->clip.getPattern()->getTrack()->getTrackName(),
            textBounds, Justification::bottomLeft);
    }
    else if (this->flags.isInstanceOfSelected)
    {
        constexpr float dash = 4.f;
        constexpr float dash2 = dash * 2.f;
        const auto right = w - 2.f;

        float i = 1.f;
        for (; i < right - dash; i += dash2)
        {
            g.fillRect(i + 1.f, 0.f, dash, 1.f);
            g.fillRect(i, 1.f, dash, 1.f);
        }

        g.fillRect(i + 1.f, 0.f, jmax(0.f, right - i - 1.f), 1.f);
        g.fillRect(i, 1.f, jmax(0.f, right - i + 1.f), 1.f);
    }

    if (this->clip.isMuted())
    {
        HelioTheme::drawText(g,
            "M", textBounds, Justification::topRight);
    }

    if (this->clip.isSoloed())
    {
        HelioTheme::drawText(g,
            "S", textBounds, Justification::topRight);
    }

    if (this->flags.isMergeTarget)
    {
        constexpr float dash = 3.f;
        constexpr float dash2 = dash * 2.f;
        for (float i = 1.f; i < this->getHeight() - 2.f; i += dash2)
        {
            g.fillRect(0.f, i, 1.f, dash);
            g.fillRect(float(this->getWidth() - 1), i, 1.f, dash);
        }
    }
    else // left and right lines
    {
        g.setColour(this->frameBorderColour);
        g.fillRect(0, 1, 1, this->getHeight() - 2);
        g.fillRect(this->getWidth() - 1, 1, 1, this->getHeight() - 2);
        g.setColour(this->frameCornerColour);
    }
    
    if (!this->flags.isSelected) // add little corners for the [  ] look
    {
        constexpr auto cornerSize = 3;

        if (!this->flags.isInstanceOfSelected)
        {
            g.fillRect(0, 0, cornerSize, 1);
            g.fillRect(this->getWidth() - cornerSize, 0, cornerSize, 1);
        }

        g.fillRect(0, this->getHeight() - 1, cornerSize, 1);
        g.fillRect(this->getWidth() - cornerSize, this->getHeight() - 1, cornerSize, 1);
    }
}

//===----------------------------------------------------------------------===//
// SelectableComponent
//===----------------------------------------------------------------------===//

void ClipComponent::setSelected(bool selected)
{
    const bool stateIsChanging = selected != this->isSelected();
    RollChildComponentBase::setSelected(selected);
    if (stateIsChanging)
    {
        // this is not super effective (nested loops etc),
        // but way simpler code and there won't be too much clips
        // in the project anyway - tens, maybe hundreds:
        this->getRoll().updateHighlightedInstances();
    }
}

void ClipComponent::setHighlightedAsInstance(bool isHighlighted)
{
    // if already selected, don't highlight it as an instance:
    const bool isInstanceOfSelected =
        !this->flags.isSelected && isHighlighted;

    if (this->flags.isInstanceOfSelected != isInstanceOfSelected)
    {
        this->flags.isInstanceOfSelected = isInstanceOfSelected;
        this->updateColours();
        this->roll.triggerBatchRepaintFor(this);
    }
}

void ClipComponent::setHighlightedAsMergeTarget(bool isHighlighted)
{
    if (this->flags.isMergeTarget != isHighlighted)
    {
        this->flags.isMergeTarget = isHighlighted;
        this->roll.triggerBatchRepaintFor(this);
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int ClipComponent::compareElements(ClipComponent *first, ClipComponent *second)
{
    if (first == second)
    {
        return 0;
    }
    const float diff = first->getBeat() - second->getBeat();
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    return (diffResult != 0) ? diffResult : (first->clip.getId() - second->clip.getId());
}

void ClipComponent::checkpointIfNeeded()
{
    if (!this->firstChangeDone)
    {
        this->clip.getPattern()->checkpoint();
        this->firstChangeDone = true;
    }
}

void ClipComponent::setNoCheckpointNeededForNextAction()
{
    this->firstChangeDone = true;
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void ClipComponent::startDragging()
{
    this->firstChangeDone = false;
    this->state = State::Dragging;
    this->anchor = this->getClip();
}

bool ClipComponent::isDragging() const noexcept
{
    return this->state == State::Dragging;
}

bool ClipComponent::getDraggingDelta(const MouseEvent &e, float &deltaBeat)
{
    this->dragger.dragComponent(this, e);
    const float newBeat =
        this->getRoll().getBeatForClipByXPosition(this->clip,
            this->getX() + this->floatLocalBounds.getX() + 1);
    deltaBeat = (newBeat - this->anchor.getBeat());
    return this->getBeat() != newBeat;
}

Clip ClipComponent::continueDragging(float deltaBeat)
{
    const auto newBeat = this->anchor.getBeat() + deltaBeat;
    return this->getClip().withBeat(newBeat);
}

void ClipComponent::endDragging()
{
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Velocity
//===----------------------------------------------------------------------===//

void ClipComponent::startTuning()
{
    this->firstChangeDone = false;
    this->state = State::Tuning;
    this->anchor = this->getClip();
}

Clip ClipComponent::continueTuningLinear(float delta) const noexcept
{
    const float newVelocity = (this->anchor.getVelocity() - delta);
    return this->getClip().withVelocity(newVelocity);
}

Clip ClipComponent::continueTuning(const MouseEvent &e) const noexcept
{
    return this->continueTuningLinear(e.getDistanceFromDragStartY() / 250.0f);
}

void ClipComponent::endTuning()
{
    this->state = State::None;
    this->roll.triggerBatchRepaintFor(this);
}
