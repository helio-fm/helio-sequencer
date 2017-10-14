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
            this->currentPageIndex--;
            
            if (this->list[this->currentPageIndex].wasObjectDeleted())
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
            this->currentPageIndex++;
            
            if (this->list[this->currentPageIndex].wasObjectDeleted())
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
    
    TreeItem *getCurrentItem() const
    {
        return this->list[this->currentPageIndex];
    }
    
    void addItemIfNeeded(TreeItem *item)
    {
        if (this->isLocked)
        {
            return;
        }
        
        if (this->getCurrentItem() != item)
        {
            this->list.removeRange(this->currentPageIndex + 1, this->list.size());
            this->list.add(item);
            this->currentPageIndex = this->list.size() - 1;
            this->sendChangeMessage();
            //Logger::writeToLog("add, current = " + String(this->currentPageIndex));
        }
    }
    
private:
    
    Array<WeakReference<TreeItem>> list;
    
    bool isLocked;
    
    int currentPageIndex;
    
};
