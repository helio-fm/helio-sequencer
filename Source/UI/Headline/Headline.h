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

#include "TreeNode.h"
#include "NavigationHistory.h"
#include "HeadlineNavigationPanel.h"
#include "UserInterfaceFlags.h"

class IconButton;
class HeadlineItem;
class HeadlineItemDataSource;

class Headline final : public Component,
    public AsyncUpdater,
    public UserInterfaceFlags::Listener
{
public:

    Headline();
    ~Headline();

    static constexpr auto itemsOverlapOffset = 16;

    void syncWithTree(NavigationHistory &history, WeakReference<TreeNode> leaf);
    
    void showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource);
    void hideSelectionMenu();

    HeadlineItem *getTailItem() const noexcept;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void resized() override;
    void handleCommandMessage(int commandId) override;

    //===------------------------------------------------------------------===//
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onRollAnimationsFlagChanged(bool enabled) override;

private:

    // A way to receive a single coalesced update from multiple signaling sub-items:
    void handleAsyncUpdate() override;
    int rebuildChain(WeakReference<TreeNode> leaf);
    int getChainWidth() const noexcept;

    ComponentAnimator animator;
    int fadeInTimeMs = Globals::UI::fadeInLong;
    int fadeOutTimeMs = Globals::UI::fadeOutShort;

    // A number of items associated with tree hierarchy
    OwnedArray<HeadlineItem> chain;

    // A special item for `current selection` menu
    // (if present, is always shown at the tail of chain)
    UniquePointer<HeadlineItem> selectionItem;

    float getAlphaForAnimation() const noexcept;

    static constexpr auto rootNodeOffset = Globals::UI::sidebarWidth;

    UniquePointer<HeadlineNavigationPanel> navPanel;
    UniquePointer<IconButton> consoleButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Headline)
};
