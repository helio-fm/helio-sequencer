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
class ProgressIndicator;

#include "ComponentFader.h"
#include "Revision.h"
//[/Headers]

#include "../Themes/LightShadowDownwards.h"
#include "../Themes/FramePanel.h"

class StageComponent  : public Component,
                        public ListBoxModel,
                        public ChangeListener,
                        public ButtonListener
{
public:

    StageComponent (VersionControl &versionControl);

    ~StageComponent();

    //[UserMethods]

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    // listens head to refresh diff info
    void changeListenerCallback(ChangeBroadcaster* source) override;


    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    Component *refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate) override;

    void listBoxItemClicked(int row, const MouseEvent &e) override;

    void listBoxItemDoubleClicked(int row, const MouseEvent &e) override;

    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics &g,
        int width, int height, bool rowIsSelected) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    VersionControl &vcs;

    ReadWriteLock diffLock;
    ValueTree lastDiff;
    String commitMessage;

    ComponentFader fader;
    void startProgressAnimation();
    void stopProgressAnimation();

    void updateList();
    void clearList();

    void updateToggleButton();
    void toggleButtonAction();

    //[/UserVariables]

    ScopedPointer<TextButton> toggleChangesButton;
    ScopedPointer<Component> horizontalCenter;
    ScopedPointer<TextButton> commitButton;
    ScopedPointer<TextButton> resetButton;
    ScopedPointer<Label> titleLabel;
    ScopedPointer<LightShadowDownwards> shadow;
    ScopedPointer<ProgressIndicator> indicator;
    ScopedPointer<FramePanel> panel;
    ScopedPointer<ListBox> changesList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StageComponent)
};
