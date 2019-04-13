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
#include "TimeSignatureComponent.h"
//[/Headers]


class TimeSignatureLargeComponent final : public TimeSignatureComponent
{
public:

    TimeSignatureLargeComponent(TimeSignaturesProjectMap &parent, const TimeSignatureEvent &targetEvent);
    ~TimeSignatureLargeComponent();

    //[UserMethods]
    void updateContent() override;
    void setRealBounds(const Rectangle<float> bounds) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;


private:

    //[UserVariables]

    ComponentDragger dragger;
    TimeSignatureEvent anchor;

    int numerator;
    int denominator;

    Rectangle<float> boundsOffset;
    Point<int> clickOffset;
    bool draggingState;
    bool draggingHadCheckpoint;

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered;

    //[/UserVariables]

    UniquePointer<Label> numeratorLabel;
    UniquePointer<Label> denominatorLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureLargeComponent)
};


