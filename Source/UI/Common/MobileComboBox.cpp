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
#include "MenuPanel.h"
#include "PanelBackgroundC.h"

MobileComboBox::MobileComboBox(WeakReference<Component> editor, WeakReference<Component> primer) :
    editor(editor),
    primer(primer)
{
    this->background = make<Component>();
    this->addAndMakeVisible(background.get());

    this->menu = make<MenuPanel>();
    this->addAndMakeVisible(menu.get());

    this->shadow = make<ShadowDownwards>(ShadowType::Light);
    this->addAndMakeVisible(shadow.get());

    this->separator = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(separator.get());

    this->currentNameLabel = make<Label>(String(), String());
    this->addAndMakeVisible(currentNameLabel.get());

    this->triggerButtton = make<MobileComboBox::Trigger>();
    this->addAndMakeVisible(triggerButtton.get());

    this->currentNameLabel->setFont(Font (21.00f, Font::plain));
    this->currentNameLabel->setJustificationType(Justification::centredLeft);
    this->currentNameLabel->setEditable(false, false, false);

    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);
}

MobileComboBox::~MobileComboBox()
{
    this->background = nullptr;
    this->menu = nullptr;
    this->triggerButtton = nullptr;
    this->shadow = nullptr;
    this->separator = nullptr;
    this->currentNameLabel = nullptr;
}

void MobileComboBox::resized()
{
    static constexpr auto menuY = 34;
    static constexpr auto triggerSize = 32;
    static constexpr auto shadowHeight = 16;

    if (this->hasCaption)
    {
        this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
    }
    else
    {
        this->background->setBounds(0, menuY, this->getWidth(), this->getHeight() - menuY);
    }

    this->menu->setBounds(2, menuY, this->getWidth() - 4, getHeight() - menuY);
    this->triggerButtton->setBounds(this->getWidth() - triggerSize, 0, triggerSize, triggerSize);
    this->shadow->setBounds(1, menuY - 1, this->getWidth() - 2, shadowHeight);
    this->separator->setBounds(1, menuY - 2, this->getWidth() - 2, 2);
    this->currentNameLabel->setBounds(0, 0, this->getWidth() - 0, menuY - 2);

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

void MobileComboBox::handleCommandMessage (int commandId)
{
    if (this->getParentComponent() != nullptr && this->editor != nullptr)
    {
        if (commandId != CommandIDs::ToggleShowHideCombo)
        {
            this->getParentComponent()->postCommandMessage(commandId);
        }

        this->animator.animateComponent(this, this->editor->getBounds(), 0.f, 150, true, 0.0, 1.0);
        this->getParentComponent()->removeChildComponent(this);
    }
}

void MobileComboBox::initMenu(MenuPanel::Menu menu)
{
    this->menu->updateContent(menu);
}

void MobileComboBox::initCaption(TextEditor *editor)
{
    this->currentNameLabel->setFont(editor->getFont());
    this->initCaption(editor->getText());
}

void MobileComboBox::initCaption(Label *label)
{
    this->currentNameLabel->setFont(label->getFont());
    this->initCaption(label->getText());
}

void MobileComboBox::initCaption(const String &text)
{
    this->hasCaption = text.isNotEmpty();
    this->setInterceptsMouseClicks(this->hasCaption, true);
    this->currentNameLabel->setVisible(this->hasCaption);
    this->currentNameLabel->setText(text, dontSendNotification);
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
    this->combo->initMenu(menu);
    this->combo->initBackground(newCustomBackground);
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

        // Show combo
        this->combo->setAlpha(1.f);
        if (auto *ed = dynamic_cast<TextEditor *>(this->textEditor.get()))
        {
            this->combo->initCaption(ed);
        }
        else if (auto *label = dynamic_cast<Label *>(this->textEditor.get()))
        {
            this->combo->initCaption(label);
        }
        else
        {
            this->combo->initCaption(String());
        }

        // todo mofo!!! search!

        this->getParentComponent()->addAndMakeVisible(this->combo.get());
        this->combo->setBounds(this->textEditor->getBounds());
        this->animator.animateComponent(this->combo.get(), this->getBounds(), 1.f, 150, false, 1.0, 0.0);
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
        const int w = 64;
        const int h = 32;
        const int x = parent->getWidth() - w / 2 - 16;
        const int y = 0;
        this->setBounds(x, y, w, h);
        this->setAlwaysOnTop(true);
        this->toFront(false);
    }
}
