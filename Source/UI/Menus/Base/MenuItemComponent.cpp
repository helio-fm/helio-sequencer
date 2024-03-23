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
#include "MenuItemComponent.h"
#include "MenuPanel.h"
#include "HelioTheme.h"
#include "IconComponent.h"
#include "HotkeyScheme.h"
#include "MainLayout.h"
#include "Config.h"

#include <utility>

#if JUCE_MAC
#   define HAS_OPENGL_BUG 1
#endif

//===----------------------------------------------------------------------===//
// Check mark
//===----------------------------------------------------------------------===//

class MenuItemComponentCheckMark final : public Component
{
public:

    MenuItemComponentCheckMark()
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        g.fillRect(this->x, this->y - 1, this->w, 1);
        g.fillRect(this->x, this->y + this->h, this->w, 1);
        g.fillRect(this->x - 1, this->y, 1, this->h);
        g.fillRect(this->x + this->w, this->y, 1, this->h);
    }

    void resized() override
    {
        const int minSize = jmin(this->getWidth(), this->getHeight());
        const auto squareMarker = Rectangle<int>(0, 0, minSize, minSize)
            .reduced(5).withCentre(this->getLocalBounds().getCentre());
        this->x = squareMarker.getX();
        this->y = squareMarker.getY();
        this->w = squareMarker.getWidth();
        this->h = squareMarker.getHeight();
    }

private:

    const Colour colour = findDefaultColour(Label::textColourId).withAlpha(0.125f);

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemComponentCheckMark)
};


//===----------------------------------------------------------------------===//
// MenuItem
//===----------------------------------------------------------------------===//

MenuItem::Ptr MenuItem::empty()
{
    MenuItem::Ptr description(new MenuItem());
    description->colour = Colours::transparentBlack;
    return description;
}

inline static String findHotkeyText(int commandId)
{
#if PLATFORM_DESKTOP
    return App::Config().getHotkeySchemes()->getCurrent()->findHotkeyDescription(commandId);
#elif PLATFORM_MOBILE
    // Don't show any hotkeys on mobile devices
    return {};
#endif
}

MenuItem::Ptr MenuItem::item(Icons::Id iconId, int commandId, String text /*= ""*/)
{
    MenuItem::Ptr description(new MenuItem());
    description->iconId = iconId;
    description->commandText = move(text);
    description->commandId = commandId;
    description->hotkeyText = findHotkeyText(commandId);
    description->flags.isToggled = false;
    description->flags.isDisabled = false;
    description->flags.shouldCloseMenu = false;
    description->flags.hasSubmenu = false;
    description->colour = findDefaultColour(Label::textColourId);
    return description;
}

MenuItem::Ptr MenuItem::item(Icons::Id iconId, String text)
{
    return MenuItem::item(iconId, -1, move(text));
}

MenuItem::Ptr MenuItem::withAlignment(Alignment alignment)
{
    MenuItem::Ptr description(this);
    description->alignment = alignment;
    return description;
}

MenuItem::Ptr MenuItem::withSubmenu()
{
    MenuItem::Ptr description(this);
    description->flags.hasSubmenu = true;
    return description;
}

MenuItem::Ptr MenuItem::withSubmenuIf(bool condition)
{
    if (condition)
    {
        return this->withSubmenu();
    }

    return this;
}

MenuItem::Ptr MenuItem::toggledIf(bool shouldBeToggled)
{
    MenuItem::Ptr description(this);
    description->flags.isToggled = shouldBeToggled;
    return description;
}

MenuItem::Ptr MenuItem::withColour(const Colour &colour)
{
    MenuItem::Ptr description(this);
    description->colour = colour.interpolatedWith(findDefaultColour(Label::textColourId), 0.5f);
    return description;
}

MenuItem::Ptr MenuItem::withTooltip(String tooltip)
{
    MenuItem::Ptr description(this);
    description->tooltipText = move(tooltip);

    if (this->hotkeyText.isNotEmpty())
    {
        this->tooltipText << '\n' << TRANS(I18n::Tooltips::hotkey) << " " << this->hotkeyText;
    }

    return description;
}

String MenuItem::createTooltip(String message, int commandId)
{
    auto result = move(message);
    const auto hotkeyText = findHotkeyText(commandId);
    if (hotkeyText.isNotEmpty())
    {
        result << '\n' << TRANS(I18n::Tooltips::hotkey) << " " << hotkeyText;
    }
    return result;
}

String MenuItem::createTooltip(String message, KeyPress keyPress)
{
    return move(message) + '\n' + TRANS(I18n::Tooltips::hotkey) + " " + keyPress.getTextDescription();
}

MenuItem::Ptr MenuItem::withHotkeyText(int commandId)
{
    MenuItem::Ptr description(this);
    description->hotkeyText = findHotkeyText(commandId);
    return description;
}

MenuItem::Ptr MenuItem::disabledIf(bool condition)
{
    MenuItem::Ptr description(this);
    description->flags.isDisabled = condition;
    return description;
}

MenuItem::Ptr MenuItem::withAction(const Callback &lambda)
{
    MenuItem::Ptr description(this);
    description->callback = lambda;
    return description;
}

MenuItem::Ptr MenuItem::withActionIf(bool condition, const Callback &lambda)
{
    return condition ? this->withAction(lambda) : this;
}

MenuItem::Ptr MenuItem::closesMenu()
{
    MenuItem::Ptr description(this);
    jassert(description->callback == nullptr);
    description->flags.shouldCloseMenu = true;
    return description;
}

MenuItem::Ptr MenuItem::withWeight(float weight)
{
    MenuItem::Ptr description(this);
    description->weight = weight;
    return description;
}

MenuItem::Ptr MenuItem::withButton(bool isEnabled, Icons::Id icon, const Callback &lambda)
{
    MenuItem::Ptr description(this);
    description->buttons.insert(0, { isEnabled, icon, lambda });
    return description;
}

//===----------------------------------------------------------------------===//
// Highlighters
//===----------------------------------------------------------------------===//

class CommandDragHighlighter final : public Component
{
public:

    CommandDragHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fgColour);
        g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 2.f);
    }

    const Colour fgColour = findDefaultColour(Label::textColourId).withAlpha(0.0175f);
};

class CommandItemSelector final : public Component
{
public:

    CommandItemSelector()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const auto fgColour = findDefaultColour(Label::textColourId);
        g.setColour(fgColour.withAlpha(0.075f));
        g.fillRect(this->getLocalBounds());
    }
};

MenuItemComponent::MenuItemComponent(Component *parentCommandReceiver,
    Viewport *parentViewport, const MenuItem::Ptr desc) :
    DraggingListBoxComponent(parentViewport, false),
    parent(parentCommandReceiver),
    description(MenuItem::empty())
{
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);

    this->subLabel = make<Label>();
    this->addAndMakeVisible(this->subLabel.get());
    this->subLabel->setFont(MenuItemComponent::fontSize);
    this->subLabel->setJustificationType(Justification::centredRight);
    this->subLabel->setInterceptsMouseClicks(false, false);
    this->subLabel->setColour(Label::textColourId, desc->colour.withMultipliedAlpha(0.35f));

    this->textLabel = make<Label>();
    this->addAndMakeVisible(this->textLabel.get());
    this->textLabel->setFont(MenuItemComponent::fontSize);
    this->textLabel->setJustificationType(Justification::centredLeft);
    this->textLabel->setInterceptsMouseClicks(false, false);

    this->submenuMarker = make<IconComponent>(Icons::submenu, 0.25f);
    this->addAndMakeVisible(this->submenuMarker.get());

    static constexpr auto buttonIconSize = 12;
    for (int i = 0; i < desc->buttons.size(); ++i)
    {
        const auto &buttonDescription = desc->buttons[i];
        auto button = make<IconButton>(buttonDescription.iconId, i, this, buttonIconSize);
        button->setEnabled(buttonDescription.isEnabled);
        // those buttons are tiny, lets emphasize the hover state:
        button->setMouseCursor(buttonDescription.isEnabled ?
            MouseCursor::PointingHandCursor : MouseCursor::NormalCursor);
        this->addAndMakeVisible(button.get());
        this->buttons.add(move(button));
    }

    this->setSize(64, Globals::UI::menuPanelRowHeight);
    this->update(desc);
}

MenuItemComponent::~MenuItemComponent() = default;

void MenuItemComponent::paint(Graphics &g)
{
    if (this->parentViewport != nullptr)
    {
        g.setColour(Colour(0x06ffffff));
        g.fillRect(0, 0, this->getWidth(), 1);

        g.setColour(Colour(0x0f000000));
        g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);
    }

    g.setOpacity(1.f);

    const int iconX = this->hasText() ?
        MenuItemComponent::iconSize / 2 + MenuItemComponent::iconMargin :
        this->getWidth() / 2;

    jassert(this->icon.isValid());

    Icons::drawImageRetinaAware(this->icon, g, iconX, this->getHeight() / 2);
}

void MenuItemComponent::resized()
{
    constexpr auto rightMargin = 4;
    constexpr auto subLabelWidth = 128;
    constexpr auto iconSize = MenuItemComponent::iconSize;
    this->subLabel->setBounds(this->getWidth() - subLabelWidth - rightMargin, 0, subLabelWidth, this->getHeight());
    this->submenuMarker->setBounds(this->getWidth() - iconSize - rightMargin,
        (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);

    if (this->checkMarker != nullptr)
    {
        this->checkMarker->setBounds(this->hasText() ?
            this->getLocalBounds().withWidth(iconSize + MenuItemComponent::iconMargin * 2) :
            this->getLocalBounds());
    }

    this->icon = Icons::findByName(this->description->iconId, iconSize);

    this->textLabel->setBounds(iconSize + MenuItemComponent::iconMargin, 0,
        this->getWidth() - iconSize - MenuItemComponent::iconMargin, this->getHeight());

    for (int i = 0; i < this->buttons.size(); ++i)
    {
        auto *button = this->buttons.getReference(i).get();
        constexpr auto buttonRightMargin = iconSize * 2;
        button->setBounds(this->getWidth() - buttonRightMargin - iconSize * (i + 1),
            0, iconSize, this->getHeight());
    }
}

void MenuItemComponent::handleCommandMessage(int commandId)
{
    jassert(commandId >= 0 && commandId < this->buttons.size());
    if (commandId >= 0 && commandId < this->description->buttons.size())
    {
        const auto &button = this->description->buttons[commandId];

        jassert(button.callback);
        if (button.callback)
        {
            button.callback();
        }
    }
}

void MenuItemComponent::mouseDown(const MouseEvent &e)
{
    if (!this->isEnabled())
    {
        return;
    }

#if PLATFORM_DESKTOP
    App::Layout().hideTooltipIfAny();
#endif

    if (!this->hasText())
    {
        DraggingListBoxComponent::mouseDown(e);
    }
    else
    {
        if (!this->description->flags.isDisabled &&
            (this->description->commandId > 0 || this->description->callback != nullptr))
        {
            this->clickMarker = make<CommandDragHighlighter>();
            this->addChildComponent(this->clickMarker.get());
            this->clickMarker->setBounds(this->getLocalBounds());
            this->clickMarker->toBack();

#if HAS_OPENGL_BUG
            this->clickMarker->setVisible(true);
#else
            this->animator.animateComponent(this->clickMarker.get(),
                this->getLocalBounds(), 1.f, Globals::UI::fadeInShort, true, 0.0, 0.0);
#endif
        }

        if (this->listCanBeScrolled())
        {
            DraggingListBoxComponent::mouseDown(e);
        }
    }

    this->mouseDownWasTriggered = true;
}

void MenuItemComponent::mouseUp(const MouseEvent &e)
{
    if (!this->mouseDownWasTriggered)
    {
        return;
    }

    BailOutChecker checker(this);

    if (!this->hasText())
    {
        DraggingListBoxComponent::mouseUp(e);
    }
    else
    {
        if (this->clickMarker)
        {
#if ! HAS_OPENGL_BUG
            this->animator.animateComponent(this->clickMarker.get(),
                this->getLocalBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
#endif

            this->removeChildComponent(this->clickMarker.get());
            this->clickMarker = nullptr;
        }

        if (this->listCanBeScrolled())
        {
            DraggingListBoxComponent::mouseUp(e);
        }
        else
        {
            if (this->contains(e.getPosition()))
            {
                this->setSelected(true);
            }
        }
    }

    if (checker.shouldBailOut()) { return; }

    this->mouseDownWasTriggered = false;
}

void MenuItemComponent::mouseEnter(const MouseEvent &e)
{
    HighlightedComponent::mouseEnter(e);

#if PLATFORM_DESKTOP
    if (this->description->tooltipText.isEmpty())
    {
        return;
    }

    App::Layout().showTooltip(this->description->tooltipText,
        MainLayout::TooltipIcon::None, Globals::UI::tooltipDelayMs);
#endif
}

void MenuItemComponent::mouseExit(const MouseEvent &e)
{
    HighlightedComponent::mouseExit(e);

#if PLATFORM_DESKTOP
    if (this->description->tooltipText.isEmpty())
    {
        return;
    }

    App::Layout().hideTooltipIfAny();
#endif
}

void MenuItemComponent::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected && (this->parent != nullptr))
    {
        if (!this->hasText())
        {
#if ! HAS_OPENGL_BUG
            // possible glDeleteTexture bug here?
            auto highlighter = make<CommandItemSelector>();
            this->addAndMakeVisible(highlighter.get());
            highlighter->setBounds(this->getLocalBounds());
            this->animator.animateComponent(highlighter.get(),
                this->getLocalBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
            this->removeChildComponent(highlighter.get());
#endif
        }

        this->doAction();
    }
}

void MenuItemComponent::setChecked(bool shouldBeChecked)
{
    if (this->description->flags.isToggled == shouldBeChecked)
    {
        return;
    }

    this->description->flags.isToggled = shouldBeChecked;

    if (shouldBeChecked)
    {
        this->showCheckMark();
    }
    else
    {
        this->hideCheckMark();
    }
}

void MenuItemComponent::update(const MenuItem::Ptr desc)
{
    if (this->description->commandText != desc->commandText)
    {
        this->clearHighlighterAndStopAnimations();
    }

    if (desc->flags.isToggled && !this->description->flags.isToggled)
    {
        this->showCheckMark();
    }
    else if (!desc->flags.isToggled && (this->checkMarker != nullptr))
    {
        this->hideCheckMark();
    }

    this->textLabel->setColour(Label::textColourId,
        desc->colour.withMultipliedAlpha(desc->flags.isDisabled ? 0.5f : 1.f));

    this->textLabel->setJustificationType(desc->alignment == MenuItem::Alignment::Left ?
        Justification::centredLeft : Justification::centredRight);

    this->submenuMarker->setVisible(desc->flags.hasSubmenu);
    this->description = desc;

    if (this->hasText())
    {
        this->textLabel->setText(desc->commandText, dontSendNotification);
        this->textLabel->setVisible(true);
        this->subLabel->setText(desc->hotkeyText, dontSendNotification);
        this->subLabel->setVisible(true);
    }
    else
    {
        this->textLabel->setVisible(false);
        this->subLabel->setVisible(false);
    }

    this->resized();
}

bool MenuItemComponent::hasText() const noexcept
{
    return this->description->commandText.isNotEmpty();
}

Component *MenuItemComponent::createHighlighterComponent()
{
    if (!this->description->flags.isDisabled)
    {
        MenuItem::Ptr desc2 = MenuItem::empty();
        desc2->iconId = this->description->iconId;
        desc2->commandText = this->description->commandText;
        //desc2->hotkeyText = this->description->hotkeyText;
        desc2->flags.hasSubmenu = this->description->flags.hasSubmenu;
        desc2->colour = this->description->colour;
        desc2->alignment = this->description->alignment;
        return new MenuItemComponent(this->parent, nullptr, desc2);
    }

    return nullptr;
}

void MenuItemComponent::doAction()
{
    if (this->description->flags.isDisabled)
    {
        return;
    }

    const BailOutChecker checker(this);

    if (this->description->callback != nullptr)
    {
        this->description->callback();
    }

    if (checker.shouldBailOut())
    {
        return;
    }

    if (this->description->commandId > 0)
    {
        if (this->parent != nullptr)
        {
            this->parent->postCommandMessage(this->description->commandId);
        }

        App::Layout().broadcastCommandMessage(this->description->commandId);
    }

    if (checker.shouldBailOut())
    {
        return;
    }

    auto panel = dynamic_cast<MenuPanel *>(this->parent.getComponent());
    if (this->description->flags.shouldCloseMenu && panel != nullptr)
    {
        panel->dismiss();
    }
}

void MenuItemComponent::showCheckMark()
{
    this->checkMarker = make<MenuItemComponentCheckMark>();
    this->addAndMakeVisible(this->checkMarker.get());
    this->resized();
}

void MenuItemComponent::hideCheckMark()
{
    this->checkMarker = nullptr;
}
