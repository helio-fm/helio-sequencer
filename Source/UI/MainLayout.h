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

class NavigationSidebar;
class TransientTreeItem;
class TooltipContainer;
class Headline;

#include "LastShownTreeItems.h"
#include "ComponentFader.h"

#if HELIO_DESKTOP
#   define HAS_FADING_PAGECHANGE 1
#elif HELIO_MOBILE
#   define HAS_FADING_PAGECHANGE 0
#endif

#define TRACK_SCROLLER_HEIGHT_DEFAULT (128)
#define TRACK_SCROLLER_HEIGHT_PHONE (64)

class MainLayout : public Component
{
public:

    MainLayout();
    ~MainLayout() override;

    void init();
    void forceRestoreLastOpenedPage();
    
    void hideConsole();
    void showConsole(bool alsoShowLog);

    static int getScrollerHeight();

    
    //===------------------------------------------------------------------===//
    // Pages stack
    //===------------------------------------------------------------------===//

    LastShownTreeItems &getLastShownItems();
    WeakReference<TreeItem> getActiveTreeItem() const;
    
    void showPrevPageIfAny();
    void showNextPageIfAny();
    
    void showTransientItem(ScopedPointer<TransientTreeItem> newItem, TreeItem *parent);
    void showPage(Component *page, TreeItem *source = nullptr);

    
    //===------------------------------------------------------------------===//
    // UI
    //===------------------------------------------------------------------===//

    void setStatus(const String &text);
    void showTooltip(const String &message, int timeOutMs = 15000);
    void showTooltip(Component *newTooltip, int timeOutMs = 15000);
    void showTooltip(Component *newTooltip, Rectangle<int> callerScreenBounds, int timeOutMs = 15000);
    void showModalNonOwnedDialog(Component *targetComponent);
    void showBlockingNonModalDialog(Component *targetComponent);
    Rectangle<int> getPageBounds() const;
    
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void childBoundsChanged(Component *child) override;
    void lookAndFeelChanged() override;
    bool keyPressed(const KeyPress &key) override;

private:

    ComponentFader fader;
    
#if HAS_FADING_PAGECHANGE
    ComponentAnimator pageFader;
#endif
    
    ScopedPointer<Component> initScreen;
    SafePointer<Component> currentContent;

    ScopedPointer<Headline> headline;
    ScopedPointer<NavigationSidebar> navSidebar;
    ScopedPointer<ResizableEdgeComponent> sidebarBorder;

    ScopedPointer<TooltipContainer> tooltipContainer;
    
    LastShownTreeItems lastShownItems;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainLayout)

};
