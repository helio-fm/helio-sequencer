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
#include "ColourIDs.h"

class HeadlineItemHighlighter final : public Component
{
public:

    explicit HeadlineItemHighlighter(WeakReference<HeadlineItemDataSource> targetItem) :
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
            const int textWidth = this->titleLabel->getFont().getStringWidth(this->titleLabel->getText());
            this->setSize(textWidth + 64, Globals::UI::headlineHeight);
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

    int getArrowWidth() const noexcept
    {
        return this->arrow->getWidth();
    }

private:

    WeakReference<HeadlineItemDataSource> item;

    UniquePointer<Label> titleLabel;
    UniquePointer<IconComponent> icon;
    UniquePointer<HeadlineItemArrow> arrow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemHighlighter)
};

HeadlineDropdown::HeadlineDropdown(WeakReference<HeadlineItemDataSource> targetItem, const Point<int> &position) :
    item(targetItem)
{
    this->header = make<HeadlineItemHighlighter>(targetItem);
    this->addAndMakeVisible(this->header.get());

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
#if PLATFORM_DESKTOP
    this->stopTimer();
#endif
}

void HeadlineDropdown::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::Breadcrumbs::fill).brighter(0.03f));
    g.fillPath(this->backgroundShape);
    g.fillRect(1, Globals::UI::headlineHeight - 2,
        this->getWidth() - 2, this->getHeight() - Globals::UI::headlineHeight + 2);

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark).withMultipliedAlpha(0.5f));
    g.drawHorizontalLine(0, 1.f, float(this->getWidth() - this->header->getArrowWidth() + 1));

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
    g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));
    g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth() - 1));
    g.drawVerticalLine(this->getWidth() - 1, float(Globals::UI::headlineHeight), float(this->getHeight() - 1));

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
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

    this->header->setBounds(0, 0, this->getWidth() - 0, Globals::UI::headlineHeight);

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

void HeadlineDropdown::inputAttemptWhenModal()
{
    this->stopTimer();
    Desktop::getInstance().getAnimator().cancelAllAnimations(false);
    this->exitModalState(0);
    UniquePointer<Component> deleter(this);
}

template<typename T>
T *findParent(Component *target)
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
    auto *componentUnderMouse = Desktop::getInstance().
        getMainMouseSource().getComponentUnderMouse();

    if (componentUnderMouse != nullptr &&
        this != findParent<HeadlineDropdown>(componentUnderMouse))
    {
        this->stopTimer();
        this->exitModalState(0);

        Desktop::getInstance().getAnimator().animateComponent(this,
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
