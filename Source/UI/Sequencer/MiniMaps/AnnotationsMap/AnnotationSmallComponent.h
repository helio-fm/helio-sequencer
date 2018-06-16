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
class AnnotationEvent;

#include "AnnotationsProjectMap.h"
//[/Headers]


class AnnotationSmallComponent final : public Component
{
public:

    AnnotationSmallComponent(AnnotationsProjectMap<AnnotationSmallComponent> &parent, const AnnotationEvent &targetEvent);
    ~AnnotationSmallComponent();

    //[UserMethods]
    const AnnotationEvent &getEvent() const;
    float getBeat() const;
    float getTextWidth() const;

    void updateContent();
    void setRealBounds(const Rectangle<float> bounds);

    static int compareElements(const AnnotationSmallComponent *first,
                               const AnnotationSmallComponent *second)
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
    void parentHierarchyChanged() override;


private:

    //[UserVariables]

    const AnnotationEvent &event;
    AnnotationsProjectMap<AnnotationSmallComponent> &editor;

    Rectangle<float> boundsOffset;
    float textWidth;
    Colour lastColour;

    //[/UserVariables]

    ScopedPointer<Label> annotationLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnnotationSmallComponent)
};
