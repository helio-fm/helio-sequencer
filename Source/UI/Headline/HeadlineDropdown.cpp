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
#include "HeadlineDropdown.h"
#include "Headline.h"
#include "HeadlineItem.h"
#include "HeadlineItemArrow.h"
#include "HeadlineItemDataSource.h"
#include "IconComponent.h"
#include "MainLayout.h"
#include "Config.h"
#include "ComponentIDs.h"

class HeadlineItemCursorComponent final : public Component
{
public:

    HeadlineItemCursorComponent(int x, int width) :
        x(x),
        width(width)
    {
        this->setAccessible(false);
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g)
    {
        const auto margin = 4;
        const auto bounds = this->getLocalBounds().
            withTrimmedTop(margin).withTrimmedBottom(margin).
            withX(this->x).withWidth(this->width + margin);

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

    const int x;
    const int width;

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::cursorFill);
    const Colour shadowColour = findDefaultColour(ColourIDs::Menu::cursorShade);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemCursorComponent)
};

class HeadlineDropdownHeader final : public Component
{
public:

    explicit HeadlineDropdownHeader(WeakReference<HeadlineItemDataSource> targetItem) :
        item(targetItem)
    {
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        this->titleLabel = make<Label>();
        this->addAndMakeVisible(this->titleLabel.get());
        this->titleLabel->setFont(Globals::UI::Fonts::M);
        this->titleLabel->setJustificationType(Justification::centredLeft);
        this->titleLabel->setInterceptsMouseClicks(false, false);

        this->icon = make<IconComponent>(Icons::helio);
        this->addAndMakeVisible(this->icon.get());

        this->arrow = make<HeadlineItemArrow>();
        this->addAndMakeVisible(this->arrow.get());

        if (this->item != nullptr)
        {
            this->icon->setIconImage(this->item->getIcon());
            this->titleLabel->setText(this->item->getName(), dontSendNotification);
            this->textWidth = this->titleLabel->getFont().getStringWidth(this->titleLabel->getText());
            this->setSize(this->textWidth + 64, Globals::UI::headlineHeight);
        }
    }

    void resized() override
    {
        constexpr auto iconX = 10;
        constexpr auto iconSize = 22;
        constexpr auto titleX = 30;
        constexpr auto titleHeight = 30;

        this->titleLabel->setBounds(titleX,
            (this->getHeight() / 2) - (titleHeight / 2), this->getWidth() - titleX, titleHeight);
        this->icon->setBounds(iconX, (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
        this->arrow->setBounds(this->getWidth() - this->arrow->getWidth(),
            0, this->arrow->getWidth(), this->getHeight());
    }

    int getContentX() const noexcept
    {
        return this->icon->getX();
    }

    int getContentWidth() const noexcept
    {
        return this->titleLabel->getX() + this->textWidth;
    }

    int getArrowWidth() const noexcept
    {
        return this->arrow->getWidth();
    }

private:

    int textWidth = 0;

    WeakReference<HeadlineItemDataSource> item;

    UniquePointer<Label> titleLabel;
    UniquePointer<IconComponent> icon;
    UniquePointer<HeadlineItemArrow> arrow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineDropdownHeader)
};

HeadlineDropdown::HeadlineDropdown(WeakReference<HeadlineItemDataSource> targetItem,
    const Point<int> &position, bool shouldShowCursor) :
    item(targetItem)
{
    this->setComponentID(ComponentIDs::menu);

    this->header = make<HeadlineDropdownHeader>(targetItem);
    this->addAndMakeVisible(this->header.get());

    if (shouldShowCursor)
    {
        this->showCursor();
    }

    this->setAlpha(0.f);
    this->setTopLeftPosition(position);
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    if (this->item != nullptr)
    {
        this->content = this->item->createMenu();
        if (this->content != nullptr)
        {
            this->addAndMakeVisible(this->content.get());
            this->syncWidthWithContent();
        }
    }
}

HeadlineDropdown::~HeadlineDropdown()
{
    this->stopTimer();
}

void HeadlineDropdown::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillPath(this->backgroundShape);
    g.fillRect(1, Globals::UI::headlineHeight - 2,
        this->getWidth() - 2, this->getHeight() - Globals::UI::headlineHeight + 2);

    g.setColour(this->borderDarkColour.withMultipliedAlpha(0.75f));
    g.drawHorizontalLine(0, 1.f, float(this->getWidth() - this->header->getArrowWidth() + 1));

    g.setColour(this->borderLightColour.withMultipliedAlpha(0.5f));
    g.drawHorizontalLine(1, 1.f, float(this->getWidth() - this->header->getArrowWidth()));

    g.setColour(this->borderDarkColour);
    g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));
    g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth() - 1));
    g.drawVerticalLine(this->getWidth() - 1, float(Globals::UI::headlineHeight), float(this->getHeight() - 1));

    g.setColour(this->borderLightColour);
    g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
    g.fillRect(1.f, 1.f, 4.f, float(this->getHeight() - 3));
    g.drawVerticalLine(this->getWidth() - 2, float(Globals::UI::headlineHeight), float(this->getHeight() - 1));
}

void HeadlineDropdown::resized()
{
    this->content->setBounds(HeadlineDropdown::padding / 2 + 2,
        Globals::UI::headlineHeight - 1,
        this->getWidth() - HeadlineDropdown::padding,
        this->getHeight() - Globals::UI::headlineHeight);

    this->header->setBounds(0, 0, this->getWidth(), Globals::UI::headlineHeight);

    if (this->cursor != nullptr)
    {
        this->cursor->setBounds(this->header->getBounds());
    }

    this->backgroundShape.clear();
    this->backgroundShape.startNewSubPath(1.f, 1.f);
    this->backgroundShape.lineTo(float(this->getWidth() - this->header->getArrowWidth() + 1), 1.f);
    this->backgroundShape.lineTo(float(this->getWidth() - 1), float(Globals::UI::headlineHeight - 1));
    this->backgroundShape.lineTo(1.f, float(Globals::UI::headlineHeight - 1));
    this->backgroundShape.closeSubPath();
}

void HeadlineDropdown::mouseDown(const MouseEvent &e)
{
    if (this->item != nullptr)
    {
        this->item->onSelectedAsMenuItem();
    }
}

void HeadlineDropdown::mouseEnter(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    this->stopTimer();
#endif
}

void HeadlineDropdown::mouseExit(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    this->startTimer(50);
#endif
}

void HeadlineDropdown::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::DismissModalComponentAsync:
    case CommandIDs::MenuDismiss:
        // exit triggered asynchronously by the content component:
        // fixme doesnt work well, the menu may re-appear instantly
        this->dismiss();
        return;
    case CommandIDs::MenuSelect:
        if (this->cursor != nullptr && this->item != nullptr)
        {
            this->item->onSelectedAsMenuItem();
            return;
        }
        else
        {
            this->content->postCommandMessage(commandId);
        }
        break;
    case CommandIDs::MenuCursorTryExitUp:
        this->showCursor();
        this->resized();
        this->content->postCommandMessage(CommandIDs::MenuCursorHide);
        break;
    case CommandIDs::MenuCursorUp:
    case CommandIDs::MenuCursorPageUp:
        this->shouldDismissOnMouseExit = false;
        if (this->cursor == nullptr)
        {
            this->content->postCommandMessage(commandId);
        }
        break;
    case CommandIDs::MenuCursorDown:
    case CommandIDs::MenuCursorPageDown:
        this->cursor = nullptr;
        this->shouldDismissOnMouseExit = false;
        this->content->postCommandMessage(commandId);
        break;
    case CommandIDs::MenuForward:
        if (this->cursor != nullptr)
        {
            App::Layout().showNeighbourMenu(this->item, 1);
        }
        else
        {
            this->content->postCommandMessage(commandId);
        }
        break;
    case CommandIDs::MenuBack:
        if (this->cursor != nullptr)
        {
            App::Layout().showNeighbourMenu(this->item, -1);
        }
        else
        {
            this->content->postCommandMessage(commandId);
        }
        break;
    default:
        break;
    }
}

HotkeyScheme::Ptr HeadlineDropdown::getHotkeyScheme()
{
    return App::Config().getHotkeySchemes()->getCurrent();
}

bool HeadlineDropdown::keyPressed(const KeyPress &key)
{
    getHotkeyScheme()->dispatchKeyPress(key, this, this);
    return true;
}

bool HeadlineDropdown::keyStateChanged(bool isKeyDown)
{
    getHotkeyScheme()->dispatchKeyStateChange(isKeyDown, this, this);
    return true;
}

void HeadlineDropdown::inputAttemptWhenModal()
{
    this->dismiss();
}

void HeadlineDropdown::dismiss()
{
    this->stopTimer();
    App::cancelAnimation(this);
    this->exitModalState(0);
    UniquePointer<Component> deleter(this);
}

void HeadlineDropdown::showCursor()
{
    this->shouldDismissOnMouseExit = false;
    this->cursor = make<HeadlineItemCursorComponent>(this->header->getContentX(), this->header->getContentWidth());
    this->addAndMakeVisible(this->cursor.get());
}

template<typename T>
T *getComponentOrParentAs(Component *target)
{
    Component *c = target;

    while (c != nullptr)
    {
        if (T *cast = dynamic_cast<T *>(c))
        {
            return cast;
        }

        c = c->getParentComponent();
    }

    return nullptr;
}

void HeadlineDropdown::childBoundsChanged(Component *child)
{
    this->syncWidthWithContent();
}

void HeadlineDropdown::timerCallback()
{
#if PLATFORM_DESKTOP

    auto *componentUnderMouse =
        Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();

    if (componentUnderMouse == nullptr &&
        this->content->getScreenBounds().contains(
            Desktop::getInstance().getMainMouseSource().getScreenPosition().toInt()))
    {
        return; // wtf, componentUnderMouse shouldn't be null, but hasn't updated yet?
    }

    const auto notHoveringSelf =
        getComponentOrParentAs<HeadlineDropdown>(componentUnderMouse) != this;

    const auto hoveringOtherMenu =
        getComponentOrParentAs<HeadlineItem>(componentUnderMouse) != nullptr;

    const auto shouldDismiss =
        this->shouldDismissOnMouseExit ?
        notHoveringSelf :
        notHoveringSelf && hoveringOtherMenu;

    if (shouldDismiss)
    {
        this->stopTimer();
        this->exitModalState(0);

        App::animateComponent(this,
            this->getBounds(), 0.f, Globals::UI::fadeOutShort / 3, true, 0.f, 1.f);

        UniquePointer<Component> deleter(this);
    }

#endif
}

void HeadlineDropdown::syncWidthWithContent()
{
    if (this->getWidth() != this->content->getWidth() + HeadlineDropdown::padding ||
        this->header->getWidth() != this->content->getWidth() + HeadlineDropdown::padding ||
        this->getHeight() != this->content->getHeight() + Globals::UI::headlineHeight)
    {
        const int w = jmax(this->header->getWidth(), this->content->getWidth() + HeadlineDropdown::padding);
        const int h = this->content->getHeight() + Globals::UI::headlineHeight;
        this->setSize(w, jmin(h, App::getWindowBounds().getHeight()));
    }
}
