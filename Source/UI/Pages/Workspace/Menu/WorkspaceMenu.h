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
class Workspace;

#include "RecentFilesList.h"

#if HELIO_DESKTOP
#    define DEFAULT_RECENT_FILES_ROW_HEIGHT (56)
#elif HELIO_MOBILE
#    define DEFAULT_RECENT_FILES_ROW_HEIGHT (56)
#endif
//[/Headers]

#include "../../../Themes/ShadowHorizontalFading.h"
#include "../../../Themes/SeparatorHorizontalFadingReversed.h"
#include "../../../Themes/SeparatorHorizontalFading.h"

class WorkspaceMenu final : public Component,
                            public ListBoxModel
{
public:

    WorkspaceMenu(Workspace *parentWorkspace);
    ~WorkspaceMenu();

    //[UserMethods]

    void loadFile(RecentFileDescription::Ptr fileDescription);
    void unloadFile(RecentFileDescription::Ptr fileDescription);

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    Component *refreshComponentForRow(int, bool, Component *) override;
    void listBoxItemClicked(int row, const MouseEvent &e) override;
    int getNumRows() override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    Workspace *workspace;
    //[/UserVariables]

    ScopedPointer<ShadowHorizontalFading> component;
    ScopedPointer<ListBox> listBox;
    ScopedPointer<SeparatorHorizontalFadingReversed> separator1;
    ScopedPointer<SeparatorHorizontalFading> separator2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorkspaceMenu)
};
