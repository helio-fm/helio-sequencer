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
class MidiRoll;
//[/Headers]

#include "../../Themes/ShadowLeftwards.h"
#include "../../Themes/ShadowRightwards.h"

class InsertSpaceHelper  : public Component
{
public:

    InsertSpaceHelper (MidiRoll &parentRoll);

    ~InsertSpaceHelper();

    //[UserMethods]
    void setStartBeat(float beat);
    void setEndBeat(float beat);

    float getLeftMostBeat() const;
    float getRightMostBeat() const;

    bool isInverted() const;
    bool shouldCheckpoint() const;
    void setNeedsCheckpoint(bool val);

    void snapWidth();
    void rebound();
    void fadeIn();

    float getDragDelta() const;
    void resetDragDelta();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    MidiRoll &roll;

    float startBeat;
    float endBeat;
    float endBeatAnchor;

    bool needsCheckpoint;

    //[/UserVariables]

    ScopedPointer<ShadowLeftwards> shadowL;
    ScopedPointer<ShadowRightwards> shadowR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InsertSpaceHelper)
};
