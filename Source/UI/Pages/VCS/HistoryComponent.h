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
class VersionControl;
class PushComponent;
class PullComponent;

#include "Client.h"
//[/Headers]

#include "../../Themes/FramePanel.h"
#include "../../Themes/LightShadowDownwards.h"

class HistoryComponent  : public Component,
                          public Button::Listener
{
public:

    HistoryComponent (VersionControl &owner);

    ~HistoryComponent();

    //[UserMethods]
    void rebuildRevisionTree();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    VersionControl &vcs;

    SafePointer<PushComponent> pushComponent;
    SafePointer<PullComponent> pullComponent;

    //[/UserVariables]

    ScopedPointer<FramePanel> panel;
    ScopedPointer<Viewport> revisionViewport;
    ScopedPointer<TextButton> pushButton;
    ScopedPointer<TextButton> pullButton;
    ScopedPointer<Label> revisionTreeLabel;
    ScopedPointer<LightShadowDownwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HistoryComponent)
};
