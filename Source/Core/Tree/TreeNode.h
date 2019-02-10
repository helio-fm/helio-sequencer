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

#pragma once

#if HELIO_DESKTOP
#   define TREE_LARGE_ICON_HEIGHT (24)
#   define TREE_ITEM_HEIGHT (32)
#   define TREE_FONT_SIZE (18)
#elif HELIO_MOBILE
#   define TREE_LARGE_ICON_HEIGHT (32)
#   define TREE_ITEM_HEIGHT (42)
#   define TREE_FONT_SIZE (20)
#endif

#include "HeadlineItemDataSource.h"

// TODO: rename as TreeNode and remove TreeViewItem dependency;
// rename children as well, like ProjectTreeItem -> ProjectNode

class TreeNode :
    public Serializable,
    public TreeViewItem,
    public HeadlineItemDataSource
{
public:

    TreeNode(const String &name, const Identifier &type);
    ~TreeNode() override;
    
    static TreeNode *getSelectedItem(Component *anyComponentInTree);
    static void getAllSelectedItems(Component *anyComponentInTree, OwnedArray<TreeNode> &selectedNodes);
    static bool isNodeInChildren(TreeNode *nodeToScan, TreeNode *nodeToCheck);

    int getNumSelectedSiblings() const;
    int getNumSelectedChildren() const;
    bool haveAllSiblingsSelected() const;
    bool haveAllChildrenSelected() const;
    bool haveAllChildrenSelectedWithDeepSearch() const;

    String getName() const noexcept override;
    virtual void safeRename(const String &newName, bool sendNotifications);

    static const String xPathSeparator;
    static String createSafeName(const String &nameStr);

    // Deprecated
    bool isPrimarySelection() const noexcept;
    void setPrimarySelection(bool isSelected) noexcept;

    template<typename T>
    static T *getActiveItem(TreeNode *root)
    {
        if (root == nullptr)
        {
            return nullptr;
        }
        
        Array<T *> childrenOfType = root->findChildrenOfType<T>();
        
        for (int i = 0; i < childrenOfType.size(); ++i)
        {
            T *p = static_cast<T *>(childrenOfType.getUnchecked(i));
            
            if (p->isPrimarySelection())
            {
                return p;
            }
        }
    
        return nullptr;
    }

    template<typename T>
    T *findParentOfType() const
    {
        const TreeViewItem *rootItem = this;

        while (TreeViewItem *item = rootItem->getParentItem())
        {
            rootItem = item;

            if (T *parentOfType = dynamic_cast<T *>(item))
            { return parentOfType; }
        }

        return nullptr;
    }

    template<typename T>
    T *findChildOfType() const
    {
        const TreeViewItem *rootItem = this;

        for (int i = 0; i < rootItem->getNumSubItems(); ++i)
        {
            if (T *childOfType = dynamic_cast<T *>(rootItem->getSubItem(i)))
            { return childOfType; }
        }

        return nullptr;
    }

    template<typename T>
    bool selectChildOfType() const
    {
        if (T *child = this->findChildOfType<T>())
        {
            child->setSelected(true, true);
            return true;
        }

        return false;
    }

    template<typename T>
    Array<T *> findChildrenOfType(bool pickOnlySelectedOnes = false) const
    {
        Array<T *> children;
        TreeNode::collectChildrenOfType<T, Array<T *>>(this, children, pickOnlySelectedOnes);
        return children;
    }

    template<typename T>
    Array<WeakReference<T>> findChildrenRefsOfType(bool pickOnlySelectedOnes = false) const
    {
        Array<WeakReference<T>> children;
        TreeNode::collectChildrenOfType<T, Array<WeakReference<T>>>(this, children, pickOnlySelectedOnes);
        return children;
    }

    Array<TreeNode *> findSelectedSubItems() const
    {
        Array<TreeNode *> selection;
        TreeNode::collectSelectedSubItems(this, selection);
        return selection;
    }

    TreeNode *findPrimaryActiveItem() const
    {
        Array<TreeNode *> activeItems;
        TreeNode::collectActiveSubItems(this, activeItems);
        
        jassert(activeItems.size() > 0);
        
        if (activeItems.size() > 0) {
            return activeItems.getFirst();
        }
        
        // this should never happen normally
        return nullptr;
    }

    inline TreeNode *getRootTreeItem()
    {
        TreeViewItem *item = this;
        
        while (item->getParentItem())
        {
            item = item->getParentItem();
        }
        
        return static_cast<TreeNode *>(item);
    }

    String getUniqueName() const override;
    bool mightContainSubItems() override;
    bool isSelectedOrHasSelectedChild() const;

    //===------------------------------------------------------------------===//
    // Page stuff
    //===------------------------------------------------------------------===//
    
    virtual void showPage() = 0;
    virtual void recreatePage() {}
    void recreateSubtreePages();

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void itemDropped(const DragAndDropTarget::SourceDetails &, int) override;
    bool isInterestedInFileDrag(const StringArray &) override { return false; }
    void filesDropped(const StringArray &, int) override {}
    var getDragSourceDescription() override { return{}; }

    //===------------------------------------------------------------------===//
    // Adding nodes to the tree and removing them
    //===------------------------------------------------------------------===//

    void addChildTreeItem(TreeNode *child, int insertIndex = -1, bool sendNotifications = true);
    virtual void onItemAddedToTree(bool sendNotifications) {}

    static bool deleteItem(TreeNode *itemToDelete, bool sendNotifications);
    virtual void onItemDeletedFromTree(bool sendNotifications) {}

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    void reset() override;
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;

protected:

    void dispatchChangeTreeItemView();
    void itemSelectionChanged(bool isNowSelected) override;

    template<typename T, typename ArrayType>
    static void collectChildrenOfType(const TreeNode *rootNode, ArrayType &resultArray, bool pickOnlySelectedOnes)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getSubItem(i));

            if (T *targetTreeItem = dynamic_cast<T *>(child))
            {
                if (!pickOnlySelectedOnes || child->isSelected())
                {
                    resultArray.add(targetTreeItem);
                }
            }

            if (child->getNumSubItems() > 0)
            {
                TreeNode::collectChildrenOfType<T, ArrayType>(child, resultArray, pickOnlySelectedOnes);
            }
        }
    }

    static void collectSelectedSubItems(const TreeNode *rootNode, Array<TreeNode *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getSubItem(i));
        
            if (child->isSelected())
            {
                resultArray.add(child);
            }
        
            if (child->getNumSubItems() > 0)
            {
                TreeNode::collectSelectedSubItems(child, resultArray);
            }
        }
    }

    static void collectActiveSubItems(const TreeNode *rootNode, Array<TreeNode *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getSubItem(i));
        
            if (child->isPrimarySelection())
            {
                resultArray.add(child);
            }
        
            if (child->getNumSubItems() > 0)
            {
                TreeNode::collectActiveSubItems(child, resultArray);
            }
        }
    }

    String name;
    String type;

    bool isPrimarySelectedItem;

    void removeItemFromParent();
    void deleteAllSubItems();

    static void openOrCloseAllSubGroups(TreeViewItem &item, bool shouldOpen);

    //===------------------------------------------------------------------===//
    // HeadlineItemDataSource
    //===------------------------------------------------------------------===//

    bool canBeSelectedAsMenuItem() const noexcept override
    {
        return !this->isSelected();
    }

    void onSelectedAsMenuItem() override
    {
        return this->setSelected(true, true);
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeNode)
    JUCE_DECLARE_WEAK_REFERENCEABLE(TreeNode)
};
