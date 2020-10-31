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

#include "Common.h"
#include "MobileComboBox.h"
#include "PanelBackgroundC.h"

MobileComboBox::MobileComboBox(WeakReference<Component> editor, WeakReference<Component> primer) :
    editor(editor),
    primer(primer)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->background = make<Component>();
    this->addAndMakeVisible(this->background.get());

    this->menu = make<MenuPanel>();
    this->addAndMakeVisible(this->menu.get());

    this->triggerButton = make<MobileComboBox::Trigger>();
    this->addAndMakeVisible(this->triggerButton.get());

    this->separator = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(this->separator.get());

    this->currentNameLabel = make<Label>();
    this->addAndMakeVisible(this->currentNameLabel.get());
    this->currentNameLabel->setFont({ 21.f });
    this->currentNameLabel->setJustificationType(Justification::centredLeft);

    this->searchTextBox = make<TextEditor>();
    this->addAndMakeVisible(this->searchTextBox.get());
    this->searchTextBox->setFont({ 21.f });
    this->searchTextBox->setMultiLine(false);
    this->searchTextBox->setReturnKeyStartsNewLine(false);
    this->searchTextBox->setScrollbarsShown(true);
    this->searchTextBox->setPopupMenuEnabled(false);
    this->searchTextBox->setCaretVisible(true);
    this->searchTextBox->setEnabled(true);

    this->searchTextBox->onEscapeKey = [this]()
    {
        this->postCommandMessage(CommandIDs::ToggleShowHideCombo);
    };
    
    this->searchTextBox->onTextChange = [this]()
    {
        this->menu->applyFilter(this->searchTextBox->getText());
    };
}

void MobileComboBox::resized()
{
    static constexpr auto menuY = 32;

    if (this->isSimpleDropdown())
    {
        this->background->setBounds(0, menuY, this->getWidth(), this->getHeight() - menuY);
    }
    else
    {
        this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
    }

    this->separator->setBounds(1, menuY - 2, this->getWidth() - 2, 2);
    this->currentNameLabel->setBounds(0, 0, this->getWidth() - 0, menuY - 2);

    this->menu->setBounds(2, menuY, this->getWidth() - 4, this->getHeight() - menuY);
    this->searchTextBox->setBounds(1, 1, this->getWidth() - 2, menuY - 2);

    // a hack to prevent sending `resized` message to menu
    // and thus to prevent it from starting its animation,
    // until my own animation is complete:
    if (this->primer == nullptr ||
        this->getLocalBounds() != this->primer->getLocalBounds())
    {
        this->menu->setBounds(0, 0, 0, 0);
    }
}

void MobileComboBox::parentHierarchyChanged()
{
    if (this->primer != nullptr)
    {
        this->setBounds(this->primer->getBounds());
    }
}

void MobileComboBox::parentSizeChanged()
{
    if (this->primer != nullptr)
    {
        this->setBounds(this->primer->getBounds());
    }
}

void MobileComboBox::handleCommandMessage(int commandId)
{
    if (this->getParentComponent() != nullptr && this->editor != nullptr)
    {
        if (commandId != CommandIDs::ToggleShowHideCombo)
        {
            this->getParentComponent()->postCommandMessage(commandId);
        }

        this->animator.animateComponent(this,
            this->isSimpleDropdown() ? this->getBounds() : this->editor->getBounds(),
            0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);

        this->getParentComponent()->removeChildComponent(this);
    }
}

void MobileComboBox::initMenu(MenuPanel::Menu menu)
{
    this->menu->updateContent(menu);
}

void MobileComboBox::initHeader(TextEditor *editor, bool hasSearch, bool hasCaption)
{
    this->searchTextBox->setFont(editor->getFont());
    this->currentNameLabel->setFont(editor->getFont());

    this->initHeader(editor->getText(), hasSearch, hasCaption);
}

void MobileComboBox::initHeader(Label *label, bool hasSearch, bool hasCaption)
{
    this->searchTextBox->setFont(label->getFont());
    this->currentNameLabel->setFont(label->getFont());

    this->initHeader(label->getText(), hasSearch, hasCaption);
}

void MobileComboBox::initHeader(const String &text, bool hasSearch, bool hasCaption)
{
    this->setInterceptsMouseClicks(hasSearch || hasCaption, true);

    this->searchTextBox->setText({}, dontSendNotification);
    this->currentNameLabel->setText(text, dontSendNotification);

    // on mobile, just show label: search box is not very convenient
#if HELIO_DESKTOP
    this->searchTextBox->setVisible(hasSearch);
    this->currentNameLabel->setVisible(hasCaption);
#elif HELIO_MOBILE
    this->searchTextBox->setVisible(false);
    this->currentNameLabel->setVisible(hasSearch || hasCaption);
#endif

    if (this->searchTextBox->isVisible())
    {
        this->searchTextBox->grabKeyboardFocus();
    }
}

void MobileComboBox::initBackground(Component *newCustomBackground)
{
    this->background.reset(newCustomBackground != nullptr ?
        newCustomBackground : new PanelBackgroundC());
    this->addAndMakeVisible(this->background.get());
    this->background->toBack();
}

MobileComboBox::Primer::Primer()
{
    this->setInterceptsMouseClicks(false, false);
}

MobileComboBox::Primer::~Primer()
{
    this->cleanup();
}

void MobileComboBox::Primer::initWith(WeakReference<Component> editor,
    MenuPanel::Menu menu, Component *newCustomBackground)
{
    this->toFront(false);
    this->textEditor = editor;
    this->combo = make<MobileComboBox>(editor, this);
    this->combo->initBackground(newCustomBackground);
    this->combo->initMenu(menu);
    this->comboTrigger = make<MobileComboBox::Trigger>(this);

    if (this->textEditor != nullptr)
    {
        this->textEditor->addAndMakeVisible(this->comboTrigger.get());
    }
}

void MobileComboBox::Primer::initWith(WeakReference<Component> textEditor,
    Function<MenuPanel::Menu(void)> menuInitializer,
    Component *newCustomBackground /*= nullptr*/)
{
    this->menuInitializer = menuInitializer;
    this->initWith(textEditor, MenuPanel::Menu(), newCustomBackground);
}

void MobileComboBox::Primer::updateMenu(MenuPanel::Menu menu)
{
    this->combo->initMenu(menu);
}

void MobileComboBox::Primer::cleanup()
{
    this->comboTrigger = nullptr;
}

void MobileComboBox::Primer::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::ToggleShowHideCombo &&
        this->getParentComponent() != nullptr &&
        this->combo != nullptr)
    {
        // Deferred menu initialization, if needed
        if (this->menuInitializer != nullptr)
        {
            this->combo->initMenu(this->menuInitializer());
            this->menuInitializer = nullptr;
        }

        const bool hasSearchBox = this->combo->menu->getMenuSize() > 20;
        const bool hasCaptionLabel = !hasSearchBox;

        this->getParentComponent()->addAndMakeVisible(this->combo.get());

        if (auto *ed = dynamic_cast<TextEditor *>(this->textEditor.get()))
        {
            this->combo->initHeader(ed, hasSearchBox, hasCaptionLabel);
        }
        else if (auto *label = dynamic_cast<Label *>(this->textEditor.get()))
        {
            this->combo->initHeader(label, hasSearchBox, hasCaptionLabel);
        }
        else
        {
            this->combo->initHeader(String(), false, false);
        }

        this->combo->setAlpha(1.f);
        this->combo->setBounds(this->textEditor->getBounds());
        this->animator.animateComponent(this->combo.get(),
            this->getBounds(), 1.f, Globals::UI::fadeInLong, false, 1.0, 0.0);
    }
}

MobileComboBox::Trigger::Trigger(WeakReference<Component> listener) :
    IconButton(Icons::findByName(Icons::down, 16),
        CommandIDs::ToggleShowHideCombo, listener) {}

void MobileComboBox::Trigger::parentHierarchyChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::parentSizeChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::updateBounds()
{
    if (const auto *parent = this->getParentComponent())
    {
        static constexpr auto triggerSize = 32;

        const int w = triggerSize * 2;
        const int h = triggerSize;
        const int x = parent->getWidth() - w / 2 - triggerSize / 2;
        const int y = 0;

        this->setBounds(x, y, w, h);
        this->setAlwaysOnTop(true);
        this->toFront(false);
    }
}
