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

#include "TreeItem.h"

class ProjectTreeItem;
class MidiTrackTreeItem;

class TrackGroupTreeItem final : public TreeItem
{
public:

    explicit TrackGroupTreeItem(const String &name);

    static void removeAllEmptyGroupsInProject(ProjectTreeItem *project); // sanitize the tree
    
    void sortByNameAmongSiblings();

    Image getIcon() const noexcept override;

    void showPage() override;
    void safeRename(const String &newName, bool sendNotifications) override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override;
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;

};
