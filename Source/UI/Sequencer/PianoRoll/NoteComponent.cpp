/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    continueResizingRight
    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "NoteComponent.h"
#include "PianoSequence.h"
#include "PianoRoll.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "MidiTrack.h"
#include "Note.h"
#include "SelectionComponent.h"
#include "SequencerOperations.h"
#include "ColourIDs.h"

static PianoSequence *getPianoSequence(const Lasso &selection)
{
    const auto &firstEvent = selection.getFirstAs<NoteComponent>()->getNote();
    return static_cast<PianoSequence *>(firstEvent.getSequence());
}

NoteComponent::NoteComponent(PianoRoll &editor, const Note &event, const Clip &clip, bool ghostMode) noexcept :
    MidiEventComponent(editor, ghostMode),
    note(event),
    clip(clip)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setFloatBounds(this->getRoll().getEventBounds(this));

    //this->setInterceptsMouseClicks(true, true); //notes now intercept mouse clicks

    this->setName("NoteComponent");
}

PianoRoll &NoteComponent::getRoll() const noexcept
{
    return static_cast<PianoRoll &>(this->roll);
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

void NoteComponent::updateColours()
{
    const bool ghost = this->flags.isGhost || !this->flags.isActive;
    const auto base = findDefaultColour(ColourIDs::Roll::noteFill);

    this->colour = this->getNote().getTrackColour()
        .interpolatedWith(base, ghost ? 0.15f : 0.4f)
        .brighter(this->flags.isSelected ? 1.15f : 0.f)
        .withMultipliedSaturationHSL(ghost ? 1.5f : 1.f)
        .withAlpha(ghost ? 0.25f : 0.9f);

    if (ghost)
    {
        this->colour = HelioTheme::getCurrentTheme().isDark() ?
            this->colour.brighter(0.55f) : this->colour.darker(0.45f);
    }

    this->colourLighter = this->colour.brighter(0.125f).withMultipliedAlpha(1.45f);
    this->colourDarker = this->colour.darker(0.175f).withMultipliedAlpha(1.45f);
    this->colourVolume = this->colour.darker(0.8f).withAlpha(ghost ? 0.f : 0.5f);
}

bool NoteComponent::shouldGoQuickSelectLayerMode(const ModifierKeys &modifiers) const
{
    return (modifiers.isAltDown() || modifiers.isRightButtonDown()) && !this->isActive();
}

void NoteComponent::deleteSelf()
{
    this->roll.selectEvent(this, true); //selects moused over note
    SequencerOperations::deleteSelection(this->roll.getLassoSelection()); //deletes the note

    return;
}
//===----------------------------------------------------------------------===//
// MidiEventComponent
//===----------------------------------------------------------------------===//

const String &NoteComponent::getSelectionGroupId() const noexcept
{
    return this->note.getSequence()->getTrackId();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

bool NoteComponent::keyStateChanged(bool isKeyDown)
{
    return this->roll.keyStateChanged(isKeyDown);
}

void NoteComponent::modifierKeysChanged(const ModifierKeys &modifiers)
{
    this->roll.modifierKeysChanged(modifiers);
}

void NoteComponent::mouseMove(const MouseEvent &e)
{
    //DBG("mousemove NoteComponent!");
    if (this->shouldGoQuickSelectLayerMode(e.mods))
    {
        return;
    }

    if (! this->isActive())
    {
        this->roll.mouseMove(e.getEventRelativeTo(&this->roll));
        return;
    }

    const auto resizeEdge = this->getResizableEdge();

    if (this->isResizingOrScaling() ||
        (this->canResize() && (e.x >= (this->getWidth() - resizeEdge) || e.x <= resizeEdge)))
    {
        this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    }
    else
    {
        this->setMouseCursor(MouseCursor::UpDownLeftRightResizeCursor); //added omnidirectional dragging cursor to make it easier to know when you are hovering over a note
    }

}

void NoteComponent::mouseEnter(const MouseEvent& e) //added mouseenter for rightclickdrag (mouseMove led to too many accidental deletions)
{
    DBG("mouseEnter NoteComponent!");

    if ( e.mods.isRightButtonDown() && ( this->isActive() || ( ! this->isActive() && e.mods.isCtrlDown() ) ) )
    {
        this->roll.deselectAll(); //deselect everything first (maybe we can manage to delete without having to create a new selection later)
        this->roll.selectEvent(this, false); //selects moused over note
        SequencerOperations::deleteSelection(this->roll.getLassoSelection()); //deletes the note

        return;
    }

}

#define forEachSelectedNote(lasso, child) \
    for (int _i = 0; _i < lasso.getNumSelected(); _i++) \
        if (auto *child = dynamic_cast<NoteComponent *>(lasso.getSelectedItem(_i)))

void NoteComponent::mouseDown(const MouseEvent &e)
{
    if (e.mods.isRightButtonDown() && (this->isActive() ||  (!this->isActive() && e.mods.isCtrlDown())  ) )//added condition for deleting notes by right clicking on them - RPM
    {
        //e.getPosition()
        this->roll.deselectAll();
        this->roll.selectEvent(this, true); //selects moused over note
        SequencerOperations::deleteSelection(this->roll.getLassoSelection()); //deletes the note

        return;
    }

    if (e.mods.isMiddleButtonDown() && !e.mods.isAnyModifierKeyDown()) //middle click drag even on notes (allows user to blindly drag without minding their manners)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (this->shouldGoQuickSelectLayerMode(e.mods))
    {
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        this->switchActiveSegmentToSelected(e.mods.isAnyModifierKeyDown());
        return;
    }
    
    if (! this->isActive())
    {
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    MidiEventComponent::mouseDown(e);

    const Lasso &selection = this->roll.getLassoSelection();

    if (e.mods.isMiddleButtonDown() && e.mods.isCtrlDown())
    {
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
        forEachSelectedNote(selection, selectedNote)
        {
            selectedNote->startTuning();
        }
    }

    if (e.mods.isLeftButtonDown())
    {
        const bool shouldSendMidi =
            selection.getNumSelected() <= NoteComponent::maxDragPolyphony;
        
        if (shouldSendMidi)
        {
            this->stopSound();
        }

        const auto resizeEdge = this->getResizableEdge();
        if (this->canResize() && e.x >= (this->getWidth() - resizeEdge)) //governs where the resize area is
        {
            if (e.mods.isShiftDown()) //group resize if shift is down
            {
                const float groupStartBeat = SequencerOperations::findStartBeat(selection);
                forEachSelectedNote(selection, selectedNote)
                {
                    if (selection.shouldDisplayGhostNotes())
                    {
                        selectedNote->getRoll().showGhostNoteFor(selectedNote);
                    }

                    selectedNote->startGroupScalingRight(groupStartBeat);
                }
            }
            else //otherwise resize the individual note
            {
                forEachSelectedNote(selection, selectedNote)
                {
                    if (selection.shouldDisplayGhostNotes())
                    {
                        selectedNote->getRoll().showGhostNoteFor(selectedNote);
                    }

                    selectedNote->startResizingRight(shouldSendMidi); //begin resizing each indivual note to the right
                }
            }
        }
        else if (this->canResize() && e.x <= resizeEdge)
        {
            if (e.mods.isShiftDown())
            {
                const float groupEndBeat = SequencerOperations::findEndBeat(selection);
                forEachSelectedNote(selection, selectedNote)
                {
                    if (selection.shouldDisplayGhostNotes())
                    {
                        selectedNote->getRoll().showGhostNoteFor(selectedNote);
                    }

                    selectedNote->startGroupScalingLeft(groupEndBeat);
                }
            }
            else
            {
                forEachSelectedNote(selection, selectedNote)
                {
                    if (selection.shouldDisplayGhostNotes())
                    {
                        selectedNote->getRoll().showGhostNoteFor(selectedNote);
                    }

                    selectedNote->startResizingLeft(shouldSendMidi);
                }
            }
        }
        else
        {
            this->dragger.startDraggingComponent(this, e);
            forEachSelectedNote(selection, selectedNote)
            {
                if (selection.shouldDisplayGhostNotes())
                {
                    selectedNote->getRoll().showGhostNoteFor(selectedNote);
                }

                selectedNote->startDragging(shouldSendMidi);
            }
        }
    }
    
}



static int lastDeltaKey = 0;

void NoteComponent::mouseDrag(const MouseEvent &e)
{

    if (e.mods.isMiddleButtonDown() && !e.mods.isAnyModifierKeyDown()) //middle click drag even on notes (allows user to blindly drag without minding their manners)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (this->shouldGoQuickSelectLayerMode(e.mods))
    {
        return;
    }
    
    if (!this->isActive())
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(RollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }
    
    const auto &selection = this->roll.getLassoSelection();

    if (this->state == State::DraggingResizing)
    {
        int deltaKey = 0;
        float deltaLength = 0.f;
        const bool eventChanged = this->getDraggingResizingDelta(e, deltaLength, deltaKey);

        const bool shouldSendMidi = (lastDeltaKey != deltaKey) &&
            (selection.getNumSelected() <= NoteComponent::maxDragPolyphony);

        lastDeltaKey = deltaKey;

        if (eventChanged)
        {
            this->checkpointIfNeeded();

            if (shouldSendMidi)
            {
                this->stopSound();
            }

            Array<Note> groupBefore, groupAfter;
            for (int i = 0; i < selection.getNumSelected(); ++i)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i); //pay attention to this (for delete drag in rollbase)
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueDraggingResizing(deltaLength, deltaKey, shouldSendMidi));
                this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());
            }

            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::ResizingRight) //on mousedrag, if the state is resizing right then do the following:
    {
        float deltaLength = 0.0f;
        const bool lengthChanged = this->getResizingRightDelta(e, deltaLength);

        if (lengthChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;

            for (int i = 0; i < selection.getNumSelected(); ++i)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i);
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueResizingRight(deltaLength, e));
                this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());
            }

            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::ResizingLeft)
    {
        float deltaLength = 0.f;
        const bool lengthChanged = this->getResizingLeftDelta(e, deltaLength);
        
        if (lengthChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            for (int i = 0; i < selection.getNumSelected(); ++i)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i);
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueResizingLeft(deltaLength, e));
                this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());
            }

            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::GroupScalingRight) //scales whole group to the right
    {
        float groupScaleFactor = 1.f;
        const bool scaleFactorChanged = this->getGroupScaleRightFactor(e, groupScaleFactor);
        
        if (scaleFactorChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            for (int i = 0; i < selection.getNumSelected(); ++i)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i);
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueGroupScalingRight(groupScaleFactor));
            }
                
            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::GroupScalingLeft)
    {
        float groupScaleFactor = 1.f;
        const bool scaleFactorChanged = this->getGroupScaleLeftFactor(e, groupScaleFactor);
        
        if (scaleFactorChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            for (int i = 0; i < selection.getNumSelected(); ++i)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i);
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueGroupScalingLeft(groupScaleFactor));
            }
                
            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::Dragging) //dragging individual note (or note group)
    {


        //my observations so far: - RPM
        //it seems that both drag and drag/copy operations only operate on the TOPMOST notes (no matter the channel)

        this->getRoll().showDragHelpers();

        int deltaKey = 0;
        float deltaBeat = 0.f;
        const bool eventChanged = this->getDraggingDelta(e, deltaBeat, deltaKey);
        
        const bool shouldSendMidi = (lastDeltaKey != deltaKey) &&
            (selection.getNumSelected() <= NoteComponent::maxDragPolyphony);

        lastDeltaKey = deltaKey;
        
        this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        
        if (eventChanged) //if something changed (eventChanged is a bool)
        {

            const bool firstChangeIsToCome = !this->firstChangeDone;

            this->checkpointIfNeeded();
            
            // Drag-and-copy logic:
            if (firstChangeIsToCome && e.mods.isShiftDown() && !(e.mods.isCtrlDown() || e.mods.isAltDown()))
                //changed to if only shift is down. ctrl+drag will be for precise dragging
            {
                // We duplicate the notes only at the very moment when they are about to be moved into the new position,
                // to make sure that simple shift-clicks on a selection won't confuse a user with lots of silently created notes.

                // Another trick is that user just continues to drag the originally selected notes,
                // while the duplicated ones stay in their places, not vice versa, which simplifies the logic pretty much
                // (though that behaviour may appear counterintuitive later, if we ever create a merge-tool with a diff viewer).

                SequencerOperations::duplicateSelection(this->getRoll().getLassoSelection(), false);

                // Ghost note markers become useless from now on, as there are duplicates in their places already:
                this->getRoll().hideAllGhostNotes();

                // Finally, bring selection back to front
               forEachSelectedNote(selection, noteComponent)
                {
                    noteComponent->toFront(false); //invetigate this -RPM
                }
            }

            this->getRoll().moveDragHelpers(deltaBeat, deltaKey);
            
            if (shouldSendMidi)
            {
                this->stopSound();
            }
            
            Array<Note> groupBefore, groupAfter;
            for (int i = 0; i < selection.getNumSelected(); ++i) //for each note in the selection (we're dragging the whole thing)
            {
                const auto *nc = selection.getItemAs<NoteComponent>(i);
                groupBefore.add(nc->getNote());
                groupAfter.add(nc->continueDragging(deltaBeat, deltaKey, shouldSendMidi));
            }
                
            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
    }
    else if (this->state == State::Tuning)
    {
        this->checkpointIfNeeded();
        
        Array<Note> groupBefore, groupAfter;
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            const auto *nc = selection.getItemAs<NoteComponent>(i);
            groupBefore.add(nc->getNote());
            groupAfter.add(nc->continueTuning(e));
            this->getRoll().setDefaultNoteVolume(groupAfter.getLast().getVelocity());
        }

        getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
    }
}

void NoteComponent::mouseUp(const MouseEvent &e)
{
    if (this->shouldGoQuickSelectLayerMode(e.mods))
    {
        return;
    }
    
    if (!this->isActive())
    {
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() && this->roll.getEditMode().isMode(RollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }
    
    this->getRoll().hideAllGhostNotes();

    const bool shouldSendMidi = true;
    
    const Lasso &selection = this->roll.getLassoSelection();

    if (this->state == State::DraggingResizing)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endDraggingResizing();
        }
    }
    else if (this->state == State::ResizingRight)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endResizingRight();
        }
    }
    else if (this->state == State::ResizingLeft)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endResizingLeft();
        }
    }
    else if (this->state == State::GroupScalingRight)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endGroupScalingRight();
        }
    }
    else if (this->state == State::GroupScalingLeft)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endGroupScalingLeft();
        }
    }
    else if (this->state == State::Dragging)
    {
        this->getRoll().hideDragHelpers();
        this->setFloatBounds(this->getRoll().getEventBounds(this));
        
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            nc->endDragging(shouldSendMidi);
        }
    }
    else if (this->state == State::Tuning)
    {
        for (int i = 0; i < selection.getNumSelected(); i++)
        {
            auto *note = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            note->endTuning();
        }

        this->setMouseCursor(MouseCursor::NormalCursor);
    }

    this->getRoll().setDefaultNoteVolume(this->note.getVelocity()); //when mousing up, set default veloity as the note in question
    this->getRoll().setDefaultNoteLength(this->note.getLength()); //when mousing up, set default length\k as the note in question
}

void NoteComponent::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
    if (!this->isSelected()) //pass to roll if the note isn't selected - RPM
    {
        this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
        return;
    }
    if (!event.mods.isAnyModifierKeyDown()) //pass to roll if alt is down (we're zooming) - RPM
    {
        this->roll.mouseWheelMove(event.getEventRelativeTo(&this->roll), wheel);
        return;
    }

    //cannot be just ctrl+scroll because zoom events too often inadvertantly edit note velocities
    if (event.mods.isCtrlDown() && this->isSelected()) //this portion allows the user to ctrl+shift+scroll over a note or selection and change their velocities by scrolling
    {
        const Lasso& selection = this->roll.getLassoSelection(); //get the selection
        

        float delta = wheel.deltaY; //get the wheel delta y (were only converned with whether it's positive or negative)
        const float defaultdelta = 0.05f; //0.1 by default.

        if (delta > 0)
        {
            delta = defaultdelta;
        }
        else
        {
            delta = -1 * defaultdelta;
        }

        //DBG("wheel delta y = " + std::to_string(delta) + " & numSelected = " + std::to_string(selection.getNumSelected()));

        if (selection.getNumSelected() > 0 && this->isSelected()) //if there are multible notes selected (and we're hovering over one of them)
        {
            this->checkpointIfNeeded(); //add an undo checkpoint
            Array<Note> groupBefore, groupAfter; //establish before and after arrays

            for (int i = 0; i < selection.getNumSelected(); ++i) //for each note in the selection
            {
                const auto* nc = selection.getItemAs<NoteComponent>(i); //nc is the new note
                const auto newVelocity = nc->getVelocity() + delta; //newVelocity is the old velocity +- delta
                groupBefore.add(nc->getNote()); //add current note to the "old group"
                groupAfter.add(nc->note.withVelocity(newVelocity)); //add a new note to the "new group" with new velocity
            }

            getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true); //swap old group with new group in the piano roll sequence
        }
        else //if we dont have a selection at all (we are only dealing with one note)
        {
            const auto newVelocity = this->note.getVelocity() + delta; //newVelocity is the old velocity +- delta
            static_cast<PianoSequence*>(this->note.getSequence())->change(this->note, this->note.withVelocity(newVelocity), true); //swap the old note with a note note with newVelocity
        }

        this->getRoll().setDefaultNoteVolume(this->note.getVelocity()); //when scrolling, set default veloity as the note in question
        this->getRoll().setDefaultNoteLength(this->note.getLength()); //when scrolling, set default lengthtk as the note in question


    }
}
//===----------------------------------------------------------------------===//
// Notes painting
//===----------------------------------------------------------------------===//

// Always use only either drawHorizontalLine/drawVerticalLine,
// or fillRect - these are the ones with minimal overhead:
void NoteComponent::paint(Graphics &g) noexcept
{
    const float w = this->floatLocalBounds.getWidth() - .5f; // a small gap between notes
    const float h = this->floatLocalBounds.getHeight();
    const float x = this->floatLocalBounds.getX();
    const float y = this->floatLocalBounds.getY();
    
    g.setColour(this->colour);
    g.fillRect(x + 0.5f, y + h / 6.f, 0.5f, h / 1.5f);

    if (w >= 1.25f)
    {
        g.fillRect(x + w - 0.75f, y + h / 6.f, 0.5f, h / 1.5f);
        g.fillRect(x + 0.75f, y + 1.f, w - 1.25f, h - 2.f);
    }

    if (w >= 2.25f)
    {
        g.setColour(this->colourLighter);
        g.fillRect(x + 1.25f, roundf(y), w - 2.25f, 1.f);

        g.setColour(this->colourDarker);
        g.fillRect(x + 1.25f, roundf(y + h - 1), w - 2.25f, 1.f);
    }

    if (w >= 4.f)
    {
        g.setColour(this->colourVolume);
        const float sx = x + 2.f;
        const float sy = float(this->getHeight() - 4);
        const float sw1 = (w - 4.f) * this->note.getVelocity();
        const float sw2 = (w - 4.f) * this->note.getVelocity() * this->clip.getVelocity();
        g.fillRect(sx, sy, sw1, 3.f);
        g.fillRect(sx, sy, sw2, 3.f);
    }

    const auto tuplet = this->note.getTuplet();
    if (tuplet > 1 && this->getWidth() > 25)
    {
        g.setColour(this->colourLighter);
        for (int i = 1; i < tuplet; ++i)
        {
            g.fillRect(x + i * (w / tuplet) - 1.f, y, 1.f, h);
        }

        g.setColour(this->colourVolume);
        for (int i = 1; i < tuplet; ++i)
        {
            g.fillRect(x + i * (w / tuplet), y, 1.5f, h);
        }
    }

    // debug
    //g.setColour(Colours::orangered);
    //const auto edge = this->getResizableEdge();
    //g.fillRect(0, 0, edge, this->getHeight());
    //g.fillRect(this->getWidth() - edge, 0, edge, this->getHeight());
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

bool NoteComponent::belongsTo(const WeakReference<MidiTrack> &track, const Clip &clip) const noexcept
{
    return this->clip == clip && this->note.getSequence()->getTrack() == track;
}

void NoteComponent::switchActiveSegmentToSelected(bool zoomToScope) const
{
    auto *track = this->getNote().getSequence()->getTrack();
    this->roll.getProject().setEditableScope(track, this->getClip(), zoomToScope);
    if (zoomToScope)
    {
        this->getRoll().zoomOutImpulse(0.5f);
    }
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void NoteComponent::startDragging(const bool sendMidiMessage) //initializes the drag
{
    this->firstChangeDone = false;
    this->state = State::Dragging;
    this->anchor = this->getNote();
    
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getDraggingDelta(const MouseEvent &e, float &deltaBeat, int &deltaKey) //gets the dragging delta. LOL unneccecary comment
{
    this->dragger.dragComponent(this, e, nullptr);

    int newKey = -1;
    float newBeat = -1;
    //-v this line seems to establish the new key by getRowsCols (instead of get XY)
    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + 1 /*+ this->clickOffset.getX()*/,
        this->getY() + this->floatLocalBounds.getY() + this->clickOffset.getY(),
        newKey, newBeat);

    if (e.mods.isCtrlDown()) //if ctrl is down use fine adjustment mode
    {
        this->getRoll().getFineRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + 1 /*+ this->clickOffset.getX()*/,
            this->getY() + this->floatLocalBounds.getY() + this->clickOffset.getY(),
            newKey, newBeat);


    }
    else //all other cases
    {
        this->getRoll().getRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + 1 /*+ this->clickOffset.getX()*/,
            this->getY() + this->floatLocalBounds.getY() + this->clickOffset.getY(),
            newKey, newBeat);
    }



    deltaKey = (newKey - this->anchor.getKey());
    deltaBeat = (newBeat - this->anchor.getBeat());

    const bool keyChanged = (this->getKey() != newKey);
    const bool beatChanged = (this->getBeat() != newBeat);

    return (keyChanged || beatChanged);
}

Note NoteComponent::continueDragging(float deltaBeat, int deltaKey, bool sendMidiMessage) const noexcept //continues the drag
{
    const int newKey = this->anchor.getKey() + deltaKey;
    const float newBeat = this->anchor.getBeat() + deltaBeat;

    if (sendMidiMessage)
    {
        this->sendNoteOn(newKey, this->getVelocity());
    }

    return this->getNote().withKeyBeat(newKey, newBeat);
}

void NoteComponent::endDragging(bool sendStopSoundMessage)
{
    if (sendStopSoundMessage)
    {
        this->stopSound();
    }
    
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Creating note mode
//===----------------------------------------------------------------------===//

MouseCursor NoteComponent::startEditingNewNote(const MouseEvent &e)
{
    jassert (!this->isInEditMode());

    // always send midi in this mode:
    constexpr bool sendMidi = true;

    // the default mode is editing key+length, which I think is more convenient,
    // yet some folks might have muscle memory for a more widespread behavior,
    // which is just dragging the newly created note, so if any modifier key was down,
    // we'll be dragging both key and beat; if not, we'll be dragging key + length:
    if (e.mods.isAnyModifierKeyDown())
    {
        // normally this would have been be set by mouseDown:
        this->dragger.startDraggingComponent(this, e);
        this->startDragging(sendMidi);
    }
    else
    {
        this->startDraggingResizing(sendMidi);
    }

    // normally this would have been be set by mouseDown,
    // but this note was created by the roll:
    this->clickOffset.setXY(e.x, e.y);

    // hack warning: this note is supposed to be created by roll in two actions,
    // (1) adding one, and (2) dragging it afterwards, so two checkpoints would happen,
    // which we don't want (adding a note should appear as a single transaction to the user):
    this->firstChangeDone = true;

    return (this->state == State::DraggingResizing) ?
        MouseCursor::LeftRightResizeCursor : MouseCursor::NormalCursor;
}

//===----------------------------------------------------------------------===//
// Dragging + resizing (the default creating note mode)
//===----------------------------------------------------------------------===//

bool NoteComponent::isInEditMode() const
{
    return this->state != State::None;
}

void NoteComponent::startDraggingResizing(bool sendMidiMessage)
{
    this->firstChangeDone = false;
    this->state = State::DraggingResizing;
    this->anchor = this->getNote();

    // always send midi in this mode:
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getDraggingResizingDelta(const MouseEvent &e, float &deltaLength, int &deltaKey) const
{
    int newKey = -1;
    float newBeat = -1;

    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newKey, newBeat);

    const float newLength = newBeat - this->getBeat();
    deltaLength = newLength - this->anchor.getLength();
    deltaKey = newKey - this->anchor.getKey();

    const bool keyChanged = (this->getKey() != newKey);
    const bool lengthChanged = (this->getLength() != newLength);
    return (keyChanged || lengthChanged);
}

Note NoteComponent::continueDraggingResizing(float deltaLength, int deltaKey, bool sendMidi) const noexcept
{
    const int newKey = this->anchor.getKey() + deltaKey;

    // the minimal length should depend on the current zoom level:
    // when zoomed closer, we need to be able to edit shorter lengths,
    // when zoomed away, the length snaps should be convenient == proportionally larger
    const float minLength = this->roll.getMinVisibleBeatForCurrentZoomLevel();
    const float newLength = jmax(this->anchor.getLength() + deltaLength, minLength);

    if (sendMidi)
    {
        this->sendNoteOn(newKey, this->getVelocity());
    }

    return this->getNote().withKeyLength(newKey, newLength);
}

void NoteComponent::endDraggingResizing()
{
    this->stopSound();
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Resizing Right
//===----------------------------------------------------------------------===//

void NoteComponent::startResizingRight(bool sendMidiMessage) //this function only initializes the resizing by setting the anchor and such
{
    this->firstChangeDone = false;
    this->state = State::ResizingRight;
    this->anchor = this->getNote();
    
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getResizingRightDelta(const MouseEvent &e, float &deltaLength) const // gets the "delta" and is called on a mousedrag when resizing right
{
    int newNote = -1; //init
    float newBeat = -1; //init

    if (e.mods.isCtrlDown()) //if ctrl is down
    {
        this->getRoll().getFineRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);


    }
    else //all other cases
    {
        this->getRoll().getRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);
    }

    const float newLength = newBeat - this->getBeat();
    deltaLength = newLength - this->anchor.getLength();

    const bool lengthChanged = (this->getLength() != newLength);
    return lengthChanged;
}

Note NoteComponent::continueResizingRight(float deltaLength, const MouseEvent& e) const noexcept //this function actually establishes the new length of the note
{
    // the minimal length should depend on the current zoom level:

    float minLength = this->roll.getMinVisibleBeatForCurrentZoomLevel(); // initialize minlength to the current minimum visible beat. also changed minlength to be modifiable
    if (e.mods.isCtrlDown())
    {
        minLength = 0.1f; //if ctrl is down set minlength to 1 (for fine adjustment) - RPM
    }


    const float newLength = jmax(this->anchor.getLength() + deltaLength, minLength);
    return this->getNote().withLength(newLength);
}

void NoteComponent::endResizingRight()
{
    this->stopSound();
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Resizing Left
//===----------------------------------------------------------------------===//

void NoteComponent::startResizingLeft(bool sendMidiMessage)
{
    this->firstChangeDone = false;
    this->state = State::ResizingLeft;
    this->anchor = this->getNote();
    
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getResizingLeftDelta(const MouseEvent &e, float &deltaLength) const
{
    int newNote = -1;
    float newBeat = -1;
    
    if (e.mods.isCtrlDown()) //if ctrl is down
    {
        this->getRoll().getFineRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);


    }
    else //all other cases
    {
        this->getRoll().getRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);
    }
    
    deltaLength = this->anchor.getBeat() - newBeat;
    const bool lengthChanged = (this->getBeat() != newBeat);
    return lengthChanged;
}

Note NoteComponent::continueResizingLeft(float deltaLength, const MouseEvent& e) const noexcept
{
    // the minimal length should depend on the current zoom level:
    float minLength = this->roll.getMinVisibleBeatForCurrentZoomLevel();  //changed from const to regular - RPM
    if (e.mods.isCtrlDown())
    {
        minLength = 0.1f; //if ctrl is down set minlength to 0.1 (for fine adjustment) - RPM
    }

    const float newLength = jmax(this->anchor.getLength() + deltaLength, minLength);
    const float newBeat = this->anchor.getBeat() + this->anchor.getLength() - newLength;
    return this->getNote().withBeat(newBeat).withLength(newLength);
}

void NoteComponent::endResizingLeft()
{
    this->stopSound();
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Group Scaling Right
//===----------------------------------------------------------------------===//

void NoteComponent::startGroupScalingRight(float groupStartBeat)
{
    this->firstChangeDone = false;
    this->state = State::GroupScalingRight;
    const float newLength = this->getLength() + (this->getBeat() - groupStartBeat);
    this->anchor = this->getNote();
    this->groupScalingAnchor = { groupStartBeat, newLength };
}

bool NoteComponent::getGroupScaleRightFactor(const MouseEvent &e, float &absScaleFactor) const
{
    int newNote = -1;
    float newBeat = -1;

    if (e.mods.isCtrlDown()) //if ctrl is down
    {
        this->getRoll().getFineRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);


    }
    else //all other cases
    {
        this->getRoll().getRowsColsByComponentPosition(
            this->getX() + this->floatLocalBounds.getX() + e.x,
            this->getY() + this->floatLocalBounds.getY() + e.y,
            newNote, newBeat);
    }
    
    const float minGroupLength = 1.f;
    const float myEndBeat = this->getBeat() + this->getLength();
    const float newGroupLength = jmax(minGroupLength, newBeat - this->groupScalingAnchor.getBeat());

    absScaleFactor = newGroupLength / this->groupScalingAnchor.getLength();
    
    const bool endBeatChanged = (newBeat != myEndBeat);
    return endBeatChanged;
}

Note NoteComponent::continueGroupScalingRight(float absScaleFactor) const noexcept
{
    const float anchorBeatDelta = this->anchor.getBeat() - this->groupScalingAnchor.getBeat();
    const float newLength = this->anchor.getLength() * absScaleFactor;
    const float newBeat = this->groupScalingAnchor.getBeat() + (anchorBeatDelta * absScaleFactor);
    return this->getNote().withBeat(newBeat).withLength(newLength);
}

void NoteComponent::endGroupScalingRight()
{
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Group Scaling Left
//===----------------------------------------------------------------------===//

void NoteComponent::startGroupScalingLeft(float groupEndBeat)
{
    this->firstChangeDone = false;
    this->state = State::GroupScalingLeft;
    this->anchor = this->getNote();
    const float newLength = groupEndBeat - this->getBeat();
    this->groupScalingAnchor = { this->getNote().getBeat(), newLength };
}

bool NoteComponent::getGroupScaleLeftFactor(const MouseEvent &e, float &absScaleFactor) const
{
    int newNote = -1;
    float newBeat = -1;
    
    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newNote, newBeat);
    
    const float minGroupLength = 1.f;
    const float groupAnchorEndBeat = this->groupScalingAnchor.getBeat() + this->groupScalingAnchor.getLength();

    const float newGroupLength = jmax(minGroupLength, (groupAnchorEndBeat - newBeat));
    absScaleFactor = newGroupLength / this->groupScalingAnchor.getLength();
    
    const bool endBeatChanged = (newBeat != this->getBeat());
    return endBeatChanged;
}

Note NoteComponent::continueGroupScalingLeft(float absScaleFactor) const noexcept
{
    const float groupAnchorEndBeat = this->groupScalingAnchor.getBeat() + this->groupScalingAnchor.getLength();
    const float anchorBeatDelta = groupAnchorEndBeat - this->anchor.getBeat();
    const float newLength = this->anchor.getLength() * absScaleFactor;
    const float newBeat = groupAnchorEndBeat - (anchorBeatDelta * absScaleFactor);
    return this->getNote().withBeat(newBeat).withLength(newLength);
}

void NoteComponent::endGroupScalingLeft()
{
    this->state = State::None;
}

//===----------------------------------------------------------------------===//
// Velocity
//===----------------------------------------------------------------------===//

void NoteComponent::startTuning()
{
    this->firstChangeDone = false;
    this->state = State::Tuning;
    this->anchor = this->getNote();
}

Note NoteComponent::continueTuningLinear(float delta) const noexcept
{
    const float newVelocity = (this->anchor.getVelocity() - delta);
    return this->getNote().withVelocity(newVelocity);
}

Note NoteComponent::continueTuningMultiplied(float factor) const noexcept
{
    // -1 .. 0   ->   0 .. anchor
    // 0 .. 1    ->   anchor .. 1
    const float av = this->anchor.getVelocity();
    //const float newVelocity = (factor < 0) ? (av * (factor + 1.f)) : (av + ((1.f - av) * factor));
    const float newVelocity = (factor < 0) ? (av * (factor + 1.f)) : (av + (av * factor * 2));
    return this->getNote().withVelocity(newVelocity);
}

Note NoteComponent::continueTuningSine(float factor, float midline, float phase) const noexcept
{
    // -1 .. 0   ->   0 .. anchor
    // 0 .. 1    ->   anchor .. 1
    const float amplitude = jmin(midline, (1.f - midline));
    const float sine = cosf(phase) * amplitude;
    const float av = this->anchor.getVelocity();
    const float f = (factor < 0) ? (factor + 1.f) : factor;
    const float downscale = ((av * f) + (midline * (1.f - f)));
    const float upscale = ((midline + sine) * f) + (av * (1.f - f));
    const float newVelocity = (factor < 0) ? downscale : upscale;
    return this->getNote().withVelocity(newVelocity);
}

Note NoteComponent::continueTuning(const MouseEvent &e) const noexcept
{
    return this->continueTuningLinear(e.getDistanceFromDragStartY() / 250.0f);
}

void NoteComponent::endTuning()
{
    this->state = State::None;
    this->roll.triggerBatchRepaintFor(this);
}

//===----------------------------------------------------------------------===//
// Shorthands
//===----------------------------------------------------------------------===//

void NoteComponent::checkpointIfNeeded()
{
    if (!this->firstChangeDone)
    {
        this->note.getSequence()->checkpoint();
        this->firstChangeDone = true;
    }
}

void NoteComponent::stopSound()
{
    const auto &trackId = this->getNote().getSequence()->getTrackId();
    this->getRoll().getTransport().stopSound(trackId);
}

void NoteComponent::sendNoteOn(int noteKey, float velocity) const
{
    const auto &trackId = this->note.getSequence()->getTrackId();
    this->getRoll().getTransport().previewKey(trackId,
        noteKey + this->clip.getKey(),
        velocity * this->clip.getVelocity(),
        this->note.getLength());
}
