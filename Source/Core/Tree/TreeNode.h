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

    TreeNodeBase() = default;
    virtual ~TreeNodeBase() {}

    int getNumChildren() const noexcept;
    TreeNodeBase *getChild(int index) const noexcept;
    TreeNodeBase *getParent() const noexcept;

    void addChild(TreeNodeBase *newNode, int insertPosition = -1);
    bool removeChild(int index, bool deleteNode = true);

    int getIndexInParent() const noexcept;
    String getNodeIdentifier() const;

    bool isSelected() const noexcept;
    void setSelected(NotificationType shouldNotify = sendNotification);
    // late notify, if called setSelected(doneSendNotification) earlier
    // used in version control operations, which don't send notifications
    void sendSelectionNotification();

protected:

    virtual void nodeSelectionChanged(bool isNowSelected) {}
    TreeNodeBase *getTopLevelNode() noexcept;

private:

    void deselectAllRecursively(TreeNodeBase *toIgnore);

    bool selected = false;
    TreeNodeBase *parent = nullptr;
    OwnedArray<TreeNodeBase> children;
};

class TreeNode : public TreeNodeBase,
                 public Serializable,
                 public HeadlineItemDataSource
{
public:

    TreeNode(const String &name, const Identifier &type);
    ~TreeNode() override;
    
    String getName() const noexcept override;
    virtual void safeRename(const String &newName, bool sendNotifications);

    static const String xPathSeparator;
    static String createSafeName(const String &nameStr);
    
    template<typename T>
    T *findParentOfType() const
    {
        const TreeNodeBase *rootNode = this;

        while (TreeNodeBase *node = rootNode->getParent())
        {
            rootNode = node;

            if (T *parentOfType = dynamic_cast<T *>(node))
            {
                return parentOfType;
            }
        }

        return nullptr;
    }

    template<typename T>
    T *findChildOfType() const
    {
        const TreeNodeBase *rootNode = this;

        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            if (T *childOfType = dynamic_cast<T *>(rootNode->getChild(i)))
            {
                return childOfType;
            }
        }

        return nullptr;
    }

    template<typename T>
    bool selectChildOfType() const
    {
        if (T *child = this->findChildOfType<T>())
        {
            child->setSelected();
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

    TreeNode *findActiveNode() const
    {
        Array<TreeNode *> activeNodes;
        TreeNode::collectActiveSubNodes(this, activeNodes);
        
        jassert(activeNodes.size() > 0);
        if (activeNodes.size() > 0)
        {
            return activeNodes.getFirst();
        }
        
        return const_cast<TreeNode *>(this); // root
    }

    inline TreeNode *getRootNode() noexcept
    {
        return static_cast<TreeNode *>(this->getTopLevelNode());
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

    void addChildNode(TreeNode *child, int insertIndex = -1, bool sendNotifications = true);
    virtual void onNodeAddedToTree(bool sendNotifications) {}

    static bool deleteNode(TreeNode *nodeToDelete, bool sendNotifications);
    virtual void onNodeDeletedFromTree(bool sendNotifications) {}

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    void reset() override;
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;

protected:

    void dispatchChangeTreeNodeViews();
    void nodeSelectionChanged(bool isNowSelected) override;

    template<typename T, typename ArrayType>
    static void collectChildrenOfType(const TreeNode *rootNode, ArrayType &resultArray, bool pickOnlySelectedOnes)
    {
        for (int i = 0; i < rootNode->getNumChildren(); ++i)
        {
            TreeNode *child = static_cast<TreeNode *>(rootNode->getChild(i));

            if (T *targetNode = dynamic_cast<T *>(child))
            {
                if (!pickOnlySelectedOnes || child->isSelected())
                {
                    resultArray.add(targetNode);
                }
            }

            if (child->getNumChildren() > 0)
            {
                TreeNode::collectChildrenOfType<T, ArrayType>(child, resultArray, pickOnlySelectedOnes);
            }
        }
    }

    static void collectActiveSubNodes(const TreeNode *rootNode, Array<TreeNode *> &resultArray)
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
                TreeNode::collectActiveSubNodes(child, resultArray);
            }
        }
    }

    String name;
    String type;

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
        return this->setSelected();
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeNode)
    JUCE_DECLARE_WEAK_REFERENCEABLE(TreeNode)
};
