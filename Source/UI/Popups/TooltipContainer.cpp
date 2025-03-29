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
#include "TooltipContainer.h"
#include "HelioTheme.h"

TooltipContainer::TooltipContainer()
{
    this->setVisible(false);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->tooltipComponent = make<Component>();
    this->addAndMakeVisible(this->tooltipComponent.get());

    if (App::isRunningOnPhone())
    {
        this->setSize(450, 36);
    }
    else
    {
        this->setSize(500, 48);
    }
}

TooltipContainer::~TooltipContainer() = default;

void TooltipContainer::paint(Graphics &g)
{
    g.setColour(this->backgroundColour);
    g.fillRect(this->getLocalBounds().reduced(1, 1));

    g.setColour(this->borderColour);
    g.fillRect(1, 0, this->getWidth() - 2, 1);
    g.fillRect(1, this->getHeight() - 1, this->getWidth() - 2, 1);
    g.fillRect(0, 1, 1, this->getHeight() - 2);
    g.fillRect(this->getWidth() - 1, 1, 1, this->getHeight() - 2);
}

void TooltipContainer::resized()
{
    this->tooltipComponent->setBounds(this->getLocalBounds());
}

void TooltipContainer::parentHierarchyChanged()
{
    this->updatePosition();
}

void TooltipContainer::parentSizeChanged()
{
    this->updatePosition();
}

void TooltipContainer::updatePosition()
{
    this->setCentrePosition(this->getParentWidth() / 2,
        this->alignedToBottom ?
        (this->getParentHeight() - Globals::UI::projectMapHeight / 2) :
        ((this->getHeight() / 2) + 8));
}

void TooltipContainer::timerCallback()
{
    const int clicksCount = Desktop::getInstance().getMouseButtonClickCounter();

    if (clicksCount > this->clicksCountOnStart)
    {
        this->hide();
    }

    this->timeCounter += TooltipContainer::timerMs;

    if (this->timeCounter > this->hideTimeout)
    {
        this->hide();
    }
}

void TooltipContainer::showWithComponent(UniquePointer<Component> newComponent, int timeOutMs)
{
    const Point<int> clickOrigin =
        Desktop::getInstance().getMainMouseSource().getLastMouseDownPosition().toInt();

    this->showWithComponent(move(newComponent),
        Rectangle<int>(clickOrigin, clickOrigin.translated(1, 1)),
        timeOutMs);
}

void TooltipContainer::showWithComponent(UniquePointer<Component> newComponent,
    Rectangle<int> callerScreenBounds, int timeOutMs)
{
    if (newComponent == nullptr)
    {
        this->hide();
        return;
    }

#if PLATFORM_MOBILE

    const Point<int> callerOrigin = callerScreenBounds.getCentre();

    const Point<int> topLevelOrigin =
        this->getTopLevelComponent()->getScreenPosition();

    // there's much visual less space on mobile platforms,
    // so tooltip will try to detect a better position

    this->alignedToBottom =
        (callerOrigin - topLevelOrigin).getY() < (this->getTopLevelComponent()->getHeight() / 2);

#endif

    this->clicksCountOnStart = Desktop::getInstance().getMouseButtonClickCounter();
    this->timeCounter = 0;

    if (!this->isVisible())
    {
        this->setVisible(true);
        this->setAlpha(1.f);
    }

    this->animator.cancelAllAnimations(false);
    this->removeChildComponent(this->tooltipComponent.get());
    this->stopTimer();

    this->updatePosition();
    this->hideTimeout = (timeOutMs > 0) ? timeOutMs : (1000 * 60 * 60);
    this->startTimer(TooltipContainer::timerMs);

    this->tooltipComponent = move(newComponent);
    this->addAndMakeVisible(this->tooltipComponent.get());
    this->resized();
    this->toFront(false);
}

void TooltipContainer::hide()
{
    this->stopTimer();
    if (this->isVisible())
    {
        this->animator.fadeOut(this, Globals::UI::fadeOutLong);
        this->tooltipComponent = make<Component>(); // empty, but not nullptr
        this->setVisible(false);
    }
}
