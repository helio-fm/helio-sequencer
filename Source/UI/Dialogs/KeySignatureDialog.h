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

#include "DialogBase.h"
#include "KeySignatureEvent.h"
#include "ScaleEditor.h"
#include "KeySelector.h"
#include "MobileComboBox.h"

class Transport;
class ProjectNode;
class KeySignaturesSequence;
class SeparatorHorizontalFading;
class PlayButton;

class KeySignatureDialog final : public DialogBase,
    public TextEditor::Listener,
    public ScaleEditor::Listener,
    public KeySelector::Listener
{
public:

    KeySignatureDialog(ProjectNode &project, KeySignaturesSequence *keySequence,
        const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);

    ~KeySignatureDialog();

    static UniquePointer<Component> editingDialog(ProjectNode &project,
        const KeySignatureEvent &event);

    static UniquePointer<Component> addingDialog(ProjectNode &project,
        KeySignaturesSequence *annotationsLayer, float targetBeat);

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    void onKeyChanged(int key) override;
    void onRootKeyPreview(int key) override;

    void onScaleChanged(const Scale::Ptr scale) override;
    void onScaleNotePreview(int key) override;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    Component *getPrimaryFocusTarget() override;

    ProjectNode &project;
    Transport &transport;
    KeySignatureEvent originalEvent;
    KeySignaturesSequence *const originalSequence;

    Array<Scale::Ptr> scales;

    void reloadScalesList();

    void cancelAndDisappear();
    void updateButtonsState();

    bool addsNewEvent = false;
    bool hasMadeChanges = false;

    void previewNote(int key) const;
    void sendEventChange(const KeySignatureEvent &newEvent);
    void removeEvent();

    int rootKey = 0;
    Scale::Ptr scale;

    void savePreset();

    UniquePointer<Thread> scalePreviewThread;

    UniquePointer<MobileComboBox::Container> presetsCombo;
    UniquePointer<Label> messageLabel;

    UniquePointer<KeySelector> keySelector;
    UniquePointer<SeparatorHorizontalFading> separator;

    UniquePointer<ScaleEditor> scaleEditor;
    UniquePointer<TextEditor> scaleNameEditor;
    UniquePointer<IconButton> savePresetButton;
    UniquePointer<PlayButton> playButton;

    UniquePointer<TextButton> okButton;
    UniquePointer<TextButton> removeEventButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureDialog)
};
