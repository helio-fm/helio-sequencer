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
#include "FadingDialog.h"
#include "Pattern.h"
#include "Clip.h"

class JavaScriptTokeniser final : public CodeTokeniser
{
public:

    JavaScriptTokeniser() = default;
    int readNextToken(CodeDocument::Iterator&) override;
    CodeEditorComponent::ColourScheme getDefaultColourScheme() override;

    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_keyword,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation
    };

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JavaScriptTokeniser)
};

//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class ScriptDialog final : public FadingDialog,
                           public CodeDocument::Listener,
                           private Timer,
                           public Button::Listener
{
public:

    ScriptDialog(Component &owner, Pattern *pattern, const Clip &clip);
    ~ScriptDialog();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked(Button* buttonThatWasClicked) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    Clip clip;
    Pattern *pattern;
    Component &ownerComponent;

    JavaScriptTokeniser tokeniser;
    CodeDocument document;

    void codeDocumentTextInserted(const String& newText, int insertIndex) override;
    void codeDocumentTextDeleted(int startIndex, int endIndex) override;

    void timerCallback() override;

    inline void cancelAndDisappear();
    inline void disappear();
    inline void updateOkButtonState();

    bool hasMadeChanges;
    void sendClipChange(const Clip &newClip);
    void cancelChangesIfAny();

    //[/UserVariables]

    UniquePointer<DialogPanel> background;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<SeparatorHorizontal> separatorH;
    UniquePointer<SeparatorVertical> separatorV;
    UniquePointer<CodeEditorComponent> codeEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScriptDialog)
};
