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

class VersionControl;
class VersionControlEditor;
class ProjectTreeItem;

class VersionControlTreeItem :
    public TreeItem,
    public ChangeListener // listens to VCS::Remote when it is done pulling
{
public:

    VersionControlTreeItem(String id = {}, String key = {});
    ~VersionControlTreeItem() override;

    String getName() const override;
    Colour getColour() const override;
    Image getIcon() const override;
    void showPage() override;
    void recreatePage() override;
    
    String getId() const;
    String getStatsString() const;
    
    void commitProjectInfo();
    void asyncPullAndCheckoutOrDeleteIfFailed();
    void changeListenerCallback(ChangeBroadcaster* source) override;

    bool deletePermanentlyFromRemoteRepo();
    void toggleQuickStash();
    
    
    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void onItemParentChanged() override;
    var getDragSourceDescription() override { return var::null; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override
    { return false; }


    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    ScopedPointer<Component> createItemMenu() override;


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

protected:

    ScopedPointer<VersionControl> vcs;
    ScopedPointer<VersionControlEditor> editor;

private:

    String existingId;
    String existingKey;
    
    void initVCS();
    void shutdownVCS();
    
    void initEditor();
    void shutdownEditor();

};
