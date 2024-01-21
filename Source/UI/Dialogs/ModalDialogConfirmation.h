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

using SimpleDialogCallback = Function<void()>;

class ModalDialogConfirmation final : public DialogBase
{
public:

    ModalDialogConfirmation(const String &message, const String &okText, const String &cancelText);
    ~ModalDialogConfirmation();

    SimpleDialogCallback onOk;
    SimpleDialogCallback onCancel;

    struct Presets final
    {
        static UniquePointer<ModalDialogConfirmation> deleteProject();
        static UniquePointer<ModalDialogConfirmation> forceCheckout();
        static UniquePointer<ModalDialogConfirmation> resetChanges();
    };

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;

private:

    void cancel();
    void okay();

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalDialogConfirmation)
};
