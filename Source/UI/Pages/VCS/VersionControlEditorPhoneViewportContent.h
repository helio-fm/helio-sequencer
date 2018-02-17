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

#include "StageComponent.h"
#include "HistoryComponent.h"

class VersionControlEditorPhoneViewportContent  : public VersionControlEditor,
                                                  public Button::Listener
{
public:

    VersionControlEditorPhoneViewportContent (VersionControl &versionControl);

    ~VersionControlEditorPhoneViewportContent();

    //[UserMethods]

    void updateState() override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;


private:

    //[UserVariables]
    ComponentAnimator animator;
    //[/UserVariables]

    ScopedPointer<StageComponent> stageComponent;
    ScopedPointer<HistoryComponent> historyComponent;
    ScopedPointer<Component> anchor;
    ScopedPointer<TextButton> slideRightButton;
    ScopedPointer<TextButton> slideLeftButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VersionControlEditorPhoneViewportContent)
};
