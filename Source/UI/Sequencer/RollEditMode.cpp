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
#include "RollEditMode.h"
#include "Icons.h"

void RollEditMode::addListener(Listener *listener)
{
    this->listeners.add(listener);
}

void RollEditMode::removeListener(Listener *listener)
{
    this->listeners.remove(listener);
}

bool RollEditMode::forbidsViewportDragging(const ModifierKeys &mods) const
{
#if PLATFORM_DESKTOP
    return
        this->isMode(RollEditMode::selectionMode);
#elif PLATFORM_MOBILE
    return
        this->isMode(RollEditMode::selectionMode) ||
        this->isMode(RollEditMode::drawMode) ||
        this->isMode(RollEditMode::eraseMode) ||
        this->isMode(RollEditMode::knifeMode) ||
        this->isMode(RollEditMode::mergeMode);
#endif
}

bool RollEditMode::forcesViewportDragging(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::dragMode);
}

bool RollEditMode::forbidsSelectionMode(const ModifierKeys &mods) const
{
    return
        this->isMode(RollEditMode::dragMode) ||
        this->isMode(RollEditMode::knifeMode) ||
        this->isMode(RollEditMode::mergeMode) ||
        this->isMode(RollEditMode::eraseMode) ||
        (this->isMode(RollEditMode::drawMode) && !mods.isCtrlDown());
}

bool RollEditMode::forcesSelectionMode(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::selectionMode);
}

bool RollEditMode::forbidsAddingEvents(const ModifierKeys &mods) const
{
    return
        !this->isMode(RollEditMode::defaultMode) &&
        !this->isMode(RollEditMode::drawMode);
}

bool RollEditMode::forcesAddingEvents(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::drawMode) && !mods.isCtrlDown();
}

bool RollEditMode::forbidsCuttingEvents(const ModifierKeys &mods) const
{
    return !this->isMode(RollEditMode::knifeMode) || mods.isRightButtonDown();
}

bool RollEditMode::forcesCuttingEvents(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::knifeMode);
}

bool RollEditMode::forbidsMergingEvents(const ModifierKeys &mods) const
{
    return !this->isMode(RollEditMode::mergeMode);
}

bool RollEditMode::forcesMergingEvents(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::mergeMode);
}

bool RollEditMode::forbidsErasingEvents(const ModifierKeys &mods) const
{
    return !this->isMode(RollEditMode::eraseMode);
}

bool RollEditMode::forcesErasingEvents(const ModifierKeys &mods) const
{
    return this->isMode(RollEditMode::eraseMode);
}

bool RollEditMode::shouldInteractWithChildren() const
{
    switch (this->mode)
    {
        case defaultMode:
        case drawMode:
            return true;
            break;

        case selectionMode:
        case dragMode:
        case eraseMode:
        case knifeMode:
        case mergeMode:
            return false;
            break;
    }

    return true;
}

MouseCursor RollEditMode::getCursor() const
{
    switch (this->mode)
    {
        case defaultMode:
            return MouseCursor::NormalCursor;
            break;

        case dragMode:
            return MouseCursor::DraggingHandCursor;
            break;

        case selectionMode:
        case knifeMode:
        case mergeMode:
            return MouseCursor::CrosshairCursor;
            break;

        case drawMode:
            return Icons::getCopyingCursor();
            break;

        case eraseMode:
            return Icons::getErasingCursor();
            break;
    }

    return MouseCursor::NormalCursor;
}

bool RollEditMode::isMode(Mode targetMode) const
{
    return this->mode == targetMode;
}

void RollEditMode::setMode(Mode newMode)
{
    if (this->mode == newMode)
    {
        return;
    }

    this->mode = newMode;
    this->modeToRecoverTo.reset();

    this->listeners.call(&Listener::onChangeEditMode, *this);
}

void RollEditMode::setTemporaryMode(Mode newMode)
{
    if (this->mode == newMode)
    {
        return;
    }

    this->modeToRecoverTo = this->mode;
    this->mode = newMode;

    this->listeners.call(&Listener::onChangeEditMode, *this);
}

void RollEditMode::recoverFromTemporaryModeIfNeeded()
{
    if (!this->modeToRecoverTo.hasValue())
    {
        return;
    }

    this->mode = *this->modeToRecoverTo;
    this->modeToRecoverTo.reset();

    this->listeners.call(&Listener::onChangeEditMode, *this);
}
