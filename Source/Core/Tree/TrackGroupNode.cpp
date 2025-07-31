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

#include "Common.h"
#include "TrackGroupNode.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "Pattern.h"
#include "Icons.h"

TrackGroupNode::TrackGroupNode(const String &name) :
    TreeNode(name, Serialization::Core::trackGroup) {}

Image TrackGroupNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::pianoTrack, Globals::UI::headlineIconSize);
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
                TreeNode::deleteNode(groupsToDelete.getUnchecked(i), true);
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
            
            if (auto *layerGroupItem = dynamic_cast<TrackGroupNode *>(parentItem->getChild(i)))
            {
                currentChildName = layerGroupItem->getName();
            }
            else if (auto *layerItem = dynamic_cast<MidiTrackNode *>(parentItem->getChild(i)))
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
        
        parentItem->addChildNode(this, insertIndex);
    }
}

void TrackGroupNode::showPage()
{
    Array<MidiTrackNode *> subTracks(this->findChildrenOfType<MidiTrackNode>());
    if (!subTracks.isEmpty())
    {
        if (auto *parentProject = this->findParentOfType<ProjectNode>())
        {
            auto *firstClip = subTracks.getFirst()->getPattern()->getClips().getFirst();
            parentProject->showLinearEditor(*firstClip, this);
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

UniquePointer<Component> TrackGroupNode::createMenu()
{
    return nullptr;
}
