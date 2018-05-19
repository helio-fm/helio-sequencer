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
#include "ProjectTreeItem.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "PatternRoll.h"
#include "Pattern.h"

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
    jassert(clip.isValid());
    this->headColour = Colours::white
        .interpolatedWith(this->getClip().getTrackColour(), 0.55f)
        .withAlpha(this->ghostMode ? 0.2f : 0.95f)
        .brighter(this->selectedState ? 0.25f : 0.f);

    this->headColourLighter = this->headColour.brighter(0.125f);
    this->headColourDarker = this->headColour.darker(0.35f);

    this->fillColour =
        headColour.withMultipliedAlpha(this->selectedState ? 0.7f : 0.6f);
}

//===----------------------------------------------------------------------===//
// MidiEventComponent
//===----------------------------------------------------------------------===//

void ClipComponent::setSelected(bool selected)
{
    //this->roll.wantVolumeSliderFor(this, selected);
    MidiEventComponent::setSelected(selected);
}

float ClipComponent::getBeat() const
{
    return this->clip.getBeat();
}

String ClipComponent::getSelectionGroupId() const
{
    return this->clip.getPattern()->getTrackId();
}

String ClipComponent::getId() const
{
    return this->clip.getId();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ClipComponent::mouseDoubleClick(const MouseEvent &e)
{
    // TODO signal to switch to piano roll and focus on a clip area
    //this->getRoll().
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

        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            if (auto cc = dynamic_cast<ClipComponent *>(selection.getSelectedItem(i)))
            {
                //if (selection.shouldDisplayGhostClips()) { cc->getRoll().showGhostClipFor(cc); }
                cc->startDragging();
            }
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
}

void ClipComponent::paint(Graphics& g)
{
    g.setColour(this->fillColour);
    g.fillRect(1.f, 1.f, float(this->getWidth() - 2), float(this->getHeight() - 2));

    g.setColour(this->headColourLighter);
    g.drawHorizontalLine(0, 1.f, float(this->getWidth() - 1));

    if (this->selectedState)
    {
        g.setColour(this->headColour);
        g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(2, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(3, 0.f, float(this->getWidth()));
    }

    g.setColour(this->headColourDarker);
    g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 1, 1.f, float(this->getHeight() - 1));
    g.drawHorizontalLine(this->getHeight() - 1, 1.f, float(this->getWidth() - 1));
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
