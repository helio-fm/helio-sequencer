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

class HybridRoll;
class ProjectNode;

#include "CommandPaletteActionsProvider.h"
#include "CommandPaletteModel.h"

class CommandPaletteTextEditor final : public TextEditor
{
public:
    bool keyPressed(const KeyPress &key) override;
};

class CommandPalette final : public Component,
                             public TextEditor::Listener,
                             public ListBoxModel
{
public:

    CommandPalette(ProjectNode *project, HybridRoll *roll);
    ~CommandPalette();

    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;
    void inputAttemptWhenModal() override;

private:

    static constexpr auto rowHeight = 28;

    //===------------------------------------------------------------------===//
    // TextEditor::Listener
    //===------------------------------------------------------------------===//

    void textEditorTextChanged(TextEditor &) override;
    void textEditorReturnKeyPressed(TextEditor &) override;
    void textEditorEscapeKeyPressed(TextEditor &) override;
    void textEditorFocusLost(TextEditor &) override;

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemClicked(int row, const MouseEvent &) override;
    void moveRowSelectionBy(int offset);

    void applySelectedCommand();
    int getHeightToFitActions();
    int getNumVisibleRows() const noexcept;

    void dismiss();
    void fadeOut();
    void updatePosition();

    Array<WeakReference<CommandPaletteActionsProvider>> actionsProviders;
    WeakReference<CommandPaletteActionsProvider> currentActionsProvider;
    WeakReference<CommandPaletteActionsProvider> rootActionsProvider;

    UniquePointer<Component> shadowDn;
    UniquePointer<Component> bg;
    UniquePointer<Component> shadowL;
    UniquePointer<Component> shadowR;
    UniquePointer<CommandPaletteTextEditor> textEditor;
    UniquePointer<ListBox> actionsList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommandPalette)
};
