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

#include "MenuPanel.h"
#include "UndoStack.h"
#include "Clip.h"

class ClipMenu final : public MenuPanel
{
public:
    
    ClipMenu(const Clip &clip, WeakReference<UndoStack> undoStack);
    
private:
    
    MenuPanel::Menu makeDefaultMenu();
    MenuPanel::Menu makeQuantizationMenu();
    MenuPanel::Menu makeChannelSelectionMenu();
    MenuPanel::Menu makeInstrumentSelectionMenu();

    const Clip &clip;

    WeakReference<UndoStack> undoStack;
};
