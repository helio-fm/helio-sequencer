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

class CutPointMark : public Component
{
public:

    CutPointMark(SafePointer<Component> targetComponent, float absPosX);
    ~CutPointMark();

    void fadeIn();
    void updatePosition(float pos);
    void updateBounds(bool forceNoAnimation = false);

    Component *getComponent() const noexcept;
    float getCutPosition() const noexcept;

protected:

    SafePointer<Component> targetComponent;

    bool initialized = false;
    float absPosX = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutPointMark)
};

class NoteCutPointMark final : public CutPointMark
{
public:

    NoteCutPointMark(SafePointer<Component> targetComponent, float absPosX);

    void paint(Graphics &g) override;
};

class ClipCutPointMark final : public CutPointMark
{
public:

    explicit ClipCutPointMark(SafePointer<Component> targetComponent);

    void paint(Graphics &g) override;

    void updatePositionFromMouseEvent(int mouseX, int mouseY);
};
