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
#include "ProjectTreeItem.h"
#include "MidiLayer.h"
#include "MidiEvent.h"
#include "MidiRoll.h"

MidiEventComponent::MidiEventComponent(MidiRoll &editor, const MidiEvent &event) :
    roll(editor),
    midiEvent(event),
    dragger(),
    selectedState(false),
    activeState(true),
    anchorBeat(0),
    colour(Colours::white),
    clickOffset(0, 0)    
{
    this->setWantsKeyboardFocus(false);
}

bool MidiEventComponent::isActive() const
{
    return this->activeState;
}

void MidiEventComponent::setActive(bool val, bool force)
{
    if (!force && this->activeState == val)
    {
        return;
    }

    this->activeState = val;

    if (this->activeState)
    {
        this->toFront(false);
    }
    else
    {
        this->toBack();
    }
}

void MidiEventComponent::setSelected(const bool selected)
{
    if (this->selectedState != selected)
    {
        this->selectedState = selected;
        this->roll.triggerBatchRepaintFor(this);
    }
}

bool MidiEventComponent::isSelected() const
{
    return this->selectedState;
}

float MidiEventComponent::getBeat() const
{
    return this->midiEvent.getBeat();
}


const MidiEvent &MidiEventComponent::getEvent() const
{
    return this->midiEvent;
}

bool MidiEventComponent::belongsToLayerSet(Array<MidiLayer *> layers) const
{
    for (int i = 0; i < layers.size(); ++i)
    {
        if (this->getEvent().getLayer() == layers.getUnchecked(i))
        {
            return true;
        }
    }
    
    return false;
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MidiEventComponent::mouseDown(const MouseEvent &e)
{
    if (!this->activeState)
    {
        if (!e.mods.isLeftButtonDown())
        {
            this->activateCorrespondingLayer(false, true);
        }

        return;
    }

    this->clickOffset.setXY(e.x, e.y);

    // shift-alt-logic
    MidiEventSelection &selection = this->roll.getLassoSelection();

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

int MidiEventComponent::compareElements(MidiEventComponent *first, MidiEventComponent *second)
{
    if (first == second) { return 0; }
    const float diff = first->getBeat() - second->getBeat();
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    return (diffResult != 0) ? diffResult : (first->midiEvent.getID().compare(second->midiEvent.getID()));
}

void MidiEventComponent::activateCorrespondingLayer(bool selectOthers, bool deselectOthers)
{
    MidiLayer *layer = this->getEvent().getLayer();
	this->roll.getProject().activateLayer(layer, selectOthers, deselectOthers);
}
