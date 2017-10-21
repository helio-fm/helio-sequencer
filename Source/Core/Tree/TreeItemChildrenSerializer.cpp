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
#include "Serializable.h"
#include "TreeItem.h"
#include "ProjectTreeItem.h"
#include "TrackGroupTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "InstrumentsRootTreeItem.h"
#include "InstrumentTreeItem.h"
#include "VersionControlTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "SettingsTreeItem.h"

void TreeItemChildrenSerializer::serializeChildren(const TreeItem &parentItem, XmlElement &parentXml)
{
    for (int i = 0; i < parentItem.getNumSubItems(); ++i)
    {
        if (TreeViewItem *sub = parentItem.getSubItem(i))
        {
            TreeItem *treeItem = static_cast<TreeItem *>(sub);
            parentXml.addChildElement(treeItem->serialize());
        }
    }
}

void TreeItemChildrenSerializer::deserializeChildren(TreeItem &parentItem, const XmlElement &parentXml)
{
    forEachXmlChildElementWithTagName(parentXml, e, Serialization::Core::treeItem)
    {
        // Legacy support:
        const String typeFallback = 
            e->getStringAttribute(Serialization::Core::treeItemType.toLowerCase());

        const String type =
            e->getStringAttribute(Serialization::Core::treeItemType, typeFallback);

        TreeItem *child = nullptr;

        if (type == Serialization::Core::project)
        {
            child = new ProjectTreeItem("");
        }
        else if (type == Serialization::Core::settings)
        {
            child = new SettingsTreeItem();
        }
        else if (type == Serialization::Core::layerGroup)
        {
            child = new TrackGroupTreeItem("");
        }
        else if (type == Serialization::Core::pianoLayer)
        {
            child = new PianoTrackTreeItem( "");
        }
        else if (type == Serialization::Core::autoLayer)
        {
            child = new AutomationTrackTreeItem("");
        }
        else if (type == Serialization::Core::instrumentRoot)
        {
            child = new InstrumentsRootTreeItem();
        }
        else if (type == Serialization::Core::instrument)
        {
            child = new InstrumentTreeItem();
        }
        else if (type == Serialization::Core::versionControl)
        {
            child = new VersionControlTreeItem();
        }
        else if (type == Serialization::Core::patternSet)
        {
            child = new PatternEditorTreeItem();
        }

        if (child != nullptr)
        {
            parentItem.addChildTreeItem(child);
            child->deserialize(*e);
        }
    }

    // todo. загрузка долгая и стремная. рассылаются кучи событий,
    // перебилдится кэш миди кучу раз. чо за фигня, чувак.

    // and we need to go deeper.
}
