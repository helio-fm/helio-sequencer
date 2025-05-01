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

#include "MobileComboBox.h"
#include "SeparatorHorizontal.h"

class UserInterfaceSettings final : public Component
{
public:

    UserInterfaceSettings();
    ~UserInterfaceSettings();

    void resized() override;
    void visibilityChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    void updateButtons();
    StringArray systemFonts;

    UniquePointer<MobileComboBox::Container> fontsCombo;
    UniquePointer<ToggleButton> openGLRendererButton;
    UniquePointer<TextEditor> fontEditor;
    UniquePointer<ToggleButton> nativeTitleBarButton;
    UniquePointer<SeparatorHorizontal> wheelFlagsSeparator;
    UniquePointer<ToggleButton> wheelAltModeButton;
    UniquePointer<ToggleButton> wheelVerticalPanningButton;
    UniquePointer<ToggleButton> wheelVerticalZoomingButton;
    UniquePointer<SeparatorHorizontal> miscFlagsSeparator;
    UniquePointer<ToggleButton> followPlayheadButton;
    UniquePointer<ToggleButton> animationsEnabledButton;
    UniquePointer<SeparatorHorizontal> uiScaleSeparator;
    UniquePointer<Label> uiScaleTitle;
    UniquePointer<ToggleButton> scaleUi1;
    UniquePointer<ToggleButton> scaleUi125;
    UniquePointer<ToggleButton> scaleUi15;
    UniquePointer<ToggleButton> scaleUi2;
    UniquePointer<SeparatorHorizontal> noteNamesSeparator;
    UniquePointer<Label> noteNamesTitle;
    UniquePointer<ToggleButton> germanNotation;
    UniquePointer<ToggleButton> fixedDoNotation;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserInterfaceSettings)
};
