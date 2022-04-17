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
#include "HeadlineItem.h"

#include "IconComponent.h"
#include "Headline.h"
#include "HeadlineDropdown.h"
#include "CachedLabelImage.h"
#include "ColourIDs.h"
#include "MainLayout.h"

// A dashed bottom stroke to indicate that
// a context menu has been opened for this item
class HeadlineContextMenuMarker final : public Component
{
public:

    HeadlineContextMenuMarker()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
        this->setSize(1, 3);
        this->setAlpha(0.f);
    }

    void paint(Graphics &g) override
    {
        static constexpr auto dashLength = 8;
        static constexpr auto s = dashLength - 2;
        const auto width = this->getWidth() - s;
        const auto w = width - (width % dashLength);

        g.setColour(this->dark);
        g.fillRect(s + 1, 1, w, 1);
        g.fillRect(s, 2, w, 1);

        g.setColour(this->light);
        for (int i = s; i < w; i += (dashLength * 2))
        {
            g.fillRect(i + 1, 1, dashLength, 1);
            g.fillRect(i, 2, dashLength, 1);
        }
    }

    const Colour dark = findDefaultColour(ColourIDs::Common::borderLineDark).withMultipliedAlpha(0.5f);
    const Colour light = findDefaultColour(ColourIDs::Common::borderLineLight).withMultipliedAlpha(2.f);
};

HeadlineItem::HeadlineItem(WeakReference<HeadlineItemDataSource> treeItem, AsyncUpdater &parent) :
    item(treeItem),
    parentHeadline(parent)
{
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(false);

    this->titleLabel = make<Label>();
    this->addAndMakeVisible(this->titleLabel.get());
    this->titleLabel->setFont(Globals::UI::Fonts::M);
    this->titleLabel->setJustificationType(Justification::centredLeft);
    this->titleLabel->setInterceptsMouseClicks(false, false);
    this->titleLabel->setCachedComponentImage(new CachedLabelImage(*this->titleLabel));

    this->icon = make<IconComponent>(Icons::helio);
    this->addAndMakeVisible(this->icon.get());

    this->arrow = make<HeadlineItemArrow>();
    this->addAndMakeVisible(this->arrow.get());

    this->menuMarker = make<HeadlineContextMenuMarker>();
    this->addChildComponent(this->menuMarker.get());

    this->setSize(256, Globals::UI::headlineHeight);

    if (this->item != nullptr)
    {
        this->item->addChangeListener(this);
    }
}

HeadlineItem::~HeadlineItem()
{
    this->stopTimer();

    if (this->item != nullptr)
    {
        this->item->removeChangeListener(this);
    }
}

void HeadlineItem::paint(Graphics &g)
{
    g.setColour(this->bgColour);
    g.fillPath(this->backgroundShape);
}

void HeadlineItem::resized()
{
    this->menuMarker->setBounds(Headline::itemsOverlapOffset,
        this->getHeight() - this->menuMarker->getHeight(),
        this->getWidth() - Headline::itemsOverlapOffset,
        this->menuMarker->getHeight());

    constexpr auto iconX = 12;
    constexpr auto iconSize = 26;
    constexpr auto titleX = 33;
    constexpr auto titleHeight = 30;

    this->titleLabel->setBounds(titleX,
        (this->getHeight() / 2) - (titleHeight / 2),
        this->getWidth() - titleX, titleHeight);

    this->icon->setBounds(iconX,
        (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);

    this->arrow->setBounds(this->getWidth() - HeadlineItemArrow::arrowWidth,
        0, HeadlineItemArrow::arrowWidth, this->getHeight());

    this->backgroundShape.clear();
    this->backgroundShape.startNewSubPath(2.0f, 1.0f);
    this->backgroundShape.lineTo(float(this->getWidth() - HeadlineItemArrow::arrowWidth), 1.0f);
    this->backgroundShape.lineTo(float(this->getWidth() - 2), float(this->getHeight() - 2));
    this->backgroundShape.lineTo(1.0f, float(this->getHeight() - 1));
    this->backgroundShape.lineTo(2.0f, float(this->getHeight() - 2));
    this->backgroundShape.closeSubPath();
}

void HeadlineItem::mouseEnter(const MouseEvent& e)
{
#if PLATFORM_DESKTOP
    // A hacky way to prevent re-opening the menu again after the new page is shown.
    // Somehow comparing current mouse screen position to e.getMouseDownScreenPosition()
    // won't work (maybe a JUCE bug), so get it from getMainMouseSource:
    const auto lastMouseDown =
        Desktop::getInstance().getMainMouseSource().getLastMouseDownPosition().toInt();
    if (lastMouseDown != e.getScreenPosition())
    {
        this->startTimer(100);
    }
#endif
}

void HeadlineItem::mouseExit(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    this->stopTimer();
#endif
}

void HeadlineItem::mouseDown(const MouseEvent &e)
{
    if (this->item != nullptr)
    {
        this->stopTimer();

        // on desktop versions, as quick click on a headline item opens its node's page,
        // on mobile versions, it always opens the menu first
#if PLATFORM_DESKTOP
        if (this->item->canBeSelectedAsMenuItem())
        {
            this->item->onSelectedAsMenuItem();
        }
        else
        {
            this->showMenuIfAny();
        }
#elif PLATFORM_MOBILE
        this->showMenuIfAny();
#endif
    }
}

void HeadlineItem::mouseUp(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    this->stopTimer();
#endif
}

WeakReference<HeadlineItemDataSource> HeadlineItem::getDataSource() const noexcept
{
    return this->item;
}

void HeadlineItem::updateContent()
{
    if (this->item != nullptr)
    {
        this->icon->setIconImage(this->item->getIcon());
        this->titleLabel->setText(this->item->getName(), dontSendNotification);
        const int textWidth = this->titleLabel->getFont().getStringWidth(this->titleLabel->getText());
        const int maxTextWidth = this->titleLabel->getWidth();
        this->setSize(jmin(textWidth, maxTextWidth) + 46 + Headline::itemsOverlapOffset, Globals::UI::headlineHeight - 1);
    }
}

void HeadlineItem::changeListenerCallback(ChangeBroadcaster *source)
{
    this->parentHeadline.triggerAsyncUpdate();
}

void HeadlineItem::timerCallback()
{
    this->stopTimer();
    this->showMenuIfAny();
}

void HeadlineItem::showMenuIfAny()
{
    if (this->item != nullptr && this->item->hasMenu())
    {
        App::showModalComponent(make<HeadlineDropdown>(this->item, this->getPosition()));
    }
}

void HeadlineItem::showContextMenuMarker()
{
    this->animator.fadeIn(this->menuMarker.get(), Globals::UI::fadeInLong);
    //this->menuMarker->setVisible(true);
}

void HeadlineItem::hideContextMenuMarker()
{
    this->animator.fadeOut(this->menuMarker.get(), Globals::UI::fadeOutLong);
    //this->menuMarker->setVisible(false);
}
