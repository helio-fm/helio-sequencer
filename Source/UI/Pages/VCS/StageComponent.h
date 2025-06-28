/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class VersionControl;

#include "Revision.h"
#include "RevisionItem.h"
#include "HeadlineItemDataSource.h"
#include "SeparatorHorizontalFadingReversed.h"

class StageComponent final : public Component,
    public ListBoxModel,
    public HeadlineItemDataSource
{
public:

    explicit StageComponent(VersionControl &versionControl);
    ~StageComponent();

    void selectAll(NotificationType notificationType);
    void clearSelection();

    void updateList();

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
    UniquePointer<Component> createMenu() override;
    Image getIcon() const override;
    String getName() const override;
    bool canBeSelectedAsMenuItem() const override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void handleCommandMessage(int commandId) override;

private:

    VersionControl &vcs;

    ReadWriteLock diffLock;
    String lastCommitMessage;

    ReferenceCountedArray<VCS::RevisionItem> stageDeltas;

    void toggleQuickStash();
    void commitSelected();
    void resetSelected();

    UniquePointer<Label> titleLabel;
    UniquePointer<ListBox> changesList;
    UniquePointer<SeparatorHorizontalFadingReversed> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StageComponent)
};
