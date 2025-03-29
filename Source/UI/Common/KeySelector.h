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

#include "RadioButton.h"
#include "Temperament.h"

class KeySelector final : public Component, public RadioButton::Listener
{
public:

    explicit KeySelector(const Temperament::Period &period);

    Function<void(int key)> onKeyPreview;
    Function<void(int key, const String &keyName)> onKeyChanged;

    void onRadioButtonClicked(const MouseEvent &e, RadioButton *button) override;
    void setSelectedKey(int key, const String &keyName);

    int getButtonWidth() const noexcept;

    void resized() override;

private:

    struct KeyEnharmonics final
    {
        UniquePointer<RadioButton> mainKey;
        UniquePointer<RadioButton> flatKey;
        UniquePointer<RadioButton> sharpKey;
    };

    Array<KeyEnharmonics> buttons;

    int buttonWidth = 30;

    static constexpr auto mainRowHeight = 32;
    static constexpr auto altRowHeight = 26;

    bool hasFlatsRow = false;
    bool hasSharpsRow = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySelector)
};
