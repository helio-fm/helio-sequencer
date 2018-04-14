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
    source(nullptr) {}

void SelectionComponent::beginLasso(const MouseEvent &e, LassoSource<SelectableComponent *> *const lassoSource)
{
    jassert(lassoSource != nullptr);
    jassert(getParentComponent() != nullptr);

    if (lassoSource != nullptr)
    {
        source = lassoSource;
        originalSelection = lassoSource->getLassoSelection().getItemArray();
        this->setSize(0, 0);
        this->toFront(false);
        dragStartPos = e.getMouseDownPosition();
    }
}

void SelectionComponent::dragLasso(const MouseEvent &e)
{
    if (source != nullptr)
    {
        this->setBounds(Rectangle<int>(dragStartPos, e.getPosition()));
        this->setVisible(true);

        Array<SelectableComponent *> itemsInLasso;
        source->findLassoItemsInArea(itemsInLasso, getBounds());

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

        source->getLassoSelection() = Lasso(itemsInLasso);
    }
}

void SelectionComponent::endLasso()
{
    if (source != nullptr)
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
