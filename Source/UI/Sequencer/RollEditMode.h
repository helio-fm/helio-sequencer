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

#pragma once

class RollEditMode final
{
public:

    enum Mode
    {
        defaultMode,
        drawMode,  // these two
        eraseMode, // are displayed as one
        selectionMode,
        dragMode,
        knifeMode, // these two
        mergeMode  // are displayed as one
    };

    RollEditMode() = default;
    RollEditMode(Mode mode) : mode(mode) {}
    RollEditMode(const RollEditMode &other) :
        mode(other.mode),
        previousMode(other.previousMode) {}

    //===------------------------------------------------------------------===//
    // Listeners
    //===------------------------------------------------------------------===//

    class Listener
    {
    public:

        Listener() = default;
        virtual ~Listener() = default;

        virtual void onChangeEditMode(const RollEditMode &mode) = 0;
    };

    void addListener(Listener *listener);
    void removeListener(Listener *listener);

    //===------------------------------------------------------------------===//
    // Action checks
    //===------------------------------------------------------------------===//

    bool forbidsViewportDragging(const ModifierKeys &mods) const;
    bool forcesViewportDragging(const ModifierKeys &mods) const;

    bool forbidsSelectionMode(const ModifierKeys &mods) const;
    bool forcesSelectionMode(const ModifierKeys &mods) const;

    bool forbidsAddingEvents(const ModifierKeys &mods) const;
    bool forcesAddingEvents(const ModifierKeys &mods) const;

    bool forbidsCuttingEvents(const ModifierKeys &mods) const;
    bool forcesCuttingEvents(const ModifierKeys &mods) const;

    bool forbidsMergingEvents(const ModifierKeys &mods) const;
    bool forcesMergingEvents(const ModifierKeys &mods) const;

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

    ListenerList<Listener> listeners;

    JUCE_LEAK_DETECTOR(RollEditMode)
};
