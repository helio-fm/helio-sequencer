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

#include "DialogBase.h"
#include "KeySignatureEvent.h"
#include "MobileComboBox.h"

class Transport;
class ProjectNode;
class KeySignaturesSequence;
class SeparatorHorizontalFading;
class PlayButton;
class ScalePreviewThread;
class ScaleEditor;
class KeySelector;

class KeySignatureDialog final : public DialogBase
{
public:

    KeySignatureDialog(ProjectNode &project, KeySignaturesSequence *keySequence,
        const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat) noexcept;

    ~KeySignatureDialog();

    static UniquePointer<Component> editingDialog(ProjectNode &project,
        const KeySignatureEvent &event);

    static UniquePointer<Component> addingDialog(ProjectNode &project,
        KeySignaturesSequence *annotationsLayer, float targetBeat);

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;

private:

    void dialogCancelAction() override;
    void dialogApplyAction() override;
    void dialogDeleteAction() override;

    Component *getPrimaryFocusTarget() override;

    ProjectNode &project;
    Transport &transport;
    KeySignatureEvent originalEvent;
    KeySignaturesSequence *const originalSequence;

    Array<Scale::Ptr> scales;

    void reloadScalesList();
    void updateButtonsState();

    bool addsNewEvent = false;
    bool hasMadeChanges = false;

    void previewNote(int key) const;
    void sendEventChange(const KeySignatureEvent &newEvent);
    void removeEvent();

    int rootKey = 0;
    String rootKeyName;
    Scale::Ptr scale;

    void savePreset();

    UniquePointer<ScalePreviewThread> scalePreviewThread;

    UniquePointer<MobileComboBox::Container> presetsCombo;
    UniquePointer<Label> messageLabel;

    UniquePointer<Viewport> keySelectorViewport;
    UniquePointer<KeySelector> keySelector;
    UniquePointer<Component> keySelectorShadowLeft;
    UniquePointer<Component> keySelectorShadowRight;
    UniquePointer<SeparatorHorizontalFading> separator;

    UniquePointer<Viewport> scaleEditorViewport;
    UniquePointer<ScaleEditor> scaleEditor;
    UniquePointer<Component> scaleEditorShadowLeft;
    UniquePointer<Component> scaleEditorShadowRight;
    UniquePointer<TextEditor> scaleNameEditor;
    UniquePointer<IconButton> savePresetButton;
    UniquePointer<PlayButton> playButton;

    UniquePointer<TextButton> okButton;
    UniquePointer<TextButton> removeEventButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySignatureDialog)
};
