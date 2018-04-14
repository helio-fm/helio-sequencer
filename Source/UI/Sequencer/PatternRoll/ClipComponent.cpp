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

ClipComponent::ClipComponent(HybridRoll &editor, const Clip &clip) :
    MidiEventComponent(editor),
    clip(clip)
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
        .interpolatedWith(this->getClip().getColour(), 0.5f)
        .withAlpha(this->ghostMode ? 0.2f : 0.95f)
        .brighter(this->selectedState ? 0.5f : 0.f);

    this->headColourLighter = this->headColour.brighter(0.125f);
    this->headColourDarker = this->headColour.darker(0.35f);

    this->fillColour = headColour.withMultipliedAlpha(0.5f);
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
    return this->clip.getStartBeat();
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

void ClipComponent::mouseDown(const MouseEvent &e)
{
    this->clickOffset.setXY(e.x, e.y);

    // shift-alt-logic
    Lasso &selection = this->roll.getLassoSelection();

    if (!selection.isSelected(this))
    {
        if (e.mods.isShiftDown())
        {
            this->roll.selectEvent(this, false);
        }
        else
        {
            this->roll.selectEvent(this, true);
        }
    }
    else if (selection.isSelected(this) && e.mods.isAltDown())
    {
        this->roll.deselectEvent(this);
        return;
    }
}

void ClipComponent::mouseDoubleClick(const MouseEvent &e)
{
    // TODO signal to switch to piano roll and focus on a clip area
    //this->getRoll().
}

void ClipComponent::paint(Graphics& g)
{
    g.setColour(this->fillColour);
    g.fillRect(1.f, 1.f, float(this->getWidth() - 2), float(this->getHeight() - 2));

    g.setColour(this->headColourLighter);
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    g.setColour(this->headColour);
    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
    g.drawHorizontalLine(2, 0.f, float(this->getWidth()));

    g.setColour(this->headColourDarker);
    g.drawVerticalLine(0, 3.f, float(this->getHeight() - 1));
    g.drawVerticalLine(this->getWidth() - 1, 3.f, float(this->getHeight() - 1));
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
