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

class Instrument;
class ProjectNode;
class MidiTrackNode;

#include "MenuPanel.h"

class ProjectMenu final : public MenuPanel
{
public:
    
    ProjectMenu(ProjectNode &parentProject, AnimationType animationType);    
    void handleCommandMessage(int commandId) override;

    static void showNewTrackDialog(ProjectNode &project,
        const String &instrumentId, float beatPosition);

private:

    ProjectNode &project;
    Array<Instrument *> instruments;

    void showMainMenu(AnimationType animationType);
    void showBatchActionsMenu(AnimationType animationType);
    void showRenderMenu();
    void showSetInstrumentMenu();
    void showTemperamentsMenu(bool convertTracks);

    void showCreateItemsMenu(AnimationType animationType);
    void showNewTrackMenu(AnimationType animationType);
    void showNewAutomationMenu(AnimationType animationType);
    void showControllersMenuForInstrument(const WeakReference<Instrument> instrument);

};
