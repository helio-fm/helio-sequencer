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
#include "MobileComboBox.h"
#include "PanelBackground.h"
#include "HelioTheme.h"

MobileComboBox::MobileComboBox(WeakReference<Component> editor, WeakReference<Component> container) :
    editor(editor),
    container(container)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->background = make<Component>();
    this->addAndMakeVisible(this->background.get());

    this->menu = make<MenuPanel>();
    this->addAndMakeVisible(this->menu.get());

    this->separator = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(this->separator.get());

    this->currentNameLabel = make<Label>();
    this->addAndMakeVisible(this->currentNameLabel.get());
    this->currentNameLabel->setFont(Globals::UI::Fonts::M);
    this->currentNameLabel->setJustificationType(Justification::centredLeft);

    this->searchTextBox = HelioTheme::makeSingleLineTextEditor(true);
    this->searchTextBox->setPopupMenuEnabled(false);
    this->addAndMakeVisible(this->searchTextBox.get());

    this->triggerButton = make<MobileComboBox::Trigger>(this);

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
    static constexpr auto menuY = Globals::UI::textEditorHeight + 2;

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
    this->searchTextBox->setBounds(1, 1, this->getWidth() - 2, Globals::UI::textEditorHeight);

    // a hack to prevent sending `resized` message to menu
    // and thus to prevent it from starting its animation,
    // until my own animation is complete:
    if (this->container == nullptr ||
        this->getLocalBounds() != this->container->getLocalBounds())
    {
        this->menu->setBounds(0, 0, 0, 0);
    }
}

void MobileComboBox::parentHierarchyChanged()
{
    if (this->container != nullptr)
    {
        this->setBounds(this->container->getBounds());
    }
}

void MobileComboBox::parentSizeChanged()
{
    if (this->container != nullptr)
    {
        this->setBounds(this->container->getBounds());
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
    this->searchTextBox->setIndents(editor->getLeftIndent(), editor->getTopIndent());
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
    this->currentNameLabel->setInterceptsMouseClicks(false, true);

    // on mobile, just show the label: search box is not very convenient
#if PLATFORM_DESKTOP
    this->searchTextBox->setVisible(hasSearch);
    this->currentNameLabel->setVisible(hasCaption);
    if (hasCaption)
    {
        this->currentNameLabel->addAndMakeVisible(this->triggerButton.get());
    }
    else
    {
        this->searchTextBox->addAndMakeVisible(this->triggerButton.get());
    }
#elif PLATFORM_MOBILE
    this->searchTextBox->setVisible(false);
    this->currentNameLabel->setVisible(hasSearch || hasCaption);
    this->currentNameLabel->addAndMakeVisible(this->triggerButton.get());
#endif

    if (this->searchTextBox->isVisible())
    {
        this->searchTextBox->grabKeyboardFocus();
    }
}

void MobileComboBox::initBackground(Component *newCustomBackground)
{
    this->background.reset(newCustomBackground != nullptr ?
        newCustomBackground : new PanelBackground());
    this->addAndMakeVisible(this->background.get());
    this->background->toBack();
}

MobileComboBox::Container::Container()
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setMouseClickGrabsKeyboardFocus(false);
}

MobileComboBox::Container::~Container()
{
    this->cleanup();
}

void MobileComboBox::Container::initWith(WeakReference<Component> editor,
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

void MobileComboBox::Container::initWith(WeakReference<Component> textEditor,
    Function<MenuPanel::Menu(void)> menuInitializer,
    Component *newCustomBackground /*= nullptr*/)
{
    this->menuInitializer = menuInitializer;
    this->initWith(textEditor, MenuPanel::Menu(), newCustomBackground);
}

void MobileComboBox::Container::updateMenu(MenuPanel::Menu menu)
{
    this->combo->initMenu(menu);
}

void MobileComboBox::Container::cleanup()
{
    this->comboTrigger = nullptr;
}

void MobileComboBox::Container::handleCommandMessage(int commandId)
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

        const bool hasSearchBox =
            this->combo->menu->getMenuSize() > 16 ||
            this->combo->menu->getMenuHeight() > this->getHeight();

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
    IconButton(Icons::findByName(Icons::down, Trigger::iconSize),
        CommandIDs::ToggleShowHideCombo, listener) {}

void MobileComboBox::Trigger::parentHierarchyChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::parentSizeChanged()
{
    this->updateBounds();
}

void MobileComboBox::Trigger::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    // a hack to pass wheel to the grandparent (parent is often just a text editor)
    auto *parent = this->getParentComponent();
    if (parent == nullptr)
    {
        return;
    }

    auto *grandParent = parent->getParentComponent();
    if (grandParent != nullptr)
    {
        grandParent->mouseWheelMove(e, wheel);
    }
}

void MobileComboBox::Trigger::paint(Graphics &g)
{
    g.setColour(this->colour.withMultipliedAlpha(this->alpha));

    if (this->image.isNull())
    {
        const Image i(Icons::findByName(this->iconId, Trigger::iconSize));

        Icons::drawImageRetinaAware(i, g,
            this->getWidth() - (Trigger::triggerSize / 2), this->getHeight() / 2);
    }
    else
    {
        Icons::drawImageRetinaAware(this->image, g,
            this->getWidth() - (Trigger::triggerSize / 2), this->getHeight() / 2);
    }
}

void MobileComboBox::Trigger::updateBounds()
{
    const auto *parent = this->getParentComponent();
    if (parent == nullptr)
    {
        return;
    }

    bool allowsClicksOnSelf = false;
    bool allowsClicksOnChildren = false;
    parent->getInterceptsMouseClicks(allowsClicksOnSelf, allowsClicksOnChildren);
    const auto maxParentHeight = jmin(parent->getHeight(), Globals::UI::textEditorHeight);
    if (allowsClicksOnSelf) // && dynamic_cast<const TextEditor *>(parent))
    {
        const int w = Trigger::triggerSize;
        const int x = parent->getWidth() - Trigger::triggerSize;
        this->setBounds(x, 0, w, maxParentHeight);
    }
    else
    {
        this->setBounds(parent->getLocalBounds().withHeight(maxParentHeight));
    }

    this->setAlwaysOnTop(true);
    this->toFront(false);
}

Component *MobileComboBox::Trigger::createHighlighterComponent()
{
    return new MobileComboBox::Trigger(nullptr);
}
