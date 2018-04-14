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

class Instrument;
class ProjectTreeItem;
class MidiTrackTreeItem;

#include "CommandPanel.h"

class ProjectCommandPanel : public CommandPanel
{
public:
    
    ProjectCommandPanel(ProjectTreeItem &parentProject, AnimationType animationType);
    
    void handleCommandMessage(int commandId) override;
    
private:

    ProjectTreeItem &project;
    
    WeakReference<Instrument> lastSelectedInstrument;

    void initMainMenu(AnimationType animationType);
    void initBatchMenu(AnimationType animationType);
    void initRenderMenu();
    void initInstrumentSelection();
    
    void initNewSubItemsMenu(AnimationType animationType);
    void initSubItemTypeSelectionMenu();

    bool haveSetBatchCheckpoint;
    
    ValueTree createPianoTrackTempate(const String &name) const;
    ValueTree createAutoLayerTempate(const String &name, int controllerNumber, const String &instrumentId = "") const;
    
    void proceedToRenderDialog(const String &extension);
    void dismiss();

};
