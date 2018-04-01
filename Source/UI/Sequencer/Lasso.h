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

#pragma once

#include "SelectableComponent.h"
#include "MidiSequence.h"

class SelectionProxyArray : public Array<SelectableComponent *>, public ReferenceCountedObject
{
public:

    SelectionProxyArray() {}

    typedef ReferenceCountedObjectPtr<SelectionProxyArray> Ptr;

    template<typename T>
    T *getFirstAs() const
    {
        return static_cast<T *>(this->getFirst());
    }

    template<typename T>
    T *getItemAs(const int index) const
    {
        return static_cast<T *>(this->getUnchecked(index));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionProxyArray);
};

class Lasso : public SelectedItemSet<SelectableComponent *>
{
public:

    Lasso() : SelectedItemSet() {}
    
    explicit Lasso(const ItemArray &items) : SelectedItemSet(items)
    {
        this->invalidateCache();
    }

    explicit Lasso(const SelectedItemSet &other) : SelectedItemSet(other)
    {
        this->invalidateCache();
    }

    void itemSelected(SelectableComponent *item) override
    {
        this->invalidateCache();
        item->setSelected(true);
    }

    void itemDeselected(SelectableComponent *item) override
    {
        this->invalidateCache();
        item->setSelected(false);
    }

    void needsToCalculateSelectionBounds()
    {
        this->bounds = Rectangle<int>();

        for (int i = 0; i < this->getNumSelected(); ++i)
        {
            this->bounds = this->bounds.getUnion(this->getSelectedItem(i)->getBounds());
        }
    }

    Rectangle<int> getSelectionBounds() const
    {
        return this->bounds;
    }
    
    typedef SparseHashMap<String, SelectionProxyArray::Ptr, StringHash> GroupedSelections;

    void invalidateCache()
    {
        this->selectionsCache.clear();
    }

    const GroupedSelections &getGroupedSelections() const
    {
        if (this->selectionsCache.size() == 0)
        {
            this->rebuildCache();
        }
        
        return this->selectionsCache;
    }

    template<typename T>
    T *getFirstAs() const
    {
        return static_cast<T *>(this->getSelectedItem(0));
    }

    template<typename T>
    T *getItemAs(const int index) const
    {
        return static_cast<T *>(this->getSelectedItem(index));
    }

    bool shouldDisplayGhostNotes() const
    {
        return (this->getNumSelected() <= 32); // just a sane limit
    }

private:

    Rectangle<int> bounds;
    
    mutable GroupedSelections selectionsCache;

    void rebuildCache() const
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
};
