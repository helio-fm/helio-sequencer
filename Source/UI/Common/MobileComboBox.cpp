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
    this->setOpaque(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setAccessible(false);

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

    this->triggerButton = MobileComboBox::HelperButton::makeComboTriggerButton(this);

    this->searchTextBox->onEscapeKey = [this]()
    {
        this->postCommandMessage(CommandIDs::ToggleShowHideCombo);
    };
    
    this->searchTextBox->onTextChange = [this]()
    {
        this->menu->applyFilter(this->searchTextBox->getText());
    };
}

void MobileComboBox::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setTiledImageFill(theme.getSidebarBackground(), 0, 0, 1.f);
    g.fillRect(this->getLocalBounds());
}

void MobileComboBox::resized()
{
    static constexpr auto menuY = Globals::UI::textEditorHeight + 2;

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
    jassert(commandId != CommandIDs::SelectPreviousPreset &&
            commandId != CommandIDs::SelectNextPreset);

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

// todo remove this when migrating to C++17
constexpr int MobileComboBox::HelperButton::iconSize;
constexpr int MobileComboBox::HelperButton::triggerButtonSize;

MobileComboBox::Container::Container()
{
    this->setAccessible(false);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setMouseClickGrabsKeyboardFocus(false);
}

MobileComboBox::Container::~Container() = default;

void MobileComboBox::Container::initWith(WeakReference<Component> editor,
    MenuPanel::Menu menu, bool shouldShowNextPreviousButtons)
{
    this->toFront(false);
    this->textEditor = editor;
    this->combo = make<MobileComboBox>(editor, this);
    this->combo->initMenu(menu);

    this->comboTrigger = HelperButton::makeComboTriggerButton(this);
    if (this->textEditor != nullptr)
    {
        this->textEditor->addAndMakeVisible(this->comboTrigger.get());
    }

    if (shouldShowNextPreviousButtons)
    {
        constexpr auto buttonSize = 28;
        this->previousPresetButton = make<HelperButton>(this,
            Icons::findByName(Icons::back, HelperButton::iconSize),
            int(CommandIDs::SelectPreviousPreset),
            buttonSize, HelperButton::triggerButtonSize + buttonSize);
        this->nextPresetButton = make<HelperButton>(this,
            Icons::findByName(Icons::forward, HelperButton::iconSize),
            int(CommandIDs::SelectNextPreset),
            buttonSize, HelperButton::triggerButtonSize);

        this->previousPresetButton->setIconAlphaMultiplier(0.25f);
        this->nextPresetButton->setIconAlphaMultiplier(0.25f);

        if (this->textEditor != nullptr)
        {
            this->textEditor->addAndMakeVisible(this->previousPresetButton.get());
            this->textEditor->addAndMakeVisible(this->nextPresetButton.get());
        }
    }
}

void MobileComboBox::Container::initWith(WeakReference<Component> textEditor,
    Function<MenuPanel::Menu(void)> menuInitializer)
{
    this->menuInitializer = menuInitializer;
    this->initWith(textEditor, MenuPanel::Menu(), false);
}

void MobileComboBox::Container::updateMenu(MenuPanel::Menu menu)
{
    this->combo->initMenu(menu);
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
    else if (commandId == CommandIDs::SelectPreviousPreset)
    {
        // the next/previous preset buttons were added as a quick hack,
        // so certain menu options are not supported here
        jassert(this->menuInitializer == nullptr);
        jassert(dynamic_cast<TextEditor *>(this->textEditor.get()));

        if (auto *ed = dynamic_cast<TextEditor *>(this->textEditor.get()))
        {
            jassert(this->previousPresetButton != nullptr);
            const int index = this->combo->menu->indexOfItemNamed(ed->getText());
            if (index < 0)
            {
                this->previousPresetButton->setHighlighted(false);
                return;
            }
            const int previous = jlimit(0, this->combo->menu->getMenuSize(), index - 1);
            if (auto menuItem = this->combo->menu->getMenuItem(previous))
            {
                jassert(menuItem->callback == nullptr); // not supported for now
                this->getParentComponent()->postCommandMessage(menuItem->commandId);
                #if PLATFORM_DESKTOP
                this->previousPresetButton->setHighlighted(true);
                #endif
            }
        }
    }
    else if (commandId == CommandIDs::SelectNextPreset)
    {
        jassert(this->menuInitializer == nullptr);
        jassert(dynamic_cast<TextEditor *>(this->textEditor.get()));

        if (auto *ed = dynamic_cast<TextEditor *>(this->textEditor.get()))
        {
            jassert(this->nextPresetButton != nullptr);
            const int index = this->combo->menu->indexOfItemNamed(ed->getText());
            if (index < 0)
            {
                this->nextPresetButton->setHighlighted(false);
                return;
            }
            const int next = jlimit(0, this->combo->menu->getMenuSize(), index + 1);
            if (auto menuItem = this->combo->menu->getMenuItem(next))
            {
                jassert(menuItem->callback == nullptr); // not supported for now
                this->getParentComponent()->postCommandMessage(menuItem->commandId);
                #if PLATFORM_DESKTOP
                this->nextPresetButton->setHighlighted(true);
                #endif
            }
        }
    }
}

MobileComboBox::HelperButton::HelperButton(WeakReference<Component> listener,
    Image targetImage, int commandId, int buttonSize, int xOffset) :
    IconButton(targetImage, commandId, listener),
    buttonSize(buttonSize),
    xOffset(xOffset) {}

UniquePointer<MobileComboBox::HelperButton>
MobileComboBox::HelperButton::makeComboTriggerButton(WeakReference<Component> listener)
{
    return make<HelperButton>(listener,
        Icons::findByName(Icons::down, HelperButton::iconSize),
        int(CommandIDs::ToggleShowHideCombo),
        HelperButton::triggerButtonSize, 0);
}

void MobileComboBox::HelperButton::parentHierarchyChanged()
{
    this->updateBounds();
}

void MobileComboBox::HelperButton::parentSizeChanged()
{
    this->updateBounds();
}

void MobileComboBox::HelperButton::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
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

void MobileComboBox::HelperButton::paint(Graphics &g)
{
    g.setColour(this->colour.withMultipliedAlpha(this->alpha));

    if (this->image.isNull())
    {
        const auto i = Icons::findByName(this->iconId, HelperButton::iconSize);
        Icons::drawImageRetinaAware(i, g,
            this->getWidth() - (this->buttonSize / 2), this->getHeight() / 2);
    }
    else
    {
        Icons::drawImageRetinaAware(this->image, g,
            this->getWidth() - (this->buttonSize / 2), this->getHeight() / 2);
    }
}

void MobileComboBox::HelperButton::updateBounds()
{
    const auto *parent = this->getParentComponent();
    if (parent == nullptr)
    {
        return;
    }

    bool parentAllowsClicks = false;
    bool parentAllowsClicksOnChildren = false;
    parent->getInterceptsMouseClicks(parentAllowsClicks, parentAllowsClicksOnChildren);
    const auto maxParentHeight = jmin(parent->getHeight(), Globals::UI::textEditorHeight);
    const auto isAlignedToEdge = this->xOffset == 0;
    if (parentAllowsClicks || !isAlignedToEdge)
    {
        const int w = this->buttonSize;
        const int x = parent->getWidth() - this->xOffset - w;
        this->setBounds(x, 0, w, maxParentHeight);
    }
    else
    {
        this->setBounds(parent->getLocalBounds().withHeight(maxParentHeight));
    }

    this->setAlwaysOnTop(true);
    this->toFront(false);
}

Component *MobileComboBox::HelperButton::createHighlighterComponent()
{
    auto highlighter = make<MobileComboBox::HelperButton>(nullptr,
        this->image, this->commandId, this->buttonSize, 0);
    highlighter->setIconAlphaMultiplier(jmin(1.f, this->alpha * 2.f));
    return highlighter.release();
}
