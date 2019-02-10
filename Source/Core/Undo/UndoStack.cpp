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
#include "UndoAction.h"
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
#include "PatternActions.h"

#define MAX_TRANSACTIONS_TO_STORE 10

using namespace Serialization;

UndoStack::ActionSet::ActionSet(ProjectNode &project, const String &transactionName) :
    project(project),
    name(transactionName) {}
    
bool UndoStack::ActionSet::perform() const
{
    for (int i = 0; i < actions.size(); ++i)
    {
        if (! actions.getUnchecked(i)->perform())
        {
            return false;
        }
    }
        
    return true;
}
    
bool UndoStack::ActionSet::undo() const
{
    for (int i = actions.size(); --i >= 0;)
    {
        if (! actions.getUnchecked(i)->undo())
        {
            return false;
        }
    }
        
    return true;
}
    
int UndoStack::ActionSet::getTotalSize() const
{
    int total = 0;
    for (int i = actions.size(); --i >= 0;)
    {
        total += actions.getUnchecked(i)->getSizeInUnits();
    }
        
    return total;
}
    
ValueTree UndoStack::ActionSet::serialize() const
{
    ValueTree tree(Serialization::Undo::transaction);

    for (int i = 0; i < this->actions.size(); ++i)
    {
        tree.appendChild(this->actions.getUnchecked(i)->serialize(), nullptr);
    }
        
    return tree;
}
    
void UndoStack::ActionSet::deserialize(const ValueTree &tree)
{
    this->reset();

    for (const auto &childAction : tree)
    {
        if (UndoAction *action = createUndoActionsByTagName(childAction.getType()))
        {
            action->deserialize(childAction);
            this->actions.add(action);
        }
    }
}
    
void UndoStack::ActionSet::reset()
{
    this->actions.clear();
}

UndoAction *UndoStack::ActionSet::createUndoActionsByTagName(const Identifier &tagName)
{
    if      (tagName == Undo::pianoTrackInsertAction)                { return new PianoTrackInsertAction(this->project, &this->project); }
    else if (tagName == Undo::pianoTrackRemoveAction)                { return new PianoTrackRemoveAction(this->project, &this->project); }
    else if (tagName == Undo::automationTrackInsertAction)           { return new AutomationTrackInsertAction(this->project, &this->project); }
    else if (tagName == Undo::automationTrackRemoveAction)           { return new AutomationTrackRemoveAction(this->project, &this->project); }
    else if (tagName == Undo::midiTrackRenameAction)                 { return new MidiTrackRenameAction(this->project); }
    else if (tagName == Undo::midiTrackChangeColourAction)           { return new MidiTrackChangeColourAction(this->project); }
    else if (tagName == Undo::midiTrackChangeInstrumentAction)       { return new MidiTrackChangeInstrumentAction(this->project); }
    else if (tagName == Undo::midiTrackMuteAction)                   { return new MidiTrackMuteAction(this->project); }
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
    else if (tagName == Undo::annotationEventsGroupInsertAction)     { return new AnnotationEventsGroupInsertAction(this->project); }
    else if (tagName == Undo::annotationEventsGroupRemoveAction)     { return new AnnotationEventsGroupRemoveAction(this->project); }
    else if (tagName == Undo::annotationEventsGroupChangeAction)     { return new AnnotationEventsGroupChangeAction(this->project); }
    else if (tagName == Undo::timeSignatureEventInsertAction)        { return new TimeSignatureEventInsertAction(this->project); }
    else if (tagName == Undo::timeSignatureEventRemoveAction)        { return new TimeSignatureEventRemoveAction(this->project); }
    else if (tagName == Undo::timeSignatureEventChangeAction)        { return new TimeSignatureEventChangeAction(this->project); }
    else if (tagName == Undo::timeSignatureEventsGroupInsertAction)  { return new TimeSignatureEventsGroupInsertAction(this->project); }
    else if (tagName == Undo::timeSignatureEventsGroupRemoveAction)  { return new TimeSignatureEventsGroupRemoveAction(this->project); }
    else if (tagName == Undo::timeSignatureEventsGroupChangeAction)  { return new TimeSignatureEventsGroupChangeAction(this->project); }
    else if (tagName == Undo::keySignatureEventInsertAction)         { return new KeySignatureEventInsertAction(this->project); }
    else if (tagName == Undo::keySignatureEventRemoveAction)         { return new KeySignatureEventRemoveAction(this->project); }
    else if (tagName == Undo::keySignatureEventChangeAction)         { return new KeySignatureEventChangeAction(this->project); }
    else if (tagName == Undo::keySignatureEventsGroupInsertAction)   { return new KeySignatureEventsGroupInsertAction(this->project); }
    else if (tagName == Undo::keySignatureEventsGroupRemoveAction)   { return new KeySignatureEventsGroupRemoveAction(this->project); }
    else if (tagName == Undo::keySignatureEventsGroupChangeAction)   { return new KeySignatureEventsGroupChangeAction(this->project); }
    else if (tagName == Undo::automationEventInsertAction)           { return new AutomationEventInsertAction(this->project); }
    else if (tagName == Undo::automationEventRemoveAction)           { return new AutomationEventRemoveAction(this->project); }
    else if (tagName == Undo::automationEventChangeAction)           { return new AutomationEventChangeAction(this->project); }
    else if (tagName == Undo::automationEventsGroupInsertAction)     { return new AutomationEventsGroupInsertAction(this->project); }
    else if (tagName == Undo::automationEventsGroupRemoveAction)     { return new AutomationEventsGroupRemoveAction(this->project); }
    else if (tagName == Undo::automationEventsGroupChangeAction)     { return new AutomationEventsGroupChangeAction(this->project); }

    // Here we could meet deprecated legacy actions
    return nullptr;
}

UndoStack::UndoStack(ProjectNode &parentProject,
    int maxNumberOfUnitsToKeep,
    int minimumTransactions) :
    project(parentProject),
    totalUnitsStored(0),
    nextIndex(0),
    newTransaction(true),
    reentrancyCheck(false),
    maxNumUnitsToKeep(maxNumberOfUnitsToKeep),
    minimumTransactionsToKeep(minimumTransactions) {}

void UndoStack::clearUndoHistory()
{
    this->transactions.clear();
    this->totalUnitsStored = 0;
    this->nextIndex = 0;
    this->sendChangeMessage();
}

bool UndoStack::perform (UndoAction *const newAction, const String &actionName)
{
    if (this->perform(newAction))
    {
        if (actionName.isNotEmpty())
        {
            this->setCurrentTransactionName(actionName);
        }
        
        return true;
    }
    
    return false;
}

bool UndoStack::perform (UndoAction *const newAction)
{
    if (newAction != nullptr)
    {
        ScopedPointer<UndoAction> action (newAction);
        
        if (this->reentrancyCheck)
        {
            jassertfalse;  // don't call perform() recursively from the UndoAction::perform()
            // or undo() methods, or else these actions will be discarded!
            return false;
        }

        if (action->perform())
        {
            ActionSet *actionSet = getCurrentSet();
            
            if (actionSet != nullptr && ! newTransaction)
            {
                for (signed int i = (actionSet->actions.size() - 1); i >= 0; --i)
                {
                    if (UndoAction *const lastAction = actionSet->actions[i])
                    {
                        if (UndoAction *const coalescedAction = lastAction->createCoalescedAction(action))
                        {
                            action = coalescedAction;
                            this->totalUnitsStored -= lastAction->getSizeInUnits();
                            actionSet->actions.remove(i);
                            break;
                        }
                    }
                }
            }
            else
            {
                actionSet = new ActionSet(this->project, newTransactionName);
                transactions.insert (nextIndex, actionSet);
                ++nextIndex;
            }
            
            this->totalUnitsStored += action->getSizeInUnits();
            actionSet->actions.add(action.release());
            this->newTransaction = false;
            
            this->clearFutureTransactions();
            this->sendChangeMessage();
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
        this->transactions.remove (0);
        --this->nextIndex;
        
        // if this fails, then some actions may not be returning
        // consistent results from their getSizeInUnits() method
        jassert (this->totalUnitsStored >= 0);
    }
}

void UndoStack::beginNewTransaction() noexcept
{
    this->beginNewTransaction({});
}

void UndoStack::beginNewTransaction(const String &actionName) noexcept
{
    this->newTransaction = true;
    this->newTransactionName = actionName;
}

void UndoStack::setCurrentTransactionName(const String &newName) noexcept
{
    if (this->newTransaction)
    {
        this->newTransactionName = newName;
    }
    else if (ActionSet *action = this->getCurrentSet())
    {
        action->name = newName;
    }
}

UndoStack::ActionSet *UndoStack::getCurrentSet() const noexcept     { return this->transactions [nextIndex - 1]; }
UndoStack::ActionSet *UndoStack::getNextSet() const noexcept        { return this->transactions [nextIndex]; }

bool UndoStack::canUndo() const noexcept   { return this->getCurrentSet() != nullptr; }
bool UndoStack::canRedo() const noexcept   { return this->getNextSet()    != nullptr; }

bool UndoStack::undo()
{
    if (const auto s = getCurrentSet())
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
        this->sendChangeMessage();
        return true;
    }
    
    return false;
}

bool UndoStack::redo()
{
    if (const ActionSet *const s = this->getNextSet())
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
        this->sendChangeMessage();
        return true;
    }
    
    return false;
}

String UndoStack::getUndoDescription() const
{
    if (const auto s = this->getCurrentSet())
    {
        return s->name;
    }
    
    return {};
}

String UndoStack::getRedoDescription() const
{
    if (const auto s = this->getNextSet())
    {
        return s->name;
    }
    
    return {};
}

bool UndoStack::undoCurrentTransactionOnly()
{
    return this->newTransaction ? false : this->undo();
}

void UndoStack::getActionsInCurrentTransaction(Array<const UndoAction *> &actionsFound) const
{
    if (!this->newTransaction)
    {
        if (const auto s = this->getCurrentSet())
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
    if (!this->newTransaction) {
        if (const auto s = this->getCurrentSet()) {
            return s->actions.size();
        }
    }
    
    return 0;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree UndoStack::serialize() const
{
    ValueTree tree(Serialization::Undo::undoStack);
    
    int currentIndex = (this->nextIndex - 1);
    int numStoredTransactions = 0;
    
    while (currentIndex >= 0 &&
           numStoredTransactions < MAX_TRANSACTIONS_TO_STORE)
    {
        if (ActionSet *action = this->transactions[currentIndex])
        {
            tree.addChild(action->serialize(), 0, nullptr);
        }
        
        --currentIndex;
        ++numStoredTransactions;
    }
    
    return tree;
}

void UndoStack::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Undo::undoStack) ?
        tree : tree.getChildWithName(Serialization::Undo::undoStack);
    
    if (!root.isValid())
    { return; }
    
    this->reset();
    
    for (const auto &childTransaction : root)
    {
        auto actionSet = new ActionSet(this->project, {});
        actionSet->deserialize(childTransaction);
        this->transactions.insert(this->nextIndex, actionSet);
        ++this->nextIndex;
    }
}

void UndoStack::reset()
{
    this->clearUndoHistory();
}
