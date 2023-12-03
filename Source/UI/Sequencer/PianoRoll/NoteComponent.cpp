/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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

NoteComponent::NoteComponent(PianoRoll &editor, const Note &event, const Clip &clip, bool ghostMode) noexcept :
    RollChildComponentBase(editor, ghostMode),
    note(event),
    clip(clip)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setFloatBounds(this->getRoll().getEventBounds(this));
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

bool NoteComponent::shouldGoQuickSelectTrackMode(const ModifierKeys &modifiers) const
{
    return modifiers.isRightButtonDown() && !this->isActive();
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
    if (this->shouldGoQuickSelectTrackMode(e.mods))
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
        return;
    }

    this->setMouseCursor(MouseCursor::NormalCursor);
}

// "if ... static_cast" is here only so the macro can use the if's scope
#define forEachSelectedNote(lasso, child) \
    for (int _i = 0; _i < lasso.getNumSelected(); _i++) \
        if (auto *child = static_cast<NoteComponent *>(lasso.getSelectedItem(_i)))

void NoteComponent::mouseDown(const MouseEvent &e)
{
    if (this->roll.hasMultiTouch(e))
    {
        return;
    }

    if (this->shouldGoQuickSelectTrackMode(e.mods))
    {
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        this->switchActiveTrackToSelected(e.mods.isAnyModifierKeyDown());
        return;
    }
    
    if (! this->isActive())
    {
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }
    
    // rclick and drag in the default mode means dragging the canvas;
    // rclick and drag in the pen mode means switching to note deletion mode;
    // both are implemented in the roll, so we'll pass the event:
    if (e.mods.isRightButtonDown() &&
        (this->roll.getEditMode().isMode(RollEditMode::defaultMode) ||
         this->roll.getEditMode().isMode(RollEditMode::drawMode)))
    {
        // see the comment above PianoRoll::startErasingEvents for
        // the explanation of how erasing events works and why:
        this->roll.mouseDown(e.getEventRelativeTo(&this->roll));
        return;
    }

    RollChildComponentBase::mouseDown(e);

    const auto &selection = this->roll.getLassoSelection();

    if (e.mods.isLeftButtonDown())
    {
        const bool shouldSendMidi =
            selection.getNumSelected() <= NoteComponent::maxDragPolyphony;
        
        if (shouldSendMidi)
        {
            this->stopSound();
        }

        if (selection.getNumSelected() == 1)    //trying to do this with multiple notes selected would lead to confusing behavior - RPM
        {
            this->getRoll().setDefaultNoteLength(this->getLength());
            this->getRoll().setDefaultNoteVolume(this->getVelocity());
        }

        const auto resizeEdge = this->getResizableEdge();
        if (this->canResize() && e.x >= (this->getWidth() - resizeEdge))
        {
            if (e.mods.isShiftDown())
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
            else
            {
                forEachSelectedNote(selection, selectedNote)
                {
                    if (selection.shouldDisplayGhostNotes())
                    {
                        selectedNote->getRoll().showGhostNoteFor(selectedNote);
                    }

                    selectedNote->startResizingRight(shouldSendMidi);
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
    else if (e.mods.isMiddleButtonDown())
    {
        this->setMouseCursor(MouseCursor::UpDownResizeCursor);
        forEachSelectedNote(selection, selectedNote)
        {
            selectedNote->startTuning();
        }
    }
}

static int lastDeltaKey = 0;

void NoteComponent::mouseDrag(const MouseEvent &e)
{
    if (this->roll.hasMultiTouch(e))
    {
        return;
    }

    bool snap = !e.mods.isAltDown();    //snap is disabled

    if (this->shouldGoQuickSelectTrackMode(e.mods))
    {
        return;
    }

    if (!this->isActive())
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return; // happens on mobile after a long-tap switch-to-track
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(RollEditMode::defaultMode))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (e.mods.isRightButtonDown() &&
        this->roll.getEditMode().isMode(RollEditMode::eraseMode))
    {
        this->roll.mouseDrag(e.getEventRelativeTo(&this->roll));
        return;
    }

    if (this->state == State::DraggingResizing)
    {
        int deltaKey = 0;
        float deltaLength = 0.f;
        const bool eventChanged = this->getDraggingResizingDelta(e, deltaLength, deltaKey, snap);

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
            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueDraggingResizing(deltaLength, deltaKey, shouldSendMidi, snap));
            }

            this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());

            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::ResizingRight)
    {
        float deltaLength = 0.f;
        const bool lengthChanged = this->getResizingRightDelta(e, deltaLength, snap);

        if (lengthChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;

            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueResizingRight(deltaLength, snap));
            }

            this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());

            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::ResizingLeft)
    {
        float deltaLength = 0.f;
        const bool lengthChanged = this->getResizingLeftDelta(e, deltaLength, snap);
        
        if (lengthChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueResizingLeft(deltaLength, snap));
            }

            this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());

            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::GroupScalingRight)
    {
        float groupScaleFactor = 1.f;
        const bool scaleFactorChanged = this->getGroupScaleRightFactor(e, groupScaleFactor, snap);
        
        if (scaleFactorChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueGroupScalingRight(groupScaleFactor));
            }

            this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());

            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::GroupScalingLeft)
    {
        float groupScaleFactor = 1.f;
        const bool scaleFactorChanged = this->getGroupScaleLeftFactor(e, groupScaleFactor, snap);
        
        if (scaleFactorChanged)
        {
            this->checkpointIfNeeded();
            Array<Note> groupBefore, groupAfter;
                
            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueGroupScalingLeft(groupScaleFactor));
            }

            this->getRoll().setDefaultNoteLength(groupAfter.getLast().getLength());

            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
        else
        {
            this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        }
    }
    else if (this->state == State::Dragging)
    {
        int deltaKey = 0;
        float deltaBeat = 0.f;
        const bool eventChanged = this->getDraggingDelta(e, deltaBeat, deltaKey, snap);
        
        const bool shouldSendMidi = (lastDeltaKey != deltaKey) &&
            (selection.getNumSelected() <= NoteComponent::maxDragPolyphony);

        lastDeltaKey = deltaKey;
        
        this->setFloatBounds(this->getRoll().getEventBounds(this)); // avoids glitches
        
        if (eventChanged)
        {
            const bool firstChangeIsToCome = !this->firstChangeDone;

            this->checkpointIfNeeded();

            if (firstChangeIsToCome)
            {
                this->getRoll().showDragHelpers();
            }

            // Drag-and-copy logic:
            if (firstChangeIsToCome && e.mods.isShiftDown())    //changed to shift (perhaps ctrl? though, ctrl may be a good candidate for fine control mode)
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
                    noteComponent->toFront(false);
                }
            }

            this->getRoll().updateDragHelpers(deltaKey);
            
            if (shouldSendMidi)
            {
                this->stopSound();
            }
            
            Array<Note> groupBefore, groupAfter;
            forEachSelectedNote(selection, noteComponent)
            {
                groupBefore.add(noteComponent->getNote());
                groupAfter.add(noteComponent->continueDragging(deltaBeat, deltaKey, shouldSendMidi, snap));
            }
                
            SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
        }
    }
    else if (this->state == State::Tuning)
    {
        this->checkpointIfNeeded();
        
        Array<Note> groupBefore, groupAfter;
        forEachSelectedNote(selection, noteComponent)
        {
            groupBefore.add(noteComponent->getNote());
            groupAfter.add(noteComponent->continueTuning(e));
            this->getRoll().setDefaultNoteVolume(groupAfter.getLast().getVelocity());
        }

        SequencerOperations::getPianoSequence(selection)->changeGroup(groupBefore, groupAfter, true);
    }
}

void NoteComponent::mouseUp(const MouseEvent &e)
{
    // no multi-touch check here, need to exit the editing mode (if any) even in multi-touch
    //if (this->roll.hasMultiTouch(e)) { return; }

    if (this->shouldGoQuickSelectTrackMode(e.mods))
    {
        return;
    }
    
    if (!this->isActive())
    {
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }

    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return; // happens on mobile after a long-tap switch-to-track
    }

    this->getRoll().hideAllGhostNotes();

    // deleting the note on desktop platforms
    if (e.mods.isRightButtonDown() &&
        (this->roll.getEditMode().isMode(RollEditMode::defaultMode) ||
         this->roll.getEditMode().isMode(RollEditMode::eraseMode)))
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
        this->roll.mouseUp(e.getEventRelativeTo(&this->roll));
        return;
    }
    
    // deleting the note on mobile platforms is done by
    // tapping on the note in the pen mode without editing it
    if (!this->firstChangeDone && e.source.isTouch() &&
        this->roll.getEditMode().isMode(RollEditMode::drawMode))
    {
        auto *sequence = static_cast<PianoSequence *>(this->note.getSequence());
        sequence->checkpoint();
        sequence->remove(this->note, true);
        return;
    }

    const bool shouldSendMidi = true;

    if (this->state == State::DraggingResizing)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endDraggingResizing();
        }
    }
    else if (this->state == State::ResizingRight)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endResizingRight();
        }
    }
    else if (this->state == State::ResizingLeft)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endResizingLeft();
        }
    }
    else if (this->state == State::GroupScalingRight)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endGroupScalingRight();
        }
    }
    else if (this->state == State::GroupScalingLeft)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endGroupScalingLeft();
        }
    }
    else if (this->state == State::Dragging)
    {
        this->getRoll().hideDragHelpers();
        this->setFloatBounds(this->getRoll().getEventBounds(this));
        
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endDragging(shouldSendMidi);
        }
    }
    else if (this->state == State::Tuning)
    {
        forEachSelectedNote(selection, noteComponent)
        {
            noteComponent->endTuning();
        }

        this->setMouseCursor(MouseCursor::NormalCursor);
    }
}

void NoteComponent::mouseDoubleClick(const MouseEvent &e)
{
    if (!this->isActive())
    {
        this->roll.mouseDoubleClick(e.getEventRelativeTo(&this->roll));
        return;
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
        g.fillRect(x + 0.75f, y + 0.75f, w - 1.25f, h - 1.5f);
    }

    if (w >= 2.25f)
    {
        g.setColour(this->colourLighter);
        g.fillRect(x + 1.25f, roundf(y), w - 2.25f, 1.f);

        g.setColour(this->colourDarker);
        g.fillRect(x + 1.25f, roundf(y + h - 1), w - 2.25f, 1.f);
    }

    if (w >= 6.f)
    {
        g.setColour(this->colourVolume);
        const float sx = x + 2.f;
        const float sh = jmin(h - 2.f, 4.f);
        const float sy = h - sh - 1.f;
        const float sw1 = (w - 4.f) * this->note.getVelocity();
        const float sw2 = (w - 4.f) * this->note.getVelocity() * this->clip.getVelocity();

        g.fillRect(sx, sy, sw1, sh);
        g.fillRect(sx, sy, sw2, sh);

        g.fillRect(sx + sw1, sy + 1.f, 1.f, sh - 1.f);
        g.fillRect(sx + sw2, sy + 1.f, 1.f, sh - 1.f);
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

void NoteComponent::switchActiveTrackToSelected(bool zoomToScope) const
{
    this->roll.getProject().setEditableScope(this->getClip(), zoomToScope);
    if (zoomToScope)
    {
        this->getRoll().zoomOutImpulse(0.5f);
    }
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void NoteComponent::startDragging(const bool sendMidiMessage)
{
    this->firstChangeDone = false;
    this->state = State::Dragging;
    this->anchor = this->getNote();
    
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getDraggingDelta(const MouseEvent &e, float &deltaBeat, int &deltaKey, bool snap)
{
    const auto componentClickOffset = this->dragger.dragComponent(this, e);

    int newKey = -1;
    float newBeat = -1;
    const auto x = this->getX() + this->floatLocalBounds.getX() + 1;
    this->getRoll().getRowsColsByComponentPosition(x,
        this->getY() + this->floatLocalBounds.getY() + componentClickOffset.getY(),
        newKey, newBeat, snap);

    // make sure that the note is snapping to the original beat as well as
    // to the displayed snaps, so that vertical dragging is more convenient
    // when the snaps density is lower at higher zoom levels:
    const auto newX = this->getRoll().getXPositionByBeat(newBeat + this->clip.getBeat());
    const auto anchorX = this->getRoll().getXPositionByBeat(this->anchor.getBeat() + this->clip.getBeat());
    if (fabs(x - anchorX) < fabs(x - newX))
    {
        newBeat = this->anchor.getBeat();
    }

    deltaKey = (newKey - this->anchor.getKey());
    deltaBeat = (newBeat - this->anchor.getBeat());

    const bool keyChanged = (this->getKey() != newKey);
    const bool beatChanged = (this->getBeat() != newBeat);

    return (keyChanged || beatChanged);
}

Note NoteComponent::continueDragging(float deltaBeat, int deltaKey, bool sendMidiMessage, bool snap) const noexcept
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
    jassert(!this->isInEditMode());

    // don't send midi in this mode, it was already sent when this event was added:
    constexpr bool sendMidi = false;

    // normally this would have been be set by mouseDown:
    this->dragger.startDraggingComponent(this, e);

#if PLATFORM_DESKTOP
    // on desktop the default mode is editing key+length, which I think is more convenient,
    // yet some folks might have muscle memory for a more widespread behavior,
    // which is just dragging the newly created note, so if any modifier key was down,
    // we'll be dragging both key and beat; if not, we'll be dragging key + length:
    if (e.mods.isAnyModifierKeyDown())
    {
        this->startDragging(sendMidi);
    }
    else
    {
        this->startDraggingResizing(sendMidi);
    }
#elif PLATFORM_MOBILE
    // on mobile platforms the default dragging mode makes more sense,
    // since the resizing helpers are on the screen already
    // and no modifier keys are available to change the mode
    this->startDragging(sendMidi);
#endif

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

bool NoteComponent::getDraggingResizingDelta(const MouseEvent &e, float &deltaLength, int &deltaKey, bool snap) const
{
    int newKey = -1;
    float newBeat = -1;

    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newKey, newBeat, snap);

    const float newLength = newBeat - this->getBeat();
    deltaLength = newLength - this->anchor.getLength();
    deltaKey = newKey - this->anchor.getKey();

    const bool keyChanged = (this->getKey() != newKey);
    const bool lengthChanged = (this->getLength() != newLength);
    return (keyChanged || lengthChanged);
}

Note NoteComponent::continueDraggingResizing(float deltaLength, int deltaKey, bool sendMidi, bool snap) const noexcept
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

void NoteComponent::startResizingRight(bool sendMidiMessage)
{
    this->firstChangeDone = false;
    this->state = State::ResizingRight;
    this->anchor = this->getNote();
    
    if (sendMidiMessage)
    {
        this->sendNoteOn(this->getKey(), this->getVelocity());
    }
}

bool NoteComponent::getResizingRightDelta(const MouseEvent &e, float &deltaLength, bool snap) const
{
    int newNote = -1;
    float newBeat = -1;

    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newNote, newBeat, snap);

    const float newLength = newBeat - this->getBeat();
    deltaLength = newLength - this->anchor.getLength();

    const bool lengthChanged = (this->getLength() != newLength);
    return lengthChanged;
}

Note NoteComponent::continueResizingRight(float deltaLength, bool snap) const noexcept
{
    // the minimal length should depend on the current zoom level:
    const float minLength = this->roll.getMinVisibleBeatForCurrentZoomLevel();
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

bool NoteComponent::getResizingLeftDelta(const MouseEvent &e, float &deltaLength, bool snap) const
{
    int newNote = -1;
    float newBeat = -1;
    
    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newNote, newBeat, snap);
    
    deltaLength = this->anchor.getBeat() - newBeat;
    const bool lengthChanged = (this->getBeat() != newBeat);
    return lengthChanged;
}

Note NoteComponent::continueResizingLeft(float deltaLength, bool snap) const noexcept
{
    // the minimal length should depend on the current zoom level:
    const float minLength = this->roll.getMinVisibleBeatForCurrentZoomLevel();
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

bool NoteComponent::getGroupScaleRightFactor(const MouseEvent &e, float &absScaleFactor, bool snap) const
{
    int newNote = -1;
    float newBeat = -1;
    
    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newNote, newBeat, snap);
    
    const float minGroupLength = 1.f;
    const float myEndBeat = this->getBeat() + this->getLength();
    const float newGroupLength = jmax(minGroupLength, newBeat - this->groupScalingAnchor.getBeat());

    absScaleFactor = newGroupLength / this->groupScalingAnchor.getLength();
    
    const bool endBeatChanged = (newBeat != myEndBeat);
    return endBeatChanged;
}

Note NoteComponent::continueGroupScalingRight(float absScaleFactor, bool snap) const noexcept
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

bool NoteComponent::getGroupScaleLeftFactor(const MouseEvent &e, float &absScaleFactor, bool snap) const
{
    int newNote = -1;
    float newBeat = -1;
    
    this->getRoll().getRowsColsByComponentPosition(
        this->getX() + this->floatLocalBounds.getX() + e.x,
        this->getY() + this->floatLocalBounds.getY() + e.y,
        newNote, newBeat, snap);
    
    const float minGroupLength = 1.f;
    const float groupAnchorEndBeat = this->groupScalingAnchor.getBeat() + this->groupScalingAnchor.getLength();

    const float newGroupLength = jmax(minGroupLength, (groupAnchorEndBeat - newBeat));
    absScaleFactor = newGroupLength / this->groupScalingAnchor.getLength();
    
    const bool endBeatChanged = (newBeat != this->getBeat());
    return endBeatChanged;
}

Note NoteComponent::continueGroupScalingLeft(float absScaleFactor, bool snap) const noexcept
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
