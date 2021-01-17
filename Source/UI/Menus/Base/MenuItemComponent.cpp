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
#include "MenuItemComponent.h"
#include "MenuItemComponentMarker.h"
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

MenuItem::Ptr MenuItem::item(Icons::Id iconId, int commandId, const String &text /*= ""*/)
{
    MenuItem::Ptr description(new MenuItem());
    description->iconId = iconId;
    description->commandText = text;
    description->commandId = commandId;
    description->hotkeyText = findHotkeyText(commandId);
    description->flags.isToggled = false;
    description->flags.isDisabled = false;
    description->flags.shouldCloseMenu = false;
    description->flags.hasSubmenu = false;
    description->colour = findDefaultColour(Label::textColourId);
    return description;
}

MenuItem::Ptr MenuItem::item(Image image, int commandId, const String &text /*= ""*/)
{
    MenuItem::Ptr description(new MenuItem());
    description->image = move(image);
    description->commandText = text;
    description->commandId = commandId;
    description->hotkeyText = findHotkeyText(commandId);
    description->flags.isToggled = false;
    description->flags.isDisabled = false;
    description->flags.shouldCloseMenu = false;
    description->flags.hasSubmenu = false;
    description->colour = findDefaultColour(Label::textColourId);
    return description;
}

MenuItem::Ptr MenuItem::item(Icons::Id iconId, const String &text)
{
    return MenuItem::item(iconId, -1, text);
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

MenuItem::Ptr MenuItem::toggled(bool shouldBeToggled)
{
    MenuItem::Ptr description(this);
    description->flags.isToggled = shouldBeToggled;
    return description;
}

MenuItem::Ptr MenuItem::colouredWith(const Colour &colour)
{
    MenuItem::Ptr description(this);
    description->colour = colour.interpolatedWith(findDefaultColour(Label::textColourId), 0.4f);
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

MenuItem::Ptr MenuItem::closesMenu()
{
    MenuItem::Ptr description(this);
    jassert(description->callback == nullptr);
    description->flags.shouldCloseMenu = true;
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

    public: void paint(Graphics &g) override
    {
        const auto fgColour = findDefaultColour(Label::textColourId);
        g.setColour(fgColour.withAlpha(0.0175f));
        g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 2.f);
    }
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
    DraggingListBoxComponent(parentViewport),
    parent(parentCommandReceiver),
    description(MenuItem::empty())
{
    this->subLabel = make<Label>();
    this->addAndMakeVisible(this->subLabel.get());
    this->subLabel->setFont({ 21.f });
    this->subLabel->setJustificationType(Justification::centredRight);
    this->subLabel->setInterceptsMouseClicks(false, false);
    this->subLabel->setColour(Label::textColourId, desc->colour.withMultipliedAlpha(0.5f));

    this->textLabel = make<Label>();
    this->addAndMakeVisible(this->textLabel.get());
    this->textLabel->setFont({ 21.f });
    this->textLabel->setJustificationType(Justification::centredLeft);
    this->textLabel->setInterceptsMouseClicks(false, false);

    this->submenuMarker = make<IconComponent>(Icons::submenu, 0.25f);
    this->addAndMakeVisible(this->submenuMarker.get());

    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, true);
    this->setPaintingIsUnclipped(true);

    this->setSize(64, COMMAND_PANEL_BUTTON_HEIGHT);
    this->update(desc);
}

MenuItemComponent::~MenuItemComponent() = default;

void MenuItemComponent::paint(Graphics &g)
{
    if (this->parentViewport != nullptr)
    {
        g.setColour(Colour(0x06ffffff));
        g.fillRect(0, 0, this->getWidth(), 1);

        g.setColour (Colour(0x0f000000));
        g.fillRect (0, this->getHeight() - 1, this->getWidth(), 1);
    }

    g.setOpacity(1.f);

    const int iconX = this->hasText() ?
        (this->icon.getWidth() / 2) + MenuItemComponent::iconMargin : (this->getWidth() / 2);

    jassert(this->icon.isValid());

    Icons::drawImageRetinaAware(this->icon, g, iconX, this->getHeight() / 2);
}

void MenuItemComponent::resized()
{
    constexpr auto iconSize = 20;
    constexpr auto textLabelX = 48;
    constexpr auto subLabelWidth = 128;
    constexpr auto rightMargin = 4;

    this->subLabel->setBounds(this->getWidth() - subLabelWidth - rightMargin, 0, subLabelWidth, this->getHeight());
    this->textLabel->setBounds(textLabelX, 0, this->getWidth() - textLabelX - rightMargin, this->getHeight());
    this->submenuMarker->setBounds(this->getWidth() - iconSize - rightMargin,
                                   (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);

    if (this->checkMarker != nullptr)
    {
        // this one might be still animating, and may screw up the bounds if not updated:
        if (this->checkMarker->getLocalBounds() != this->getLocalBounds() &&
            this->animator.isAnimating(this->checkMarker.get()))
        {
            this->animator.animateComponent(this->checkMarker.get(),
                this->getLocalBounds(), 1.f, Globals::UI::fadeInShort, false, 0.0, 0.0);
        }

        this->checkMarker->setBounds(this->getLocalBounds());
    }

    if (!this->icon.isValid() && this->description->image.isValid())
    {
        this->icon = this->description->image;
    }
    else
    {
        this->icon = Icons::findByName(this->description->iconId, iconSize);
    }

    const int xMargin = MenuItemComponent::iconMargin + 2;
    this->textLabel->setBounds(int(this->icon.getWidth() + xMargin),
        (this->getHeight() / 2) - (this->getHeight() / 2),
        int(this->getWidth() - this->icon.getWidth() - xMargin),
        this->getHeight());

    const float fontSize = 18.f ;//jmin(MAX_MENU_FONT_SIZE, float(this->getHeight() / 2) + 1.f);
    this->subLabel->setFont(Font(Font::getDefaultSansSerifFontName(), fontSize, Font::plain));
    this->textLabel->setFont(Font(Font::getDefaultSansSerifFontName(), fontSize, Font::plain));
}

void MenuItemComponent::mouseDown(const MouseEvent &e)
{
    if (!this->hasText())
    {
        DraggingListBoxComponent::mouseDown(e);
    }
    else
    {
        if (!this->description->flags.isDisabled)
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
    if (! this->mouseDownWasTriggered)
    {
        return;
    }

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

    this->mouseDownWasTriggered = false;
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
        desc2->image = this->description->image;
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
    this->checkMarker = make<MenuItemComponentMarker>();
    this->checkMarker->setBounds(this->getLocalBounds());

#if ! HAS_OPENGL_BUG
    this->checkMarker->setAlpha(0.f);
    this->animator.animateComponent(this->checkMarker.get(),
        this->getLocalBounds(), 1.f, Globals::UI::fadeInShort, false, 0.0, 0.0);
#endif

    this->addAndMakeVisible(this->checkMarker.get());
}

void MenuItemComponent::hideCheckMark()
{
    this->checkMarker = nullptr;
}
