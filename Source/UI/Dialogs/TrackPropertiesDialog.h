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
#include "DialogBase.h"
#include "ColourButton.h"
#include "ColourSwatches.h"
#include "MidiTrack.h"

class ProjectNode;
class AnnotationsSequence;
//[/Headers]


class TrackPropertiesDialog final : public DialogBase,
                                    public TextEditor::Listener,
                                    public ColourButton::Listener,
                                    private Timer,
                                    public Button::Listener
{
public:

    TrackPropertiesDialog(ProjectNode &project, WeakReference<MidiTrack> track, const String &title = "", const String &confirmation = "");
    ~TrackPropertiesDialog();

    //[UserMethods]
    Function<void()> onOk;
    Function<void()> onCancel;

    void onColourButtonClicked(ColourButton *button) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked(Button *buttonThatWasClicked) override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    ProjectNode &project;
    WeakReference<MidiTrack> track;

    String originalName;
    String newName;

    Colour originalColour;
    Colour newColour;

    bool hasMadeChanges = false;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    inline void updateOkButtonState();

    void applyChangesIfAny();
    void cancelChangesIfAny();
    bool hasChanges() const;

    void doCancel();
    void doOk();

    //[/UserVariables]

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<ColourSwatches> colourSwatches;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackPropertiesDialog)
};


