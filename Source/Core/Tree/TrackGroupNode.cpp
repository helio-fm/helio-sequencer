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

#include "Common.h"
#include "TrackGroupNode.h"
#include "ProjectNode.h"

#include "TreeNodeSerializer.h"
#include "PianoTrackNode.h"
#include "MainLayout.h"
#include "Icons.h"

#define GROUP_COMPACT_SEPARATOR 5

TrackGroupNode::TrackGroupNode(const String &name) :
    TreeNode(name, Serialization::Core::trackGroup) {}

Image TrackGroupNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::trackGroup, HEADLINE_ICON_SIZE);
}

void TrackGroupNode::removeAllEmptyGroupsInProject(ProjectNode *project)
{
    if (project != nullptr)
    {
        int numGroupsDeleted = 0;
        
        do
        {
            Array<TrackGroupNode *> groupsToDelete;
            Array<TrackGroupNode *> groups(project->findChildrenOfType<TrackGroupNode>());
            
            for (int i = 0; i < groups.size(); ++i)
            {
                TrackGroupNode *group = groups.getUnchecked(i);
                Array<TreeNode *> groupChildren(group->findChildrenOfType<TreeNode>());
                
                if (groupChildren.size() == 0)
                {
                    groupsToDelete.add(group);
                }
            }
            
            numGroupsDeleted = groupsToDelete.size();
            
            for (int i = 0; i < groupsToDelete.size(); ++i)
            {
                TreeNode::deleteItem(groupsToDelete.getUnchecked(i), true);
            }
        }
        while (numGroupsDeleted > 0);
    }
}

void TrackGroupNode::sortByNameAmongSiblings()
{
    if (TreeNode *parentItem = dynamic_cast<TreeNode *>(this->getParent()))
    {
        parentItem->removeChild(this->getIndexInParent(), false);
        
        // пройти по всем чайлдам группы
        // и вставить в нужное место, глядя на имена.
        
        bool foundRightPlace = false;
        int insertIndex = 0;
        String previousChildName = "";
        
        for (int i = 0; i < parentItem->getNumChildren(); ++i)
        {
            String currentChildName;
            
            if (TrackGroupNode *layerGroupItem = dynamic_cast<TrackGroupNode *>(parentItem->getChild(i)))
            {
                currentChildName = layerGroupItem->getName();
            }
            else if (MidiTrackNode *layerItem = dynamic_cast<MidiTrackNode *>(parentItem->getChild(i)))
            {
                currentChildName = layerItem->getName();
            }
            else
            {
                continue;
            }
            
            insertIndex = i;
            
            if ((this->getName().compareIgnoreCase(previousChildName) > 0) &&
                (this->getName().compareIgnoreCase(currentChildName) <= 0))
            {
                foundRightPlace = true;
                break;
            }
            
            previousChildName = currentChildName;
        }

        if (!foundRightPlace) { ++insertIndex; }
        
        parentItem->addChildTreeItem(this, insertIndex);
    }
}

// Deprecated, we no longer use tree view and multi-track selection
static void applySelectionPolicyForGroup(TrackGroupNode *rootNode, bool forceSelectAll = false)
{
    const bool shiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    bool shouldSelectAll = shiftPressed;

    if (! rootNode->haveAllChildrenSelectedWithDeepSearch())
    {
        shouldSelectAll = true;
    }
    
    // ищем активный слой по всему проекту
    MidiTrackNode *activeItem = TreeNode::getActiveItem<MidiTrackNode>(rootNode->getRootTreeItem());
    
    if (shouldSelectAll || forceSelectAll)
    {
        rootNode->setSelected(true, false, dontSendNotification);

        for (signed int i = (rootNode->getNumChildren() - 1); i >= 0 ; --i)
        {
            if (MidiTrackNode *layerItem = dynamic_cast<MidiTrackNode *>(rootNode->getChild(i)))
            {
                rootNode->getChild(i)->setSelected(true, false, sendNotification);
            }
            else if (TrackGroupNode *group = dynamic_cast<TrackGroupNode *>(rootNode->getChild(i)))
            {
                applySelectionPolicyForGroup(group, true);
            }
        }
        
        // это нужно, чтоб сохранить активный слой активным
        // (если нажат шифт или выделен кто-то из его соседей)
        if (activeItem)
        {
            if (shiftPressed || activeItem->getNumSelectedSiblings() > 1)
            {
                activeItem->setSelected(false, false, sendNotification);
                activeItem->setSelected(true, false, sendNotification);
            }
        }
    }
    else
    {
        // если в группе есть активный слой, выделяем его, снимаем выделение с остального
        // todo? если в группе есть активный слой, но, кроме этой группы, есть еще то-то выделенное,
        // ставим активным один из тех выделенных элементов, а с группы все снимаем?
        
        bool hasActiveChild = false;
        
        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            if (MidiTrackNode *layerItem = dynamic_cast<MidiTrackNode *>(rootNode->getChild(i)))
            {
                if (layerItem->isPrimarySelection())
                {
                    layerItem->setSelected(false, false, sendNotification);
                    layerItem->setSelected(true, true, sendNotification);
                    hasActiveChild = true;
                    break;
                }
            }
        }
        
        // если активного слоя в этой группе не нашлось - просто снимаем выделение со всех слоев
        if (activeItem)
        {
            if (! hasActiveChild)
            {
                rootNode->setSelected(false, false, sendNotification);
                
                for (int i = 0; i < rootNode->getNumChildren(); ++i)
                {
                    if (MidiTrackNode *layerItem = dynamic_cast<MidiTrackNode *>(rootNode->getChild(i)))
                    {
                        rootNode->getChild(i)->setSelected(false, false, sendNotification);
                    }
                    else if (TrackGroupNode *group = dynamic_cast<TrackGroupNode *>(rootNode->getChild(i)))
                    {
                        applySelectionPolicyForGroup(group);
                    }
                }
            }
            
            activeItem->setSelected(false, false, sendNotification);
            activeItem->setSelected(true, false, sendNotification);
        }
    }
}

void TrackGroupNode::showPage()
{
    applySelectionPolicyForGroup(this);
    
    Array<MidiTrackNode *> subTracks(this->findChildrenOfType<MidiTrackNode>());
    if (! subTracks.isEmpty())
    {
        if (ProjectNode *parentProject = this->findParentOfType<ProjectNode>())
        {
            parentProject->showLinearEditor(subTracks.getFirst(), this);
        }
    }
}

void TrackGroupNode::safeRename(const String &newName, bool sendNotifications)
{
    TreeNode::safeRename(newName, sendNotifications);
    this->sortByNameAmongSiblings();
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool TrackGroupNode::hasMenu() const noexcept
{
    return false;
}

ScopedPointer<Component> TrackGroupNode::createMenu()
{
    return nullptr;
}
