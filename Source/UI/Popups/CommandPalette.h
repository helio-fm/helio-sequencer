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

//[Headers]
class ProjectNode;
class HybridRoll;

#include "CommandPaletteActionsProvider.h"
#include "CommandPaletteModel.h"

class CommandPaletteTextEditor final : public TextEditor
{
public:
    bool keyPressed(const KeyPress &key) override;
};
//[/Headers]

#include "../Themes/ShadowDownwards.h"
#include "../Themes/PanelBackgroundC.h"
#include "../Themes/ShadowLeftwards.h"
#include "../Themes/ShadowRightwards.h"

class CommandPalette final : public Component,
                             public TextEditor::Listener,
                             public ListBoxModel
{
public:

    CommandPalette(ProjectNode *project, HybridRoll *roll);
    ~CommandPalette();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

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

    void cancelAndDismiss();
    void dismiss();
    void fadeOut();
    void updatePosition();

    Array<WeakReference<CommandPaletteActionsProvider>> actionsProviders;
    WeakReference<CommandPaletteActionsProvider> currentActionsProvider;
    WeakReference<CommandPaletteActionsProvider> defaultActionsProvider;

    //[/UserVariables]

    UniquePointer<ShadowDownwards> shadowDn;
    UniquePointer<PanelBackgroundC> bg;
    UniquePointer<ShadowLeftwards> shadowL;
    UniquePointer<ShadowRightwards> shadowR;
    UniquePointer<CommandPaletteTextEditor> textEditor;
    UniquePointer<ListBox> actionsList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommandPalette)
};


