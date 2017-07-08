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
#include "MidiLayer.h"
#include "MidiEvent.h"
#include "HybridRoll.h"
#include "Pattern.h"

ClipComponent::ClipComponent(HybridRoll &editor, Clip clip) :
	HybridRollEventComponent(editor),
    clip(clip)
{
    this->setWantsKeyboardFocus(false);
}

const Clip ClipComponent::getClip() const
{
    return this->clip;
}


//===----------------------------------------------------------------------===//
// HybridRollEventComponent
//===----------------------------------------------------------------------===//

void ClipComponent::setSelected(bool selected)
{
	//this->roll.wantVolumeSliderFor(this, selected);
	HybridRollEventComponent::setSelected(selected);
}

float ClipComponent::getBeat() const
{
	return this->clip.getStartBeat();
}

String ClipComponent::getSelectionGroupId() const
{
	return this->clip.getPattern()->getPatternIdAsString();
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
