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
#include "TreeItem.h"
#include "TreeItemComponentDefault.h"
#include "TreeItemChildrenSerializer.h"
#include "SerializationKeys.h"
#include "App.h"
#include "MainLayout.h"
#include "HelioCallout.h"

const String TreeItem::xPathSeparator = "/";

String TreeItem::createSafeName(const String &name)
{
    return File::createLegalFileName(name).removeCharacters(TreeItem::xPathSeparator);
}

TreeItem::TreeItem(const String &name, const Identifier &type) :
    name(TreeItem::createSafeName(name)),
    type(type.toString()),
    markerIsVisible(false),
    itemShouldBeVisible(true)
{

//#if HELIO_DESKTOP
    this->setLinesDrawnForSubItems(false);
//#elif HELIO_MOBILE
//    this->setLinesDrawnForSubItems(true);
//#endif

    this->setDrawsInLeftMargin(true);
}

TreeItem::~TreeItem()
{
    this->removeAllChangeListeners();
    this->deleteAllSubItems();
    this->removeItemFromParent();
    this->masterReference.clear();
}

void TreeItem::setMarkerVisible(bool shouldBeVisible) noexcept
{
    this->markerIsVisible = shouldBeVisible;
}

bool TreeItem::isMarkerVisible() const noexcept
{
    return this->markerIsVisible;
}

int TreeItem::getNumSelectedSiblings() const
{
    if (TreeItem *parent = dynamic_cast<TreeItem *>(this->getParentItem()))
    {
        return parent->getNumSelectedChildren();
    }

    return 0;
}

int TreeItem::getNumSelectedChildren() const
{
    int res = 0;

    for (int i = 0; i < this->getNumSubItems(); ++i)
    {
        res += (this->getSubItem(i)->isSelected() ? 1 : 0);
    }

    return res;
}

bool TreeItem::haveAllSiblingsSelected() const
{
    if (TreeItem *parent = dynamic_cast<TreeItem *>(this->getParentItem()))
    {
        if (parent->haveAllChildrenSelected())
        {
            return true;
        }
    }

    return false;
}

bool TreeItem::haveAllChildrenSelected() const
{
    for (int i = 0; i < this->getNumSubItems(); ++i)
    {
        if (! this->getSubItem(i)->isSelected())
        {
            return false;
        }
    }

    return true;
}

bool TreeItem::haveAllChildrenSelectedWithDeepSearch() const
{
    Array<TreeItem *> allChildren(this->findChildrenOfType<TreeItem>());

    for (int i = 0; i < allChildren.size(); ++i)
    {
        if (! allChildren.getUnchecked(i)->isSelected())
        {
            return false;
        }
    }

    return true;
}


//===----------------------------------------------------------------------===//
// Rename
//===----------------------------------------------------------------------===//

void TreeItem::safeRename(const String &newName)
{
    this->name = TreeItem::createSafeName(newName);
}

String TreeItem::getName() const
{
    return this->name;
}


//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

void TreeItem::itemSelectionChanged(bool isNowSelected)
{
    if (isNowSelected)
    {
        this->showPage();
    }
}

//===----------------------------------------------------------------------===//
// Cleanup
//===----------------------------------------------------------------------===//

bool TreeItem::deleteItem(TreeItem *itemToDelete)
{
    if (!itemToDelete) { return false; }

    if (itemToDelete->getRootTreeItem() != itemToDelete) // не удалять рут
    {
        WeakReference<TreeItem> switchTo = nullptr;
        WeakReference<TreeItem> parent = dynamic_cast<TreeItem *>(itemToDelete->getParentItem());

        const TreeItem *const markerItem = TreeItem::getActiveItem<TreeItem>(itemToDelete->getRootTreeItem());
        const bool markerItemIsDeleted = (markerItem == nullptr);

        if (itemToDelete->isMarkerVisible() || markerItemIsDeleted)
        {
            switchTo = parent;
        }

        delete itemToDelete;

        if (switchTo != nullptr)
        {
            switchTo->setSelected(true, true);
        }
        else if (parent != nullptr)
        {
            while (parent)
            {
                if (parent->isMarkerVisible())
                {
                    parent->setSelected(true, true);
                    break;
                }

                parent = dynamic_cast<TreeItem *>(parent->getParentItem());
            }
        }

        return true;
    }

    return false;
}

void TreeItem::deleteAllSelectedItems(Component *componentInTree)
{
    TreeItem *itemToDelete = TreeItem::getSelectedItem(componentInTree);
    TreeItem::deleteItem(itemToDelete);
}

void TreeItem::deleteAllSubItems()
{
    while (this->getNumSubItems() > 0)
    {
        delete this->getSubItem(0); // сам себя удалит из иерархии
    }
}

void TreeItem::removeItemFromParent()
{
    if (TreeItem *parent = dynamic_cast<TreeItem *>(this->getParentItem()))
    {
        for (int i = 0; i < parent->getNumSubItems(); ++i)
        {
            if (TreeItem *item = dynamic_cast<TreeItem *>(parent->getSubItem(i)))
            {
                if (item == this)
                {
                    parent->removeSubItem(i, false);
                }
            }
        }
    }
}

static void notifySubtreeParentChanged(TreeItem *node)
{
    node->onItemParentChanged();

    for (int i = 0; i < node->getNumSubItems(); ++i)
    {
        TreeItem *child = static_cast<TreeItem *>(node->getSubItem(i));
        notifySubtreeParentChanged(child);
    }
}

void TreeItem::addChildTreeItem(TreeItem *child, int insertIndex /*= -1*/)
{
    this->addSubItem(child, insertIndex);
    notifySubtreeParentChanged(child);
}

void TreeItem::dispatchChangeTreeItemView()
{
    this->sendChangeMessage(); // update listeners
    this->treeHasChanged(); // updates ownerView, if any
}

//===----------------------------------------------------------------------===//
// Painting
//===----------------------------------------------------------------------===//

Component *TreeItem::createItemComponent()
{
    if (! this->itemShouldBeVisible)
    {
        return nullptr;
    }
    
    return new TreeItemComponentDefault(*this);
}

void TreeItem::paintItem(Graphics &g, int width, int height)
{
    TreeItemComponentDefault::paintBackground(g, width, height, this->isSelected(), this->isMarkerVisible());
}

void TreeItem::paintOpenCloseButton(Graphics &g, const Rectangle<float> &area,
                                    Colour backgroundColour, bool isMouseOver)
{
    //if (this->getNumSubItems() == 0) { return; }

//#if HELIO_MOBILE
//    return;
//#endif

    Path p;
    p.addTriangle(0.0f, 0.0f, 1.0f, this->isOpen() ? 0.0f : 0.5f, this->isOpen() ? 0.5f : 0.0f, 1.0f);

    g.setColour(backgroundColour.contrasting().withAlpha(isMouseOver ? 0.7f : 0.4f));
    g.fillPath(p, p.getTransformToScaleToFit(area.reduced(1, area.getHeight() / 8), true));
}

void TreeItem::paintHorizontalConnectingLine(Graphics &g, const Line<float> &line)
{
}

void TreeItem::paintVerticalConnectingLine(Graphics &g, const Line<float> &line)
{
}

int TreeItem::getItemHeight() const
{
    if (! this->itemShouldBeVisible)
    {
        return 0;
    }

    return TREE_ITEM_HEIGHT;
}

Font TreeItem::getFont() const
{
    return Font(Font::getDefaultSansSerifFontName(), TREE_FONT_SIZE, Font::plain); //this->getItemHeight() * TREE_FONT_HEIGHT_PROPORTION, Font::plain);
}

Colour TreeItem::getColour() const
{
    return Colour(100, 150, 200);
    //return this->colour; // todo
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void TreeItem::reset()
{
    this->deleteAllSubItems();
}

ValueTree TreeItem::serialize() const
{
    ValueTree tree(Serialization::Core::treeItem);
    tree.setProperty(Serialization::Core::treeItemType, this->type);
    tree.setProperty(Serialization::Core::treeItemName, this->name);
    TreeItemChildrenSerializer::serializeChildren(*this, tree);
    return tree;
}

void TreeItem::deserialize(const ValueTree &tree)
{
    // Do not reset here, subclasses may rely
    // on this method in their deserialization
    //this->reset();

    this->name = tree.getProperty(Serialization::Core::treeItemName);

    TreeItemChildrenSerializer::deserializeChildren(*this, tree);
}



// protected static

void TreeItem::openOrCloseAllSubGroups(TreeViewItem &item, bool shouldOpen)
{
    item.setOpen(shouldOpen);

    for (int i = item.getNumSubItems(); --i >= 0;)
    {
        if (TreeViewItem *sub = item.getSubItem(i))
        {
            openOrCloseAllSubGroups(*sub, shouldOpen);
        }
    }
}

void TreeItem::getAllSelectedItems(Component *componentInTree, OwnedArray<TreeItem> &selectedNodes)
{
    TreeView *tree = dynamic_cast <TreeView *>(componentInTree);

    if (tree == nullptr)
    { tree = componentInTree->findParentComponentOfClass<TreeView>(); }

    if (tree != nullptr)
    {
        const int numSelected = tree->getNumSelectedItems();

        for (int i = 0; i < numSelected; ++i)
        {
            if (TreeItem *p = dynamic_cast <TreeItem *>(tree->getSelectedItem(i)))
            { selectedNodes.add(p); }
        }
    }
}

TreeItem *TreeItem::getSelectedItem(Component *componentInTree)
{
    TreeView *tree = dynamic_cast<TreeView *>(componentInTree);

    if (tree == nullptr)
    { tree = componentInTree->findParentComponentOfClass<TreeView>(); }

    if (tree != nullptr)
    {
        const int numSelected = tree->getNumSelectedItems();

        for (int i = 0; i < numSelected; ++i)
        {
            if (TreeItem *p = dynamic_cast<TreeItem *>(tree->getSelectedItem(i)))
            {
                return p;
            }
        }
    }

    return nullptr;
}

bool TreeItem::isNodeInChildren(TreeItem *nodeToScan, TreeItem *nodeToCheck)
{
    if (nodeToScan == nodeToCheck) { return true; }

    for (int i = 0; i < nodeToScan->getNumSubItems(); ++i)
    {
        TreeItem *child = static_cast<TreeItem *>(nodeToScan->getSubItem(i));

        if (child == nodeToCheck) { return true; }

        if (TreeItem::isNodeInChildren(child, nodeToCheck)) { return true; }
    }

    return false;
}

String TreeItem::getUniqueName() const
{
    return String(this->getIndexInParent());
}

bool TreeItem::mightContainSubItems()
{
    return (this->getNumSubItems() > 0);
}

void TreeItem::recreateSubtreePages()
{
    Array<TreeItem *> subtree;
    this->collectChildrenOfType<TreeItem>(this, subtree, false);
    this->recreatePage();

    for (int i = 0; i < subtree.size(); ++i)
    {
        subtree.getUnchecked(i)->recreatePage();
    }
}

void TreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (TreeView *tree = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    {
        TreeItem *selected = TreeItem::getSelectedItem(tree);

        if (!selected) { return; }

        TreeViewItem *parent = selected->getParentItem();

        int insertIndexCorrection = 0;

        for (int i = 0; i < parent->getNumSubItems(); ++i)
        {
            if (TreeItem *item = dynamic_cast<TreeItem *>(parent->getSubItem(i)))
            {
                if (item == selected)
                {
                    // фикс для корявости в поведении TreeView при драге на себя же:
                    if ((parent == this) && ((insertIndex == i) || (insertIndex == (i + 1))))
                    { return; }

                    // после удаления индексы дерева поменяются
                    if ((parent == this) && (insertIndex > i))
                    { insertIndexCorrection = -1; }

                    parent->removeSubItem(i, false);
                }
            }
        }

        this->addChildTreeItem(selected, insertIndex + insertIndexCorrection);
    }
}

void TreeItem::setVisible(bool shouldBeVisible) noexcept
{
    this->itemShouldBeVisible = shouldBeVisible;
}
