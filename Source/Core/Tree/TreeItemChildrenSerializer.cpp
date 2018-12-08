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
#include "TreeItemChildrenSerializer.h"
#include "TreeItem.h"
#include "ProjectTreeItem.h"
#include "TrackGroupTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "OrchestraPitTreeItem.h"
#include "InstrumentTreeItem.h"
#include "VersionControlTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "SettingsTreeItem.h"

void TreeItemChildrenSerializer::serializeChildren(const TreeItem &parentItem, ValueTree &parent)
{
    for (int i = 0; i < parentItem.getNumSubItems(); ++i)
    {
        if (TreeViewItem *sub = parentItem.getSubItem(i))
        {
            TreeItem *treeItem = static_cast<TreeItem *>(sub);
            parent.appendChild(treeItem->serialize(), nullptr);
        }
    }
}

void TreeItemChildrenSerializer::deserializeChildren(TreeItem &parentItem, const ValueTree &parent)
{
    using namespace Serialization;

    forEachValueTreeChildWithType(parent, e, Core::treeItem)
    {
        const auto type = Identifier(e.getProperty(Core::treeItemType));

        TreeItem *child = nullptr;

        if (type == Core::project)              { child = new ProjectTreeItem(); }
        else if (type == Core::settings)        { child = new SettingsTreeItem(); }
        else if (type == Core::trackGroup)      { child = new TrackGroupTreeItem(""); }
        else if (type == Core::pianoTrack)      { child = new PianoTrackTreeItem(""); }
        else if (type == Core::automationTrack) { child = new AutomationTrackTreeItem(""); }
        else if (type == Core::instrumentsList) { child = new OrchestraPitTreeItem(); }
        else if (type == Core::instrumentRoot)  { child = new InstrumentTreeItem(); }
        else if (type == Core::versionControl)  { child = new VersionControlTreeItem(); }
        else if (type == Core::patternSet)      { child = new PatternEditorTreeItem(); }

        if (child != nullptr)
        {
            parentItem.addChildTreeItem(child);
            child->deserialize(e);
        }
    }
}
