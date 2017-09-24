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
#include "IconComponent.h"
#include "HighlightedComponent.h"
#include "ComponentFader.h"
//[/Headers]

#include "../Themes/ShadeLight.h"

class RootTreeItemPanelCompact  : public Component
{
public:

    RootTreeItemPanelCompact ();

    ~RootTreeItemPanelCompact();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;


private:

    //[UserVariables]
    ComponentFader fader;
    //[/UserVariables]

    ScopedPointer<ShadeLight> shade;
    ScopedPointer<IconComponent> workspaceIcon;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RootTreeItemPanelCompact)
};
