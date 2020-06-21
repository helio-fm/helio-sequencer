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
#include "HeaderSelectionIndicator.h"
#include "ColourIDs.h"

HeaderSelectionIndicator::HeaderSelectionIndicator() :
    fill(findDefaultColour(ColourIDs::SelectionComponent::outline)),
    currentFill(fill)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->setSize(128, 10);
}

void HeaderSelectionIndicator::paint(Graphics &g)
{
    g.setColour(this->currentFill);
    g.fillRect(0, this->getHeight() - 3, this->getWidth(), 2);
    g.fillRect(1, this->getHeight() - 4, this->getWidth() - 2, 1);
}

void HeaderSelectionIndicator::parentHierarchyChanged()
{
    this->updateBounds();
}

void HeaderSelectionIndicator::parentSizeChanged()
{
    this->updateBounds();
}

void HeaderSelectionIndicator::setStartAnchor(double absX)
{
    this->startAbsPosition = absX;
    this->endAbsPosition = absX;
    this->updateBounds();
}

void HeaderSelectionIndicator::setEndAnchor(double absX)
{
    this->endAbsPosition = absX;
    this->updateBounds();
}

void HeaderSelectionIndicator::updateBounds()
{
    const double start = jmin(this->startAbsPosition, this->endAbsPosition);
    const double end = jmax(this->startAbsPosition, this->endAbsPosition);

    const int startX = int(double(this->getParentWidth()) * start);
    const int endX = int(double(this->getParentWidth()) * end);

    this->setBounds(startX, this->getY(), (endX - startX), this->getHeight());
}

void HeaderSelectionIndicator::timerCallback()
{
    const auto newFill = this->currentFill.interpolatedWith(this->targetFill, 0.4f);

    if (this->currentFill == newFill)
    {
        this->stopTimer();

        if (newFill.getAlpha() == 0)
        {
            this->setVisible(false);
        }
    }
    else
    {
        this->currentFill = newFill;
        this->updateBounds();
        this->repaint();
    }
}

void HeaderSelectionIndicator::fadeIn()
{
    this->targetFill = this->fill;
    this->setVisible(true);
    this->startTimerHz(60);
}

void HeaderSelectionIndicator::fadeOut()
{
    this->targetFill = Colours::transparentBlack;
    this->startTimerHz(60);
}
