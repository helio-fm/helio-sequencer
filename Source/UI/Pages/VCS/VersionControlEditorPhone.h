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
#include "VersionControlEditor.h"
//[/Headers]

#include "../Themes/PanelBackgroundB.h"
#include "../Themes/LightShadowRightwards.h"

class VersionControlEditorPhone  : public VersionControlEditor
{
public:

    VersionControlEditorPhone (VersionControl &versionControl);

    ~VersionControlEditorPhone();

    //[UserMethods]

    void updateState() override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    ComponentAnimator animator;
    //[/UserVariables]

    ScopedPointer<PanelBackgroundB> background;
    ScopedPointer<LightShadowRightwards> shadow;
    ScopedPointer<Viewport> viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VersionControlEditorPhone)
};
