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
#include "UserProfile.h"
//[/Headers]


class DashboardMenu final : public Component,
                            public ListBoxModel
{
public:

    DashboardMenu(Workspace *parentWorkspace);
    ~DashboardMenu();

    //[UserMethods]

    void updateListContent();
    void loadFile(RecentProjectInfo::Ptr fileDescription);
    void unloadFile(RecentProjectInfo::Ptr fileDescription);

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
    FlatHashSet<String, StringHash> loadedProjectIds;
    //[/UserVariables]

    UniquePointer<ListBox> listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DashboardMenu)
};
