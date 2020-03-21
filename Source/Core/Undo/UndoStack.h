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

class ProjectNode;

#include "UndoAction.h"
#include "UndoActionIDs.h"

class UndoStack final : public ChangeBroadcaster, public Serializable
{
public:

    explicit UndoStack(ProjectNode &parentProject,
        int maxNumberOfUnitsToKeep = 30000,
        int minimumTransactionsToKeep = 30);
    
    void clearUndoHistory();

    bool perform(UndoAction *action);
    bool perform(UndoAction *action, UndoActionId transactionId);
    
    void beginNewTransaction() noexcept;
    void beginNewTransaction(UndoActionId transactionId) noexcept;
    
    bool canUndo() const noexcept;
    UndoActionId getUndoActionId() const;
    
    bool undo();
    bool undoCurrentTransactionOnly();
    
    bool canRedo() const noexcept;
    UndoActionId getRedoActionId() const;
    bool redo();
    
    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;
    
    template<typename T>
    bool undoHas() const
    {
        return UndoStack::transactionHas<T>(this->getCurrentSet());
    }

    template<typename T>
    bool redoHas() const
    {
        return UndoStack::transactionHas<T>(this->getNextSet());
    }

    // for multi-step interactive actions which might involve >1 checkpoints
    bool mergeTransactionsUpTo(UndoActionId transactionId);

private:
    
    void getActionsInCurrentTransaction(Array<const UndoAction *> &actionsFound) const;
    int getNumActionsInCurrentTransaction() const;

    ProjectNode &project;
    
    struct Transaction final : public Serializable
    {
        explicit Transaction(ProjectNode &project,
            UndoActionId transactionId = UndoActionIDs::None);

        bool perform() const;
        bool undo() const;
        int getTotalSize() const;

        SerializedData serialize() const;
        void deserialize(const SerializedData &data);
        void reset();

        UndoAction *createUndoActionByTag(const Identifier &tagName) const;

        OwnedArray<UndoAction> actions;
        UndoActionId id;

        ProjectNode &project;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transaction)
    };
    
    void setCurrentUndoActionId(UndoActionId transactionId) noexcept;
    OwnedArray<Transaction> transactions;
    UndoActionId newUndoActionId;
    
    int totalUnitsStored = 0;
    int maxNumUnitsToKeep = 0;
    int minimumTransactionsToKeep = 0;
    int nextIndex = 0;
    bool hasNewEmptyTransaction = true;
    bool reentrancyCheck = false;
    
    Transaction *getCurrentSet() const noexcept;
    Transaction *getNextSet() const noexcept;

    template<typename T>
    inline static bool transactionHas(Transaction *s)
    {
        if (s != nullptr) // might be an empty set
        {
            for (int i = 0; i < s->actions.size(); ++i)
            {
                if (dynamic_cast<T *>(s->actions.getUnchecked(i)))
                {
                    return true;
                }
            }
        }

        return false;
    }

    void clearFutureTransactions();
    
    JUCE_DECLARE_WEAK_REFERENCEABLE(UndoStack)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndoStack)
};
