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

class HeadlineItemDataSource;
class TransientTreeItem;
class TooltipContainer;
class TreeNode;
class Headline;

#include "ComponentFader.h"
#include "HotkeyScheme.h"

class MainLayout final : public Component
{
public:

    MainLayout();
    ~MainLayout() override;

    void show();
    void restoreLastOpenedPage();

    Rectangle<int> getPageBounds() const;
    static constexpr int getScrollerHeight() { return (40 + 32); }
    
    //===------------------------------------------------------------------===//
    // Pages and headline
    //===------------------------------------------------------------------===//

    void showPage(Component *page, TreeNode *source);
    bool isShowingPage(Component *page) const noexcept;

    void showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource);
    void hideSelectionMenu();

    // Posts command id recursively to all components that have non-empty command id
    void broadcastCommandMessage(int commandId);

    //===------------------------------------------------------------------===//
    // Tooltip: non-modal and can only be one at the time
    //===------------------------------------------------------------------===//

    void showTooltip(const String &message, int timeoutMs = 15000);
    void showTooltip(Component *newTooltip, Rectangle<int> callerScreenBounds, int timeoutMs = 15000);
    void hideTooltipIfAny();

    //===------------------------------------------------------------------===//
    // Modal components (like dialogs)
    //===------------------------------------------------------------------===//

    // modal components are unowned (which sucks, but we still need
    // to let modal dialogs delete themselves when they want to):
    void showModalComponentUnowned(Component *targetComponent);
    void hideModalComponentIfAny();

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void lookAndFeelChanged() override;
    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;
    void modifierKeysChanged(const ModifierKeys &modifiers) override;
    void handleCommandMessage(int commandId) override;

private:

    ComponentFader fader;
    
    ScopedPointer<Headline> headline;
    ScopedPointer<Component> initScreen;
    ScopedPointer<TooltipContainer> tooltipContainer;
    
    SafePointer<Component> currentContent;

    HotkeyScheme::Ptr hotkeyScheme;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)
};
