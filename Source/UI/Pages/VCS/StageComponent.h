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

#include "Revision.h"
#include "RevisionItem.h"
#include "HeadlineItemDataSource.h"
#include "ComponentFader.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalFadingReversed.h"

class StageComponent final : public Component,
                             public ListBoxModel,
                             public ChangeListener,
                             public HeadlineItemDataSource
{
public:

    StageComponent(VersionControl &versionControl);
    ~StageComponent();

    //[UserMethods]

    void selectAll(NotificationType notificationType);
    void clearSelection();

    //===------------------------------------------------------------------===//
    // ChangeListener - listens VCS head to refresh diff info
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component *) override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}
    void selectedRowsChanged(int) override;

    //===------------------------------------------------------------------===//
    // HeadlineItemDataSource
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;
    Image getIcon() const override;
    String getName() const override;
    bool canBeSelectedAsMenuItem() const override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    VersionControl &vcs;

    ReadWriteLock diffLock;
    String lastCommitMessage;

    ReferenceCountedArray<VCS::RevisionItem> stageDeltas;

    ComponentFader fader;
    void startProgressAnimation();
    void stopProgressAnimation();

    void updateList();
    void clearList();

    void toggleQuickStash();
    void commitSelected();
    void resetSelected();

    //[/UserVariables]

    UniquePointer<Component> horizontalCenter;
    UniquePointer<Label> titleLabel;
    UniquePointer<ProgressIndicator> indicator;
    UniquePointer<ListBox> changesList;
    UniquePointer<SeparatorHorizontalFadingReversed> separator3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StageComponent)
};
