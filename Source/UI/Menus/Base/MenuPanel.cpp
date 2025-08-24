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

#include "Common.h"
#include "MenuPanel.h"
#include "MainLayout.h"
#include "Config.h"
#include "ComponentIDs.h"

MenuPanel::MenuPanel()
{
    this->setComponentID(ComponentIDs::menu);

    this->setAccessible(false);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->listBox = make<ListBox>();
    this->addAndMakeVisible(this->listBox.get());

    MenuPanelUtils::disableKeyboardFocusForAllChildren(this);
}

void MenuPanel::resized()
{
    this->listBox->setBounds(this->getMenuBounds());

    // Parent component may reposition menu at any time,
    // need to update content bounds animation:
    if (this->menu.size() != 0)
    {
        this->updateContent(this->menu,
            this->lastAnimationType, this->shouldResizeToFitContent,
            this->defaultItemIndex, this->customFooter.get());
    }
}

void MenuPanel::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::MenuDismiss:
        this->postCommandMessageToParent(CommandIDs::DismissModalComponentAsync);
        break;
    case CommandIDs::MenuSelect:
        if (this->cursorPosition.hasValue())
        {
            this->doActionAtCursor();
        }
        break;
    case CommandIDs::MenuCursorHide:
        this->cursorPosition.reset();
        this->listBox->updateContent();
        break;
    case CommandIDs::MenuCursorUp:
        if (!this->moveCursor(-1))
        {
            this->postCommandMessageToParent(CommandIDs::MenuCursorTryExitUp);
        }
        break;
    case CommandIDs::MenuCursorDown:
        if (!this->moveCursor(1))
        {
            this->postCommandMessageToParent(CommandIDs::MenuCursorTryExitDown);
        }
        break;
    case CommandIDs::MenuCursorPageUp:
        if (!this->moveCursor(-this->listBox->getNumRowsOnScreen()))
        {
            this->postCommandMessageToParent(CommandIDs::MenuCursorTryExitUp);
        }
        break;
    case CommandIDs::MenuCursorPageDown:
        if (!this->moveCursor(this->listBox->getNumRowsOnScreen()))
        {
            this->postCommandMessageToParent(CommandIDs::MenuCursorTryExitDown);
        }
        break;
    case CommandIDs::MenuForward:
        if (this->cursorPosition.hasValue() &&
            *this->cursorPosition > 0 &&
            this->getMenuOrFiltered()[*this->cursorPosition]->hasSubmenu())
        {
            this->doActionAtCursor();
        }
        break;
    case CommandIDs::MenuBack:
        // a hack: assume that the back button is the first,
        // and it is always a lambda, never a command id:
        if (!this->menu.isEmpty() &&
            this->menu.getFirst()->iconId == Icons::back &&
            this->menu.getFirst()->callback != nullptr)
        {
            this->menu.getFirst()->callback();
        }
        break;
    default:
        // the default behavior is to pass the command from the menu item up the hierarchy,
        // but subclasses may override handleCommandMessage to process the command instead:
        this->postCommandMessageToParent(commandId);
        break;
    }
}

int MenuPanel::indexOfItemNamed(const String &nameToLookFor)
{
    auto &menuOrFiltered = this->getMenuOrFiltered();
    for (int i = 0; i < menuOrFiltered.size(); ++i)
    {
        if (menuOrFiltered.getUnchecked(i)->commandText == nameToLookFor)
        {
            return i;
        }
    }

    return -1;
}

const MenuItem::Ptr MenuPanel::getMenuItem(int index)
{
    auto &menuOrFiltered = this->getMenuOrFiltered();
    jassert(index >= 0 && index < menuOrFiltered.size());
    return menuOrFiltered[index];
}

void MenuPanel::scrollToItem(int index)
{
    jassert(index >= 0 && index < this->getMenuOrFiltered().size());
    this->listBox->scrollToEnsureRowIsOnscreen(index);
}

void MenuPanel::updateContent(const Menu &commands,
    AnimationType animationType, bool resizeToFitContent,
    int newDefaultItem, Component *newFooter)
{
    this->lastAnimationType = animationType;
    this->shouldResizeToFitContent = resizeToFitContent;
    this->defaultItemIndex = newDefaultItem;

    constexpr auto fadeInTime = Globals::UI::fadeInShort;
    constexpr auto fadeOutTime = Globals::UI::fadeOutShort;

    // If has new commands, fade out old list and create a new one
    const bool receivedNewCommands = commands != this->menu;
    if (receivedNewCommands)
    {
        this->menu = commands;
        this->filteredMenu.clearQuick();

        if (this->cursorPosition.hasValue())
        {
            this->cursorPosition = this->getDefaultCursorPosition();
            // reversed order to check the back button last:
            for (int i = this->menu.size(); i --> 0 ;)
            {
                const auto menuItem = this->menu.getUnchecked(i);
                if (this->recentCursorPositions.contains(String(i) + menuItem->getText()))
                {
                    this->cursorPosition = i;
                    break;
                }
            }
        }

        if (animationType == Fading)
        {
            this->animator.fadeOut(this->listBox.get(), fadeOutTime);
        }
        else if (animationType == SlideLeft)
        {
            const auto b = this->listBox->getBounds().translated(-this->listBox->getWidth(), 0);
            this->animator.animateComponent(this->listBox.get(), b, 1.f, fadeOutTime, true, 1.0, 0.0);
        }
        else if (animationType == SlideRight)
        {
            const auto b = this->listBox->getBounds().translated(this->listBox->getWidth(), 0);
            this->animator.animateComponent(this->listBox.get(), b, 1.f, fadeOutTime, true, 1.0, 0.0);
        }
        else if (animationType == SlideUp)
        {
            const auto b = this->listBox->getBounds().translated(0, -this->listBox->getHeight());
            this->animator.animateComponent(this->listBox.get(), b, 1.f, fadeOutTime, true, 1.0, 0.0);
        }
        else if (animationType == SlideDown)
        {
            const auto b = this->listBox->getBounds().translated(0, this->listBox->getHeight());
            this->animator.animateComponent(this->listBox.get(), b, 1.f, fadeOutTime, true, 1.0, 0.0);
        }

        this->removeChildComponent(this->listBox.get());

        this->listBox = make<ListBox>();
        this->listBox->setModel(this);
        this->listBox->setMultipleSelectionEnabled(false);
        this->listBox->setRowHeight(Globals::UI::menuPanelRowHeight);
        this->listBox->updateContent();
        this->addAndMakeVisible(this->listBox.get());
    }
    else
    {
        // clear the filters anyway
        this->filteredMenu.clearQuick();
        this->listBox->updateContent();
    }

    if (this->customFooter.get() != newFooter)
    {
        this->customFooter.reset(newFooter);
        this->addAndMakeVisible(this->customFooter.get());
    }

    // If no new commands received, just update the animation:
    if (animationType == Fading)
    {
        this->listBox->setBounds(this->getMenuBounds());
        this->animator.fadeIn(this->listBox.get(), fadeInTime);

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds());
            this->animator.fadeIn(this->customFooter.get(), fadeInTime);
        }
    }
    else if (animationType == SlideLeft)
    {
        this->listBox->setBounds(this->getMenuBounds().translated(this->getWidth(), 0));
        this->animator.animateComponent(this->listBox.get(),
            this->getMenuBounds(), 1.f, fadeInTime, false, 1.0, 0.0);

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds().translated(this->getWidth(), 0));
            this->animator.animateComponent(this->customFooter.get(),
                this->getFooterBounds(), 1.f, fadeInTime, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideRight)
    {
        this->listBox->setBounds(this->getMenuBounds().translated(-this->getWidth(), 0));
        this->animator.animateComponent(this->listBox.get(),
            this->getMenuBounds(), 1.f, fadeInTime, false, 1.0, 0.0);

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds().translated(-this->getWidth(), 0));
            this->animator.animateComponent(this->customFooter.get(),
                this->getFooterBounds(), 1.f, fadeInTime, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideUp)
    {
        this->listBox->setBounds(this->getMenuBounds().translated(0, this->getHeight()));
        this->animator.animateComponent(this->listBox.get(),
            this->getMenuBounds(), 1.f, fadeInTime, false, 1.0, 0.0);

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds().translated(0, this->getHeight()));
            this->animator.animateComponent(this->customFooter.get(),
                this->getFooterBounds(), 1.f, fadeInTime, false, 1.0, 0.0);
        }
    }
    else if (animationType == SlideDown)
    {
        this->listBox->setBounds(this->getMenuBounds().translated(0, -this->getHeight()));
        this->animator.animateComponent(this->listBox.get(),
            this->getMenuBounds(), 1.f, fadeInTime, false, 1.0, 0.0);

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds().translated(0, -this->getHeight()));
            this->animator.animateComponent(this->customFooter.get(),
                this->getFooterBounds(), 1.f, fadeInTime, false, 1.0, 0.0);
        }
    }
    else
    {
        this->listBox->setBounds(this->getMenuBounds());

        if (this->customFooter.get() != nullptr)
        {
            this->customFooter->setBounds(this->getFooterBounds());
        }
    }

    this->scrollToDefaultItemIfAny();

    if (this->shouldResizeToFitContent && receivedNewCommands)
    {
        const Font stringFont(MenuItemComponent::fontSize);

        int estimatedWidth = 0;
        for (const auto &command : commands)
        {
            constexpr auto buttonsMargin = MenuItemComponent::iconSize;
            const int buttonsWidth =
                (command->buttons.size() * MenuItemComponent::iconSize) +
                (command->buttons.size() > 0 ? buttonsMargin : 0);
            const int stringWidth =
                stringFont.getStringWidth(command->commandText) +
                stringFont.getStringWidth(command->hotkeyText);

            estimatedWidth = jmax(estimatedWidth, stringWidth + buttonsWidth);
        }

        constexpr auto widthMargin = 69;
        const int newWidth = jmax(estimatedWidth + widthMargin, this->getWidth());

        const int menuHeight = jmax(this->getHeight(),
            commands.size() * Globals::UI::menuPanelRowHeight);
        const int maxMenuHeight = Globals::UI::menuPanelRowHeight * 16; // hard-coded for now
        const int newHeight = jmin(menuHeight, maxMenuHeight) + this->getFooterHeight();

        this->setSize(newWidth, newHeight);
    }

    MenuPanelUtils::disableKeyboardFocusForAllChildren(this);
}

void MenuPanel::setDefaultItem(int newDefaultItem)
{
    this->defaultItemIndex = newDefaultItem;

    // also make sure that the cursor appears at defaultItemIndex
    this->cursorPosition.reset();

    this->updateContent(this->menu,
        this->lastAnimationType, this->shouldResizeToFitContent,
        this->defaultItemIndex, this->customFooter.get());
}

void MenuPanel::applyFilter(const String &text)
{
    this->cursorPosition.reset();

    this->filteredMenu.clearQuick();

    if (text.isNotEmpty())
    {
        for (const auto item : this->menu)
        {
            if (item->commandText.containsIgnoreCase(text))
            {
                this->filteredMenu.add(item);
            }
        }
    }

    this->listBox->updateContent();

    if (this->filteredMenu.isEmpty())
    {
        this->scrollToDefaultItemIfAny();
    }
    else
    {
        this->listBox->getViewport()->setViewPosition(
            this->listBox->getViewport()->getViewPositionX(), 0);
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

bool MenuPanel::moveCursor(int delta)
{
    jassert(delta != 0);
    if (!this->cursorPosition.hasValue())
    {
        this->cursorPosition = this->getDefaultCursorPosition();
    }
    else
    {
        if ((delta < 0 && this->cursorPosition == 0) ||
            (delta > 0 && this->cursorPosition == this->getNumRows() - 1))
        {
            return false;
        }

        this->cursorPosition =
            jlimit(0, this->getNumRows() - 1, (*this->cursorPosition + delta));
    }

    this->listBox->scrollToEnsureRowIsOnscreen(*this->cursorPosition);
    this->listBox->updateContent();
    return true;
}

void MenuPanel::doActionAtCursor()
{
    if (!this->cursorPosition.hasValue())
    {
        jassertfalse;
        return;
    }

    const auto cursorRowIndex = *this->cursorPosition;
    this->listBox->scrollToEnsureRowIsOnscreen(cursorRowIndex);
    auto *rowComponent = this->listBox->getComponentForRowNumber(cursorRowIndex);
    auto *menuItemComponent = dynamic_cast<MenuItemComponent *>(rowComponent);
    if (menuItemComponent == nullptr)
    {
        jassertfalse;
        return;
    }

    for (int i = this->menu.size(); i --> 0 ;)
    {
        const auto menuItem = this->menu.getUnchecked(i);
        this->recentCursorPositions.erase(String(i) + menuItem->getText());
    }
    this->recentCursorPositions[String(cursorRowIndex) + menuItemComponent->getText()] = cursorRowIndex;

    menuItemComponent->doAction();
}

int MenuPanel::getDefaultCursorPosition() const
{
    if (!this->filteredMenu.isEmpty())
    {
        return 0;
    }

    return jlimit(0, this->menu.size() - 1, this->defaultItemIndex);
}

void MenuPanel::postCommandMessageToParent(int commandId)
{
    if (this->getParentComponent() != nullptr)
    {
        this->getParentComponent()->postCommandMessage(commandId);
    }
}

void MenuPanel::scrollToDefaultItemIfAny()
{
    if (this->defaultItemIndex >= 0 &&
        this->defaultItemIndex < this->menu.size())
    {
        // instead of using this
        //this->listBox->scrollToEnsureRowIsOnscreen(this->defaultItemIndex);
        // center the view on the default item:
        auto *listViewport = this->listBox->getViewport();
        const auto rowHeight = this->listBox->getRowHeight();
        listViewport->setViewPosition(listViewport->getViewPositionX(),
            jmax(0, (this->defaultItemIndex * rowHeight) -
            (listViewport->getViewHeight() / 2) +
                rowHeight / 2));
    }
}

inline MenuPanel::Menu &MenuPanel::getMenuOrFiltered()
{
    return this->filteredMenu.isEmpty() ? this->menu : this->filteredMenu;
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int MenuPanel::getNumRows()
{
    return this->getMenuOrFiltered().size();
}

Component *MenuPanel::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    auto &menuToShow = this->getMenuOrFiltered();

    if (rowNumber >= menuToShow.size())
    {
        return existingComponentToUpdate;
    }

    const auto itemDescription = menuToShow[rowNumber];
    const auto isDisplayedAsCurrent =
        this->filteredMenu.isEmpty() && (rowNumber == this->defaultItemIndex);
    const auto isCursorShown =
        this->cursorPosition.hasValue() && *this->cursorPosition == rowNumber;

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->setCursorShown(isCursorShown);
            row->setDisplayedAsCurrent(isDisplayedAsCurrent);
            row->update(itemDescription);
        }
    }
    else
    {
        auto *row = new MenuItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        row->setCursorShown(isCursorShown);
        row->setDisplayedAsCurrent(isDisplayedAsCurrent);
        return row;
    }

    return existingComponentToUpdate;
}

Rectangle<int> MenuPanel::getMenuBounds() const
{
    const auto maxHeight = App::Layout().getBoundsForPopups().getHeight();
    return this->getLocalBounds().withHeight(jmin(maxHeight, this->getHeight()) - this->getFooterHeight());
}

Rectangle<int> MenuPanel::getFooterBounds() const
{
    const int fh = this->getFooterHeight();
    return { 0, this->getHeight() - fh, this->getWidth(), fh };
}

int MenuPanel::getFooterHeight() const
{
    return (this->customFooter != nullptr) ? this->customFooter->getHeight() : 0;
}
