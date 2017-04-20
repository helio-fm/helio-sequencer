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
#include "MidiRollEditMode.h"
#include "Icons.h"

MidiRollEditMode::MidiRollEditMode() :
    mode(defaultMode),
    previousMode(defaultMode)
{
}

bool MidiRollEditMode::forbidsViewportDragging() const
{
    return
    this->isMode(MidiRollEditMode::drawMode) ||
    this->isMode(MidiRollEditMode::selectionMode) ||
    this->isMode(MidiRollEditMode::zoomMode) ||
    this->isMode(MidiRollEditMode::insertSpaceMode) ||
    this->isMode(MidiRollEditMode::wipeSpaceMode);
}

bool MidiRollEditMode::forcesViewportDragging() const
{
    return
    this->isMode(MidiRollEditMode::dragMode);
}

bool MidiRollEditMode::forbidsViewportZooming() const
{
    return
    (this->mode != MidiRollEditMode::zoomMode);
}

bool MidiRollEditMode::forcesViewportZooming() const
{
    return
    this->isMode(MidiRollEditMode::zoomMode);
}

bool MidiRollEditMode::forbidsSelectionMode() const
{
    return
    this->isMode(MidiRollEditMode::drawMode) ||
    this->isMode(MidiRollEditMode::zoomMode) ||
    this->isMode(MidiRollEditMode::dragMode) ||
    this->isMode(MidiRollEditMode::insertSpaceMode) ||
    this->isMode(MidiRollEditMode::wipeSpaceMode);
}

bool MidiRollEditMode::forcesSelectionMode() const
{
    return
    this->isMode(MidiRollEditMode::selectionMode);
}

bool MidiRollEditMode::forbidsAddingEvents() const
{
    return
    this->isMode(MidiRollEditMode::selectionMode) ||
    this->isMode(MidiRollEditMode::zoomMode) ||
    this->isMode(MidiRollEditMode::dragMode) ||
    this->isMode(MidiRollEditMode::insertSpaceMode) ||
    this->isMode(MidiRollEditMode::wipeSpaceMode);
}

bool MidiRollEditMode::forcesAddingEvents() const
{
    return
    this->isMode(MidiRollEditMode::drawMode);
}

bool MidiRollEditMode::forbidsSpaceWipe() const
{
    return
    (this->mode != MidiRollEditMode::wipeSpaceMode);
}

bool MidiRollEditMode::forcesSpaceWipe() const
{
    return
    this->isMode(MidiRollEditMode::wipeSpaceMode);
}

bool MidiRollEditMode::forbidsSpaceInsert() const
{
    return
    (this->mode != MidiRollEditMode::insertSpaceMode);
}

bool MidiRollEditMode::forcesSpaceInsert() const
{
    return
    this->isMode(MidiRollEditMode::insertSpaceMode);
}


bool MidiRollEditMode::shouldInteractWithChildren() const
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
        case insertSpaceMode:
        case wipeSpaceMode:
        case scissorsMode:
            return false;
            break;
    }
    
    return true;
}

MouseCursor MidiRollEditMode::getCursor() const
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
            
        case insertSpaceMode:
            return MouseCursor::LeftRightResizeCursor;
            break;
            
        case wipeSpaceMode:
            return MouseCursor::CrosshairCursor;
            break;
    }
    
    return MouseCursor::NormalCursor;
}

void MidiRollEditMode::unsetLastMode()
{
    //Logger::writeToLog("Unsetting to " + String(this->previousMode));
    Mode temp = this->mode;
    this->mode = this->previousMode;
    this->previousMode = temp;
    this->sendChangeMessage();
}

void MidiRollEditMode::setMode(Mode newMode, bool force)
{
    if ((this->mode == newMode) && !force)
    {
        return;
    }
    
    this->previousMode = this->mode;
    this->mode = newMode;
    this->sendChangeMessage();
}

bool MidiRollEditMode::isMode(Mode targetMode) const
{
    return (this->mode == targetMode);
}
