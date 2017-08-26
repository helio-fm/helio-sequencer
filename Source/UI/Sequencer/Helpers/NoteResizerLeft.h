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
class NoteComponent;
class Lasso;

#include "IconComponent.h"
#include "ComponentFader.h"
//[/Headers]


class NoteResizerLeft  : public Component
{
public:

    NoteResizerLeft (HybridRoll &parentRoll);

    ~NoteResizerLeft();

    //[UserMethods]
    void updateBounds(NoteComponent *anchorComponent = nullptr);
    void updateTopPosition();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    bool hitTest (int x, int y) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    NoteComponent *findLeftMostEvent(const Lasso &selection);

    ComponentFader fader;
    ComponentDragger dragger;
    HybridRoll &roll;
    SafePointer<NoteComponent> noteComponent;

    //[/UserVariables]

    ScopedPointer<IconComponent> resizeIcon;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteResizerLeft)
};
