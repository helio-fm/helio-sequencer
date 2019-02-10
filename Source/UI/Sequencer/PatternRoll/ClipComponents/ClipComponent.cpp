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
#include "ClipComponent.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "PatternRoll.h"
#include "Pattern.h"
#include "CommandIDs.h"

static Pattern *getPattern(SelectionProxyArray::Ptr selection)
{
    const auto &firstEvent = selection->getFirstAs<ClipComponent>()->getClip();
    return firstEvent.getPattern();
}

ClipComponent::ClipComponent(HybridRoll &editor, const Clip &clip) :
    MidiEventComponent(editor),
    clip(clip),
    anchor(clip),
    firstChangeDone(false),
    state(None)
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
    this->fillColour = Colours::black.withAlpha(0.2f);
    this->headBrightColour = Colours::white
        .interpolatedWith(this->getClip().getTrackColour(), 0.55f)
        .withAlpha(this->ghostMode ? 0.2f : 0.7f)
        .brighter(this->selectedState ? 0.25f : 0.f);
    this->headDarkColour = this->headBrightColour.withAlpha(0.25f);
    this->eventColour = this->getClip().getTrackColour()
        .interpolatedWith(Colours::white, .35f)
        .withAlpha(.15f + 0.5f * this->clip.getVelocity());
}

//===----------------------------------------------------------------------===//
// MidiEventComponent
//===----------------------------------------------------------------------===//

void ClipComponent::setSelected(bool selected)
{
    MidiEventComponent::setSelected(selected);
}

float ClipComponent::getBeat() const noexcept
{
    return this->clip.getBeat();
}

const String &ClipComponent::getSelectionGroupId() const noexcept
{
    return this->clip.getPattern()->getTrackId();
}

const String &ClipComponent::getId() const noexcept
{
    return this->clip.getId();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

#define forEachSelectedClip(lasso, child) \
    for (int _i = 0; _i < lasso.getNumSelected(); _i++) \
        if (auto *child = dynamic_cast<ClipComponent *>(lasso.getSelectedItem(_i)))

void ClipComponent::mouseDoubleClick(const MouseEvent &e)
{
    // signal to switch to piano roll and focus on a clip area
    this->getRoll().postCommandMessage(CommandIDs::EditClip);
}

void ClipComponent::mouseDown(const MouseEvent &e)
{
    if (!this->isActive())
    {
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(HybridRollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    MidiEventComponent::mouseDown(e);

    const Lasso &selection = this->roll.getLassoSelection();
    if (e.mods.isLeftButtonDown())
    {
        this->dragger.startDraggingComponent(this, e);

        forEachSelectedClip(selection, clip)
        {
            //if (selection.shouldDisplayGhostClips()) { clip->getRoll().showGhostClipFor(clip); }
            clip->startDragging();
        }
    }
    else if (e.mods.isMiddleButtonDown())
    {
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
        forEachSelectedClip(selection, clip)
        {
            clip->startTuning();
        }
    }
}

void ClipComponent::mouseDrag(const MouseEvent &e)
{
    if (!this->isActive())
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(HybridRollEditMode::defaultMode))
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    const auto &selection = this->roll.getLassoSelection();

    if (this->state == Dragging)
    {
        float deltaBeat = 0.f;
        const bool eventChanged = this->getDraggingDelta(e, deltaBeat);
        this->setFloatBounds(this->getRoll().getEventBounds(this));

        if (eventChanged)
        {
            const bool firstChangeIsToCome = !this->firstChangeDone;

            this->checkpointIfNeeded();

            // TODO Drag-and-copy logic:
            //if (firstChangeIsToCome && e.mods.isAnyModifierKeyDown())
            //{
            //    SequencerOperations::duplicateSelection(this->getRoll().getLassoSelection(), false);
            //    this->getRoll().hideAllGhostClips();
            //}

            for (const auto &s : selection.getGroupedSelections())
            {
                const auto patternSelection(s.second);
                Array<Clip> groupDragBefore, groupDragAfter;

                for (int i = 0; i < patternSelection->size(); ++i)
                {
                    auto cc = static_cast<ClipComponent *>(patternSelection->getUnchecked(i));
                    groupDragBefore.add(cc->getClip());
                    groupDragAfter.add(cc->continueDragging(deltaBeat));
                }

                getPattern(patternSelection)->changeGroup(groupDragBefore, groupDragAfter, true);
            }
        }
    }
    else if (this->state == Tuning)
    {
        this->checkpointIfNeeded();

        for (const auto &s : selection.getGroupedSelections())
        {
            const auto trackSelection(s.second);
            Array<Clip> groupBefore, groupAfter;

            for (int i = 0; i < trackSelection->size(); ++i)
            {
                const auto *nc = static_cast<ClipComponent *>(trackSelection->getUnchecked(i));
                groupBefore.add(nc->getClip());
                groupAfter.add(nc->continueTuning(e));
            }

            getPattern(trackSelection)->changeGroup(groupBefore, groupAfter, true);
        }
    }
}

void ClipComponent::mouseUp(const MouseEvent &e)
{
    if (!this->isActive())
    {
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() && this->roll.getEditMode().isMode(HybridRollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    Lasso &selection = this->roll.getLassoSelection();

    if (this->state == Dragging)
    {
        this->setFloatBounds(this->getRoll().getEventBounds(this));

        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto cc = static_cast<ClipComponent *>(selection.getSelectedItem(i));
            cc->endDragging();
        }
    }
    else if (this->state == Tuning)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *cc = static_cast<ClipComponent *>(selection.getSelectedItem(i));
            cc->endTuning();
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
    g.fillRect(1.f, 0.f, w - 2.f, h);

    const Rectangle<int> textBounds(4, 5, this->getWidth() - 8, this->getHeight() - 7);
    g.setColour(this->headBrightColour);
    
    if (this->selectedState)
    {
        g.fillRect(1.f, 1.f, w - 2.f, 3.f);
        g.drawHorizontalLine(this->getHeight() - 1, 1.f, w - 1.f);
        const float wv = (w - 4.f) * v;
        const float fs = ((w - 4.f) - wv) / 2.f;
        g.drawHorizontalLine(5, 2.f + fs, w - fs - 2.f);
        g.drawText(this->clip.getPattern()->getTrack()->getTrackName(),
            textBounds, Justification::bottomLeft, false);
    }

    g.drawText(this->clip.getKeyString(), textBounds, Justification::topLeft, false);

    g.setColour(this->headDarkColour);
    g.drawVerticalLine(0, 2.f, h - 1.f);
    g.drawVerticalLine(this->getWidth() - 1, 2.f, h - 1.f);

    // Set colour to be used by all child events
    g.setColour(this->eventColour);
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int ClipComponent::compareElements(ClipComponent *first, ClipComponent *second)
{
    if (first == second) { return 0; }
    const float diff = first->getBeat() - second->getBeat();
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    return (diffResult != 0) ? diffResult : (first->clip.getId().compare(second->clip.getId()));
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
    this->state = Dragging;
    this->anchor = this->getClip();
}

bool ClipComponent::isDragging() const noexcept
{
    return this->state == Dragging;
}

bool ClipComponent::getDraggingDelta(const MouseEvent &e, float &deltaBeat)
{
    this->dragger.dragComponent(this, e, nullptr);
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
    this->state = None;
}

//===----------------------------------------------------------------------===//
// Velocity
//===----------------------------------------------------------------------===//

void ClipComponent::startTuning()
{
    this->firstChangeDone = false;
    this->state = Tuning;
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
    this->state = None;
    this->roll.triggerBatchRepaintFor(this);
}
