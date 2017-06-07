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

class TreePanel;

#include "Serializable.h"

#if HELIO_DESKTOP
#   define TREE_ICON_HEIGHT (16)
#   define TREE_LARGE_ICON_HEIGHT (24)
#   define TREE_ITEM_HEIGHT (32)
#   define TREE_FONT_SIZE (18)
#elif HELIO_MOBILE
#   define TREE_ICON_HEIGHT (24)
#   define TREE_LARGE_ICON_HEIGHT (32)
#   define TREE_ITEM_HEIGHT (42)
#   define TREE_FONT_SIZE (20)
#endif

class TreeItem :
    public Serializable,
    public TreeViewItem
{
public:

    explicit TreeItem(const String &nameStr);

    ~TreeItem() override;
    
	static const String xPathSeparator;


    static String createSafeName(const String &nameStr);

    int getNumSelectedSiblings() const;
    int getNumSelectedChildren() const;
    bool haveAllSiblingsSelected() const;
    bool haveAllChildrenSelected() const;
    bool haveAllChildrenSelectedWithDeepSearch() const;

    
    String getUniqueName() const override;

    TreePanel *findParentTreePanel() const;

    virtual void onItemMoved() {}


    void addChildTreeItem(TreeItem *child, int insertIndex = -1)
    {
        this->addSubItem(child, insertIndex);
        TreeItem::notifySubtreeMoved(child);
    }


    void setMarkerVisible(bool shouldBeVisible) noexcept;
    bool isMarkerVisible() const noexcept;

    void setGreyedOut(bool shouldBeGreyedOut) noexcept;
    bool isGreyedOut() const noexcept;

    bool isCompactMode() const;


    template<typename T>
    static T *getActiveItem(TreeItem *root)
    {
        if (root == nullptr)
        {
            return nullptr;
        }
        
        Array<T *> childrenOfType = root->findChildrenOfType<T>();
        
        for (int i = 0; i < childrenOfType.size(); ++i)
        {
            T *p = static_cast<T *>(childrenOfType.getUnchecked(i));
            
            if (p->isMarkerVisible())
            {
                return p;
            }
        }
    
        return nullptr;
    }

    static TreeItem *getSelectedItem(Component *anyComponentInTree);
    static void getAllSelectedItems(Component *anyComponentInTree, OwnedArray<TreeItem> &selectedNodes);

    static bool isNodeInChildren(TreeItem *nodeToScan, TreeItem *nodeToCheck);

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
    Array<T *> findChildrenOfType(bool pickOnlySelectedOnes = false) const
    {
        Array<T *> children;
        TreeItem::collectChildrenOfType<T>(this, children, pickOnlySelectedOnes);
        return children;
    }

    Array<TreeItem *> findSelectedSubItems() const
    {
        Array<TreeItem *> selection;
        TreeItem::collectSelectedSubItems(this, selection);
        return selection;
    }

    TreeItem *findPrimaryActiveItem() const
    {
        Array<TreeItem *> activeItems;
        TreeItem::collectActiveSubItems(this, activeItems);
        
        jassert(activeItems.size() > 0);
        
        if (activeItems.size() > 0) {
            return activeItems.getFirst();
        }
        
        // this should never happen normally
        return nullptr;
    }

    inline TreeItem *getRootTreeItem()
    {
        TreeViewItem *item = this;
        
        while (item->getParentItem())
        {
            item = item->getParentItem();
        }
        
        return static_cast<TreeItem *>(item);
    }

    bool mightContainSubItems() override
    {
        return (this->getNumSubItems() > 0);
    }

    //===------------------------------------------------------------------===//
    // Page stuff
    //===------------------------------------------------------------------===//
    
    virtual void showPage() = 0;
    virtual void recreatePage() {}
    void recreateSubtreePages()
    {
        Array<TreeItem *> subtree;
        this->collectChildrenOfType<TreeItem>(this, subtree, false);

        this->recreatePage();

        for (int i = 0; i < subtree.size(); ++i)
        {
            subtree.getUnchecked(i)->recreatePage();
        }
    }

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override;
    bool isInterestedInFileDrag(const StringArray &files) override { return false; }
    void filesDropped(const StringArray &files, int insertIndex) override {}

    //===------------------------------------------------------------------===//
    // Painting
    //===------------------------------------------------------------------===//

    Component *createItemComponent() override;

    void paintItem(Graphics &g, int width, int height) override;

    void paintOpenCloseButton(Graphics &, const Rectangle<float> &area,
                                      Colour backgroundColour, bool isMouseOver) override;

    void paintHorizontalConnectingLine(Graphics &, const Line<float> &line) override;

    void paintVerticalConnectingLine(Graphics &, const Line<float> &line) override;

    int getItemHeight() const override;

    virtual Image getIcon() const = 0;
    virtual Font getFont() const;
    virtual Colour getColour() const;
    virtual String getCaption() const; // a title for the component - as opposed to getName()

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    virtual Component *createItemMenu()
    {
        return nullptr;
    }

    void itemSelectionChanged(bool isNowSelected) override;

    String getName() const noexcept; // an internal name

    //===------------------------------------------------------------------===//
    // Cleanup
    //===------------------------------------------------------------------===//

    static void deleteAllSelectedItems(Component *anyComponentInTree);
    static bool deleteItem(TreeItem *itemToDelete);

    //===------------------------------------------------------------------===//
    // Renamer
    //===------------------------------------------------------------------===//

    virtual void onRename(const String &newName);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    void reset() override
    {
        this->deleteAllSubItems();
    }


protected:

    void setName(const String &newName) noexcept;

    friend class RenameTreeItemCallback;

    virtual String getNameForRenamingCallback() const
    {
        return this->getName();
    }

protected:

    void setVisible(bool shouldBeVisible) noexcept;

    void safeRename(const String &newName);

    template<typename T>
    static void collectChildrenOfType(const TreeItem *rootNode, Array<T *> &resultArray, bool pickOnlySelectedOnes)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeItem *child = static_cast<TreeItem *>(rootNode->getSubItem(i));

            if (T *targetTreeItem = dynamic_cast<T *>(child))
            {
                if (!pickOnlySelectedOnes ||
                    (pickOnlySelectedOnes && child->isSelected()))
                {
                    resultArray.add(targetTreeItem);
                }
            }

            if (child->getNumSubItems() > 0)
            {
                TreeItem::collectChildrenOfType<T>(child, resultArray, pickOnlySelectedOnes);
            }
        }
    }

    static void collectSelectedSubItems(const TreeItem *rootNode, Array<TreeItem *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeItem *child = static_cast<TreeItem *>(rootNode->getSubItem(i));
        
            if (child->isSelected())
            {
                resultArray.add(child);
            }
        
            if (child->getNumSubItems() > 0)
            {
                TreeItem::collectSelectedSubItems(child, resultArray);
            }
        }
    }

    static void collectActiveSubItems(const TreeItem *rootNode, Array<TreeItem *> &resultArray)
    {
        for (int i = 0; i < rootNode->getNumSubItems(); ++i)
        {
            TreeItem *child = static_cast<TreeItem *>(rootNode->getSubItem(i));
        
            if (child->isMarkerVisible())
            {
                resultArray.add(child);
            }
        
            if (child->getNumSubItems() > 0)
            {
                TreeItem::collectActiveSubItems(child, resultArray);
            }
        }
    }

    String name;

    bool markerIsVisible;

    void removeItemFromParent();
    void deleteAllSubItems();

    static void openOrCloseAllSubGroups(TreeViewItem &item, bool shouldOpen);
    static void notifySubtreeMoved(TreeItem *node);

private:

    bool itemIsGreyedOut;
    bool itemShouldBeVisible;

    WeakReference<TreeItem>::Master masterReference;
    friend class WeakReference<TreeItem>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItem)

};
