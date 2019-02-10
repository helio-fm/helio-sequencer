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
#include "TreeNode.h"
#include "TreeNodeSerializer.h"
#include "SerializationKeys.h"
#include "MainLayout.h"

const String TreeNode::xPathSeparator = "/";

String TreeNode::createSafeName(const String &name)
{
    return File::createLegalFileName(name).removeCharacters(TreeNode::xPathSeparator);
}

TreeNode::TreeNode(const String &name, const Identifier &type) :
    name(TreeNode::createSafeName(name)),
    type(type.toString()),
    isPrimarySelectedItem(false) {}

TreeNode::~TreeNode()
{
    this->removeAllChangeListeners();
    this->deleteAllSubItems();
    this->removeItemFromParent();
}

void TreeNode::setPrimarySelection(bool isSelected) noexcept
{
    this->isPrimarySelectedItem = isSelected;
}

bool TreeNode::isPrimarySelection() const noexcept
{
    return this->isPrimarySelectedItem;
}

int TreeNode::getNumSelectedSiblings() const
{
    if (TreeNode *parent = dynamic_cast<TreeNode *>(this->getParentItem()))
    {
        return parent->getNumSelectedChildren();
    }

    return 0;
}

int TreeNode::getNumSelectedChildren() const
{
    int res = 0;

    for (int i = 0; i < this->getNumSubItems(); ++i)
    {
        res += (this->getSubItem(i)->isSelected() ? 1 : 0);
    }

    return res;
}

bool TreeNode::haveAllSiblingsSelected() const
{
    if (TreeNode *parent = dynamic_cast<TreeNode *>(this->getParentItem()))
    {
        if (parent->haveAllChildrenSelected())
        {
            return true;
        }
    }

    return false;
}

bool TreeNode::haveAllChildrenSelected() const
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

bool TreeNode::haveAllChildrenSelectedWithDeepSearch() const
{
    Array<TreeNode *> allChildren(this->findChildrenOfType<TreeNode>());

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

void TreeNode::safeRename(const String &newName, bool sendNotifications)
{
    this->name = TreeNode::createSafeName(newName);
    if (sendNotifications)
    {
        this->dispatchChangeTreeItemView();
    }
}

String TreeNode::getName() const noexcept 
{
    return this->name;
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

void TreeNode::itemSelectionChanged(bool isNowSelected)
{
    if (isNowSelected)
    {
        this->showPage();
    }
}

//===----------------------------------------------------------------------===//
// Cleanup
//===----------------------------------------------------------------------===//

bool TreeNode::deleteItem(TreeNode *itemToDelete, bool sendNotifications)
{
    if (!itemToDelete) { return false; }
    if (itemToDelete->getRootTreeItem() == itemToDelete) { return false; }

    WeakReference<TreeNode> moveFocusTo = nullptr;
    WeakReference<TreeNode> parent = dynamic_cast<TreeNode *>(itemToDelete->getParentItem());

    const auto *activeItem = TreeNode::getActiveItem<TreeNode>(itemToDelete->getRootTreeItem());
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

            parent = dynamic_cast<TreeNode *>(parent->getParentItem());
        }
    }

    return true;
}

void TreeNode::deleteAllSubItems()
{
    while (this->getNumSubItems() > 0)
    {
        delete this->getSubItem(0); // сам себя удалит из иерархии
    }
}

void TreeNode::removeItemFromParent()
{
    if (TreeNode *parent = dynamic_cast<TreeNode *>(this->getParentItem()))
    {
        for (int i = 0; i < parent->getNumSubItems(); ++i)
        {
            if (TreeNode *item = dynamic_cast<TreeNode *>(parent->getSubItem(i)))
            {
                if (item == this)
                {
                    parent->removeSubItem(i, false);
                }
            }
        }
    }
}

static void notifySubtreeParentChanged(TreeNode *node, bool sendNotifications)
{
    node->onItemAddedToTree(sendNotifications);

    for (int i = 0; i < node->getNumSubItems(); ++i)
    {
        TreeNode *child = static_cast<TreeNode *>(node->getSubItem(i));
        notifySubtreeParentChanged(child, sendNotifications);
    }
}

void TreeNode::addChildTreeItem(TreeNode *child, int insertIndex /*= -1*/, bool sendNotifications /*= true*/)
{
    this->addSubItem(child, insertIndex);
    notifySubtreeParentChanged(child, sendNotifications);
}

void TreeNode::dispatchChangeTreeItemView()
{
    this->sendChangeMessage(); // update listeners
    this->treeHasChanged(); // updates ownerView, if any
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void TreeNode::reset()
{
    this->deleteAllSubItems();
}

ValueTree TreeNode::serialize() const
{
    ValueTree tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type, nullptr);
    tree.setProperty(Serialization::Core::treeNodeName, this->name, nullptr);
    TreeNodeSerializer::serializeChildren(*this, tree);
    return tree;
}

void TreeNode::deserialize(const ValueTree &tree)
{
    // Do not reset here, subclasses may rely
    // on this method in their deserialization
    //this->reset();

    this->name = tree.getProperty(Serialization::Core::treeNodeName);

    TreeNodeSerializer::deserializeChildren(*this, tree);
}



// protected static

void TreeNode::openOrCloseAllSubGroups(TreeViewItem &item, bool shouldOpen)
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

void TreeNode::getAllSelectedItems(Component *componentInTree, OwnedArray<TreeNode> &selectedNodes)
{
    TreeView *tree = dynamic_cast <TreeView *>(componentInTree);

    if (tree == nullptr)
    { tree = componentInTree->findParentComponentOfClass<TreeView>(); }

    if (tree != nullptr)
    {
        const int numSelected = tree->getNumSelectedItems();

        for (int i = 0; i < numSelected; ++i)
        {
            if (TreeNode *p = dynamic_cast <TreeNode *>(tree->getSelectedItem(i)))
            { selectedNodes.add(p); }
        }
    }
}

TreeNode *TreeNode::getSelectedItem(Component *componentInTree)
{
    TreeView *tree = dynamic_cast<TreeView *>(componentInTree);

    if (tree == nullptr)
    { tree = componentInTree->findParentComponentOfClass<TreeView>(); }

    if (tree != nullptr)
    {
        const int numSelected = tree->getNumSelectedItems();

        for (int i = 0; i < numSelected; ++i)
        {
            if (TreeNode *p = dynamic_cast<TreeNode *>(tree->getSelectedItem(i)))
            {
                return p;
            }
        }
    }

    return nullptr;
}

bool TreeNode::isNodeInChildren(TreeNode *nodeToScan, TreeNode *nodeToCheck)
{
    if (nodeToScan == nodeToCheck) { return true; }

    for (int i = 0; i < nodeToScan->getNumSubItems(); ++i)
    {
        TreeNode *child = static_cast<TreeNode *>(nodeToScan->getSubItem(i));

        if (child == nodeToCheck) { return true; }

        if (TreeNode::isNodeInChildren(child, nodeToCheck)) { return true; }
    }

    return false;
}

String TreeNode::getUniqueName() const
{
    return String(this->getIndexInParent());
}

bool TreeNode::mightContainSubItems()
{
    return (this->getNumSubItems() > 0);
}

bool TreeNode::isSelectedOrHasSelectedChild() const
{
    if (this->isSelected())
    {
        return true;
    }

    Array<TreeNode *> children;
    TreeNode::collectChildrenOfType<TreeNode, Array<TreeNode *>>(this, children, true);
    return !children.isEmpty();
}

void TreeNode::recreateSubtreePages()
{
    Array<TreeNode *> subtree;
    this->collectChildrenOfType<TreeNode>(this, subtree, false);
    this->recreatePage();

    for (int i = 0; i < subtree.size(); ++i)
    {
        subtree.getUnchecked(i)->recreatePage();
    }
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void TreeNode::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (TreeView *tree = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    {
        TreeNode *selected = TreeNode::getSelectedItem(tree);

        if (!selected) { return; }

        TreeViewItem *parent = selected->getParentItem();

        int insertIndexCorrection = 0;

        for (int i = 0; i < parent->getNumSubItems(); ++i)
        {
            if (TreeNode *item = dynamic_cast<TreeNode *>(parent->getSubItem(i)))
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
