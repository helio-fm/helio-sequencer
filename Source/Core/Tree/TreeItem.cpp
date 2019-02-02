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
#include "TreeItemChildrenSerializer.h"
#include "SerializationKeys.h"
#include "MainLayout.h"

const String TreeItem::xPathSeparator = "/";

String TreeItem::createSafeName(const String &name)
{
    return File::createLegalFileName(name).removeCharacters(TreeItem::xPathSeparator);
}

TreeItem::TreeItem(const String &name, const Identifier &type) :
    name(TreeItem::createSafeName(name)),
    type(type.toString()),
    isPrimarySelectedItem(false) {}

TreeItem::~TreeItem()
{
    this->removeAllChangeListeners();
    this->deleteAllSubItems();
    this->removeItemFromParent();
}

void TreeItem::setPrimarySelection(bool isSelected) noexcept
{
    this->isPrimarySelectedItem = isSelected;
}

bool TreeItem::isPrimarySelection() const noexcept
{
    return this->isPrimarySelectedItem;
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

void TreeItem::safeRename(const String &newName, bool sendNotifications)
{
    this->name = TreeItem::createSafeName(newName);
    if (sendNotifications)
    {
        this->dispatchChangeTreeItemView();
    }
}

String TreeItem::getName() const noexcept 
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

bool TreeItem::deleteItem(TreeItem *itemToDelete, bool sendNotifications)
{
    if (!itemToDelete) { return false; }
    if (itemToDelete->getRootTreeItem() == itemToDelete) { return false; }

    WeakReference<TreeItem> moveFocusTo = nullptr;
    WeakReference<TreeItem> parent = dynamic_cast<TreeItem *>(itemToDelete->getParentItem());

    const auto *activeItem = TreeItem::getActiveItem<TreeItem>(itemToDelete->getRootTreeItem());
    if (itemToDelete->isPrimarySelection() || (activeItem == nullptr))
    {
        moveFocusTo = parent;
    }

    itemToDelete->onItemDeletedFromTree(sendNotifications);
    delete itemToDelete;

    if (parent != nullptr)
    {
        parent->sendChangeMessage();
    }

    if (moveFocusTo != nullptr)
    {
        moveFocusTo->setSelected(true, true);
    }
    else if (parent != nullptr)
    {
        while (parent)
        {
            if (parent->isPrimarySelection())
            {
                parent->setSelected(true, true);
                break;
            }

            parent = dynamic_cast<TreeItem *>(parent->getParentItem());
        }
    }

    return true;
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

static void notifySubtreeParentChanged(TreeItem *node, bool sendNotifications)
{
    node->onItemAddedToTree(sendNotifications);

    for (int i = 0; i < node->getNumSubItems(); ++i)
    {
        TreeItem *child = static_cast<TreeItem *>(node->getSubItem(i));
        notifySubtreeParentChanged(child, sendNotifications);
    }
}

void TreeItem::addChildTreeItem(TreeItem *child, int insertIndex /*= -1*/, bool sendNotifications /*= true*/)
{
    this->addSubItem(child, insertIndex);
    notifySubtreeParentChanged(child, sendNotifications);
}

void TreeItem::dispatchChangeTreeItemView()
{
    this->sendChangeMessage(); // update listeners
    this->treeHasChanged(); // updates ownerView, if any
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
    tree.setProperty(Serialization::Core::treeItemType, this->type, nullptr);
    tree.setProperty(Serialization::Core::treeItemName, this->name, nullptr);
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

bool TreeItem::isSelectedOrHasSelectedChild() const
{
    if (this->isSelected())
    {
        return true;
    }

    Array<TreeItem *> children;
    TreeItem::collectChildrenOfType<TreeItem, Array<TreeItem *>>(this, children, true);
    return !children.isEmpty();
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

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

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
