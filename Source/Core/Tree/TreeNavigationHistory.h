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

#include "TreeItem.h"

class TreeNavigationHistory :
    public ChangeBroadcaster
{
public:
    
    TreeNavigationHistory() : isLocked(false), currentPageIndex(0) { }
    
    void setLocked(bool shouldBeLocked)
    {
        this->isLocked = shouldBeLocked;
    }
    
    bool canGoForward() const
    {
        return (this->currentPageIndex < (this->list.size() - 1));
    }
    
    bool canGoBackward() const
    {
        return (this->currentPageIndex > 0);
    }
    
    TreeItem *goBack()
    {
        if (!this->canGoBackward())
        {
            return nullptr;
        }
        
        while (this->list.size() > 0)
        {
            const auto currentItem = this->getCurrentItem();

            this->currentPageIndex--;
            const auto previousItem = this->getCurrentItem();
            const bool previousItemIsTheSameAsCurrent =
                ! previousItem.wasObjectDeleted() &&
                ! currentItem.wasObjectDeleted() &&
                currentItem == previousItem;

            if (previousItem.wasObjectDeleted() || previousItemIsTheSameAsCurrent)
            {
                this->list.removeRange(this->currentPageIndex, 1);
            }
            else
            {
                break;
            }
        }
        
        this->sendChangeMessage();

        TreeItem *treeItem = this->list[this->currentPageIndex];
        return treeItem;
    }
    
    TreeItem *goForward()
    {
        if (!this->canGoForward())
        {
            return nullptr;
        }
                
        while ((this->list.size() - 1) > this->currentPageIndex)
        {
            const auto currentItem = this->getCurrentItem();

            this->currentPageIndex++;
            const auto nextItem = this->getCurrentItem();
            const bool nextItemIsTheSameAsCurrent =
                ! currentItem.wasObjectDeleted() &&
                ! nextItem.wasObjectDeleted() &&
                currentItem == nextItem;
            
            if (nextItem.wasObjectDeleted() || nextItemIsTheSameAsCurrent)
            {
                this->list.removeRange(this->currentPageIndex, 1);
                this->currentPageIndex--;
            }
            else
            {
                break;
            }
        }
        
        this->sendChangeMessage();
        
        TreeItem *treeItem = this->list[this->currentPageIndex];
        return treeItem;
    }
    
    WeakReference<TreeItem> getCurrentItem() const
    {
        return this->list[this->currentPageIndex];
    }
    
    bool addItemIfNeeded(TreeItem *item)
    {
        if (this->isLocked.get())
        {
            return false;
        }
        
        // First cleanup the list from deleted items
        int i = 0;
        while ((this->list.size() - 1) > i)
        {
            if (this->list[i].wasObjectDeleted())
            {
                this->list.removeRange(i, 1);
            }
            else
            {
                i++;
            }
        }

        // Add, if valid and not duplicated
        if (item != nullptr &&
            this->getCurrentItem() != item)
        {
            this->list.removeRange(this->currentPageIndex + 1, this->list.size());
            this->list.add(item);
            this->currentPageIndex = this->list.size() - 1;
            this->sendChangeMessage();
            return true;
        }

        return false;
    }
    
private:
    
    Array<WeakReference<TreeItem>> list;
    
    // A way to prevent new items from being added when navigating back/forward
    Atomic<bool> isLocked;
    
    int currentPageIndex;
    
};
