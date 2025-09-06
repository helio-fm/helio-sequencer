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
#include "ColourIDs.h"
#include <utility>

#if JUCE_MAC
#   define HAS_OPENGL_BUG 1
#endif

//===----------------------------------------------------------------------===//
// Markers
//===----------------------------------------------------------------------===//

class MenuItemCursorComponent final : public Component
{
public:

    MenuItemCursorComponent()
    {
        this->setAccessible(false);
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g)
    {
        const auto bounds = this->getLocalBounds().reduced(5, 4);

        const auto x = bounds.getX();
        const auto y = bounds.getY();
        const auto r = bounds.getRight();
        const auto b = bounds.getBottom();

        // the corner's size
        constexpr auto lh = 7;
        constexpr auto lv = 3;

        g.setColour(this->shadowColour);
        g.fillRect(x, y, lh, 2);
        g.fillRect(x, y, 2, lv);
        g.fillRect(x, b - 2, lh, 2);
        g.fillRect(x, b - lv, 2, lv);
        g.fillRect(r - lh, y, lh, 2);
        g.fillRect(r - 2, y, 2, lv);
        g.fillRect(r - lh, b - 2, lh, 2);
        g.fillRect(r - 2, b - lv, 2, lv);

        g.setColour(this->fillColour);
        g.fillRect(x, y, lh, 1);
        g.fillRect(x, y, 1, lv);
        g.fillRect(x, b - 1, lh, 1);
        g.fillRect(x, b - lv, 1, lv);
        g.fillRect(r - lh, y, lh, 1);
        g.fillRect(r - 1, y, 1, lv);
        g.fillRect(r - lh, b - 1, lh, 1);
        g.fillRect(r - 1, b - lv, 1, lv);
    }

private:

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::cursorFill);
    const Colour shadowColour = findDefaultColour(ColourIDs::Menu::cursorShade);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemCursorComponent)
};

class MenuItemCurrentMarkComponent final : public Component
{
public:

    MenuItemCurrentMarkComponent()
    {
        this->setAccessible(false);
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g)
    {
        const auto bounds = this->getLocalBounds().reduced(3, 2);

        const auto x = bounds.getX();
        const auto y = bounds.getY();
        const auto r = bounds.getRight();
        const auto b = bounds.getBottom();

        // the corner's size
        constexpr auto lh = 9;
        constexpr auto lv = 3;

        g.setColour(this->shadowColour);
        g.fillRect(x, y, lh, 2);
        g.fillRect(x, y, 2, lv);
        g.fillRect(x, b - 2, lh, 2);
        g.fillRect(x, b - lv, 2, lv);
        g.fillRect(r - lh, y, lh, 2);
        g.fillRect(r - 2, y, 2, lv);
        g.fillRect(r - lh, b - 2, lh, 2);
        g.fillRect(r - 2, b - lv, 2, lv);

        g.setColour(this->fillColour);
        g.fillRect(x, y, lh, 1);
        g.fillRect(x, y, 1, lv);
        g.fillRect(x, b - 1, lh, 1);
        g.fillRect(x, b - lv, 1, lv);
        g.fillRect(r - lh, y, lh, 1);
        g.fillRect(r - 1, y, 1, lv);
        g.fillRect(r - lh, b - 1, lh, 1);
        g.fillRect(r - 1, b - lv, 1, lv);
    }

private:

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::currentItemMarker);
    const Colour shadowColour = findDefaultColour(ColourIDs::Menu::cursorShade);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemCurrentMarkComponent)
};

class MenuItemToggleMarkComponent final : public Component
{
public:

    MenuItemToggleMarkComponent()
    {
        this->setAccessible(false);
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

    const Colour colour = findDefaultColour(ColourIDs::Menu::toggleMarker);

    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemToggleMarkComponent)
};

class MenuItemClickMarkComponent final : public Component
{
public:

    MenuItemClickMarkComponent()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(this->getLocalBounds());
    }

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::highlight);
};

class MenuItemHighlighter final : public Component
{
public:

    MenuItemHighlighter(Icons::Id iconId, bool iconIsCentered) :
        iconIsCentered(iconIsCentered)
    {
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);

        this->icon = Icons::findByName(iconId, MenuItemComponent::iconSize);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRoundedRectangle(this->getLocalBounds().reduced(0, 1).toFloat(), 1.f);

        const int iconX = iconIsCentered ? this->getWidth() / 2 :
            MenuItemComponent::iconSize / 2 + MenuItemComponent::iconMargin;

        jassert(this->icon.isValid());
        g.setOpacity(0.7f);
        Icons::drawImageRetinaAware(this->icon, g, iconX, this->getHeight() / 2);
    }

    Image icon;
    const bool iconIsCentered;

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::highlight);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuItemHighlighter)
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
    description->textAlignment = alignment;
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

bool MenuItem::hasSubmenu() const noexcept
{
    return this->flags.hasSubmenu;
}

const String &MenuItem::getText() const noexcept
{
    return this->commandText;
}

const MenuItem::MenuItemFlags MenuItem::getFlags() const noexcept
{
    return this->flags;
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

MenuItem::Ptr MenuItem::withSubtitle(const String &string)
{
    MenuItem::Ptr description(this);
    description->hotkeyText = string;
    return description;
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
// MenuItemComponent
//===----------------------------------------------------------------------===//

MenuItemComponent::MenuItemComponent(Component *parentCommandReceiver,
    Viewport *parentViewport, const MenuItem::Ptr desc) :
    DraggingListBoxComponent(parentViewport, false),
    parent(parentCommandReceiver)
{
    this->setAccessible(false);
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

    this->submenuMarker = make<IconComponent>(Icons::submenu, 0.3f);
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
        g.setColour(this->borderLightColour);
        g.fillRect(0, 0, this->getWidth(), 1);

        g.setColour(this->borderDarkColour);
        g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);
    }

    g.setOpacity(this->description->flags.isDisabled ? 0.5f : 1.f);

    const int iconX = this->isIconCentered() ? this->getWidth() / 2 :
        MenuItemComponent::iconSize / 2 + MenuItemComponent::iconMargin;

    jassert(this->icon.isValid());

    Icons::drawImageRetinaAware(this->icon, g, iconX, this->getHeight() / 2);
}

void MenuItemComponent::resized()
{
    constexpr auto rightMargin = 4;
    constexpr auto subLabelWidth = 128;
    const auto iconSize = (this->description->iconId == Icons::empty) ? 0 : MenuItemComponent::iconSize;

    this->subLabel->setBounds(this->getWidth() - subLabelWidth - rightMargin, 0, subLabelWidth, this->getHeight());
    this->submenuMarker->setBounds(this->getWidth() - iconSize - rightMargin,
        (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);

    if (this->toggleMarker != nullptr)
    {
        this->toggleMarker->setBounds(this->isIconCentered() ? this->getLocalBounds() :
            this->getLocalBounds().withWidth(iconSize + MenuItemComponent::iconMargin * 2));
    }

    if (this->cursorMarker != nullptr &&
        this->cursorMarker->getBounds() != this->getLocalBounds())
    {
        this->animator.cancelAnimation(this->cursorMarker.get(), true);
        this->cursorMarker->setBounds(this->getLocalBounds());
    }

    if (this->currentItemMarker != nullptr)
    {
        this->currentItemMarker->setBounds(this->getLocalBounds());
    }

    this->icon = Icons::findByName(this->description->iconId, iconSize);

    this->textLabel->setBounds(iconSize + MenuItemComponent::iconMargin - 1, 0,
        this->getWidth() - iconSize - MenuItemComponent::iconMargin, this->getHeight());

    for (int i = 0; i < this->buttons.size(); ++i)
    {
        auto *button = this->buttons.getReference(i).get();
        constexpr auto buttonRightMargin = MenuItemComponent::iconSize * 2;
        button->setBounds(this->getWidth() - buttonRightMargin - MenuItemComponent::iconSize * (i + 1),
            0, iconSize, this->getHeight());
    }

    HighlightedComponent::resized();
}

// receives commands from IconButton's, if has any
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
            this->clickMarker = make<MenuItemClickMarkComponent>();
            this->addChildComponent(this->clickMarker.get(), 0);
            this->clickMarker->setBounds(this->getLocalBounds());

#if HAS_OPENGL_BUG
            this->clickMarker->setVisible(true);
#else
            this->animator.animateComponent(this->clickMarker.get(),
                this->getLocalBounds(), 1.f, Globals::UI::fadeInShort, true, 1.0, 0.0);
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
                this->getLocalBounds(), 0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);
#endif

            this->removeChildComponent(this->clickMarker.get());
            this->clickMarker = nullptr;
        }

        if (this->listCanBeScrolled())
        {
            DraggingListBoxComponent::mouseUp(e);
        }
        else if (this->contains(e.getPosition()) && e.getDistanceFromDragStart() < 10)
        {
            this->setSelected(true);
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
    // shouldn't unhighlight if highlighted by cursor
    if (this->cursorMarker == nullptr)
    {
        HighlightedComponent::mouseExit(e);
    }

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
#if !HAS_OPENGL_BUG
        if (!this->hasText() &&
            !this->description->flags.isDisabled &&
            (this->description->commandId > 0 || this->description->callback != nullptr))
        {
            // possible glDeleteTexture bug here?
            auto highlighter = make<MenuItemClickMarkComponent>();
            this->addAndMakeVisible(highlighter.get(), 0);
            highlighter->setBounds(this->getLocalBounds());
            this->animator.animateComponent(highlighter.get(),
                this->getLocalBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 1.0);
            this->removeChildComponent(highlighter.get());
        }
#endif

        this->doAction();
    }
}

void MenuItemComponent::setCursorShown(bool shouldBeShown)
{
    if ((this->cursorMarker != nullptr) == shouldBeShown)
    {
        return;
    }

    if (shouldBeShown && this->cursorMarker == nullptr)
    {
        this->cursorMarker = make<MenuItemCursorComponent>();
        this->cursorMarker->setBounds(this->getLocalBounds());
        this->addChildComponent(this->cursorMarker.get());
        this->animator.fadeIn(this->cursorMarker.get(), Globals::UI::fadeInShort);
        this->setHighlighted(true);
    }
    else if (!shouldBeShown && this->cursorMarker != nullptr)
    {
        //this->animator.fadeOut(this->cursorMarker.get(), Globals::UI::fadeOutShort / 2);
        this->cursorMarker = nullptr;
        this->setHighlighted(false);
    }
}

void MenuItemComponent::setDisplayedAsCurrent(bool shouldBeDisplayedAsCurrent)
{
    if ((this->currentItemMarker != nullptr) == shouldBeDisplayedAsCurrent)
    {
        return;
    }

    if (shouldBeDisplayedAsCurrent && this->currentItemMarker == nullptr)
    {
        this->currentItemMarker = make<MenuItemCurrentMarkComponent>();
        this->currentItemMarker->setBounds(this->getLocalBounds());
        this->addAndMakeVisible(this->currentItemMarker.get());
    }
    else if (!shouldBeDisplayedAsCurrent && this->currentItemMarker != nullptr)
    {
        this->currentItemMarker = nullptr;
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
    else if (!desc->flags.isToggled && (this->toggleMarker != nullptr))
    {
        this->hideCheckMark();
    }

    this->textLabel->setColour(Label::textColourId,
        desc->colour.withMultipliedAlpha(desc->flags.isDisabled ? 0.5f : 1.f));

    this->textLabel->setJustificationType(desc->textAlignment == MenuItem::Alignment::Left ?
        Justification::centredLeft : Justification::centredRight);

    this->submenuMarker->setVisible(desc->flags.hasSubmenu);
    this->description = desc;

    if (this->hasText())
    {
        this->textLabel->setText(desc->commandText, dontSendNotification);
        this->textLabel->setVisible(true);
        this->textLabel->setMinimumHorizontalScale(desc->commandText.containsChar(' ') ? 1.f : 0.5f);

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
#if PLATFORM_DESKTOP
    if (!this->description->flags.isDisabled)
    {
        return new MenuItemHighlighter(this->description->iconId, this->isIconCentered());
    }
#endif

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
    this->toggleMarker = make<MenuItemToggleMarkComponent>();
    this->addAndMakeVisible(this->toggleMarker.get());
    this->resized();
}

void MenuItemComponent::hideCheckMark()
{
    this->toggleMarker = nullptr;
}
