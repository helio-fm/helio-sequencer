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
#include "SequenceModifier.h"
#include "RefactoringSequenceModifier.h"
#include "Clip.h"

class ClipModifiersMenu : public MenuPanel
{
public:

    ClipModifiersMenu(const Clip &clip, WeakReference<UndoStack> undoStack);

    // either falls back to "add modifiers" menu if no modifiers found,
    // or displays an "edit modifiers" menu with "add modifiers" submenu
    MenuPanel::Menu makeModifiersMenu(const MenuItem::Callback &goBack) noexcept;
    MenuPanel::Menu makeEditModifiersMenu(const MenuItem::Callback &goBack) noexcept;
    MenuPanel::Menu makeAddModifiersMenu(const MenuItem::Callback &goBack) noexcept;

    // adds new modifiers or updates existing ones
    MenuPanel::Menu makeModifiersArpsMenu(const MenuItem::Callback &goBack,
        const MenuItem::Callback &onAdd,
        SequenceModifier::Ptr updatedModifier) noexcept;

    MenuPanel::Menu makeModifiersArpsSpeedMenu(const MenuItem::Callback &goBack,
        const MenuItem::Callback &onAdd,
        SequenceModifier::Ptr updatedModifier,
        const Arpeggiator::Ptr arp,
        const Array<float> &speedValues) noexcept;

    MenuPanel::Menu makeModifiersRefactoringStepsMenu(const MenuItem::Callback &goBack,
        const MenuItem::Callback &onAdd, SequenceModifier::Ptr updatedModifier,
        RefactoringSequenceModifier::Type type, Icons::Id iconId,
        const Array<RefactoringSequenceModifier::Parameter> &parameters) noexcept;

private:

    const Clip &clip;

    WeakReference<UndoStack> undoStack;

};

class ClipMenu final : public ClipModifiersMenu
{
public:
    
    ClipMenu(const Clip &clip, WeakReference<UndoStack> undoStack);

private:
    
    MenuPanel::Menu makeDefaultMenu() noexcept;
    MenuPanel::Menu makeRefactoringMenu() noexcept;
    MenuPanel::Menu makeQuantizationMenu() noexcept;
    MenuPanel::Menu makeChannelSelectionMenu() noexcept;
    MenuPanel::Menu makeInstrumentSelectionMenu() noexcept;

    const Clip &clip;

    WeakReference<UndoStack> undoStack;
};
