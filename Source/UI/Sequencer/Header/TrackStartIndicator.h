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
//[/Headers]

#include "../../Themes/ShadowLeftwards.h"

class TrackStartIndicator final : public Component
{
public:

    TrackStartIndicator();
    ~TrackStartIndicator();

    //[UserMethods]

    double getAnchor() const
    {
        return this->absPosition;
    }

    inline void setAnchoredAt(double absX)
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

    double absPosition;

    void updateBounds()
    {
        this->setBounds(0, 0,
            int(double(this->getParentWidth()) * this->absPosition + 1.0),
            this->getParentHeight());
    }

    //[/UserVariables]

    ScopedPointer<ShadowLeftwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackStartIndicator)
};
