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
class PopupButtonHighlighter;
class PopupButtonConfirmation;

#include "PopupButtonOwner.h"
#include "ComponentFader.h"
//[/Headers]


class PopupButton  : public Component,
                     private Timer
{
public:

    PopupButton (bool shouldShowConfirmImage);

    ~PopupButton();

    //[UserMethods]
    float getRadiusDelta() const;
    Point<int> getDragDelta() const;
    void setState(bool clicked);
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

    void onAction();
    void updateChildren();
    void timerCallback() override;

    float alpha;
    float raduisDelta;

    bool firstClickDone;
    bool showConfirmImage;

    ComponentDragger dragger;
    Point<int> anchor;

    ComponentFader fader;

    //[/UserVariables]

    ScopedPointer<PopupButtonHighlighter> mouseOverHighlighter;
    ScopedPointer<PopupButtonHighlighter> mouseDownHighlighter;
    ScopedPointer<PopupButtonConfirmation> confirmationMark;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupButton)
};
