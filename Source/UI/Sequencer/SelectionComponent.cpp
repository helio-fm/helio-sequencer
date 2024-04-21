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
#include "Lasso.h"
#include "SelectionComponent.h"
#include "PointReduction.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

SelectionComponent::SelectionComponent() :
    fill(findDefaultColour(ColourIDs::SelectionComponent::fill)),
    outline(findDefaultColour(ColourIDs::SelectionComponent::outline)),
    currentFill(fill),
    currentOutline(outline)
{
    this->setSize(0, 0);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

void SelectionComponent::beginLasso(const Point<float> &position,
    DrawableLassoSource<SelectableComponent *> *lassoSource,
    LassoType lassoType /*= LassoType::Rectangle*/)
{
    jassert(lassoSource != nullptr);
    jassert(this->getParentComponent() != nullptr);

    this->lassoType = lassoType;

    if (lassoSource == nullptr)
    {
        jassertfalse;
        return;
    }

    this->source = lassoSource;
    this->originalSelection = lassoSource->getLassoSelection().getItemArray();
    const auto absPosition = position.toDouble() / this->getParentSize();
    this->startPosition = absPosition;
    this->endPosition = this->startPosition;

    if (this->lassoType == LassoType::Path)
    {
        this->drawnAreaRaw.clearQuick();
        this->drawnArea.clearQuick();
        this->drawnPathFill.clear();
        this->drawnPathOutline.clear();
        this->drawnAreaRaw.add(absPosition);
    }

    this->setSize(0, 0);
    this->toFront(false);
    this->fadeIn();
}

void SelectionComponent::dragLasso(const MouseEvent &e)
{
    if (this->source == nullptr)
    {
        jassertfalse;
        return;
    }

    this->itemsInLasso.clearQuick();
    const auto parentSize = this->getParentSize();
    const auto absPosition = e.position.toDouble() / parentSize;

    if (this->lassoType == LassoType::Rectangle)
    {
        this->endPosition = absPosition;
        this->updateBounds();
        this->source->findLassoItemsInArea(this->itemsInLasso, this->getBounds());
    }
    else if (this->lassoType == LassoType::Path)
    {
        jassert(!this->drawnAreaRaw.isEmpty());

        constexpr auto prefilterThreshold = 1;
        const auto lastPositionInPixels = this->drawnAreaRaw.getLast() * parentSize;
        const auto d = lastPositionInPixels.getDistanceFrom(e.position.toDouble());
        if (d > prefilterThreshold)
        {
            this->drawnAreaRaw.add(absPosition);
        }

        // mapping this from the absolute values all the time
        // in case of zooming in/out while drawing the selection
        this->drawnArea.clearQuick();
        for (const auto &absPos : this->drawnAreaRaw)
        {
            this->startPosition.setXY(
                jmin(this->startPosition.x, absPos.x),
                jmin(this->startPosition.y, absPos.y));

            this->endPosition.setXY(
                jmax(this->endPosition.x, absPos.x),
                jmax(this->endPosition.y, absPos.y));

            this->drawnArea.add((absPos * parentSize).toFloat());
        }

        // noise filtering
        this->drawnArea = PointReduction<float>::simplify(this->drawnArea, 0.5f);
        //DBG("Reduced " + String(this->drawnAreaRaw.size()) + " to " + String(this->drawnArea.size()));

        if (this->drawnArea.size() >= 2)
        {
            this->drawnArea[this->drawnArea.size() - 1] = e.position;
            this->drawnArea.add(this->drawnArea.getFirst());

            const auto startPoint = (this->startPosition * parentSize).toFloat();

            this->drawnPathFill.clear();
            this->drawnPathFill.preallocateSpace(this->drawnArea.size());
            this->drawnPathFill.startNewSubPath(this->drawnArea.getFirst() - startPoint);
            for (int i = 1; i < this->drawnArea.size(); ++i)
            {
                // building a path in local bounds
                this->drawnPathFill.lineTo(this->drawnArea.getUnchecked(i) - startPoint);
            }

            this->drawnPathOutline = this->drawnPathFill;
            static Array<float> dashes(4.f, 4.f);
            PathStrokeType(1.25f).createDashedStroke(this->drawnPathOutline, this->drawnPathOutline,
                dashes.getRawDataPointer(), dashes.size());

            this->updateBounds();
            this->repaint();

            this->source->findLassoItemsInPolygon(this->itemsInLasso,
                this->getBounds(), this->drawnArea);
        }
    }

    if (e.mods.isShiftDown())
    {
        this->itemsInLasso.removeValuesIn(this->originalSelection);
        this->itemsInLasso.addArray(this->originalSelection);
    }
    else if (e.mods.isAltDown())
    {
        this->originalSelection.removeValuesIn(this->itemsInLasso);
        this->itemsInLasso = this->originalSelection;
    }

    this->source->getLassoSelection() = Lasso(this->itemsInLasso);
}

void SelectionComponent::updateBounds()
{
    const auto parentSize = this->getParentSize();
    const auto start = (this->startPosition * parentSize).roundToInt();
    const auto end = (this->endPosition * parentSize).roundToInt();
    Rectangle<int> bounds(start, end);
    bounds.setWidth(jmax(1, bounds.getWidth()));
    bounds.setHeight(jmax(1, bounds.getHeight()));
    this->setBounds(bounds);
}

void SelectionComponent::endLasso()
{
    if (this->source == nullptr)
    {
        jassertfalse;
        return;
    }

    this->source = nullptr;
    this->originalSelection.clear();
    this->fadeOut();
}

bool SelectionComponent::isDragging() const
{
    return this->source != nullptr;
}

void SelectionComponent::paint(Graphics &g)
{
    if (this->lassoType == LassoType::Rectangle)
    {
        g.setColour(this->currentFill);
        g.fillRect(this->getLocalBounds());

        g.setColour(this->currentOutline);
#if PLATFORM_DESKTOP
        HelioTheme::drawDashedFrame(g, this->getLocalBounds(), 4);
#elif PLATFORM_MOBILE
        HelioTheme::drawDashedFrame(g, this->getLocalBounds(), 6);
#endif
    }
    else if (this->lassoType == LassoType::Path)
    {
        g.setColour(this->currentFill);
        g.fillPath(this->drawnPathFill);

        g.setColour(this->currentOutline);
        g.fillPath(this->drawnPathOutline);
    }
}

const Point<double> SelectionComponent::getParentSize() const
{
    if (const auto *p = this->getParentComponent())
    {
        return { double(p->getWidth()), double(p->getHeight()) };
    }

    return { 1.0, 1.0 };
}

void SelectionComponent::timerCallback()
{
    const auto newOutline = this->currentOutline.interpolatedWith(this->targetOutline, 0.5f);
    const auto newFill = this->currentFill.interpolatedWith(this->targetFill, 0.3f);

    if (this->currentOutline == newOutline && this->currentFill == newFill)
    {
        //DBG("--- stop timer");
        this->stopTimer();

        if (newOutline.getAlpha() == 0)
        {
            //DBG("--- set visible = false");
            this->setVisible(false);
        }
    }
    else
    {
        this->currentOutline = newOutline;
        this->currentFill = newFill;
        this->updateBounds();
        this->repaint();
    }
}

void SelectionComponent::fadeIn()
{
    this->targetFill = this->fill;
    this->targetOutline = this->outline;
    this->setVisible(true);
    this->startTimerHz(60);
}

void SelectionComponent::fadeOut()
{
    this->targetFill = Colours::transparentBlack;
    this->targetOutline = Colours::transparentBlack;
    this->startTimerHz(60);
}
