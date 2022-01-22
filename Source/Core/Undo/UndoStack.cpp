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
#include "UndoStack.h"
#include "SerializationKeys.h"
#include "ProjectNode.h"

#include "MidiTrackActions.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "NoteActions.h"
#include "AnnotationEventActions.h"
#include "AutomationEventActions.h"
#include "TimeSignatureEventActions.h"
#include "KeySignatureEventActions.h"
#include "ProjectMetadataActions.h"
#include "PatternActions.h"

UndoStack::Transaction::Transaction(ProjectNode &project, UndoActionId transactionId) :
    project(project),
    id(transactionId) {}
    
bool UndoStack::Transaction::perform() const
{
    for (int i = 0; i < this->actions.size(); ++i)
    {
        if (!this->actions.getUnchecked(i)->perform())
        {
            return false;
        }
    }
        
    return true;
}
    
bool UndoStack::Transaction::undo() const
{
    for (int i = this->actions.size(); --i >= 0;)
    {
        if (!this->actions.getUnchecked(i)->undo())
        {
            return false;
        }
    }
        
    return true;
}
    
int UndoStack::Transaction::getTotalSize() const
{
    int total = 0;
    for (int i = this->actions.size(); --i >= 0;)
    {
        total += this->actions.getUnchecked(i)->getSizeInUnits();
    }
        
    return total;
}
    
SerializedData UndoStack::Transaction::serialize() const
{
    SerializedData tree(Serialization::Undo::transaction);

    for (int i = 0; i < this->actions.size(); ++i)
    {
        tree.appendChild(this->actions.getUnchecked(i)->serialize());
    }
        
    return tree;
}
    
void UndoStack::Transaction::deserialize(const SerializedData &data)
{
    this->reset();

    for (const auto &childAction : data)
    {
        if (auto *action = createUndoActionByTag(childAction.getType()))
        {
            action->deserialize(childAction);
            this->actions.add(action);
        }
    }
}
    
void UndoStack::Transaction::reset()
{
    this->actions.clear();
}

UndoAction *UndoStack::Transaction::createUndoActionByTag(const Identifier &tagName) const
{
    using namespace Serialization;

    if      (tagName == Undo::pianoTrackInsertAction)                { return new PianoTrackInsertAction(this->project, &this->project); }
    else if (tagName == Undo::pianoTrackRemoveAction)                { return new PianoTrackRemoveAction(this->project, &this->project); }
    else if (tagName == Undo::automationTrackInsertAction)           { return new AutomationTrackInsertAction(this->project, &this->project); }
    else if (tagName == Undo::automationTrackRemoveAction)           { return new AutomationTrackRemoveAction(this->project, &this->project); }
    else if (tagName == Undo::midiTrackRenameAction)                 { return new MidiTrackRenameAction(this->project); }
    else if (tagName == Undo::midiTrackChangeColourAction)           { return new MidiTrackChangeColourAction(this->project); }
    else if (tagName == Undo::midiTrackChangeInstrumentAction)       { return new MidiTrackChangeInstrumentAction(this->project); }
    else if (tagName == Undo::midiTrackChangeTimeSignatureAction)    { return new MidiTrackChangeTimeSignatureAction(this->project); }
    else if (tagName == Undo::clipInsertAction)                      { return new ClipInsertAction(this->project); }
    else if (tagName == Undo::clipRemoveAction)                      { return new ClipRemoveAction(this->project); }
    else if (tagName == Undo::clipChangeAction)                      { return new ClipChangeAction(this->project); }
    else if (tagName == Undo::clipsGroupInsertAction)                { return new ClipsGroupInsertAction(this->project); }
    else if (tagName == Undo::clipsGroupRemoveAction)                { return new ClipsGroupRemoveAction(this->project); }
    else if (tagName == Undo::clipsGroupChangeAction)                { return new ClipsGroupChangeAction(this->project); }
    else if (tagName == Undo::noteInsertAction)                      { return new NoteInsertAction(this->project); }
    else if (tagName == Undo::noteRemoveAction)                      { return new NoteRemoveAction(this->project); }
    else if (tagName == Undo::noteChangeAction)                      { return new NoteChangeAction(this->project); }
    else if (tagName == Undo::notesGroupInsertAction)                { return new NotesGroupInsertAction(this->project); }
    else if (tagName == Undo::notesGroupRemoveAction)                { return new NotesGroupRemoveAction(this->project); }
    else if (tagName == Undo::notesGroupChangeAction)                { return new NotesGroupChangeAction(this->project); }
    else if (tagName == Undo::annotationEventInsertAction)           { return new AnnotationEventInsertAction(this->project); }
    else if (tagName == Undo::annotationEventRemoveAction)           { return new AnnotationEventRemoveAction(this->project); }
    else if (tagName == Undo::annotationEventChangeAction)           { return new AnnotationEventChangeAction(this->project); }
    else if (tagName == Undo::timeSignatureEventInsertAction)        { return new TimeSignatureEventInsertAction(this->project); }
    else if (tagName == Undo::timeSignatureEventRemoveAction)        { return new TimeSignatureEventRemoveAction(this->project); }
    else if (tagName == Undo::timeSignatureEventChangeAction)        { return new TimeSignatureEventChangeAction(this->project); }
    else if (tagName == Undo::keySignatureEventInsertAction)         { return new KeySignatureEventInsertAction(this->project); }
    else if (tagName == Undo::keySignatureEventRemoveAction)         { return new KeySignatureEventRemoveAction(this->project); }
    else if (tagName == Undo::keySignatureEventChangeAction)         { return new KeySignatureEventChangeAction(this->project); }
    else if (tagName == Undo::automationEventInsertAction)           { return new AutomationEventInsertAction(this->project); }
    else if (tagName == Undo::automationEventRemoveAction)           { return new AutomationEventRemoveAction(this->project); }
    else if (tagName == Undo::automationEventChangeAction)           { return new AutomationEventChangeAction(this->project); }
    else if (tagName == Undo::automationEventsGroupInsertAction)     { return new AutomationEventsGroupInsertAction(this->project); }
    else if (tagName == Undo::automationEventsGroupRemoveAction)     { return new AutomationEventsGroupRemoveAction(this->project); }
    else if (tagName == Undo::automationEventsGroupChangeAction)     { return new AutomationEventsGroupChangeAction(this->project); }
    else if (tagName == Undo::projectTemperamentChangeAction)        { return new ProjectTemperamentChangeAction(this->project); }

    // Here we could meet deprecated legacy actions
    return nullptr;
}

UndoStack::UndoStack(ProjectNode &parentProject,
    int maxNumberOfUnitsToKeep,
    int minimumTransactions) :
    project(parentProject),
    maxNumUnitsToKeep(maxNumberOfUnitsToKeep),
    minimumTransactionsToKeep(minimumTransactions) {}

void UndoStack::clearUndoHistory()
{
    this->transactions.clear();
    this->totalUnitsStored = 0;
    this->nextIndex = 0;
}

bool UndoStack::perform(UndoAction *const newAction, UndoActionId transactionId)
{
    if (this->perform(newAction))
    {
        if (transactionId != 0)
        {
            this->setCurrentUndoActionId(transactionId);
        }
        
        return true;
    }
    
    return false;
}

bool UndoStack::perform(UndoAction *const newAction)
{
    if (newAction != nullptr)
    {
        UniquePointer<UndoAction> action(newAction);
        
        if (this->reentrancyCheck)
        {
            jassertfalse;  // don't call perform() recursively from the UndoAction::perform()
            // or undo() methods, or else these actions will be discarded!
            return false;
        }

        if (action->perform())
        {
            auto *actionSet = this->getCurrentSet();
            
            if (actionSet != nullptr && !this->hasNewEmptyTransaction)
            {
                if (auto *lastAction = actionSet->actions.getLast())
                {
                    if (auto *coalescedAction = lastAction->createCoalescedAction(action.get()))
                    {
                        action.reset(coalescedAction);
                        this->totalUnitsStored -= lastAction->getSizeInUnits();
                        actionSet->actions.removeLast();
                    }
                }
            }
            else
            {
                actionSet = new Transaction(this->project, this->newUndoActionId);
                this->transactions.insert(nextIndex, actionSet);
                this->nextIndex++;
            }
            
            this->totalUnitsStored += action->getSizeInUnits();
            actionSet->actions.add(move(action));
            this->hasNewEmptyTransaction = false;
            
            this->clearFutureTransactions();
            return true;
        }
    }
    
    return false;
}

void UndoStack::clearFutureTransactions()
{
    while (this->nextIndex < this->transactions.size())
    {
        this->totalUnitsStored -= transactions.getLast()->getTotalSize();
        this->transactions.removeLast();
    }
    
    while (this->nextIndex > 0
           && this->totalUnitsStored > this->maxNumUnitsToKeep
           && this->transactions.size() > this->minimumTransactionsToKeep)
    {
        this->totalUnitsStored -= this->transactions.getFirst()->getTotalSize();
        this->transactions.remove(0);
        --this->nextIndex;
        
        // if this fails, then some actions may not be returning
        // consistent results from their getSizeInUnits() method
        jassert(this->totalUnitsStored >= 0);
    }
}

void UndoStack::beginNewTransaction() noexcept
{
    this->beginNewTransaction(UndoActionIDs::None);
}

void UndoStack::beginNewTransaction(UndoActionId transactionId) noexcept
{
    this->hasNewEmptyTransaction = true;
    this->newUndoActionId = transactionId;
}

void UndoStack::setCurrentUndoActionId(UndoActionId transactionId) noexcept
{
    if (this->hasNewEmptyTransaction)
    {
        this->newUndoActionId = transactionId;
    }
    else if (auto *action = this->getCurrentSet())
    {
        action->id = transactionId;
    }
}

UndoStack::Transaction *UndoStack::getCurrentSet() const noexcept
{
    return this->transactions[this->nextIndex - 1];
}

UndoStack::Transaction *UndoStack::getNextSet() const noexcept
{
    return this->transactions[this->nextIndex];
}

bool UndoStack::canUndo() const noexcept
{
    return this->getCurrentSet() != nullptr;
}

bool UndoStack::canRedo() const noexcept
{
    return this->getNextSet() != nullptr;
}

bool UndoStack::undo()
{
    if (const auto *s = this->getCurrentSet())
    {
        const ScopedValueSetter<bool> setter(this->reentrancyCheck, true);
        
        if (s->undo())
        {
            --nextIndex;
        }
        else
        {
            this->clearUndoHistory();
        }
        
        this->beginNewTransaction();
        return true;
    }
    
    return false;
}

bool UndoStack::redo()
{
    if (const auto *s = this->getNextSet())
    {
        const ScopedValueSetter<bool> setter(this->reentrancyCheck, true);
        
        if (s->perform())
        {
            ++nextIndex;
        }
        else
        {
            this->clearUndoHistory();
        }
        
        this->beginNewTransaction();
        return true;
    }
    
    return false;
}

UndoActionId UndoStack::getUndoActionId() const
{
    if (const auto *s = this->getCurrentSet())
    {
        return s->id;
    }
    
    return 0;
}

UndoActionId UndoStack::getRedoActionId() const
{
    if (const auto *s = this->getNextSet())
    {
        return s->id;
    }
    
    return 0;
}

bool UndoStack::undoCurrentTransactionOnly()
{
    return this->hasNewEmptyTransaction ? false : this->undo();
}

void UndoStack::getActionsInCurrentTransaction(Array<const UndoAction *> &actionsFound) const
{
    if (!this->hasNewEmptyTransaction)
    {
        if (const auto *s = this->getCurrentSet())
        {
            for (int i = 0; i < s->actions.size(); ++i)
            {
                actionsFound.add(s->actions.getUnchecked(i));
            }
        }
    }
}

int UndoStack::getNumActionsInCurrentTransaction() const
{
    if (!this->hasNewEmptyTransaction)
    {
        if (const auto *s = this->getCurrentSet())
        {
            return s->actions.size();
        }
    }
    
    return 0;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData UndoStack::serialize() const
{
    SerializedData tree(Serialization::Undo::undoStack);
    
    int currentIndex = (this->nextIndex - 1);
    int numStoredTransactions = 0;
    
    while (currentIndex >= 0 &&
           numStoredTransactions < UndoStack::maxTransactionsToSerialize)
    {
        if (auto *action = this->transactions[currentIndex])
        {
            tree.addChild(action->serialize(), 0);
        }
        
        --currentIndex;
        ++numStoredTransactions;
    }
    
    return tree;
}

void UndoStack::deserialize(const SerializedData &data)
{
    const auto root = data.hasType(Serialization::Undo::undoStack) ?
        data : data.getChildWithName(Serialization::Undo::undoStack);
    
    if (!root.isValid())
    { return; }
    
    this->reset();
    
    for (const auto &childTransaction : root)
    {
        auto *actionSet = new Transaction(this->project, {});
        actionSet->deserialize(childTransaction);
        this->transactions.insert(this->nextIndex, actionSet);
        ++this->nextIndex;
    }
}

void UndoStack::reset()
{
    this->clearUndoHistory();
}

bool UndoStack::mergeTransactionsUpTo(UndoActionId transactionId)
{
    // make sure the transaction with that id exists
    int targetActionIndex = -1;
    for (int i = this->nextIndex; i --> 0 ;)
    {
        const auto *t = this->transactions[i];
        if (t != nullptr && t->id == transactionId)
        {
            targetActionIndex = i;
            break;
        }
    }

    auto *targetTransaction = this->transactions[targetActionIndex];

    if (targetTransaction == nullptr)
    {
        jassertfalse;
        return false;
    }

    // also bail out without assertion if there's nothing to merge
    if (targetActionIndex == (this->nextIndex - 1))
    {
        DBG("Found 1 transaction for the merge, skipping");
        return false;
    }

    DBG("Merging " + String(this->nextIndex - targetActionIndex) + " transactions");

    for (int i = targetActionIndex + 1; i < this->nextIndex;)
    {
        if (auto *t = this->transactions[i])
        {
            // hack warning: manually moving owned objects
            // from one owned array to another to avoid copying:
            targetTransaction->actions.addArray(t->actions);
            t->actions.clear(false);
        }

        this->transactions.remove(i, true);
        this->nextIndex--;
    }

    return true;
}
