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
class TreeItem;
class Headline;

#include "ComponentFader.h"
#include "HotkeyScheme.h"

#if HELIO_MOBILE
#   define HEADLINE_HEIGHT (34)
#elif HELIO_DESKTOP
#   define HEADLINE_HEIGHT (64)
#endif

class MainLayout final : public Component
{
public:

    MainLayout();
    ~MainLayout() override;

    void show();
    void forceRestoreLastOpenedPage();

    Rectangle<int> getPageBounds() const;
    static constexpr int getScrollerHeight() { return (40 + 32); }
    
    //===------------------------------------------------------------------===//
    // Pages and headline
    //===------------------------------------------------------------------===//

    void showPage(Component *page, TreeItem *source);
    bool isShowingPage(Component *page) const noexcept;

    void showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource);
    void hideSelectionMenu();

    // Posts command id recursively to all components that have non-empty command id
    void broadcastCommandMessage(int commandId);

    //===------------------------------------------------------------------===//
    // UI
    //===------------------------------------------------------------------===//

    // FIXME: these ones assume that modal component will delete itself eventually
    // which sucks, please rework into some better ownership model:
    void showTooltip(const String &message, int timeOutMs = 15000);
    void showTooltip(Component *newTooltip, int timeOutMs = 15000);
    void showTooltip(Component *newTooltip, Rectangle<int> callerScreenBounds, int timeOutMs = 15000);
    void showModalComponentUnowned(Component *targetComponent);

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
    
    ScopedPointer<Component> initScreen;
    SafePointer<Component> currentContent;

    ScopedPointer<Headline> headline;

    ScopedPointer<TooltipContainer> tooltipContainer;
    
    HotkeyScheme::Ptr hotkeyScheme;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)

};
