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

class RollBase;

#include "IconComponent.h"

class RollExpandMark final : public Component, private Timer
{
public:

    RollExpandMark(RollBase &parentRoll, float targetBeat, int numBeatsToTake);
    ~RollExpandMark();

    void paint(Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;

private:

    void timerCallback() override;
    void updatePosition();

    RollBase &roll;

    float beat = 0.f;
    float alpha = 1.f;
    int numBeats = 0;

    UniquePointer<IconComponent> plusImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollExpandMark)
};
