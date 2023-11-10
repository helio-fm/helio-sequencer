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
#include "RollChildComponentBase.h"
#include "RollBase.h"

RollChildComponentBase::RollChildComponentBase(RollBase &editor, bool isGhost) noexcept :
    roll(editor)
{
    this->flags.isActive = true;
    this->flags.isSelected = false;
    this->flags.isInstanceOfSelected = false;
    this->flags.isRecordingTarget = false;
    this->flags.isMergeTarget = false;
    this->flags.isGhost = isGhost;

    this->setWantsKeyboardFocus(false);
}

bool RollChildComponentBase::isActive() const noexcept
{
    return this->flags.isActive;
}

void RollChildComponentBase::setActive(bool val, bool force)
{
    if (!force && this->flags.isActive == val)
    {
        return;
    }

    this->flags.isActive = val;
    this->updateColours();
    this->setMouseCursor(MouseCursor::NormalCursor);

    if (this->flags.isActive)
    {
        this->toFront(false);
    }
}

void RollChildComponentBase::setGhostMode()
{
    this->flags.isGhost = true;
    this->setInterceptsMouseClicks(false, false);
    this->setEnabled(false);
    this->updateColours();
    this->repaint();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void RollChildComponentBase::mouseDown(const MouseEvent &e)
{
    jassert(!this->flags.isGhost);

    auto &selection = this->roll.getLassoSelection();

    // not using any modifiers to deselect the component here
    // (unlike it's done in the Lasso, which uses Alt to deselect),
    // because all editing logic operates on the selection and
    // it often uses modifiers for different modes,
    // so deselecting something here is error-prone
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
}

//===----------------------------------------------------------------------===//
// SelectableComponent
//===----------------------------------------------------------------------===//

void RollChildComponentBase::setSelected(bool selected)
{
    if (this->flags.isSelected != selected)
    {
        this->flags.isSelected = selected;
        this->flags.isRecordingTarget &= selected;
        this->updateColours();
        this->roll.triggerBatchRepaintFor(this);
    }
}

bool RollChildComponentBase::isSelected() const noexcept
{
    return this->flags.isSelected;
}
