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

class SelectionProxyArray final : public Array<SelectableComponent *>, public ReferenceCountedObject
{
public:

    SelectionProxyArray() = default;

    using Ptr = ReferenceCountedObjectPtr<SelectionProxyArray>;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionProxyArray)
};

class Lasso final : public SelectedItemSet<SelectableComponent *>
{
public:

    Lasso();
    explicit Lasso(const ItemArray &items);
    explicit Lasso(const SelectedItemSet &other);

    void itemSelected(SelectableComponent *item) override;
    void itemDeselected(SelectableComponent *item) override;

    void needsToCalculateSelectionBounds() noexcept;
    Rectangle<int> getSelectionBounds() const noexcept;
    
    using GroupedSelections = SparseHashMap<String, SelectionProxyArray::Ptr, StringHash>;

    // Gets all selected events, split by track
    // So that is easier to perform undo/redo actions
    const GroupedSelections &getGroupedSelections() const;
    void invalidateCache();
    bool shouldDisplayGhostNotes() const noexcept;

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

private:

    Rectangle<int> bounds;
    
    mutable GroupedSelections selectionsCache;
    void rebuildCache() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lasso)
    JUCE_DECLARE_WEAK_REFERENCEABLE(Lasso)
};
