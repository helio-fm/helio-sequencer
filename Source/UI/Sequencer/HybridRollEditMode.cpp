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
#include "HybridRollEditMode.h"
#include "Icons.h"

bool HybridRollEditMode::forbidsViewportDragging() const
{
    return
        this->isMode(HybridRollEditMode::drawMode) ||
        this->isMode(HybridRollEditMode::selectionMode) ||
        this->isMode(HybridRollEditMode::zoomMode);
}

bool HybridRollEditMode::forcesViewportDragging() const
{
    return this->isMode(HybridRollEditMode::dragMode);
}

bool HybridRollEditMode::forbidsViewportZooming() const
{
    return !this->isMode(HybridRollEditMode::zoomMode);
}

bool HybridRollEditMode::forcesViewportZooming() const
{
    return this->isMode(HybridRollEditMode::zoomMode);
}

bool HybridRollEditMode::forbidsSelectionMode() const
{
    return
        this->isMode(HybridRollEditMode::drawMode) ||
        this->isMode(HybridRollEditMode::zoomMode) ||
        this->isMode(HybridRollEditMode::dragMode) ||
        this->isMode(HybridRollEditMode::knifeMode);
}

bool HybridRollEditMode::forcesSelectionMode() const
{
    return this->isMode(HybridRollEditMode::selectionMode);
}

bool HybridRollEditMode::forbidsAddingEvents() const
{
    return
        this->isMode(HybridRollEditMode::selectionMode) ||
        this->isMode(HybridRollEditMode::zoomMode) ||
        this->isMode(HybridRollEditMode::dragMode) ||
        this->isMode(HybridRollEditMode::knifeMode);
}

bool HybridRollEditMode::forcesAddingEvents() const
{
    return this->isMode(HybridRollEditMode::drawMode);
}

bool HybridRollEditMode::forbidsCuttingEvents() const
{
    return !this->isMode(HybridRollEditMode::knifeMode);
}

bool HybridRollEditMode::forcesCuttingEvents() const
{
    return this->isMode(HybridRollEditMode::knifeMode);
}

bool HybridRollEditMode::shouldInteractWithChildren() const
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
            return false;
            break;
    }
    
    return true;
}

MouseCursor HybridRollEditMode::getCursor() const
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
    }
    
    return MouseCursor::NormalCursor;
}

void HybridRollEditMode::unsetLastMode()
{
    Mode temp = this->mode;
    this->mode = this->previousMode;
    this->previousMode = temp;
    this->sendChangeMessage();
}

void HybridRollEditMode::setMode(Mode newMode, bool force)
{
    if ((this->mode == newMode) && !force)
    {
        return;
    }
    
    this->previousMode = this->mode;
    this->mode = newMode;
    this->sendChangeMessage();
}

bool HybridRollEditMode::isMode(Mode targetMode) const
{
    return (this->mode == targetMode);
}
