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
#include "NavigationHistory.h"
#include "HeadlineNavigationPanel.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class IconButton;
class HeadlineItem;
class HeadlineItemDataSource;

class Headline final : public Component,
    public AsyncUpdater // called when any child needs to be updated
{
public:

    Headline();
    ~Headline();

    static constexpr auto itemsOverlapOffset = 11;

    void syncWithTree(NavigationHistory &history, WeakReference<TreeNode> leaf);

    void showNeighbourMenu(WeakReference<HeadlineItemDataSource> origin, int delta);
    void showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource);
    void hideSelectionMenu();

    HeadlineItem *getTailItem() const noexcept;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void resized() override;
    void handleCommandMessage(int commandId) override;

private:

    // A way to receive a single coalesced update from multiple signaling sub-items:
    void handleAsyncUpdate() override;
    int rebuildChain(WeakReference<TreeNode> leaf);
    int getChainWidth() const noexcept;

    ComponentFader animator;

    OwnedArray<HeadlineItem> chain;

    // A special item for `current selection` menus
    UniquePointer<HeadlineItem> selectionItem;

    float getAlphaForAnimation() const noexcept;

    static constexpr auto rootNodeOffset = Globals::UI::sidebarWidth;

    const Colour borderLightColour =
        findDefaultColour(ColourIDs::Common::borderLineLight);
    const Colour borderDarkColour =
        findDefaultColour(ColourIDs::Common::borderLineDark);

    UniquePointer<HeadlineNavigationPanel> navPanel;
    UniquePointer<IconButton> consoleButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Headline)
};
