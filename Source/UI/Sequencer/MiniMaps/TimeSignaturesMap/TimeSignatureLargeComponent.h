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
class TimeSignatureEvent;

#include "TimeSignaturesProjectMap.h"
//[/Headers]


class TimeSignatureLargeComponent final : public Component
{
public:

    TimeSignatureLargeComponent(TimeSignaturesProjectMap<TimeSignatureLargeComponent> &parent, const TimeSignatureEvent &targetEvent);
    ~TimeSignatureLargeComponent();

    //[UserMethods]
    const TimeSignatureEvent &getEvent() const;
    float getBeat() const;

    void updateContent();
    void setRealBounds(const Rectangle<float> bounds);

    static int compareElements(const TimeSignatureLargeComponent *first,
                               const TimeSignatureLargeComponent *second)
    {
        if (first == second) { return 0; }

        const float diff = first->event.getBeat() - second->event.getBeat();
        const int diffResult = (diff > 0.f) - (diff < 0.f);
        if (diffResult != 0) { return diffResult; }

        return first->event.getId().compare(second->event.getId());
    }
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

    const TimeSignatureEvent &event;
    TimeSignaturesProjectMap<TimeSignatureLargeComponent> &editor;

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

    ScopedPointer<Label> numeratorLabel;
    ScopedPointer<Label> denominatorLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureLargeComponent)
};
