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
class IconComponent;
//[/Headers]

#include "../../Themes/ShadowRightwards.h"

class TrackEndIndicator  : public Component
{
public:

    TrackEndIndicator ();

    ~TrackEndIndicator();

    //[UserMethods]

    float getAnchor() const
    {
        return this->absPosition;
    }

    inline void setAnchoredAt(float absX)
    {
        this->absPosition = absX;
        this->updateBounds();
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;


private:

    //[UserVariables]

    float absPosition;

    void updateBounds()
    {
//#if HELIO_DESKTOP
        this->setBounds(int(this->getParentWidth() * this->absPosition),
                        0,
                        int(this->getParentWidth() * (1.f - this->absPosition)),
                        this->getParentHeight());
//#elif HELIO_MOBILE
//        this->setBounds((this->getParentWidth() * this->absPosition),
//                        0,
//                        this->getWidth(),
//                        this->getParentHeight());
//#endif
    }

    //[/UserVariables]

    ScopedPointer<ShadowRightwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEndIndicator)
};
