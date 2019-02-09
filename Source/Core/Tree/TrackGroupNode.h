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

#include "TreeNode.h"

class ProjectNode;
class MidiTrackNode;

class TrackGroupNode final : public TreeNode
{
public:

    explicit TrackGroupNode(const String &name);

    static void removeAllEmptyGroupsInProject(ProjectNode *project); // sanitize the tree
    
    void sortByNameAmongSiblings();

    Image getIcon() const noexcept override;

    void showPage() override;
    void safeRename(const String &newName, bool sendNotifications) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;

};
