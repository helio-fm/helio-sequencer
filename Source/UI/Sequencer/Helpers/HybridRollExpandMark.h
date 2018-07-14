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

//[Headers]
class HybridRoll;
#include "IconComponent.h"
//[/Headers]


class HybridRollExpandMark final : public Component,
                                   private Timer
{
public:

    HybridRollExpandMark(HybridRoll &parentRoll, float targetBar, int numBarsToTake);
    ~HybridRollExpandMark();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;


private:

    //[UserVariables]

    void timerCallback() override;
    void updatePosition();

    HybridRoll &roll;

    float bar;
    float alpha;
    int numBars;

    //[/UserVariables]

    UniquePointer<IconComponent> plusImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HybridRollExpandMark)
};
