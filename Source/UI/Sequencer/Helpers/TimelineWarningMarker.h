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

#include "ComponentFader.h"

class RollBase;

class TimelineWarningMarker final : public Component
{
public:

    enum class WarningLevel : int8
    {
        Red = 0,
        Yellow = 1,
    };
    
    TimelineWarningMarker(WarningLevel warningLevel,
        RollBase &parentRoll, float initialBeatPosition);

    float getStartBeat() const noexcept;
    void setStartBeat(float startBeat);

    float getEndBeat() const noexcept;
    void setEndBeat(float endBeat);

    void paint(Graphics &g) override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;

    static constexpr auto minSizeInBeats = 1.f / Globals::ticksPerBeat * 2.f;
    static constexpr auto minGapInBeats = 0.5f;

private:

    RollBase &roll;

    const Colour colour;

    float startBeat = 0.f;
    float endBeat = minSizeInBeats;

    void updateBounds();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimelineWarningMarker)
};
