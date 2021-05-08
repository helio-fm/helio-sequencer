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
#include "RollEditMode.h"
#include "Icons.h"

bool RollEditMode::forbidsViewportDragging() const
{
#if PLATFORM_DESKTOP
    return
        this->isMode(RollEditMode::selectionMode) ||
        this->isMode(RollEditMode::zoomMode);
#elif PLATFORM_MOBILE
    return
        this->isMode(RollEditMode::selectionMode) ||
        this->isMode(RollEditMode::zoomMode) ||
        this->isMode(RollEditMode::drawMode) ||
        this->isMode(RollEditMode::knifeMode);
#endif
}

bool RollEditMode::forcesViewportDragging() const
{
    return this->isMode(RollEditMode::dragMode);
}

bool RollEditMode::forbidsViewportZooming() const
{
    return !this->isMode(RollEditMode::zoomMode);
}

bool RollEditMode::forcesViewportZooming() const
{
    return this->isMode(RollEditMode::zoomMode);
}

bool RollEditMode::forbidsSelectionMode() const
{
    return
        this->isMode(RollEditMode::drawMode) ||
        this->isMode(RollEditMode::zoomMode) ||
        this->isMode(RollEditMode::dragMode) ||
        this->isMode(RollEditMode::knifeMode);
}

bool RollEditMode::forcesSelectionMode() const
{
    return this->isMode(RollEditMode::selectionMode);
}

bool RollEditMode::forbidsAddingEvents() const
{
    return
        this->isMode(RollEditMode::selectionMode) ||
        this->isMode(RollEditMode::zoomMode) ||
        this->isMode(RollEditMode::dragMode) ||
        this->isMode(RollEditMode::knifeMode);
}

bool RollEditMode::forcesAddingEvents() const
{
    return this->isMode(RollEditMode::drawMode);
}

bool RollEditMode::forbidsCuttingEvents() const
{
    return !this->isMode(RollEditMode::knifeMode);
}

bool RollEditMode::forcesCuttingEvents() const
{
    return this->isMode(RollEditMode::knifeMode);
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
        case zoomMode:
        case dragMode:
        case knifeMode:
        case eraserMode:
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
            
        case drawMode:
            return MouseCursor::CopyingCursor;
            break;
            
        case selectionMode:
            return MouseCursor::CrosshairCursor;
            break;
            
        case zoomMode:
            return MouseCursor(Icons::findByName(Icons::zoomIn, 24), 12, 12);
            break;
            
        case dragMode:
            return MouseCursor::DraggingHandCursor;
            break;
            
        case knifeMode:
            return MouseCursor::CrosshairCursor;
            break;

        case eraserMode:
            return MouseCursor::CrosshairCursor;
            break;
    }
    
    return MouseCursor::NormalCursor;
}

void RollEditMode::unsetLastMode()
{
    Mode temp = this->mode;
    this->mode = this->previousMode;
    this->previousMode = temp;
    this->sendChangeMessage();
}

void RollEditMode::setMode(Mode newMode, bool force)
{
    if ((this->mode == newMode) && !force)
    {
        return;
    }
    
    this->previousMode = this->mode;
    this->mode = newMode;
    this->sendChangeMessage();
}

bool RollEditMode::isMode(Mode targetMode) const
{
    return (this->mode == targetMode);
}
