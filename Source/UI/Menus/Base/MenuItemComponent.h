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

//[Headers]
#include "Icons.h"
#include "DraggingListBoxComponent.h"

class IconComponent;

struct MenuItem final : public ReferenceCountedObject
{
    enum Alignment
    {
        Left,
        Right
    };

    using Callback = Function<void()>;
    using Ptr = ReferenceCountedObjectPtr<MenuItem>;

    Image image;
    Icons::Id iconId;
    String commandText;
    String hotkeyText;
    Colour colour;
    Alignment alignment;
    int commandId;
    bool isToggled;
    bool isDisabled;
    bool shouldCloseMenu;
    bool hasSubmenu;
    bool hasTimer;
    Callback callback;

    MenuItem();
    MenuItem::Ptr withAlignment(Alignment alignment);
    MenuItem::Ptr withSubmenu();
    MenuItem::Ptr withTimer();
    MenuItem::Ptr toggled(bool shouldBeToggled);
    MenuItem::Ptr colouredWith(const Colour &colour);
    MenuItem::Ptr disabledIf(bool condition);
    MenuItem::Ptr closesMenu();

    // Lambdas are handy way of processing menu action,
    // however, they should only be used for menu items that don't have hotkey shortcuts:
    // for example, `back` button , or dynamic lists (like a list of colours or instruments).

    // All other menu items should use command ids,
    // which are passed to the component the same way that hotkeys pass their command id's.
    MenuItem::Ptr withAction(const Callback &lambda);

    static MenuItem::Ptr empty();
    static MenuItem::Ptr item(Icons::Id iconId, const String &text);
    static MenuItem::Ptr item(Icons::Id iconId, int commandId, const String &text = {});
    static MenuItem::Ptr item(Image image, int commandId, const String &text = {});
};
//[/Headers]


class MenuItemComponent final : public DraggingListBoxComponent,
                                private Timer
{
public:

    MenuItemComponent(Component *parentCommandReceiver, Viewport *parentViewport, const MenuItem::Ptr desc);
    ~MenuItemComponent();

    //[UserMethods]

    void setSelected(bool shouldBeSelected) override;
    void update(const MenuItem::Ptr description);

    Font getFont() const noexcept
    {
        return this->textLabel->getFont();
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    Image icon;
    MenuItem::Ptr description;

    SafePointer<Component> parent;

    ScopedPointer<Component> clickMarker;
    ScopedPointer<Component> toggleMarker;
    ScopedPointer<Component> colourHighlighter;

    Point<int> lastMouseScreenPosition;

    ComponentAnimator animator;

    Component *createHighlighterComponent() override;
    void timerCallback() override;
    bool hasText() const noexcept;

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered;

    void doAction();

    //[/UserVariables]

    ScopedPointer<Label> subLabel;
    ScopedPointer<Label> textLabel;
    ScopedPointer<IconComponent> submenuMarker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuItemComponent)
};
