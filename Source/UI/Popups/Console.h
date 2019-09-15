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
#include "ConsoleActionsProvider.h"

class ConsoleTextEditor final : public TextEditor
{
public:
    bool keyPressed(const KeyPress &key) override
    {
        static const juce_wchar tildaKey = '`';
        if (key.isKeyCode(tildaKey))
        {
            this->newTransaction();
            this->escapePressed();
            return true;
        }

        return TextEditor::keyPressed(key);
    }

    void paint(Graphics &g) override
    {
        getLookAndFeel().fillTextEditorBackground(g, getWidth(), getHeight(), *this);
        getLookAndFeel().drawTextEditorOutline(g, getWidth(), getHeight(), *this);
    }
};
//[/Headers]

#include "../Themes/PanelBackgroundB.h"
#include "../Themes/ShadowDownwards.h"
#include "../Themes/ShadowLeftwards.h"
#include "../Themes/ShadowRightwards.h"

class Console final : public Component,
                      public TextEditor::Listener
{
public:

    Console();
    ~Console();

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

    void textEditorTextChanged(TextEditor &) override;
    void textEditorReturnKeyPressed(TextEditor &) override;
    void textEditorEscapeKeyPressed(TextEditor &) override;
    void textEditorFocusLost(TextEditor &) override;

    void cancelAndDismiss();
    void dismiss();
    void fadeOut();
    void updatePosition();

    //[/UserVariables]

    UniquePointer<PanelBackgroundB> bg;
    UniquePointer<ShadowDownwards> shadowDn;
    UniquePointer<ShadowLeftwards> shadowL;
    UniquePointer<ShadowRightwards> shadowR;
    UniquePointer<ConsoleTextEditor> textEditor;
    UniquePointer<ListBox> actionsList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Console)
};


