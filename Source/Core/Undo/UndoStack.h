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

class UndoStack final : public ChangeBroadcaster, public Serializable
{
public:

    explicit UndoStack(ProjectNode &parentProject,
        int maxNumberOfUnitsToKeep = 30000,
        int minimumTransactionsToKeep = 30);
    
    void clearUndoHistory();

    bool perform(UndoAction *action);
    bool perform(UndoAction *action, const String &actionName);
    
    void beginNewTransaction() noexcept;
    void beginNewTransaction(const String &actionName) noexcept;
    void setCurrentTransactionName(const String &newName) noexcept;
    
    bool canUndo() const noexcept;
    String getUndoDescription() const;
    
    bool undo();
    bool undoCurrentTransactionOnly();
    
    bool canRedo() const noexcept;
    String getRedoDescription() const;
    bool redo();
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    void getActionsInCurrentTransaction(Array<const UndoAction *> &actionsFound) const;
    int getNumActionsInCurrentTransaction() const;

    ProjectNode &project;
    
    struct ActionSet final : public Serializable
    {
        ActionSet(ProjectNode &project, const String &transactionName);

        bool perform() const;
        bool undo() const;
        int getTotalSize() const;

        ValueTree serialize() const;
        void deserialize(const ValueTree &tree);
        void reset();

        UndoAction *createUndoActionsByTagName(const Identifier &tagName);

        OwnedArray<UndoAction> actions;
        String name;

        ProjectNode &project;
    };
    
    OwnedArray<ActionSet> transactions;
    String newTransactionName;
    
    int totalUnitsStored, maxNumUnitsToKeep, minimumTransactionsToKeep, nextIndex;
    bool newTransaction, reentrancyCheck;
    
    ActionSet *getCurrentSet() const noexcept;
    ActionSet *getNextSet() const noexcept;
    
    void clearFutureTransactions();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UndoStack)
};
