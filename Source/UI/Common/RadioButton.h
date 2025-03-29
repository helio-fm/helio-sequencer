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

#include "HighlightedComponent.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class RadioButton final : public HighlightedComponent
{
public:

    struct Listener
    {
        virtual ~Listener() = default;
        virtual void onRadioButtonClicked(const MouseEvent &e, RadioButton *button) = 0;
    };

    // constructs a button with a simple label
    RadioButton(const String &labelText,
        Colour c, RadioButton::Listener *listener);

    // constructs a button with NoteNameComponent
    RadioButton(const String &noteName,
        bool shouldUseFixedDo, RadioButton::Listener *listener);

    ~RadioButton();

    void select();
    void deselect();

    bool isSelected() const noexcept;
    Colour getColour() const noexcept;
    const String &getButtonName() const noexcept;

    int getButtonIndex() const noexcept;
    void setButtonIndex(int val);

    int getMinButtonWidth() const noexcept;

    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    void handleClick(const MouseEvent &e);

    Component *createHighlighterComponent() override;

    int minWidth = 32;

    int index = 0;
    String name;
    Colour colour;

    bool selected = false;

    const Colour fillColour;
    const Colour outlineColour;

    static constexpr float contentSelectedAlpha = 1.f;
    static constexpr float contentDeselectedAlpha = 0.9f;

    RadioButton::Listener *const listener;

    UniquePointer<Component> checkMark;
    UniquePointer<Component> content;

    ComponentFader fader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RadioButton)
};


