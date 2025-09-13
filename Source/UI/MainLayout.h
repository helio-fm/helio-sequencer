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

class Headline;
class HeadlineItem;
class HeadlineItemDataSource;
class CommandPaletteCommonActions;
class TooltipContainer;
class InitScreen;
class BackForwardButtonsListener;

#include "CommandPaletteModel.h"
#include "HotkeyScheme.h"
#include "TreeNode.h"

class MainLayout final : public Component,
    public CommandPaletteModel,
    private Timer // shows tooltips after a timeout
{
public:

    MainLayout();
    ~MainLayout() override;

    void restoreLastOpenedPage();

    Rectangle<int> getBoundsForPopups() const;
    
    //===------------------------------------------------------------------===//
    // Pages and headline
    //===------------------------------------------------------------------===//

    void showPage(Component *page, TreeNode *source);
    bool isShowingPage(Component *page) const noexcept;

    HeadlineItem *getMenuTail() const;
    void showNeighbourMenu(WeakReference<HeadlineItemDataSource> origin, int delta);
    void showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource);
    void hideSelectionMenu();

    // Posts command id recursively to all components that have non-empty command id
    void broadcastCommandMessage(int commandId);

    //===------------------------------------------------------------------===//
    // Tooltip is non-modal and can only be shown one at time
    //===------------------------------------------------------------------===//

    enum class TooltipIcon : int8
    {
        None,
        Success,
        Failure
    };

    void showTooltip(const String &message,
        TooltipIcon icon = TooltipIcon::None, int delayMs = 0);
    void showTooltip(UniquePointer<Component> &&tooltipContent, int delayMs = 0);
    void hideTooltipIfAny(); // and cancel pending, if any

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void paint(Graphics &g) override {}
    void lookAndFeelChanged() override;
    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;
    void handleCommandMessage(int commandId) override;

    //===------------------------------------------------------------------===//
    // Command Palette
    //===------------------------------------------------------------------===//

    Array<CommandPaletteActionsProvider *> getCommandPaletteActionProviders() override;

private:

    void timerCallback() override;
    void showTooltipNow();
    String tooltipMessage;
    TooltipIcon tooltipIcon;
    UniquePointer<Component> tooltipContent;

    void clearInitScreen();
    UniquePointer<InitScreen> initScreen;
    friend class InitScreen;

    UniquePointer<Headline> headline;
    UniquePointer<TooltipContainer> tooltipContainer;
    
    SafePointer<Component> currentContent;
    WeakReference<TreeNode> currentProject;

    HotkeyScheme::Ptr hotkeyScheme;
    Array<Component *> visibleCommandReceivers;

    UniquePointer<CommandPaletteCommonActions> consoleCommonActions;

    UniquePointer<BackForwardButtonsListener> backForwardButtonsListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)
};
