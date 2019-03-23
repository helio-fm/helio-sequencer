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
#include "TreeNodeSerializer.h"
#include "TreeNode.h"
#include "ProjectNode.h"
#include "TrackGroupNode.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "OrchestraPitNode.h"
#include "InstrumentNode.h"
#include "VersionControlNode.h"
#include "PatternEditorNode.h"
#include "SettingsNode.h"

void TreeNodeSerializer::serializeChildren(const TreeNode &parentItem, ValueTree &parent)
{
    for (int i = 0; i < parentItem.getNumChildren(); ++i)
    {
        if (auto *sub = parentItem.getChild(i))
        {
            TreeNode *treeItem = static_cast<TreeNode *>(sub);
            parent.appendChild(treeItem->serialize(), nullptr);
        }
    }
}

void TreeNodeSerializer::deserializeChildren(TreeNode &parentItem, const ValueTree &parent)
{
    using namespace Serialization;

    forEachValueTreeChildWithType(parent, e, Core::treeNode)
    {
        const auto type = Identifier(e.getProperty(Core::treeNodeType));

        TreeNode *child = nullptr;

        if (type == Core::project)              { child = new ProjectNode(); }
        else if (type == Core::settings)        { child = new SettingsNode(); }
        else if (type == Core::trackGroup)      { child = new TrackGroupNode(""); }
        else if (type == Core::pianoTrack)      { child = new PianoTrackNode(""); }
        else if (type == Core::automationTrack) { child = new AutomationTrackNode(""); }
        else if (type == Core::instrumentsList) { child = new OrchestraPitNode(); }
        else if (type == Core::instrumentRoot)  { child = new InstrumentNode(); }
        else if (type == Core::versionControl)  { child = new VersionControlNode(); }
        else if (type == Core::patternSet)      { child = new PatternEditorNode(); }

        if (child != nullptr)
        {
            parentItem.addChildTreeItem(child);
            child->deserialize(e);
        }
    }
}
