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
#include "TrackGroupTreeItem.h"
#include "ProjectTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "PianoTrackTreeItem.h"
#include "MainLayout.h"
#include "Icons.h"

#define GROUP_COMPACT_SEPARATOR 5

TrackGroupTreeItem::TrackGroupTreeItem(const String &name) :
    TreeItem(name, Serialization::Core::trackGroup) {}

Colour TrackGroupTreeItem::getColour() const noexcept
{
    return Colour(255, 210, 255);
}

Image TrackGroupTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::group, HEADLINE_ICON_SIZE);
}

void TrackGroupTreeItem::removeAllEmptyGroupsInProject(ProjectTreeItem *project)
{
    if (project != nullptr)
    {
        int numGroupsDeleted = 0;
        
        do
        {
            Array<TrackGroupTreeItem *> groupsToDelete;
            Array<TrackGroupTreeItem *> groups(project->findChildrenOfType<TrackGroupTreeItem>());
            
            for (int i = 0; i < groups.size(); ++i)
            {
                TrackGroupTreeItem *group = groups.getUnchecked(i);
                Array<TreeItem *> groupChildren(group->findChildrenOfType<TreeItem>());
                
                if (groupChildren.size() == 0)
                {
                    groupsToDelete.add(group);
                }
            }
            
            numGroupsDeleted = groupsToDelete.size();
            
            for (int i = 0; i < groupsToDelete.size(); ++i)
            {
                TreeItem::deleteItem(groupsToDelete.getUnchecked(i));
            }
        }
        while (numGroupsDeleted > 0);
    }
}

void TrackGroupTreeItem::sortByNameAmongSiblings()
{
    if (TreeItem *parentItem = dynamic_cast<TreeItem *>(this->getParentItem()))
    {
        parentItem->removeSubItem(this->getIndexInParent(), false);
        
        // пройти по всем чайлдам группы
        // и вставить в нужное место, глядя на имена.
        
        bool foundRightPlace = false;
        int insertIndex = 0;
        String previousChildName = "";
        
        for (int i = 0; i < parentItem->getNumSubItems(); ++i)
        {
            String currentChildName;
            
            if (TrackGroupTreeItem *layerGroupItem = dynamic_cast<TrackGroupTreeItem *>(parentItem->getSubItem(i)))
            {
                currentChildName = layerGroupItem->getName();
            }
            else if (MidiTrackTreeItem *layerItem = dynamic_cast<MidiTrackTreeItem *>(parentItem->getSubItem(i)))
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

static void applySelectionPolicyForGroup(TrackGroupTreeItem *rootNode, bool forceSelectAll = false)
{
    const bool shiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    bool shouldSelectAll = shiftPressed;

    if (! rootNode->haveAllChildrenSelectedWithDeepSearch())
    {
        shouldSelectAll = true;
    }
    
    // ищем активный слой по всему проекту
    MidiTrackTreeItem *activeItem = TreeItem::getActiveItem<MidiTrackTreeItem>(rootNode->getRootTreeItem());
    
    if (shouldSelectAll || forceSelectAll)
    {
        //Logger::writeToLog(rootNode->getName() + " shouldSelectAll");
        
        rootNode->setSelected(true, false, dontSendNotification);

        for (signed int i = (rootNode->getNumSubItems() - 1); i >= 0 ; --i)
        {
            if (MidiTrackTreeItem *layerItem = dynamic_cast<MidiTrackTreeItem *>(rootNode->getSubItem(i)))
            {
                rootNode->getSubItem(i)->setSelected(true, false, sendNotification);
            }
            else if (TrackGroupTreeItem *group = dynamic_cast<TrackGroupTreeItem *>(rootNode->getSubItem(i)))
            {
                applySelectionPolicyForGroup(group, true);
            }
        }
        
        // это нужно, чтоб сохранить активный слой активным
        // (если нажат шифт или выделен кто-то из его соседей)
        if (activeItem)
        {
            //Logger::writeToLog(String(activeItem->getNumSelectedSiblings()));
            
            if (shiftPressed || activeItem->getNumSelectedSiblings() > 1)
            {
                activeItem->setSelected(false, false, sendNotification);
                activeItem->setSelected(true, false, sendNotification);
            }
        }
    }
    else
    {
        //Logger::writeToLog(rootNode->getName() + " ! shouldSelectAll");
        
        // если в группе есть активный слой, выделяем его, снимаем выделение с остального
        // todo? если в группе есть активный слой, но, кроме этой группы, есть еще то-то выделенное,
        // ставим активным один из тех выделенных элементов, а с группы все снимаем?
        
        bool hasActiveChild = false;
        
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            if (MidiTrackTreeItem *layerItem = dynamic_cast<MidiTrackTreeItem *>(rootNode->getSubItem(i)))
            {
                if (layerItem->isMarkerVisible())
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
                
                for (int i = 0; i < rootNode->getNumSubItems(); ++i)
                {
                    if (MidiTrackTreeItem *layerItem = dynamic_cast<MidiTrackTreeItem *>(rootNode->getSubItem(i)))
                    {
                        rootNode->getSubItem(i)->setSelected(false, false, sendNotification);
                    }
                    else if (TrackGroupTreeItem *group = dynamic_cast<TrackGroupTreeItem *>(rootNode->getSubItem(i)))
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

void TrackGroupTreeItem::showPage()
{
    applySelectionPolicyForGroup(this);
    
    Array<MidiTrackTreeItem *> subTracks(this->findChildrenOfType<MidiTrackTreeItem>());
    if (! subTracks.isEmpty())
    {
        if (ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>())
        {
            parentProject->showLinearEditor(subTracks.getFirst(), this);
        }
    }
}

void TrackGroupTreeItem::safeRename(const String &newName)
{
    TreeItem::safeRename(newName);
    this->sortByNameAmongSiblings();
    this->dispatchChangeTreeItemView();
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool TrackGroupTreeItem::hasMenu() const noexcept
{
    return false;
}

ScopedPointer<Component> TrackGroupTreeItem::createMenu()
{
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

var TrackGroupTreeItem::getDragSourceDescription()
{
    return Serialization::Core::trackGroup.toString();
}

bool TrackGroupTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    if (TreeView *treeView = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    {
        TreeItem *selected = TreeItem::getSelectedItem(treeView);

        if (TreeItem::isNodeInChildren(selected, this))
        { return false; }

        return (dragSourceDetails.description == Serialization::Core::track.toString()) ||
               (dragSourceDetails.description == Serialization::Core::trackGroup.toString());
    }

    return false;
}
