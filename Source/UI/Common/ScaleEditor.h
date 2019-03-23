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

#include "Scale.h"
#include "RadioButton.h"

class ScaleEditor final : public Component, public RadioButtonListener
{
public:

    ScaleEditor();
    ~ScaleEditor();

    struct Listener
    {
        virtual ~Listener() {}
        virtual void onScaleChanged(const Scale::Ptr scale) = 0;
    };

    void onRadioButtonClicked(RadioButton *button) override;
    void setScale(const Scale::Ptr scale);

    void resized() override;

private:

    Scale::Ptr scale;
    OwnedArray<RadioButton> buttons;

    void updateButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleEditor)
};
