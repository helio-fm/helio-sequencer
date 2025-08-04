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

#include "PopupButtonOwner.h"

class PopupButton : public Component
{
public:

    enum class Shape : int8
    {
        Circle,
        Hex
    };

    explicit PopupButton(Shape shapeType = Shape::Circle,
        Colour colour = Colours::black.withAlpha(0.5f));

    Point<int> getDragDelta() const noexcept;
    void setState(bool clicked);
    void setUserData(const String &data);
    const String &getUserData() const noexcept;

    void paint(Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    bool isSelected = false;

    const Colour colour;
    
    static constexpr float defaultAlpha = 0.85f;
    static constexpr float highlightedAlpha = 0.925f;
    static constexpr float selectedAlpha = 1.f;
    float alpha = defaultAlpha;

    String userData;

    ComponentDragger dragger;
    Point<int> anchor;

    Shape shapeType;
    Path shape;
    Path selectionShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupButton)
};
