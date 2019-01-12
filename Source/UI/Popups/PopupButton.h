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

#include "PopupButtonOwner.h"
#include "ComponentFader.h"

class PopupButtonHighlighter final : public Component
{
public:
    PopupButtonHighlighter(const PopupButton &parent);
    void paint(Graphics &g) override;
private:
    const PopupButton &button;
};

class PopupButtonConfirmation final : public Component
{
public:
    explicit PopupButtonConfirmation(const PopupButton &parent);
    void paint(Graphics &g) override;
private:
    const PopupButton &button;
    Path clickConfirmImage;
};

class PopupButton : public Component, private Timer
{
public:

    enum ShapeType
    {
        Circle,
        Hex
    };

    PopupButton(bool shouldShowConfirmImage, ShapeType shapeType = Circle,
        Colour colour = Colours::black.withAlpha(0.5f));

    float getRadiusDelta() const noexcept;
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

    void updateChildren();
    void timerCallback() override;

    float alpha;
    float raduisDelta;

    bool firstClickDone;
    bool showConfirmImage;

    const Colour colour;
    String userData;

    ComponentDragger dragger;
    Point<int> anchor;

    ComponentFader fader;

    friend class PopupButtonHighlighter;
    friend class PopupButtonConfirmation;

    UniquePointer<PopupButtonHighlighter> mouseOverHighlighter;
    UniquePointer<PopupButtonHighlighter> mouseDownHighlighter;
    UniquePointer<PopupButtonConfirmation> confirmationMark;

    ShapeType shapeType;
    Path shape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupButton)
};
