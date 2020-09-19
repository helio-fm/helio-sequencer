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

class TapTempoComponent;

class TempoDialog final : public DialogBase
{
public:

    explicit TempoDialog(int bpmValue);
    ~TempoDialog() override;

    Function<void()> onOk;
    Function<void()> onCancel;
    
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;
    void inputAttemptWhenModal() override;

private:

    int originalValue = 0;
    int newValue = 0;

    static constexpr auto tapTempoHeight = 48;
    static constexpr auto tapTempoMargin = 8;

    void onTextFocusLost();
    void updateOkButtonState();
    void doCancel();
    void doOk();

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<TapTempoComponent> tapTempo;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoDialog)
};
