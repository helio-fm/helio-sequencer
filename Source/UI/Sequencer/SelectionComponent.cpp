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
#include "Lasso.h"
#include "SelectionComponent.h"
#include "HelioTheme.h"

SelectionComponent::SelectionComponent() :
    source(nullptr)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

void SelectionComponent::beginLasso(const MouseEvent &e, LassoSource<SelectableComponent *> *const lassoSource)
{
    jassert(lassoSource != nullptr);
    jassert(getParentComponent() != nullptr);

    if (lassoSource != nullptr)
    {
        this->source = lassoSource;
        this->originalSelection = lassoSource->getLassoSelection().getItemArray();
        this->setSize(0, 0);
        this->toFront(false);
        this->startPosition = e.position.toDouble() / this->getParentSize();
    }
}

void SelectionComponent::dragLasso(const MouseEvent &e)
{
    if (this->source != nullptr)
    {
        this->endPosition = e.position.toDouble() / this->getParentSize();

        this->updateBounds();
        this->setVisible(true);

        Array<SelectableComponent *> itemsInLasso;
        this->source->findLassoItemsInArea(itemsInLasso, getBounds());

        if (e.mods.isShiftDown())
        {
            itemsInLasso.removeValuesIn(originalSelection);
            itemsInLasso.addArray(originalSelection);
        }
        else if (e.mods.isAltDown())
        {
            Array<SelectableComponent *> originalMinusNew(originalSelection);
            originalMinusNew.removeValuesIn(itemsInLasso);

            itemsInLasso.removeValuesIn(originalSelection);
            itemsInLasso.addArray(originalMinusNew);
        }

        this->source->getLassoSelection() = Lasso(itemsInLasso);
    }
}

void SelectionComponent::updateBounds()
{
    const auto parentSize(this->getParentSize());
    const auto start = (this->startPosition * parentSize).toInt();
    const auto end = (this->endPosition * parentSize).toInt();
    this->setBounds({ start, end });
}

void SelectionComponent::endLasso()
{
    if (this->source != nullptr)
    {
        this->source = nullptr;
        this->originalSelection.clear();
        this->setVisible(false);
    }
}

bool SelectionComponent::isDragging() const
{
    return (this->source != nullptr);
}

void SelectionComponent::paint(Graphics &g)
{
    this->getLookAndFeel().drawLasso(g, *this);
    jassert(isMouseButtonDownAnywhere());
}

const Point<double> SelectionComponent::getParentSize() const
{
    if (const auto *p = this->getParentComponent())
    {
        return { double(p->getWidth()), double(p->getHeight()) };
    }

    return { 1.0, 1.0 };
}
