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
#include "PianoRoll.h"
#include "App.h"
#include "Workspace.h"
#include "MainWindow.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "HybridRollHeader.h"
#include "Pattern.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "AnnotationsSequence.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "VersionControlTreeItem.h"
#include "ProjectTreeItem.h"
#include "ProjectTimeline.h"
#include "Note.h"
#include "NoteComponent.h"
#include "HelperRectangle.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "HelioTheme.h"
#include "ChordBuilder.h"
#include "SelectionComponent.h"
#include "HybridRollEditMode.h"
#include "HelioCallout.h"
#include "NotesTuningPanel.h"
#include "SequencerOperations.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"
#include "ColourIDs.h"
#include "Config.h"
#include "Icons.h"
#include "ArpeggiatorsManager.h"
#include "Arpeggiator.h"
#include "HeadlineItemDataSource.h"
#include "LassoListeners.h"

#define ROWS_OF_TWO_OCTAVES 24
#define DEFAULT_NOTE_LENGTH 0.25f
#define DEFAULT_NOTE_VELOCITY 0.25f

PianoRoll::PianoRoll(ProjectTreeItem &parentProject,
                     Viewport &viewportRef,
                     WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(parentProject, viewportRef, clippingDetector),
    numRows(128),
    rowHeight(PIANOROLL_MIN_ROW_HEIGHT),
    draggingNote(nullptr),
    addNewNoteMode(false),
    mouseDownWasTriggered(false),
    defaultHighlighting() // default pattern (black and white keys)
{
    this->defaultHighlighting = new HighlightingScheme(0, Scale::getNaturalMajorScale());
    this->defaultHighlighting->setRows(this->renderBackgroundCacheFor(this->defaultHighlighting));

    this->selectedNotesMenuManager = new PianoRollSelectionMenuManager(&this->selection, this->project);

    this->setComponentID(ComponentIDs::pianoRollId);
    this->setRowHeight(PIANOROLL_MIN_ROW_HEIGHT + 5);

    this->helperHorizontal = new HelperRectangleHorizontal();
    this->addChildComponent(this->helperHorizontal);

    this->reloadRollContent();
    this->setBarRange(0, 8);
}

void PianoRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->backgroundsCache.clear();

    this->eventComponents.clear();

    const auto &tracks = this->project.getTracks();
    for (auto track : tracks)
    {
        //for (int p = 0; p < track->getPattern()->size(); ++p)
        //{
        //    Clip clip = track->getPattern()->getUnchecked(p);
        //    UniquePointer<EventComponentsMap> clipMap(new EventComponentsMap());

            for (int s = 0; s < track->getSequence()->size(); ++s)
            {
                MidiEvent *event = track->getSequence()->getUnchecked(s);
                if (event->isTypeOf(MidiEvent::Note))
                {
                    Note *note = static_cast<Note *>(event);
                    auto noteComponent = new NoteComponent(*this, *note);

                    this->eventComponents[*note] = UniquePointer<NoteComponent>(noteComponent);
                    //(*clipMap)[*note] = noteComponent;

                    const bool belongsToActiveTrack = noteComponent->belongsToAnyTrack(this->selectedTracks);
                    noteComponent->setActive(belongsToActiveTrack, true);

                    this->addAndMakeVisible(noteComponent);
                }
                else if (event->isTypeOf(MidiEvent::KeySignature))
                {
                    const auto &key = static_cast<const KeySignatureEvent &>(*event);
                    this->updateBackgroundCacheFor(key);
                }
            }

        //    this->componentsMaps.add(clipMap.release());
        //    this->clipsMap[clip] = this->componentsMaps.getLast();
        //}
    }

    this->resized();
    this->repaint(this->viewport.getViewArea());
}

void PianoRoll::setSelectedTracks(Array<WeakReference<MidiTrack>> tracks,
    WeakReference<MidiTrack> activeTrack)
{
    this->selection.deselectAll();

    for (const auto &e : this->eventComponents)
    {
        const auto noteComponent = e.second.get();
        const bool shouldBeActive = noteComponent->belongsToAnyTrack(tracks);
        noteComponent->setActive(shouldBeActive);
    }

    this->selectedTracks = tracks;
    this->activeTrack = activeTrack;
    
    this->repaint(this->viewport.getViewArea());
}

void PianoRoll::setRowHeight(const int newRowHeight)
{
    if (newRowHeight == this->rowHeight || newRowHeight <= 1) { return; }

    this->rowHeight = newRowHeight;
    this->setSize(this->getWidth(),
        HYBRID_ROLL_HEADER_HEIGHT + this->numRows * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// HybridRoll
//===----------------------------------------------------------------------===//

void PianoRoll::selectAll()
{
    for (const auto &e : this->eventComponents)
    {
        const auto childComponent = e.second.get();
        if (childComponent->belongsToAnyTrack(this->selectedTracks))
        {
            this->selection.addToSelection(childComponent);
        }
    }
}

void PianoRoll::setChildrenInteraction(bool interceptsMouse, MouseCursor cursor)
{
    for (const auto &e : this->eventComponents)
    {
        const auto childComponent = e.second.get();
        childComponent->setInterceptsMouseClicks(interceptsMouse, interceptsMouse);
        childComponent->setMouseCursor(cursor);
    }
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PianoRoll::showGhostNoteFor(NoteComponent *targetNoteComponent)
{
    auto component = new NoteComponent(*this, targetNoteComponent->getNote());
    component->setEnabled(false);

    //component->setAlpha(0.2f); // setAlpha makes everything slower
    component->setGhostMode(); // use this, Luke.

    this->addAndMakeVisible(component);
    this->ghostNotes.add(component);

    this->batchRepaintList.add(component);
    this->triggerAsyncUpdate();
}

void PianoRoll::hideAllGhostNotes()
{
    for (int i = 0; i < this->ghostNotes.size(); ++i)
    {
        this->fader.fadeOut(this->ghostNotes.getUnchecked(i), 100);
    }

    this->ghostNotes.clear();
}


//===----------------------------------------------------------------------===//
// SmoothZoomListener
//===----------------------------------------------------------------------===//

void PianoRoll::zoomRelative(const Point<float> &origin, const Point<float> &factor)
{
    //Logger::writeToLog("zoomRelative " + String(factor.getY()));
    const float yZoomThreshold = 0.005f;

    if (fabs(factor.getY()) > yZoomThreshold)
    {
        const Point<float> oldViewPosition = this->viewport.getViewPosition().toFloat();
        const Point<float> absoluteOrigin = oldViewPosition + origin;
        const float oldHeight = float(this->getHeight());

        int newRowHeight = this->getRowHeight();
        newRowHeight = (factor.getY() < -yZoomThreshold) ? (newRowHeight - 1) : newRowHeight;
        newRowHeight = (factor.getY() > yZoomThreshold) ? (newRowHeight + 1) : newRowHeight;

        const float estimatedNewHeight = float(newRowHeight * this->getNumRows());

        if (estimatedNewHeight < this->viewport.getViewHeight() ||
            newRowHeight > PIANOROLL_MAX_ROW_HEIGHT ||
            newRowHeight < PIANOROLL_MIN_ROW_HEIGHT)
        {
            newRowHeight = this->getRowHeight();
        }

        this->setRowHeight(newRowHeight);

        const float newHeight = float(this->getHeight());
        const float mouseOffsetY = float(absoluteOrigin.getY() - oldViewPosition.getY());
        const float newViewPositionY = float((absoluteOrigin.getY() * newHeight) / oldHeight) - mouseOffsetY;
        this->viewport.setViewPosition(int(oldViewPosition.getX()), int(newViewPositionY + 0.5f));
    }

    HybridRoll::zoomRelative(origin, factor);
}

void PianoRoll::zoomAbsolute(const Point<float> &zoom)
{
    const float &newHeight = (this->getNumRows() * PIANOROLL_MAX_ROW_HEIGHT) * zoom.getY();
    const float &rowsOnNewScreen = float(newHeight / PIANOROLL_MAX_ROW_HEIGHT);
    const float &viewHeight = float(this->viewport.getViewHeight());
    const float &newRowHeight = floorf(viewHeight / rowsOnNewScreen + .5f);

    this->setRowHeight(int(newRowHeight));

    HybridRoll::zoomAbsolute(zoom);
}

float PianoRoll::getZoomFactorY() const
{
    const float &viewHeight = float(this->viewport.getViewHeight());
    return (viewHeight / float(this->getHeight()));
}

//===----------------------------------------------------------------------===//
// Note management
//===----------------------------------------------------------------------===//

void PianoRoll::addNote(int key, float beat, float length, float velocity)
{
    auto *activeSequence = static_cast<PianoSequence *>(this->activeTrack->getSequence());
    activeSequence->checkpoint();
    Note note(activeSequence, key, beat, length, velocity);
    activeSequence->insert(note, true);
}

Rectangle<float> PianoRoll::getEventBounds(FloatBoundsComponent *mc) const
{
    //jassert(dynamic_cast<NoteComponent *>(mc));
    NoteComponent *nc = static_cast<NoteComponent *>(mc);
    return this->getEventBounds(nc->getKey(), nc->getBeat(), nc->getLength());
}

Rectangle<float> PianoRoll::getEventBounds(int key, float beat, float length) const
{
    const double startOffsetBeat = this->firstBar * double(BEATS_PER_BAR);
    const double x = this->barWidth * double(beat - startOffsetBeat) / double(BEATS_PER_BAR);

    const float w = this->barWidth * length / float(BEATS_PER_BAR);
    const float yPosition = float(this->getYPositionByKey(key));

    return Rectangle<float> (float(x), yPosition + 1, w, float(this->rowHeight - 1));
}

void PianoRoll::getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getRoundBeatByXPosition(int(x)); /* - 0.5f ? */
    noteNumber = roundToInt((this->getHeight() - y) / this->rowHeight);
    noteNumber = jmin(jmax(noteNumber, 0), numRows - 1);
}

void PianoRoll::getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getFloorBeatByXPosition(x);
    noteNumber = roundToInt((this->getHeight() - y) / this->rowHeight);
    noteNumber = jmin(jmax(noteNumber, 0), numRows - 1);
}

int PianoRoll::getYPositionByKey(int targetKey) const
{
    return (this->getHeight() - this->rowHeight) -
        (targetKey * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// Drag helpers
//===----------------------------------------------------------------------===//

void PianoRoll::showHelpers()
{
    if (!this->helperHorizontal->isVisible())
    {
        this->selection.needsToCalculateSelectionBounds();
        this->moveHelpers(0.f, 0);
        this->helperHorizontal->setAlpha(1.f);
        this->helperHorizontal->setVisible(true);
    }
}

void PianoRoll::hideHelpers()
{
    if (this->helperHorizontal->isVisible())
    {
        const int animTime = SHORT_FADE_TIME(this);
        this->fader.fadeOut(this->helperHorizontal, animTime);
    }
}

void PianoRoll::moveHelpers(const float deltaBeat, const int deltaKey)
{
    const float firstBeat = this->firstBar * float(BEATS_PER_BAR);
    const Rectangle<int> selectionBounds = this->selection.getSelectionBounds();
    const Rectangle<float> delta = this->getEventBounds(deltaKey - 1, deltaBeat + firstBeat, 1.f);

    const int deltaX = roundToInt(delta.getTopLeft().getX());
    const int deltaY = roundToInt(delta.getTopLeft().getY() - this->getHeight() - 1);
    const Rectangle<int> selectionTranslated = selectionBounds.translated(deltaX, deltaY);

    const int vX = this->viewport.getViewPositionX();
    const int vW = this->viewport.getViewWidth();
    this->helperHorizontal->setBounds(selectionTranslated.withLeft(vX).withWidth(vW));
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoRoll::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    HybridRoll::onChangeMidiEvent(oldEvent, newEvent);
    
    if (oldEvent.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(oldEvent);
        const Note &newNote = static_cast<const Note &>(newEvent);
        if (const auto component = this->eventComponents[note].release())
        {
            // Pass ownership to another key:
            this->eventComponents.erase(note);
            // Hitting this assert means that a track somehow contains events
            // with duplicate id's. This should never, ever happen.
            jassert(! this->eventComponents.contains(newNote));
            // Always erase before updating, as it may happen both events have the same hash code:
            this->eventComponents[newNote] = UniquePointer<NoteComponent>(component);
            // Schedule to be repainted later:
            this->batchRepaintList.add(component);
            this->triggerAsyncUpdate();
        }
    }
    else if (oldEvent.isTypeOf(MidiEvent::KeySignature))
    {
        const KeySignatureEvent &oldKey = static_cast<const KeySignatureEvent &>(oldEvent);
        const KeySignatureEvent &newKey = static_cast<const KeySignatureEvent &>(newEvent);
        if (oldKey.getRootKey() != newKey.getRootKey() ||
            !oldKey.getScale()->isEquivalentTo(newKey.getScale()))
        {
            this->removeBackgroundCacheFor(oldKey);
            this->updateBackgroundCacheFor(newKey);
        }
        this->repaint();
    }
}

void PianoRoll::onAddMidiEvent(const MidiEvent &event)
{
    HybridRoll::onAddMidiEvent(event);
    
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);

        auto component = new NoteComponent(*this, note);
        this->eventComponents[note] = UniquePointer<NoteComponent>(component);
        this->addAndMakeVisible(component);

        this->fader.fadeIn(component, 150);
        this->selectEvent(component, false); // selectEvent(component, true)

        const bool isActive = component->belongsToAnyTrack(this->selectedTracks);
        component->setActive(isActive);

        this->batchRepaintList.add(component);
        this->triggerAsyncUpdate(); // instead of updateBounds

        if (this->addNewNoteMode)
        {
            this->draggingNote = component;
            this->addNewNoteMode = false;
            this->selectEvent(this->draggingNote, true); // clear prev selection
        }
    }
    else if (event.isTypeOf(MidiEvent::KeySignature))
    {
        // Repainting background caches on the fly may be costly
        const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(event);
        this->updateBackgroundCacheFor(key);
        this->repaint();
    }
}

void PianoRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    HybridRoll::onRemoveMidiEvent(event);

    if (event.isTypeOf(MidiEvent::Note))
    {
        this->hideHelpers();
        this->hideAllGhostNotes(); // Avoids crash

        const Note &note = static_cast<const Note &>(event);
        if (NoteComponent *deletedComponent = this->eventComponents[note].get())
        {
            this->fader.fadeOut(deletedComponent, 150);
            this->selection.deselect(deletedComponent);
            this->eventComponents.erase(note);
        }
    }
    else if (event.isTypeOf(MidiEvent::KeySignature))
    {
        const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(event);
        this->removeBackgroundCacheFor(key);
        this->repaint();
    }
}

void PianoRoll::onChangeTrackProperties(MidiTrack *const track)
{
    if (auto sequence = dynamic_cast<const PianoSequence *>(track->getSequence()))
    {
        // TODO only repaint notes of a changed track?
        for (const auto &e : this->eventComponents)
        {
            const auto component = e.second.get();
            component->updateColours();
        }

        this->repaint();
    }
}

void PianoRoll::onAddTrack(MidiTrack *const track)
{
    for (int j = 0; j < track->getSequence()->size(); ++j)
    {
        const MidiEvent *const event = track->getSequence()->getUnchecked(j);
        if (event->isTypeOf(MidiEvent::Note))
        {
            const auto note = static_cast<const Note *const>(event);
            auto noteComponent = new NoteComponent(*this, *note);
            this->eventComponents[*note] = UniquePointer<NoteComponent>(noteComponent);

            const bool belongsToActiveTrack = noteComponent->belongsToAnyTrack(this->selectedTracks);
            noteComponent->setActive(belongsToActiveTrack, true);

            this->addAndMakeVisible(noteComponent);
        }
        else if (event->isTypeOf(MidiEvent::KeySignature))
        {
            const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(*event);
            this->updateBackgroundCacheFor(key);
        }
    }
}

void PianoRoll::onRemoveTrack(MidiTrack *const track)
{
    this->selection.deselectAll();

    this->hideHelpers();
    this->hideAllGhostNotes(); // Avoids crash

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const auto event = track->getSequence()->getUnchecked(i);
        if (event->isTypeOf(MidiEvent::Note))
        {
            const Note &note = static_cast<const Note &>(*event);
            if (NoteComponent *deletedComponent = this->eventComponents[note].get())
            {
                this->fader.fadeOut(deletedComponent, 150);
                this->selection.deselect(deletedComponent);
                this->eventComponents.erase(note);
            }
        }
        else if (event->isTypeOf(MidiEvent::KeySignature))
        {
            const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(*event);
            this->removeBackgroundCacheFor(key);
            this->repaint();
        }
    }
}

void PianoRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadRollContent();
}

//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

void PianoRoll::selectEventsInRange(float startBeat, float endBeat, bool shouldClearAllOthers)
{
    if (shouldClearAllOthers)
    {
        this->selection.deselectAll();
    }

    for (const auto &e : this->eventComponents)
    {
        const auto component = e.second.get();
        if (component->isActive() &&
            component->getBeat() >= startBeat &&
            component->getBeat() < endBeat)
        {
            this->selection.addToSelection(component);
        }
    }
}

void PianoRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    this->selection.invalidateCache();

    for (const auto &e : this->eventComponents)
    {
        const auto component = e.second.get();
        component->setSelected(false);
    }

    for (const auto component : this->selection)
    {
        component->setSelected(true);
    }
    
    for (const auto &e : this->eventComponents)
    {
        const auto component = e.second.get();
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
            component->setSelected(true);
            itemsFound.addIfNotAlreadyThere(component);
        }
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoRoll::mouseDown(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }
    
    if (! this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, false);

        if (this->isAddEvent(e))
        {
            this->insertNewNoteAt(e);
        }
    }

    HybridRoll::mouseDown(e);

    this->mouseDownWasTriggered = true;
}

void PianoRoll::mouseDoubleClick(const MouseEvent &e)
{
    // "Add chord" dialog
    if (! this->project.getEditMode().forbidsAddingEvents())
    {
        auto popup = new ChordBuilder(this, this->activeTrack->getSequence());
        const MouseEvent &e2(e.getEventRelativeTo(&App::Layout()));
        popup->setTopLeftPosition(e2.getPosition() - Point<int>(popup->getWidth(), popup->getHeight()) / 2);
        App::Layout().addAndMakeVisible(popup);
    }
}

void PianoRoll::mouseDrag(const MouseEvent &e)
{
    // can show menus
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->draggingNote)
    {
        if (this->draggingNote->isResizing())
        {
            this->draggingNote->mouseDrag(e.getEventRelativeTo(this->draggingNote));
        }
        else
        {
            //if (PianoSequence *activeSequence = dynamic_cast<PianoSequence *>(this->activeLayer))
            //{
            //    activeSequence->checkpoint();
            //}

            this->draggingNote->startResizingRight(true);
            this->draggingNote->setNoCheckpointNeededForNextAction(); // a hack
            this->setMouseCursor(MouseCursor(MouseCursor::LeftRightResizeCursor));
        }
    }

    HybridRoll::mouseDrag(e);
}

void PianoRoll::mouseUp(const MouseEvent &e)
{
    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }
    
    // Due to weird modal component behavior,
    // a component can receive mouseUp event without receiving a mouseDown event before.
    
//    if (! this->mouseDownWasTriggered)
//    {
//        return;
//    }

    const bool justEndedDraggingNewNote = this->dismissDraggingNoteIfNeeded();

    if (! this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, true);

        // process lasso selection logic
        HybridRoll::mouseUp(e);
    }

    this->mouseDownWasTriggered = false;
}

bool PianoRoll::dismissDraggingNoteIfNeeded()
{
    // todo dismissDraggingNoteIfNeeded on edit mode change?
    if (this->draggingNote != nullptr)
    {

        this->draggingNote->endResizingRight();
        this->setMouseCursor(this->project.getEditMode().getCursor());
        this->draggingNote = nullptr;
        return true;
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Keyboard shortcuts
//===----------------------------------------------------------------------===//

// Handle all hot-key commands here:
void PianoRoll::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::SelectAllEvents:
        this->selectAll();
        break;
    case CommandIDs::ZoomIn:
        this->zoomInImpulse();
        break;
    case CommandIDs::ZoomOut:
        this->zoomOutImpulse();
        break;
    case CommandIDs::CopyEvents:
        SequencerOperations::copyToClipboard(App::Clipboard(), this->getLassoSelection());
        break;
    case CommandIDs::CutEvents:
        SequencerOperations::copyToClipboard(App::Clipboard(), this->getLassoSelection());
        SequencerOperations::deleteSelection(this->getLassoSelection());
        break;
    case CommandIDs::PasteEvents:
    {
        this->deselectAll();
        const float playheadBeat = this->getBeatByTransportPosition(this->project.getTransport().getSeekPosition());
        SequencerOperations::pasteFromClipboard(App::Clipboard(), this->project, this->getActiveTrack(), playheadBeat);
    }
        break;
    case CommandIDs::DeleteEvents:
        SequencerOperations::deleteSelection(this->getLassoSelection());
        break;
    case CommandIDs::Undo:
        this->project.undo();
        break;
    case CommandIDs::Redo:
        this->project.redo();
        break;
    case CommandIDs::StartDragViewport:
        this->header->setSoundProbeMode(true);
        //if (!this->project.getEditMode().isMode(HybridRollEditMode::dragMode))
        { this->setSpaceDraggingMode(true); }
        break;
    case CommandIDs::EndDragViewport:
        this->header->setSoundProbeMode(false);
        if (this->isUsingSpaceDraggingMode())
        {
            const bool noDraggingWasDone = (this->draggedDistance < 3);
            const bool notTooMuchTimeSpent = (Time::getCurrentTime() - this->timeEnteredDragMode).inMilliseconds() < 300;
            if (noDraggingWasDone && notTooMuchTimeSpent)
            {
                this->project.getTransport().toggleStatStopPlayback();
            }
            this->setSpaceDraggingMode(false);
        }
        break;
    case CommandIDs::TransportStartPlayback:
        if (this->project.getTransport().isPlaying())
        {
            this->startFollowingPlayhead();
        }
        else
        {
            this->project.getTransport().startPlayback();
            this->startFollowingPlayhead();
        }
        break;
    case CommandIDs::TransportPausePlayback:
        if (this->project.getTransport().isPlaying())
        {
            this->project.getTransport().stopPlayback();
        }
        else
        {
            this->resetAllClippingIndicators();
            this->resetAllOversaturationIndicators();
        }

        App::Workspace().getAudioCore().mute();
        App::Workspace().getAudioCore().unmute();
        break;
    case CommandIDs::BeatShiftLeft:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), -1.f / BEATS_PER_BAR);
        break;
    case CommandIDs::BeatShiftRight:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), 1.f / BEATS_PER_BAR);
        break;
    case CommandIDs::BarShiftLeft:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), -1.f);
        break;
    case CommandIDs::BarShiftRight:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), 1.f);
        break;
    case CommandIDs::KeyShiftUp:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), 1, true, &this->getTransport());
        break;
    case CommandIDs::KeyShiftDown:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), -1, true, &this->getTransport());
        break;
    case CommandIDs::OctaveShiftUp:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), 12, true, &this->getTransport());
        break;
    case CommandIDs::OctaveShiftDown:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), -12, true, &this->getTransport());
        break;
    case CommandIDs::CleanupOverlaps:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::removeOverlaps(this->getLassoSelection());
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::InvertChordUp:
        SequencerOperations::invertChord(this->getLassoSelection(), 12, true, &this->getTransport());
        break;
    case CommandIDs::InvertChordDown:
        SequencerOperations::invertChord(this->getLassoSelection(), -12, true, &this->getTransport());
        break;
    case CommandIDs::EditModeDefault:
        this->project.getEditMode().setMode(HybridRollEditMode::defaultMode);
        break;
    case CommandIDs::EditModeDraw:
        this->project.getEditMode().setMode(HybridRollEditMode::drawMode);
        break;
    case CommandIDs::EditModePan:
        this->project.getEditMode().setMode(HybridRollEditMode::dragMode);
        break;
    case CommandIDs::EditModeWipeSpace:
        this->project.getEditMode().setMode(HybridRollEditMode::wipeSpaceMode);
        break;
    case CommandIDs::EditModeInsertSpace:
        this->project.getEditMode().setMode(HybridRollEditMode::insertSpaceMode);
        break;
    case CommandIDs::EditModeSelect:
        this->project.getEditMode().setMode(HybridRollEditMode::selectionMode);
        break;
    case CommandIDs::ToggleQuickStash:
        if (VersionControlTreeItem *vcsTreeItem = this->project.findChildOfType<VersionControlTreeItem>())
        {
            vcsTreeItem->toggleQuickStash();
        }
        break;
    case CommandIDs::CreateArpeggiatorFromSelection:
        {
            // TODO
            //const Arpeggiator::Ptr arp(new Arpeggiator("Test", scale, sequence, root);
            //ArpeggiatorsManager::getInstance().updateUserResource(arp);
        }
        break;
    case CommandIDs::ShowArpeggiatorsPanel:
        // TODO
        //if (const auto arp = ArpeggiatorsManager::getInstance().getResourceById<Arpeggiator::Ptr>("Test"))
        //{
        //    const auto &selection = this->getLassoSelection();
        //    if (selection.getNumSelected() > 1)
        //    {
        //        const auto sequences = this->project.getTimeline()->getKeySignatures()->getSequence();
        //        const auto key = sequences->getFirstEvent<KeySignatureEvent>(selection.getFirstAs<Note>()->getBeat());
        //        SequencerOperations::arpeggiate(this->getLassoSelection(), key.getScale(), key.getRootKey(), arp);
        //    }
        //}
        break;
    case CommandIDs::ShowVolumePanel:
        if (this->selection.getNumSelected() > 0)
        {
            HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
        }
        break;
    case CommandIDs::TweakVolumeRandom:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::randomizeVolume(this->getLassoSelection(), 0.1f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::TweakVolumeFadeOut:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::fadeOutVolume(this->getLassoSelection(), 0.35f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    default:
        break;
    }

    HybridRoll::handleCommandMessage(commandId);
}

void PianoRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    HYBRID_ROLL_BULK_REPAINT_START

    for (const auto &e : this->eventComponents)
    {
        const auto component = e.second.get();
        component->setFloatBounds(this->getEventBounds(component));
    }

    for (const auto component : this->ghostNotes)
    {
        component->setFloatBounds(this->getEventBounds(component));
    }

    HybridRoll::resized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::paint(Graphics &g)
{
    const auto sequences = this->project.getTimeline()->getKeySignatures()->getSequence();
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = paintStartX + this->viewport.getViewWidth();

    // I guess there's a weird bug in JUCE OpenGL shaders,
    // so that OpenGL images tiling offset differs by 1 from native renderers.
    // FIXME: someday I may need to investigate this issue and propose them a fix
    const float paintOffsetY = MainWindow::isOpenGLRendererEnabled() ?
        float(HYBRID_ROLL_HEADER_HEIGHT + 1) : float(HYBRID_ROLL_HEADER_HEIGHT);

    int prevBarX = paintStartX;
    const HighlightingScheme *prevScheme = nullptr;
    const int y = this->viewport.getViewPositionY();
    const int h = this->viewport.getViewHeight();

    for (int nextKeyIdx = 0; nextKeyIdx < sequences->size(); ++nextKeyIdx)
    {
        const auto key = static_cast<KeySignatureEvent *>(sequences->getUnchecked(nextKeyIdx));
        const int barX = int(((key->getBeat() / float(BEATS_PER_BAR)) - this->firstBar)  * this->barWidth);
        const int index = this->binarySearchForHighlightingScheme(key);

#if DEBUG
        if (index < 0)
        {
            Logger::writeToLog("Missing " + key->toString());
            jassert(index >= 0);
        }
#endif

        const auto s = (prevScheme == nullptr) ? this->backgroundsCache.getUnchecked(index) : prevScheme;
        const FillType fillType(s->getUnchecked(this->rowHeight), AffineTransform::translation(0.f, paintOffsetY));
        g.setFillType(fillType);

        if (barX >= paintEndX)
        {
            g.fillRect(prevBarX, y, barX - prevBarX, h);
            HybridRoll::paint(g);
            return;
        }
        else if (barX >= paintStartX)
        {
            g.fillRect(prevBarX, y, barX - prevBarX, h);
        }

        prevBarX = barX;
        prevScheme = this->backgroundsCache.getUnchecked(index);
    }

    if (prevBarX < paintEndX)
    {
        const auto s = (prevScheme == nullptr) ? this->defaultHighlighting : prevScheme;
        const FillType fillType(s->getUnchecked(this->rowHeight), AffineTransform::translation(0.f, paintOffsetY));
        g.setFillType(fillType);
        g.fillRect(prevBarX, y, paintEndX - prevBarX, h);
        HybridRoll::paint(g);
    }
}

void PianoRoll::insertNewNoteAt(const MouseEvent &e)
{
    int draggingRow = 0;
    float draggingColumn = 0.f;
    this->getRowsColsByMousePosition(e.x, e.y, draggingRow, draggingColumn);
    this->addNewNoteMode = true;
    // TODO adjust default velocity in runtime,
    // e.g. use the last added note's velocity
    this->addNote(draggingRow, draggingColumn, 
        DEFAULT_NOTE_LENGTH, DEFAULT_NOTE_VELOCITY);
    //this->activeLayer->sendMidiMessage(MidiMessage::noteOn(this->activeLayer->getChannel(), draggingRow, 0.5f));
}


//===----------------------------------------------------------------------===//
// HybridRoll's legacy
//===----------------------------------------------------------------------===//

void PianoRoll::handleAsyncUpdate()
{
#if PIANOROLL_HAS_NOTE_RESIZERS
    // resizers for the mobile version
    if (this->selection.getNumSelected() > 0 &&
        this->noteResizerLeft == nullptr)
    {
        this->noteResizerLeft = new NoteResizerLeft(*this);
        this->addAndMakeVisible(this->noteResizerLeft);
    }

    if (this->selection.getNumSelected() > 0 &&
        this->noteResizerRight == nullptr)
    {
        this->noteResizerRight = new NoteResizerRight(*this);
        this->addAndMakeVisible(this->noteResizerRight);
    }

    if (this->selection.getNumSelected() == 0)
    {
        this->noteResizerLeft = nullptr;
        this->noteResizerRight = nullptr;
    }

    if (this->batchRepaintList.size() > 0)
    {
        HYBRID_ROLL_BULK_REPAINT_START

        if (this->noteResizerLeft != nullptr)
        {
            this->noteResizerLeft->updateBounds();
        }

        if (this->noteResizerRight != nullptr)
        {
            this->noteResizerRight->updateBounds();
        }

        HYBRID_ROLL_BULK_REPAINT_END
    }
#endif

    HybridRoll::handleAsyncUpdate();
}


void PianoRoll::updateChildrenBounds()
{
#if PIANOROLL_HAS_NOTE_RESIZERS
    if (this->noteResizerLeft != nullptr)
    {
        this->noteResizerLeft->updateBounds();
    }

    if (this->noteResizerRight != nullptr)
    {
        this->noteResizerRight->updateBounds();
    }
#endif

    HybridRoll::updateChildrenBounds();
}

void PianoRoll::updateChildrenPositions()
{
#if PIANOROLL_HAS_NOTE_RESIZERS
    if (this->noteResizerLeft != nullptr)
    {
        this->noteResizerLeft->updateTopPosition();
    }

    if (this->noteResizerRight != nullptr)
    {
        this->noteResizerRight->updateTopPosition();
    }
#endif

    HybridRoll::updateChildrenPositions();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree PianoRoll::serialize() const
{
    using namespace Serialization;
    ValueTree tree(UI::pianoRoll);
    
    tree.setProperty(UI::barWidth, roundf(this->getBarWidth()), nullptr);
    tree.setProperty(UI::rowHeight, this->getRowHeight(), nullptr);

    tree.setProperty(UI::startBar,
        roundf(this->getBarByXPosition(this->getViewport().getViewPositionX())), nullptr);

    tree.setProperty(UI::endBar,
        roundf(this->getBarByXPosition(this->getViewport().getViewPositionX() +
            this->getViewport().getViewWidth())), nullptr);

    tree.setProperty(UI::viewportPositionY, this->getViewport().getViewPositionY(), nullptr);

    // m?
    //tree.setProperty(UI::selection, this->getLassoSelection().serialize(), nullptr);

    return tree;
}

void PianoRoll::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;

    const auto root = tree.hasType(UI::pianoRoll) ?
        tree : tree.getChildWithName(UI::pianoRoll);

    if (!root.isValid())
    {
        return;
    }
    
    this->setBarWidth(float(root.getProperty(UI::barWidth, this->getBarWidth())));
    this->setRowHeight(root.getProperty(UI::rowHeight, this->getRowHeight()));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBar = float(root.getProperty(UI::startBar, 0.0));
    const int x = this->getXPositionByBar(startBar);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PianoRoll::reset() {}

//===----------------------------------------------------------------------===//
// Background pattern images cache
//===----------------------------------------------------------------------===//

void PianoRoll::updateBackgroundCacheFor(const KeySignatureEvent &key)
{
    int duplicateSchemeIndex = this->binarySearchForHighlightingScheme(&key);
    if (duplicateSchemeIndex < 0)
    {
        ScopedPointer<HighlightingScheme> scheme(new HighlightingScheme(key.getRootKey(), key.getScale()));
        scheme->setRows(this->renderBackgroundCacheFor(scheme));
        this->backgroundsCache.addSorted(*this->defaultHighlighting, scheme.release());
    }

#if DEBUG
    if (duplicateSchemeIndex < 0)
    {
        Logger::writeToLog("Added scheme " + key.toString());
    }
    else
    {
        Logger::writeToLog("Ignored duplicate " + key.toString());
    }
#endif
}

void PianoRoll::removeBackgroundCacheFor(const KeySignatureEvent &key)
{
    const auto sequences = this->project.getTimeline()->getKeySignatures()->getSequence();
    for (int i = 0; i < sequences->size(); ++i)
    {
        const auto *k = static_cast<KeySignatureEvent *>(sequences->getUnchecked(i));
        if (k != &key &&
            HighlightingScheme::compareElements<KeySignatureEvent, KeySignatureEvent>(k, &key) == 0)
        {
            //Logger::writeToLog("Refuse to delete a scheme");
            return;
        }
    }

    const int index = this->binarySearchForHighlightingScheme(&key);
    if (index >= 0)
    {
        this->backgroundsCache.remove(index);
    }

#if DEBUG
    if (index >= 0)
    {
        Logger::writeToLog("Removed scheme " + key.toString());
    }
    else
    {
        Logger::writeToLog("Failed to remove scheme " + key.toString());
        jassertfalse;
    }
#endif
}

Array<Image> PianoRoll::renderBackgroundCacheFor(const HighlightingScheme *const scheme) const
{
    Array<Image> result;
    const auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    for (int j = 0; j <= PIANOROLL_MAX_ROW_HEIGHT; ++j)
    {
        result.add(PianoRoll::renderRowsPattern(theme, scheme->getScale(), scheme->getRootKey(), j));
    }
    return result;
}

Image PianoRoll::renderRowsPattern(const HelioTheme &theme,
    const Scale::Ptr scale, int root, int height)
{
    if (height < PIANOROLL_MIN_ROW_HEIGHT)
    {
        return Image(Image::RGB, 1, 1, true);
    }

    // Image patterns of width 128px take up to 5mb of ram (rows from 6 to 30)
    // Width 256px == ~10Mb. Prerendered patterns are drawing fast asf.
    Image patternImage(Image::RGB, 128, height * ROWS_OF_TWO_OCTAVES, false);
    Graphics g(patternImage);

    const Colour blackKey = theme.findColour(ColourIDs::Roll::blackKey);
    const Colour blackKeyBright = theme.findColour(ColourIDs::Roll::blackKeyAlt);
    const Colour whiteKey = theme.findColour(ColourIDs::Roll::whiteKey);
    const Colour whiteKeyBright = theme.findColour(ColourIDs::Roll::whiteKeyAlt);
    const Colour rootKey = whiteKeyBright.brighter(0.085f);
    const Colour rootKeyBright = whiteKeyBright.brighter(0.090f);
    const Colour rowLine = theme.findColour(ColourIDs::Roll::rowLine);

    float currentHeight = float(height);
    float previousHeight = 0;
    float pos_y = patternImage.getHeight() - currentHeight;

    const int middleCOffset = scale->getBasePeriod() - (MIDDLE_C % scale->getBasePeriod());
    const int lastOctaveReminder = (128 % scale->getBasePeriod()) - root + middleCOffset;

    g.setColour(whiteKeyBright);
    g.fillRect(patternImage.getBounds());

    // draw rows
    for (int i = lastOctaveReminder;
        (i < ROWS_OF_TWO_OCTAVES + lastOctaveReminder) && ((pos_y + previousHeight) >= 0.0f);
        i++)
    {
        const int noteNumber = (i % 12);
        const int octaveNumber = (i) / 12;
        const bool octaveIsOdd = ((octaveNumber % 2) > 0);

        previousHeight = currentHeight;

        if (noteNumber == 0)
        {
            const Colour c = octaveIsOdd ? rootKeyBright : rootKey;
            g.setColour(c);
            g.fillRect(0, int(pos_y + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(c.brighter(0.025f));
            g.drawHorizontalLine(int(pos_y + 1), 0.f, float(patternImage.getWidth()));
        }
        else if (scale->hasKey(noteNumber))
        {
            g.setColour(whiteKeyBright.brighter(0.025f));
            g.drawHorizontalLine(int(pos_y + 1), 0.f, float(patternImage.getWidth()));
        }
        else
        {
            g.setColour(octaveIsOdd ? blackKeyBright : blackKey);
            g.fillRect(0, int(pos_y + 1), patternImage.getWidth(), int(previousHeight - 1));
        }

        // fill divider line
        g.setColour(rowLine);
        g.drawHorizontalLine(int(pos_y), 0.f, float(patternImage.getWidth()));

        currentHeight = float(height);
        pos_y -= currentHeight;
    }

    HelioTheme::drawNoise(theme, g, 2.f);

    return patternImage;
}

PianoRoll::HighlightingScheme::HighlightingScheme(int rootKey, const Scale::Ptr scale) noexcept :
    rootKey(rootKey), scale(scale) {}

int PianoRoll::binarySearchForHighlightingScheme(const KeySignatureEvent *const target) const noexcept
{
    int s = 0, e = this->backgroundsCache.size();
    while (s < e)
    {
        auto scheme = this->backgroundsCache.getUnchecked(s);
        if (HighlightingScheme::compareElements<KeySignatureEvent, HighlightingScheme>(target, scheme) == 0)
        { return s; }

        const auto halfway = (s + e) / 2;
        if (halfway == s)
        { break; }

        scheme = this->backgroundsCache.getUnchecked(halfway);
        if (HighlightingScheme::compareElements<KeySignatureEvent, HighlightingScheme>(target, scheme) >= 0)
        { s = halfway; }
        else
        { e = halfway; }
    }

    return -1;
}
