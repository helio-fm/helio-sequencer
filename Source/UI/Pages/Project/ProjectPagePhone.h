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
#include "ProjectPage.h"
//[/Headers]

#include "../../Themes/PanelBackgroundB.h"
#include "../../Themes/ShadowRightwards.h"
#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFading.h"

class ProjectPagePhone  : public ProjectPage,
                          public Label::Listener
{
public:

    ProjectPagePhone (ProjectTreeItem &parentProject);

    ~ProjectPagePhone();

    //[UserMethods]
    void updateContent() override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    //===----------------------------------------------------------------------===//
    // TransportListener
    //

    void onSeek(double absolutePosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override {}
    void onStop() override {}

    //[/UserVariables]

    ScopedPointer<PanelBackgroundB> background;
    ScopedPointer<Label> projectTitleEditor;
    ScopedPointer<Label> projectTitleLabel;
    ScopedPointer<Label> authorEditor;
    ScopedPointer<Label> authorLabel;
    ScopedPointer<Label> descriptionEditor;
    ScopedPointer<Label> descriptionLabel;
    ScopedPointer<Label> contentStatsLabel;
    ScopedPointer<Label> contentStatsText;
    ScopedPointer<Label> startTimeLabel;
    ScopedPointer<Label> startTimeText;
    ScopedPointer<Label> lengthLabel;
    ScopedPointer<Label> lengthText;
    ScopedPointer<Component> level1;
    ScopedPointer<Component> level2;
    ScopedPointer<Label> licenseLabel;
    ScopedPointer<Label> licenseEditor;
    ScopedPointer<ShadowRightwards> shadow;
    ScopedPointer<SeparatorHorizontalFading> separator1;
    ScopedPointer<SeparatorHorizontalFading> separator2;
    ScopedPointer<SeparatorHorizontalFading> separator3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProjectPagePhone)
};
