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

#include "../Themes/PanelBackgroundC.h"
#include "../Themes/SeparatorHorizontalReversed.h"
#include "../Themes/LighterShadowDownwards.h"
#include "../Themes/GradientVerticalReversed.h"

class RolloverContainer  : public Component
{
public:

    RolloverContainer (Component *newHeaderComponent, Component *newContentComponent);

    ~RolloverContainer();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> background;
    ScopedPointer<Component> rolloverContent;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<GradientVerticalReversed> gradient1;
    ScopedPointer<Component> rolloverHeader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RolloverContainer)
};
