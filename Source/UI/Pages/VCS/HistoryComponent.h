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
class RevisionTreeComponent;

#include "HeadlineItemDataSource.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalFadingReversed.h"

class HistoryComponent final : public Component,
                               public HeadlineItemDataSource
{
public:

    HistoryComponent(VersionControl &owner);
    ~HistoryComponent();

    //[UserMethods]

    void clearSelection();
    void rebuildRevisionTree();
    void onRevisionSelectionChanged();

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
    SafePointer<RevisionTreeComponent> revisionTree;
    //[/UserVariables]

    UniquePointer<Viewport> revisionViewport;
    UniquePointer<Label> revisionTreeLabel;
    UniquePointer<SeparatorHorizontalFadingReversed> separator3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HistoryComponent)
};
