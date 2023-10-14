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

#include "IconButton.h"
#include "MenuPanel.h"
#include "SeparatorHorizontalReversed.h"

class MobileComboBox final : public Component
{
public:

    MobileComboBox(WeakReference<Component> editor,
        WeakReference<Component> area);

    void initMenu(MenuPanel::Menu menu);

    class Trigger final : public IconButton
    {
    public:

        Trigger(WeakReference<Component> listener = nullptr);
        void parentHierarchyChanged() override;
        void parentSizeChanged() override;
        void updateBounds();

    private:

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trigger)
    };

    class Container final : public Component
    {
    public:

        Container();
        ~Container() override;

        void handleCommandMessage(int commandId) override;

        void initWith(WeakReference<Component> textEditor,
            MenuPanel::Menu menu,
            Component *newCustomBackground = nullptr);

        void initWith(WeakReference<Component> textEditor,
            Function<MenuPanel::Menu(void)> menuInitializer,
            Component *newCustomBackground = nullptr);

        void updateMenu(MenuPanel::Menu menu);

        // this needs to be called before target text editor is deleted
        void cleanup();

    private:

        ComponentAnimator animator;
        UniquePointer<MobileComboBox> combo;
        UniquePointer<MobileComboBox::Trigger> comboTrigger;
        WeakReference<Component> textEditor;
        Function<MenuPanel::Menu(void)> menuInitializer;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Container)
    };

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    void initHeader(TextEditor *editor, bool hasSearch, bool hasCaption);
    void initHeader(Label *label, bool hasSearch, bool hasCaption);
    void initHeader(const String &text, bool hasSearch, bool hasCaption);

    void initBackground(Component *newCustomBackground);

    bool isSimpleDropdown() const noexcept
    {
        return !this->searchTextBox->isVisible() &&
            !this->currentNameLabel->isVisible();
    }

    WeakReference<Component> container;
    WeakReference<Component> editor;
    ComponentAnimator animator;

    UniquePointer<TextEditor> searchTextBox;
    UniquePointer<SeparatorHorizontalReversed> separator;
    UniquePointer<Label> currentNameLabel;

    UniquePointer<Component> background;
    UniquePointer<MenuPanel> menu;
    UniquePointer<MobileComboBox::Trigger> triggerButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MobileComboBox)
};
