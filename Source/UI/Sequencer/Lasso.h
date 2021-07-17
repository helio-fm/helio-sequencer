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
#include "UndoActionIDs.h"

class Lasso final : public SelectedItemSet<SelectableComponent *>
{
public:

    Lasso();
    explicit Lasso(const ItemArray &items);
    explicit Lasso(const SelectedItemSet &other);

    void itemSelected(SelectableComponent *item) override;
    void itemDeselected(SelectableComponent *item) override;

    int64 getId() const noexcept;
    bool shouldDisplayGhostNotes() const noexcept;
    void needsToCalculateSelectionBounds() noexcept;
    Rectangle<int> getSelectionBounds() const noexcept;

    // Transaction identifier, and why is it needed:
    // Some actions, like dragging notes around, are performed in a single undo transaction,
    // but, unlike mouse dragging (where it's clear when to start and when to end a transaction),
    // hotkey-handled actions will always do a checkpoint at every keypress, so that
    // pressing `cursor down` 5 times and `cursor up` 3 times will result in 8 undo actions,
    // (there only should be 2, for transposing events down and up accordingly).
    // So, Lasso class re-generates its random id every time it changes,
    // and some transform operations here will use that id, combined with operation id
    // to identify the transaction and see if the last one was exactly of the same type and target,
    // and checkpoint could be skipped.
    UndoActionId generateLassoTransactionId(int actionId) const
    {
        return (actionId * UndoActionIDs::MaxUndoActionId) + this->getId();
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

private:

    Rectangle<int> bounds;
    
    // A random id which is used to distinguish one selection from another
    // (collisions are still possible, but they are not critical,
    // see SequencerOperations class for usage example)
    mutable int64 id;
    mutable Random random;
    void resetSelectionId();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lasso)
    JUCE_DECLARE_WEAK_REFERENCEABLE(Lasso)
};
