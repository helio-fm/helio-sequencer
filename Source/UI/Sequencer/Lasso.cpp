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

Lasso::Lasso() : SelectedItemSet() {}
Lasso::Lasso(const ItemArray &items) : SelectedItemSet(items) {}
Lasso::Lasso(const SelectedItemSet &other) : SelectedItemSet(other) {}

Lasso::~Lasso()
{
    this->masterReference.clear();
}

void Lasso::itemSelected(SelectableComponent *item)
{
    this->invalidateCache();
    item->setSelected(true);
}

void Lasso::itemDeselected(SelectableComponent *item)
{
    this->invalidateCache();
    item->setSelected(false);
}

void Lasso::needsToCalculateSelectionBounds() noexcept
{
    this->bounds = Rectangle<int>();

    for (int i = 0; i < this->getNumSelected(); ++i)
    {
        this->bounds = this->bounds.getUnion(this->getSelectedItem(i)->getBounds());
    }
}

Rectangle<int> Lasso::getSelectionBounds() const noexcept
{
    return this->bounds;
}

void Lasso::invalidateCache()
{
    this->selectionsCache.clear();
}

const Lasso::Lasso::GroupedSelections & Lasso::getGroupedSelections() const
{
    if (this->selectionsCache.size() == 0)
    {
        this->rebuildCache();
    }

    return this->selectionsCache;
}

bool Lasso::shouldDisplayGhostNotes() const noexcept
{
    return (this->getNumSelected() <= 32); // just a sane limit
}

void Lasso::rebuildCache() const
{
    for (int i = 0; i < this->getNumSelected(); ++i)
    {
        SelectableComponent *item = this->getSelectedItem(i);
        const String &groupId(item->getSelectionGroupId());

        SelectionProxyArray::Ptr targetArray;

        if (this->selectionsCache.contains(groupId))
        {
            targetArray = this->selectionsCache[groupId];
        }
        else
        {
            targetArray = new SelectionProxyArray();
        }

        targetArray->add(item);
        this->selectionsCache[groupId] = targetArray;
    }
}
