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
#include "HeadlineDropdown.h"

#include "HeadlineItemDataSource.h"
#include "Headline.h"
#include "ColourIDs.h"

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

    this->startTimer(150);
}

HeadlineDropdown::~HeadlineDropdown()
{
    this->stopTimer();
}

void HeadlineDropdown::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill).brighter(0.035f));
    g.fillRect(1, Globals::UI::headlineHeight - 3, this->getWidth() - 3, this->getHeight() - Globals::UI::headlineHeight + 2);
    g.fillPath(this->internalPath1);

    // Draw a nice border around the menu:
    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark).withMultipliedAlpha(0.75f));
    g.drawHorizontalLine(0, 1.f, float(this->getWidth() - Headline::itemsOverlapOffset));
    g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
    g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth() - 1));
    g.drawVerticalLine(this->getWidth() - 2, float(Globals::UI::headlineHeight), float(this->getHeight() - 1));

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
    g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
    g.fillRect(1.f, 1.f, 3.f, float(this->getHeight() - 3));
    g.drawVerticalLine(this->getWidth() - 3, float(Globals::UI::headlineHeight), float(this->getHeight() - 1));
}

void HeadlineDropdown::resized()
{
    this->content->setBounds(HeadlineDropdown::padding / 2 + 1,
        Globals::UI::headlineHeight - 1,
        this->getWidth() - HeadlineDropdown::padding,
        this->getHeight() - Globals::UI::headlineHeight);

    this->header->setBounds(0, 0, this->getWidth() - 0, Globals::UI::headlineHeight);

    this->internalPath1.clear();
    this->internalPath1.startNewSubPath(1.f, 1.f);
    this->internalPath1.lineTo(float(this->getWidth() - Headline::itemsOverlapOffset), 1.f);
    this->internalPath1.lineTo(float(this->getWidth() - 2), float(Globals::UI::headlineHeight - 2));
    this->internalPath1.lineTo(1.f, float(Globals::UI::headlineHeight - 1));
    this->internalPath1.closeSubPath();
}

void HeadlineDropdown::mouseDown(const MouseEvent &e)
{
    if (this->item != nullptr)
    {
        this->item->onSelectedAsMenuItem();
    }
}

void HeadlineDropdown::inputAttemptWhenModal()
{
    this->stopTimer();
    Desktop::getInstance().getAnimator().cancelAllAnimations(false);
    this->exitModalState(0);
    delete this;
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
    Component *componentUnderMouse =
        Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();

    HeadlineDropdown *root =
        findParent<HeadlineDropdown>(componentUnderMouse);

    if (componentUnderMouse != nullptr && root != this)
    {
        this->stopTimer();
        this->exitModalState(0);
        Desktop::getInstance().getAnimator().cancelAllAnimations(false);
        Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 100, true, 0.f, 1.f);
        delete this;
    }
}

void HeadlineDropdown::syncWidthWithContent()
{
    if (this->getWidth() != this->content->getWidth() + HeadlineDropdown::padding ||
        this->header->getWidth() != this->content->getWidth() + HeadlineDropdown::padding ||
        this->getHeight() != this->content->getHeight() + Globals::UI::headlineHeight)
    {
        const int w = jmax(this->header->getWidth(), this->content->getWidth() + HeadlineDropdown::padding);
        this->setSize(w, this->content->getHeight() + Globals::UI::headlineHeight);
    }
}
