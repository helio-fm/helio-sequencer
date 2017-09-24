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
#include "HighlightedComponent.h"
//[/Headers]

#include "../Themes/ShadeLight.h"
#include "RolloverBackButtonLeft.h"

class RolloverHeaderLeft  : public HighlightedComponent
{
public:

    RolloverHeaderLeft (const String &headerTitle);

    ~RolloverHeaderLeft();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;
    void mouseDown (const MouseEvent& e) override;


private:

    //[UserVariables]
    String title;

    Component *createHighlighterComponent() override
    {
        return new RolloverHeaderLeft(this->title);
    }
    //[/UserVariables]

    ScopedPointer<ShadeLight> shade;
    ScopedPointer<Label> titleLabel;
    ScopedPointer<Label> detailsLabel;
    ScopedPointer<RolloverBackButtonLeft> backButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolloverHeaderLeft)
};
