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
#include "MidiTrackNode.h"
#include "ProjectNode.h"
#include "MainLayout.h"

//===----------------------------------------------------------------------===//
// TreeNodeBase
//===----------------------------------------------------------------------===//

int TreeNodeBase::getNumChildren() const noexcept
{
    return this->children.size();
}

TreeNodeBase *TreeNodeBase::getChild(int index) const noexcept
{
    return this->children[index];
}

TreeNodeBase *TreeNodeBase::getParent() const noexcept
{
    return this->parent;
}

int TreeNodeBase::getIndexInParent() const noexcept
{
    return this->parent == nullptr ? 0
        : this->parent->children.indexOf(this);
}

String TreeNodeBase::getNodeIdentifier() const
{
    String s;
    if (this->parent != nullptr)
    {
        s = this->parent->getNodeIdentifier();
    }

    return s + "/" + String(this->getIndexInParent());
}

void TreeNodeBase::addChild(TreeNodeBase *newNode, int insertPosition /*= -1*/)
{
    if (newNode != nullptr)
    {
        newNode->parent = nullptr;
        newNode->parent = this;
        this->children.insert(insertPosition, newNode);
    }
}

bool TreeNodeBase::removeChild(int index, bool deleteNode /*= true*/)
{
    if (auto* child = this->children[index])
    {
        child->parent = nullptr;
        this->children.remove(index, deleteNode);
        return true;
    }

    return false;
}

bool TreeNodeBase::isSelected() const noexcept
{
    return this->selected;
}

void TreeNodeBase::setSelected(NotificationType shouldNotify /*= sendNotification*/)
{
    this->getTopLevelNode()->deselectAllRecursively(this);

    if (!this->selected)
    {
        this->selected = true;

        if (shouldNotify != dontSendNotification)
        {
            this->itemSelectionChanged(true);
        }
    }
}

TreeNodeBase *TreeNodeBase::getTopLevelNode() noexcept
{
    return this->parent == nullptr ?
        this : this->parent->getTopLevelNode();
}

void TreeNodeBase::deselectAllRecursively(TreeNodeBase *toIgnore)
{
    if (this != toIgnore && this->selected)
    {
        this->selected = false;
        this->itemSelectionChanged(false);
    }

    for (auto *i : this->children)
    {
        i->deselectAllRecursively(toIgnore);
    }
}

//===----------------------------------------------------------------------===//
// TreeNode
//===----------------------------------------------------------------------===//

const String TreeNode::xPathSeparator = "/";

String TreeNode::createSafeName(const String &name)
{
    return File::createLegalFileName(name).removeCharacters(TreeNode::xPathSeparator);
}

TreeNode::TreeNode(const String &name, const Identifier &type) :
    name(TreeNode::createSafeName(name)),
    type(type.toString()) {}

TreeNode::~TreeNode()
{
    this->removeAllChangeListeners();
    this->deleteAllChildren();
    this->removeNodeFromParent();
}

//===----------------------------------------------------------------------===//
// Rename
//===----------------------------------------------------------------------===//

void TreeNode::safeRename(const String &newName, bool sendNotifications)
{
    this->name = TreeNode::createSafeName(newName);
    if (sendNotifications)
    {
        this->dispatchChangeTreeNodeViews();
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
    if (itemToDelete->getRootNode() == itemToDelete) { return false; }
    const bool shouldRefocus = itemToDelete->isSelected();
    
    WeakReference<TreeNode> root = itemToDelete->getRootNode();
    WeakReference<TreeNode> parent = itemToDelete->findParentOfType<ProjectNode>();

    itemToDelete->onItemDeletedFromTree(sendNotifications);
    delete itemToDelete;

    if (parent != nullptr)
    {
        parent->sendChangeMessage();
    }

    if (shouldRefocus)
    {
        if (parent != nullptr)
        {
            if (auto *sibling = parent->findChildOfType<MidiTrackNode>())
            {
                sibling->setSelected();
            }
            else
            {
                parent->setSelected();
            }
        }
        else if (root != nullptr)
        {
            root->setSelected();
        }
    }

    return true;
}

void TreeNode::deleteAllChildren()
{
    while (this->getNumChildren() > 0)
    {
        delete this->getChild(0); // сам себя удалит из иерархии
    }
}

void TreeNode::removeNodeFromParent()
{
    if (TreeNode *parent = dynamic_cast<TreeNode *>(this->getParent()))
    {
        for (int i = 0; i < parent->getNumChildren(); ++i)
        {
            if (auto *item = dynamic_cast<TreeNode *>(parent->getChild(i)))
            {
                if (item == this)
                {
                    parent->removeChild(i, false);
                }
            }
        }
    }
}

static void notifySubtreeParentChanged(TreeNode *node, bool sendNotifications)
{
    node->onItemAddedToTree(sendNotifications);

    for (int i = 0; i < node->getNumChildren(); ++i)
    {
        TreeNode *child = static_cast<TreeNode *>(node->getChild(i));
        notifySubtreeParentChanged(child, sendNotifications);
    }
}

void TreeNode::addChildTreeItem(TreeNode *child, int insertIndex /*= -1*/, bool sendNotifications /*= true*/)
{
    this->addChild(child, insertIndex);
    notifySubtreeParentChanged(child, sendNotifications);
}

void TreeNode::dispatchChangeTreeNodeViews()
{
    this->sendChangeMessage(); // update listeners
    //this->treeHasChanged(); // updates ownerView, if any
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void TreeNode::reset()
{
    this->deleteAllChildren();
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
