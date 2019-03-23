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
class ProjectNode;

#include "TransportListener.h"
//[/Headers]

#include "../../Themes/PanelBackgroundB.h"
#include "../../Common/MenuButton.h"

class ProjectPage final : public Component,
                          protected TransportListener,
                          protected ChangeListener,
                          public Label::Listener,
                          public Button::Listener
{
public:

    ProjectPage(ProjectNode &parentProject);
    ~ProjectPage();

    //[UserMethods]
    void updateContent();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void labelTextChanged(Label* labelThatHasChanged) override;
    void buttonClicked(Button* buttonThatWasClicked) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    ProjectNode &project;
    MidiKeyboardState state;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===----------------------------------------------------------------------===//
    // TransportListener
    //===----------------------------------------------------------------------===//

    void onSeek(double absolutePosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override {}
    void onStop() override {}

    //[/UserVariables]

    UniquePointer<PanelBackgroundB> background;
    UniquePointer<Label> projectTitleEditor;
    UniquePointer<Label> projectTitleLabel;
    UniquePointer<Label> authorEditor;
    UniquePointer<Label> authorLabel;
    UniquePointer<Label> descriptionEditor;
    UniquePointer<Label> descriptionLabel;
    UniquePointer<Label> locationLabel;
    UniquePointer<Label> locationText;
    UniquePointer<Label> contentStatsLabel;
    UniquePointer<Label> contentStatsText;
    UniquePointer<Label> vcsStatsLabel;
    UniquePointer<Label> vcsStatsText;
    UniquePointer<Label> startTimeLabel;
    UniquePointer<Label> startTimeText;
    UniquePointer<Label> lengthLabel;
    UniquePointer<Label> lengthText;
    UniquePointer<Component> level1;
    UniquePointer<Component> level2;
    UniquePointer<Label> licenseLabel;
    UniquePointer<Label> licenseEditor;
    UniquePointer<MenuButton> menuButton;
    UniquePointer<ImageButton> revealLocationButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectPage)
};
