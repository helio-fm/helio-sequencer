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
    // Tooltip is non-modal and can only be shown one at time
    //===------------------------------------------------------------------===//

    enum class TooltipType : int8
    {
        Simple,
        Success,
        Failure
    };

    void hideTooltipIfAny();
    void showTooltip(const String &message,
        TooltipType type = TooltipType::Simple,
        int timeoutMs = 15000);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void paint(Graphics &g) override {}
    void lookAndFeelChanged() override;
    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;
    void modifierKeysChanged(const ModifierKeys &modifiers) override;
    void handleCommandMessage(int commandId) override;

private:

    ComponentFader fader;
    
    UniquePointer<Headline> headline;
    UniquePointer<Component> initScreen;
    UniquePointer<TooltipContainer> tooltipContainer;
    
    SafePointer<Component> currentContent;

    HotkeyScheme::Ptr hotkeyScheme;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)
};
