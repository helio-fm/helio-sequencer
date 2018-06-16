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
class KeySignatureEvent;

#include "KeySignaturesProjectMap.h"
//[/Headers]


class KeySignatureSmallComponent final : public Component
{
public:

    KeySignatureSmallComponent(KeySignaturesProjectMap<KeySignatureSmallComponent> &parent, const KeySignatureEvent &targetEvent);
    ~KeySignatureSmallComponent();

    //[UserMethods]
    const KeySignatureEvent &getEvent() const;
    float getBeat() const;
    float getTextWidth() const;

    void updateContent();
    void setRealBounds(const Rectangle<float> bounds);

    static int compareElements(const KeySignatureSmallComponent *first,
                               const KeySignatureSmallComponent *second)
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

    const KeySignatureEvent &event;
    KeySignaturesProjectMap<KeySignatureSmallComponent> &editor;

    float textWidth;
    String eventName;

    Rectangle<float> boundsOffset;

    //[/UserVariables]

    ScopedPointer<Label> signatureLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureSmallComponent)
};
