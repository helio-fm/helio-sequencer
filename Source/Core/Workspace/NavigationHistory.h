/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "TreeNode.h"

class NavigationHistoryLock final
{
public:
    NavigationHistoryLock() {}
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NavigationHistoryLock)
    JUCE_DECLARE_WEAK_REFERENCEABLE(NavigationHistoryLock)
};

class NavigationHistory final : public ChangeBroadcaster
{
public:
    
    NavigationHistory() = default;

    UniquePointer<NavigationHistoryLock> lock();
        
    bool canGoForward() const;
    bool canGoBackward() const;
    
    WeakReference<TreeNode> goBack();
    WeakReference<TreeNode> goForward();
    
    WeakReference<TreeNode> getCurrentItem() const;
    bool addItemIfNeeded(TreeNode *item);
    
private:
    
    Array<WeakReference<TreeNode>> list;
    
    // A way to prevent new items from being added when navigating back/forward
    WeakReference<NavigationHistoryLock> historyLock = nullptr;
    
    int currentPageIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NavigationHistory)
};
