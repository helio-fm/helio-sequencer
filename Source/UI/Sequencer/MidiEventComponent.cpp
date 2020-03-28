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
#include "MidiEventComponent.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "HybridRoll.h"

MidiEventComponent::MidiEventComponent(HybridRoll &editor, bool isGhost) noexcept :
    roll(editor),
    dragger(),
    clickOffset(0, 0)    
{
    this->flags.isActive = true;
    this->flags.isSelected = false;
    this->flags.isRecordingTarget = false;
    this->flags.isGhost = isGhost;

    this->setWantsKeyboardFocus(false);
}

bool MidiEventComponent::isActive() const noexcept
{
    return this->flags.isActive;
}

void MidiEventComponent::setActive(bool val, bool force)
{
    if (!force && this->flags.isActive == val)
    {
        return;
    }

    this->flags.isActive = val;
    this->updateColours();

    if (this->flags.isActive)
    {
        this->toFront(false);
    }
}

void MidiEventComponent::setGhostMode()
{
    this->flags.isGhost = true;
    this->updateColours();
    this->repaint();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MidiEventComponent::mouseDown(const MouseEvent &e)
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

//===----------------------------------------------------------------------===//
// SelectableComponent
//===----------------------------------------------------------------------===//

void MidiEventComponent::setSelected(bool selected)
{
    if (this->flags.isSelected != selected)
    {
        this->flags.isSelected = selected;
        this->flags.isRecordingTarget &= selected;
        this->updateColours();
        this->roll.triggerBatchRepaintFor(this);
    }
}

bool MidiEventComponent::isSelected() const noexcept
{
    return this->flags.isSelected;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int MidiEventComponent::compareElements(MidiEventComponent *first, MidiEventComponent *second) noexcept
{
    if (first == second) { return 0; }
    const float diff = first->getBeat() - second->getBeat();
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    return (diffResult != 0) ? diffResult : (first->getId() - second->getId());
}
