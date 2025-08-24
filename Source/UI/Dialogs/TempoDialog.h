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

class TempoTextEditor;
class TapTempoComponent;

class TempoDialog final : public DialogBase
{
public:

    explicit TempoDialog(int bpmValue);
    ~TempoDialog() override;

    Function<void(int newBpmValue)> onOk;
    Function<void()> onCancel;

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;

private:

    static constexpr auto tapTempoHeight = 46;
    static constexpr auto tapTempoMargin = 6;

    Component *getPrimaryFocusTarget() override;

    void updateOkButtonState();

    void dialogCancelAction() override;
    void dialogApplyAction() override;
    void dialogDeleteAction() override {};

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<TapTempoComponent> tapTempo;
    UniquePointer<TempoTextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoDialog)
};
