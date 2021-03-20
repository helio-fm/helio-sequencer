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

#include "Icons.h"
#include "DraggingListBoxComponent.h"

class IconComponent;

struct MenuItem final : public ReferenceCountedObject
{
    enum class Alignment : uint8
    {
        Left,
        Right
    };

    using Callback = Function<void()>;
    using Ptr = ReferenceCountedObjectPtr<MenuItem>;

    String commandText;
    String hotkeyText;
    String tooltipText;
    Colour colour;

    int commandId = 0;
    Icons::Id iconId = 0;

    Alignment alignment = Alignment::Left;
    Callback callback = nullptr;

    struct MenuItemFlags final
    {
        bool isToggled : 1;
        bool isDisabled : 1;
        bool shouldCloseMenu : 1;
        bool hasSubmenu : 1;
    };

    union
    {
        uint8 menuItemFlags = 0;
        MenuItemFlags flags;
    };

    MenuItem() = default;
    MenuItem::Ptr withAlignment(Alignment alignment);
    MenuItem::Ptr withSubmenu();
    MenuItem::Ptr withSubmenuIf(bool condition);
    MenuItem::Ptr withColour(const Colour &colour);
    MenuItem::Ptr withTooltip(String tooltip);
    MenuItem::Ptr toggledIf(bool shouldBeToggled);
    MenuItem::Ptr disabledIf(bool condition);
    MenuItem::Ptr closesMenu();

    // Lambdas are handy way of processing menu action,
    // however, they should only be used for menu items that don't have hotkey shortcuts:
    // for example, `back` button , or dynamic lists (like a list of colours or instruments).

    // All other menu items should use command ids,
    // which are passed to the component the same way that hotkeys pass their command id's.
    MenuItem::Ptr withAction(const Callback &lambda);

    static MenuItem::Ptr empty();
    static MenuItem::Ptr item(Icons::Id iconId, String text);
    static MenuItem::Ptr item(Icons::Id iconId, int commandId, String text = {});

    static String createTooltip(String message, int command);
    static String createTooltip(String message, KeyPress keyPress);
};

class MenuItemComponent final : public DraggingListBoxComponent
{
public:

    MenuItemComponent(Component *parentCommandReceiver,
        Viewport *parentViewport, const MenuItem::Ptr desc);

    ~MenuItemComponent();

    void setSelected(bool shouldBeSelected) override;

    void setChecked(bool shouldBeChecked);
    void update(MenuItem::Ptr description);

    Font getFont() const noexcept
    {
        return this->textLabel->getFont();
    }

    void paint(Graphics &g) override;
    void resized() override;

    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

private:

    static constexpr auto iconMargin = 6;
    static constexpr auto iconSize = 20;
    static constexpr auto fontSize = 18.f;

    Image icon;
    MenuItem::Ptr description;

    SafePointer<Component> parent;

    UniquePointer<Component> clickMarker;
    UniquePointer<Component> checkMarker;

    ComponentAnimator animator;

    Component *createHighlighterComponent() override;
    bool hasText() const noexcept;

    void showCheckMark();
    void hideCheckMark();

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered = false;

    void doAction();

    UniquePointer<Label> subLabel;
    UniquePointer<Label> textLabel;
    UniquePointer<IconComponent> submenuMarker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemComponent)
};
