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

#include "Scale.h"
#include "RadioButton.h"
#include "ColourIDs.h"

class ScaleEditor final : public Component, public RadioButton::Listener
{
public:

    ScaleEditor() = default;
    ~ScaleEditor() override;

    Function<void(int key)> onScaleNotePreview;
    Function<void(const Scale::Ptr scale)> onScaleChanged;

    void onRadioButtonClicked(const MouseEvent &e, RadioButton *button) override;
    void setScale(const Scale::Ptr scale);

    void setButtonWidth(int newWidth);

    void resized() override;

private:

    Scale::Ptr scale;

    OwnedArray<RadioButton> buttons;

    int buttonWidth = 32;

    static constexpr auto rowHeight = 32;

    const Colour buttonColour = findDefaultColour(ColourIDs::ColourButton::outline);

    void rebuildButtons();
    void updateButtonsState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleEditor)
};
