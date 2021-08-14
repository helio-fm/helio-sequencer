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

#pragma once

// belongs to ProjectTreeItem

class RollEditMode final : public ChangeBroadcaster
{
public:
    
    enum Mode
    {
        defaultMode,
        drawMode,
        eraseMode,
        selectionMode,
        zoomMode,
        dragMode,
        knifeMode
    };
    
    RollEditMode() = default;

    bool forbidsViewportDragging(const ModifierKeys &mods) const;
    bool forcesViewportDragging(const ModifierKeys &mods) const;
    
    bool forbidsViewportZooming(const ModifierKeys &mods) const;
    bool forcesViewportZooming(const ModifierKeys &mods) const;
    
    bool forbidsSelectionMode(const ModifierKeys &mods) const;
    bool forcesSelectionMode(const ModifierKeys &mods) const;
    
    bool forbidsAddingEvents(const ModifierKeys &mods) const;
    bool forcesAddingEvents(const ModifierKeys &mods) const;

    bool forbidsCuttingEvents(const ModifierKeys &mods) const;
    bool forcesCuttingEvents(const ModifierKeys &mods) const;

    bool forbidsErasingEvents(const ModifierKeys &mods) const;
    bool forcesErasingEvents(const ModifierKeys &mods) const;

    bool shouldInteractWithChildren() const;
    MouseCursor getCursor() const;
    
    void unsetLastMode();
    void setMode(Mode newMode, bool force = false);
    bool isMode(Mode targetMode) const;
    
private:
    
    Mode mode = defaultMode;
    Mode previousMode = defaultMode;

    JUCE_LEAK_DETECTOR(RollEditMode)
    
};
