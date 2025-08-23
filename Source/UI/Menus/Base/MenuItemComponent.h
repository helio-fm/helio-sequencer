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

#include "Icons.h"
#include "IconButton.h"
#include "DraggingListBoxComponent.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

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
    Icons::Id iconId = Icons::empty;

    Alignment textAlignment = Alignment::Left;
    Callback callback = nullptr;

    struct Button
    {
        bool isEnabled = true;
        Icons::Id iconId = 0;
        Callback callback = nullptr;
    };

    Array<Button> buttons;

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

    float weight = 0.f;
    struct SortByWeight final
    {
        int compareElements(const MenuItem::Ptr first, const MenuItem::Ptr second) const
        {
            const float diff = first->weight - second->weight;
            return (diff > 0.f) - (diff < 0.f);
        }
    };

    MenuItem() = default;
    MenuItem::Ptr withAlignment(Alignment alignment);
    MenuItem::Ptr withSubmenu();
    MenuItem::Ptr withSubmenuIf(bool condition);
    MenuItem::Ptr withColour(const Colour &colour);
    MenuItem::Ptr withTooltip(String tooltip);
    MenuItem::Ptr withHotkeyText(int commandId);
    MenuItem::Ptr withSubtitle(const String &string);
    MenuItem::Ptr withWeight(float weight);
    MenuItem::Ptr toggledIf(bool shouldBeToggled);
    MenuItem::Ptr disabledIf(bool condition);
    MenuItem::Ptr closesMenu();

    bool hasSubmenu() const noexcept;
    const String &getText() const noexcept;

    // Lambdas are handy way of processing menu action,
    // however, they should only be used for menu items that don't have hotkey shortcuts:
    // for example, `back` button , or dynamic lists (like a list of colours or instruments).

    // All other menu items should use command ids,
    // which are passed to the component the same way that hotkeys pass their command id's.
    MenuItem::Ptr withAction(const Callback &lambda);
    MenuItem::Ptr withActionIf(bool condition, const Callback &lambda);

    // Each menu item can have several helper buttons
    MenuItem::Ptr withButton(bool isEnabled, Icons::Id icon, const Callback &lambda);

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
    void setCursorShown(bool shouldBeShown);
    void setChecked(bool shouldBeChecked);

    void update(MenuItem::Ptr description);

    Font getFont() const noexcept
    {
        return this->textLabel->getFont();
    }

    const String &getText() const noexcept
    {
        return this->description->getText();
    }

    void paint(Graphics &g) override;
    void resized() override;

    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

    void handleCommandMessage(int commandId) override;

    static constexpr auto iconMargin = 6;
    static constexpr auto iconSize = 20;
    static constexpr auto fontSize = Globals::UI::Fonts::M;

    void doAction();

private:

    Image icon;
    MenuItem::Ptr description = MenuItem::empty();

    SafePointer<Component> parent;

    UniquePointer<Component> clickMarker;
    UniquePointer<Component> checkMarker;
    UniquePointer<Component> cursorMarker;

    Array<UniquePointer<IconButton>> buttons;

    ComponentFader animator;

    Component *createHighlighterComponent() override;

    bool hasText() const noexcept;
    inline bool isIconCentered() const noexcept
    {
        return !this->hasText();
    }

    void showCheckMark();
    void hideCheckMark();

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered = false;

    UniquePointer<Label> subLabel;
    UniquePointer<Label> textLabel;
    UniquePointer<IconComponent> submenuMarker;

    const Colour borderLightColour =
        findDefaultColour(ColourIDs::Common::borderLineLight).
            withMultipliedAlpha(0.33f);
    const Colour borderDarkColour =
        findDefaultColour(ColourIDs::Common::borderLineDark).
            withMultipliedAlpha(0.33f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemComponent)
};
