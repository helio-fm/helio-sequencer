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

#include "RadioButton.h"

class KeySelector : public Component,
    public RadioButtonListener
{
public:

    KeySelector();
    ~KeySelector();

    struct Listener
    {
        virtual ~Listener() {}
        virtual void onKeyChanged(int key) = 0;
    };

    inline static StringArray getKeysList(bool sharps = true)
    {
        return sharps ?
            StringArray("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B") :
            StringArray("C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B");
    }

    void onRadioButtonClicked(RadioButton *button) override;
    void setSelectedKey(int key);

    void resized() override;

private:

    OwnedArray<RadioButton> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySelector)
};
