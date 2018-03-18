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
class LogoFader;
//[/Headers]

#include "../../Themes/PanelBackgroundA.h"
#include "../../Themes/SeparatorHorizontalReversed.h"
#include "../../Themes/LighterShadowDownwards.h"
#include "../../Themes/PanelBackgroundB.h"

class InitScreen final : public Component,
                         private Timer
{
public:

    InitScreen();
    ~InitScreen();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    void timerCallback() override;
    void rebound();

    //[/UserVariables]

    ScopedPointer<PanelBackgroundA> bg;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<PanelBackgroundB> gradient1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InitScreen)
};
