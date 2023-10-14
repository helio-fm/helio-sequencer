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

class Lasso;
class MidiTrack;
class ProjectNode;

#include "MenuPanel.h"
#include "Scale.h"
#include "Note.h"

class PianoRollSelectionMenu final : public MenuPanel
{
public:
    
    PianoRollSelectionMenu(ProjectNode &project, WeakReference<Lasso> lasso);

private:

    ProjectNode &project;
    WeakReference<Lasso> lasso;

    Note::Key harmonicContextKey = -1;
    Scale::Ptr harmonicContextScale = nullptr;

    MenuPanel::Menu createDefaultPanel();
    MenuPanel::Menu createQuantizationPanel();
    MenuPanel::Menu createRefactoringPanel();
    MenuPanel::Menu createMoveToTrackPanel();
    MenuPanel::Menu createTupletsPanel();
    MenuPanel::Menu createScalesPanel();
    MenuPanel::Menu createArpsPanel();

};
