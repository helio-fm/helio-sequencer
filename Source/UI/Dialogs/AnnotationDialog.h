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
#include "AnnotationEvent.h"
#include "ColourButton.h"
#include "ColourSwatches.h"
#include "MobileComboBox.h"

class AnnotationsSequence;

class AnnotationDialog final : public DialogBase, public ColourButton::Listener
{
public:

    AnnotationDialog(AnnotationsSequence *sequence,
        const AnnotationEvent &editedEvent,
        bool shouldAddNewEvent, float targetBeat) noexcept;

    ~AnnotationDialog();

    static UniquePointer<Component>
        editingDialog(const AnnotationEvent &event);

    static UniquePointer<Component>
        addingDialog(AnnotationsSequence *annotations, float targetBeat);

    void onColourButtonClicked(ColourButton *button) override;

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

    AnnotationEvent originalEvent;
    AnnotationsSequence *const originalSequence;

    Component *getPrimaryFocusTarget() override;
    void updateOkButtonState();

    const bool addsNewEvent = false;
    bool hasMadeChanges = false;

    static constexpr auto colourSwatchesMargin = 4;

    void sendEventChanges(const AnnotationEvent &newEvent);
    void updateEvent();
    void removeEvent();

    UniquePointer<MobileComboBox::Container> presetsCombo;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<ColourSwatches> colourSwatches;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnnotationDialog)
};
