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

#include "HeadlineItemDataSource.h"

class TreeNodeBase
{
public:

    TreeNodeBase();
    virtual ~TreeNodeBase() {}

    int getNumChildren() const noexcept;
    TreeNodeBase *getChild(int index) const noexcept;
    TreeNodeBase *getParent() const noexcept;

    void addChild(TreeNodeBase *newNode, int insertPosition = -1);
    bool removeChild(int index, bool deleteNode = true);

    int getIndexInParent() const noexcept;
    String getNodeIdentifier() const;

    bool isSelected() const noexcept;
    void setSelected(bool shouldBeSelected, bool deselectOthersFirst,
        NotificationType shouldNotify = sendNotification);

protected:

    virtual void itemSelectionChanged(bool isNowSelected) {}

private:

    TreeNodeBase *getTopLevelNode() noexcept;
    void deselectAllRecursively(TreeNodeBase *toIgnore);

    TreeNodeBase *parent = nullptr;
    OwnedArray<TreeNodeBase> children;

    bool selected : 1;
    int uid = 0;

};

#if HELIO_DESKTOP
#   define TREE_NODE_ICON_HEIGHT (24)
#elif HELIO_MOBILE
#   define TREE_NODE_ICON_HEIGHT (32)
#endif

class TreeNode : public TreeNodeBase,
                 public Serializable,
                 public HeadlineItemDataSource
{
public:

    TreeNode(const String &name, const Identifier &type);
    ~TreeNode() override;

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
        const TreeNodeBase *rootItem = this;

        while (TreeNodeBase *item = rootItem->getParent())
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
        const TreeNodeBase *rootItem = this;

        for (int i = 0; i < rootItem->getNumChildren(); ++i)
        {
            if (T *childOfType = dynamic_cast<T *>(rootItem->getChild(i)))
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
        TreeNodeBase *item = this;
        
        while (item->getParent())
        {
            item = item->getParent();
        }
        
        return static_cast<TreeNode *>(item);
    }

    bool isSelectedOrHasSelectedChild() const;

    //===------------------------------------------------------------------===//
    // Page stuff
    //===------------------------------------------------------------------===//
    
    virtual void showPage() = 0;
    virtual void recreatePage() {}
    void recreateSubtreePages();

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

    void dispatchChangeTreeNodeViews();
    void itemSelectionChanged(bool isNowSelected) override;

    template<typename T, typename ArrayType>
    static void collectChildrenOfType(const TreeNode *rootNode, ArrayType &resultArray, bool pickOnlySelectedOnes)
    {
        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getChild(i));

            if (T *targetTreeItem = dynamic_cast<T *>(child))
            {
                if (!pickOnlySelectedOnes || child->isSelected())
                {
                    resultArray.add(targetTreeItem);
                }
            }

            if (child->getNumChildren() > 0)
            {
                TreeNode::collectChildrenOfType<T, ArrayType>(child, resultArray, pickOnlySelectedOnes);
            }
        }
    }

    static void collectSelectedSubItems(const TreeNode *rootNode, Array<TreeNode *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getChild(i));
        
            if (child->isSelected())
            {
                resultArray.add(child);
            }
        
            if (child->getNumChildren() > 0)
            {
                TreeNode::collectSelectedSubItems(child, resultArray);
            }
        }
    }

    static void collectActiveSubItems(const TreeNode *rootNode, Array<TreeNode *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getChild(i));
        
            if (child->isPrimarySelection())
            {
                resultArray.add(child);
            }
        
            if (child->getNumChildren() > 0)
            {
                TreeNode::collectActiveSubItems(child, resultArray);
            }
        }
    }

    String name;
    String type;

    bool isPrimarySelectedItem;

    void removeNodeFromParent();
    void deleteAllChildren();

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
