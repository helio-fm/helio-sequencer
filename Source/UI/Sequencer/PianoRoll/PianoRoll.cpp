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
#include "AudioCore.h"
#include "Pattern.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"
#include "PianoTrackActions.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "VersionControlNode.h"
#include "ModalDialogInput.h"
#include "ProjectNode.h"
#include "ProjectTimeline.h"
#include "Note.h"
#include "NoteComponent.h"
#include "HelperRectangle.h"
#include "HybridRollHeader.h"
#include "KnifeToolHelper.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "SelectionComponent.h"
#include "HybridRollEditMode.h"
#include "HelioCallout.h"
#include "NotesTuningPanel.h"
#include "RescalePreviewTool.h"
#include "ChordPreviewTool.h"
#include "ScalePreviewTool.h"
#include "ArpPreviewTool.h"
#include "SequencerOperations.h"
#include "SerializationKeys.h"
#include "Arpeggiator.h"
#include "HeadlineItemDataSource.h"
#include "LassoListeners.h"
#include "UndoStack.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "HelioTheme.h"
#include "ComponentIDs.h"
#include "ColourIDs.h"
#include "Config.h"
#include "Icons.h"

#define DEFAULT_NOTE_LENGTH 0.25f

#define forEachEventOfGivenTrack(map, child, track) \
    for (const auto &_c : map) \
        if (_c.first.getPattern()->getTrack() == track) \
            for (const auto &child : (*_c.second.get()))

#define forEachSequenceMapOfGivenTrack(map, child, track) \
    for (const auto &child : map) \
        if (child.first.getPattern()->getTrack() == track)

#define forEachEventComponent(map, child) \
    for (const auto &_c : this->patternMap) \
        for (const auto &child : (*_c.second.get()))


PianoRoll::PianoRoll(ProjectNode &parentProject,
    Viewport &viewportRef,
    WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(parentProject, viewportRef, clippingDetector),
    activeTrack(nullptr),
    activeClip(),
    numRows(128),
    rowHeight(PIANOROLL_MIN_ROW_HEIGHT),
    newNoteDragging(nullptr),
    addNewNoteMode(false),
    newNoteVolume(0.25f),
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
    this->patternMap.clear();

    HYBRID_ROLL_BULK_REPAINT_START

    const auto &tracks = this->project.getTracks();
    for (const auto *track : tracks)
    {
        this->loadTrack(track);

        // Re-render backgrounds for all key signatures:
        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::KeySignature))
            {
                const auto &key = static_cast<const KeySignatureEvent &>(*event);
                this->updateBackgroundCacheFor(key);
            }
        }
    }

    this->repaint(this->viewport.getViewArea());

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::loadTrack(const MidiTrack *const track)
{
    if (track->getPattern() == nullptr)
    {
        return;
    }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const Clip *clip = track->getPattern()->getUnchecked(i);

        auto sequenceMap = new SequenceMap();
        this->patternMap[*clip] = UniquePointer<SequenceMap>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Note))
            {
                const Note *note = static_cast<const Note *>(event);
                auto nc = new NoteComponent(*this, *note, *clip);
                (*sequenceMap)[*note] = UniquePointer<NoteComponent>(nc);
                const bool isActive = nc->belongsTo(this->activeTrack, this->activeClip);
                nc->setActive(isActive, true);
                this->addAndMakeVisible(nc);
                nc->setFloatBounds(this->getEventBounds(nc));
            }
        }
    }
}

void PianoRoll::setEditableScope(WeakReference<MidiTrack> activeTrack, 
    const Clip &activeClip, bool shouldZoomToArea)
{
    this->selection.deselectAll();

    this->activeTrack = activeTrack;
    this->activeClip = activeClip;

    int focusMinKey = INT_MAX;
    int focusMaxKey = 0;
    float focusMinBeat = FLT_MAX;
    float focusMaxBeat = -FLT_MAX;

    forEachEventComponent(this->patternMap, e)
    {
        auto *nc = e.second.get();
        const bool isActive = nc->belongsTo(this->activeTrack, this->activeClip);
        const auto key = nc->getKey() + activeClip.getKey();
        nc->setActive(isActive, true);

        if (shouldZoomToArea && isActive)
        {
            focusMinKey = jmin(focusMinKey, key);
            focusMaxKey = jmax(focusMaxKey, key);
            focusMinBeat = jmin(focusMinBeat, nc->getBeat());
            focusMaxBeat = jmax(focusMaxBeat, nc->getBeat() + nc->getLength());
        }
    }

    // FIXME: zoom empty tracks properly

    this->updateActiveRangeIndicator();

    if (shouldZoomToArea)
    {
        this->zoomToArea(focusMinKey, focusMaxKey,
            focusMinBeat + this->activeClip.getBeat(),
            focusMaxBeat + this->activeClip.getBeat());
    }
    else
    {
        this->repaint(this->viewport.getViewArea());
    }
}

void PianoRoll::updateActiveRangeIndicator() const
{
    if (this->activeTrack != nullptr)
    {
        const float firstBeat = this->activeTrack->getSequence()->getFirstBeat();
        const float lastBeat = this->activeTrack->getSequence()->getLastBeat();
        const float clipBeat = this->activeClip.getBeat();

        this->header->updateSubrangeIndicator(this->activeTrack->getTrackColour(),
            firstBeat + clipBeat, lastBeat + clipBeat);
    }
}

WeakReference<MidiTrack> PianoRoll::getActiveTrack() const noexcept { return this->activeTrack; }
const Clip &PianoRoll::getActiveClip() const noexcept { return this->activeClip; }

void PianoRoll::setDefaultNoteVolume(float volume) noexcept
{
    this->newNoteVolume = volume;
}

void PianoRoll::setRowHeight(int newRowHeight)
{
    if (newRowHeight == this->rowHeight) { return; }
    this->rowHeight = jlimit(PIANOROLL_MIN_ROW_HEIGHT, PIANOROLL_MAX_ROW_HEIGHT, newRowHeight);
    this->setSize(this->getWidth(), HYBRID_ROLL_HEADER_HEIGHT + this->numRows * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// HybridRoll
//===----------------------------------------------------------------------===//

void PianoRoll::selectAll()
{
    forEachEventComponent(this->patternMap, e)
    {
        const auto childComponent = e.second.get();
        if (childComponent->belongsTo(this->activeTrack, activeClip))
        {
            this->selection.addToSelection(childComponent);
        }
    }
}

void PianoRoll::setChildrenInteraction(bool interceptsMouse, MouseCursor cursor)
{
    forEachEventComponent(this->patternMap, e)
    {
        const auto childComponent = e.second.get();
        childComponent->setInterceptsMouseClicks(interceptsMouse, interceptsMouse);
        childComponent->setMouseCursor(cursor);
    }
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PianoRoll::showGhostNoteFor(NoteComponent *target)
{
    auto component = new NoteComponent(*this, target->getNote(), target->getClip());
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
    static const float yZoomThreshold = 0.005f;

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
    const float newHeight = (this->getNumRows() * PIANOROLL_MAX_ROW_HEIGHT) * zoom.getY();
    const float rowsOnNewScreen = float(newHeight / PIANOROLL_MAX_ROW_HEIGHT);
    const float viewHeight = float(this->viewport.getViewHeight());
    const float newRowHeight = floorf(viewHeight / rowsOnNewScreen + .5f);

    this->setRowHeight(int(newRowHeight));

    HybridRoll::zoomAbsolute(zoom);
}

float PianoRoll::getZoomFactorY() const noexcept
{
    return float(this->viewport.getViewHeight()) / float(this->getHeight());
}

void PianoRoll::zoomToArea(int minKey, int maxKey, float minBeat, float maxBeat)
{
    jassert(minKey >= 0);
    jassert(maxKey >= minKey);

    const int margin = 2;
    const float numKeysToFit = float(maxKey - minKey + margin);
    const float heightToFit = float(this->viewport.getViewHeight());
    this->setRowHeight(int(heightToFit / numKeysToFit));

    const int maxKeyY = this->getRowHeight() * (128 - maxKey - margin);
    this->viewport.setViewPosition(this->viewport.getViewPositionY() - HYBRID_ROLL_HEADER_HEIGHT, maxKeyY);

    HybridRoll::zoomToArea(minBeat, maxBeat);
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
    jassert(dynamic_cast<NoteComponent *>(mc));
    const auto *nc = static_cast<NoteComponent *>(mc);
    return this->getEventBounds(nc->getKey() + nc->getClip().getKey(),
        nc->getBeat() + nc->getClip().getBeat(), nc->getLength());
}

Rectangle<float> PianoRoll::getEventBounds(int key, float beat, float length) const
{
    jassert(key >= -128 && key <= 128);

    const double startOffsetBeat = this->firstBar * double(BEATS_PER_BAR);
    const double x = this->barWidth * double(beat - startOffsetBeat) / double(BEATS_PER_BAR);

    const float w = this->barWidth * length / float(BEATS_PER_BAR);
    const float yPosition = float(this->getYPositionByKey(key));

    return { float(x), yPosition + 1, w, float(this->rowHeight - 1) };
}

void PianoRoll::getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getRoundBeatByXPosition(int(x)) - this->activeClip.getBeat(); /* - 0.5f ? */
    noteNumber = int((this->getHeight() - y) / this->rowHeight) - this->activeClip.getKey();
    noteNumber = jlimit(0, numRows - 1, noteNumber);
}

void PianoRoll::getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getFloorBeatByXPosition(x) - this->activeClip.getBeat();
    noteNumber = int((this->getHeight() - y) / this->rowHeight) - this->activeClip.getKey();
    noteNumber = jlimit(0, numRows - 1, noteNumber);
}

int PianoRoll::getYPositionByKey(int targetKey) const
{
    return (this->getHeight() - this->rowHeight) - (targetKey * this->rowHeight);
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
        this->fader.fadeOut(this->helperHorizontal, SHORT_FADE_TIME);
    }
}

void PianoRoll::moveHelpers(const float deltaBeat, const int deltaKey)
{
    const float firstBeat = this->firstBar * float(BEATS_PER_BAR);
    const Rectangle<int> selectionBounds = this->selection.getSelectionBounds();
    const Rectangle<float> delta = this->getEventBounds(deltaKey - 1, deltaBeat + firstBeat, 1.f);

    const int deltaX = int(delta.getTopLeft().getX());
    const int deltaY = int(delta.getTopLeft().getY() - this->getHeight() - 1);
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
    if (oldEvent.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(oldEvent);
        const Note &newNote = static_cast<const Note &>(newEvent);
        const auto track = newEvent.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (const auto component = sequenceMap[note].release())
            {
                // Pass ownership to another key:
                sequenceMap.erase(note);
                // Hitting this assert means that a track somehow contains events
                // with duplicate id's. This should never, ever happen.
                jassert(!sequenceMap.contains(newNote));
                // Always erase before updating, as it may happen both events have the same hash code:
                sequenceMap[newNote] = UniquePointer<NoteComponent>(component);
                // Schedule to be repainted later:
                this->batchRepaintList.add(component);
                this->triggerAsyncUpdate();
            }
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

    HybridRoll::onChangeMidiEvent(oldEvent, newEvent);
}

void PianoRoll::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto track = note.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            const auto *targetParams = &c.first;
            const int i = track->getPattern()->indexOfSorted(targetParams);
            jassert(i >= 0);
            
            const Clip *realClip = track->getPattern()->getUnchecked(i);
            auto component = new NoteComponent(*this, note, *realClip);
            sequenceMap[note] = UniquePointer<NoteComponent>(component);
            this->addAndMakeVisible(component);

            this->fader.fadeIn(component, 150);

            const bool isActive = component->belongsTo(this->activeTrack, this->activeClip);
            component->setActive(isActive);

            this->batchRepaintList.add(component);
            this->triggerAsyncUpdate(); // instead of updateBounds

            // arpeggiators preview cannot work without that:
            if (isActive)
            {
                this->selection.addToSelection(component);
            }

            if (this->addNewNoteMode && isActive)
            {
                this->newNoteDragging = component;
                this->addNewNoteMode = false;
                this->selectEvent(this->newNoteDragging, true); // clear prev selection
            }
        }
    }
    else if (event.isTypeOf(MidiEvent::KeySignature))
    {
        // Repainting background caches on the fly may be costly
        const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(event);
        this->updateBackgroundCacheFor(key);
        this->repaint();
    }

    HybridRoll::onAddMidiEvent(event);
}

void PianoRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Note))
    {
        this->hideHelpers();
        this->hideAllGhostNotes(); // Avoids crash

        const Note &note = static_cast<const Note &>(event);
        const auto track = note.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (sequenceMap.contains(note))
            {
                NoteComponent *deletedComponent = sequenceMap[note].get();
                this->fader.fadeOut(deletedComponent, 150);
                this->selection.deselect(deletedComponent);
                sequenceMap.erase(note);
            }
        }
    }
    else if (event.isTypeOf(MidiEvent::KeySignature))
    {
        const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(event);
        this->removeBackgroundCacheFor(key);
        this->repaint();
    }

    HybridRoll::onRemoveMidiEvent(event);
}

void PianoRoll::onAddClip(const Clip &clip)
{
    const SequenceMap *referenceMap = nullptr;
    const auto *track = clip.getPattern()->getTrack();

    forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
    {
        // Found a sequence map for the same track
        referenceMap = c.second.get();
        break;
    }

    if (referenceMap == nullptr)
    {
        jassertfalse;
        return;
    }

    auto sequenceMap = new SequenceMap();
    this->patternMap[clip] = UniquePointer<SequenceMap>(sequenceMap);

    for (const auto &e : *referenceMap)
    {
        // reference the same note as neighbor components:
        const auto &note = e.second.get()->getNote();
        auto component = new NoteComponent(*this, note, clip);
        (*sequenceMap)[note] = UniquePointer<NoteComponent>(component);
        this->addAndMakeVisible(component);

        const bool isActive = component->belongsTo(this->activeTrack, this->activeClip);
        component->setActive(isActive);

        this->batchRepaintList.add(component);
    }

    this->triggerAsyncUpdate();
}

void PianoRoll::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->activeClip == clip)
    {
        this->activeClip = newClip;
    }

    if (auto *sequenceMap = this->patternMap[clip].release())
    {
        // Set new key for existing sequence map
        this->patternMap.erase(clip);
        this->patternMap[newClip] = UniquePointer<SequenceMap>(sequenceMap);

        // And update all components within it, as their beats should change
        for (const auto &e : *sequenceMap)
        {
            this->batchRepaintList.add(e.second.get());
        }

        if (newClip == this->activeClip)
        {
            this->updateActiveRangeIndicator();
        }

        // Schedule batch repaint
        this->triggerAsyncUpdate();
    }
}

void PianoRoll::onRemoveClip(const Clip &clip)
{
    HYBRID_ROLL_BULK_REPAINT_START

    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
    }

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::onChangeTrackProperties(MidiTrack *const track)
{
    if (dynamic_cast<const PianoSequence *>(track->getSequence()))
    {
        forEachEventOfGivenTrack(this->patternMap, e, track)
        {
            const auto component = e.second.get();
            component->updateColours();
        }

        this->updateActiveRangeIndicator(); // colour might have changed
        this->repaint();
    }
}

void PianoRoll::onAddTrack(MidiTrack *const track)
{
    HYBRID_ROLL_BULK_REPAINT_START

    this->loadTrack(track);

    for (int j = 0; j < track->getSequence()->size(); ++j)
    {
        const MidiEvent *const event = track->getSequence()->getUnchecked(j);
        if (event->isTypeOf(MidiEvent::KeySignature))
        {
            const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(*event);
            this->updateBackgroundCacheFor(key);
        }
    }

    // In case key signatures added:
    this->repaint(this->viewport.getViewArea());

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::onRemoveTrack(MidiTrack *const track)
{
    this->selection.deselectAll();

    this->hideHelpers();
    this->hideAllGhostNotes(); // Avoids crash

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const auto *event = track->getSequence()->getUnchecked(i);
        if (event->isTypeOf(MidiEvent::KeySignature))
        {
            const KeySignatureEvent &key = static_cast<const KeySignatureEvent &>(*event);
            this->removeBackgroundCacheFor(key);
        }
    }

    for (int i = 0; i < track->getPattern()->size(); ++i)
    {
        const auto &clip = *track->getPattern()->getUnchecked(i);
        if (this->patternMap.contains(clip))
        {
            this->patternMap.erase(clip);
        }
    }

    this->repaint();
}

void PianoRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->reloadRollContent();
}

void PianoRoll::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->updateActiveRangeIndicator();
    HybridRoll::onChangeProjectBeatRange(firstBeat, lastBeat);
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

    forEachEventComponent(this->patternMap, e)
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
    forEachEventComponent(this->patternMap, e)
    {
        const auto component = e.second.get();
        component->setSelected(false);
    }

    for (const auto component : this->selection)
    {
        component->setSelected(true);
    }
    
    forEachEventComponent(this->patternMap, e)
    {
        const auto component = e.second.get();
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
            component->setSelected(true);
            itemsFound.addIfNotAlreadyThere(component);
        }
    }
}

float PianoRoll::getLassoStartBeat() const
{
    return this->activeClip.getBeat() + SequencerOperations::findStartBeat(this->selection);
}

float PianoRoll::getLassoEndBeat() const
{
    return this->activeClip.getBeat() + SequencerOperations::findEndBeat(this->selection);
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
        else if (this->isKnifeToolEvent(e))
        {
            this->startCuttingEvents(e);
        }
    }

    HybridRoll::mouseDown(e);
}

void PianoRoll::mouseDoubleClick(const MouseEvent &e)
{
    if (! this->project.getEditMode().forbidsAddingEvents())
    {
        const MouseEvent &e2(e.getEventRelativeTo(&App::Layout()));
        this->showChordTool(e.mods.isAnyModifierKeyDown() ? ScalePreview : ChordPreview, e2.getPosition());
    }
}

void PianoRoll::mouseDrag(const MouseEvent &e)
{
    // can show menus
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->newNoteDragging)
    {
        if (this->newNoteDragging->isInitializing())
        {
            this->newNoteDragging->mouseDrag(e.getEventRelativeTo(this->newNoteDragging));
        }
        else
        {
            this->newNoteDragging->startInitializing();
            this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
        }
    }
    else if (this->isKnifeToolEvent(e))
    {
        this->continueCuttingEvents(e);
    }

    HybridRoll::mouseDrag(e);
}

void PianoRoll::mouseUp(const MouseEvent &e)
{
    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }
    
    // Dismiss newNoteDragging, if needed
    if (this->newNoteDragging != nullptr)
    {

        this->newNoteDragging->endInitializing();
        this->setMouseCursor(this->project.getEditMode().getCursor());
        this->newNoteDragging = nullptr;
    }

    this->endCuttingEventsIfNeeded();

    if (! this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, true);

        // process lasso selection logic
        HybridRoll::mouseUp(e);
    }
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
    case CommandIDs::ZoomEntireClip:
        this->setEditableScope(this->activeTrack, this->activeClip, true);
        this->zoomOutImpulse(0.25f); // A bit of fancy animation
        break;
    case CommandIDs::RenameTrack:
        if (auto trackNode = dynamic_cast<MidiTrackNode *>(this->project.findActiveItem()))
        {
            auto inputDialog = ModalDialogInput::Presets::renameTrack(trackNode->getXPath());
            inputDialog->onOk = trackNode->getRenameCallback();
            App::Layout().showModalComponentUnowned(inputDialog.release());
        }
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
        const auto playheadPos = this->project.getTransport().getSeekPosition();
        const float playheadBeat = this->getBeatByTransportPosition(playheadPos) - this->activeClip.getBeat();
        SequencerOperations::pasteFromClipboard(App::Clipboard(), this->project, this->getActiveTrack(), playheadBeat);
    }
        break;
    case CommandIDs::DeleteEvents:
        SequencerOperations::deleteSelection(this->getLassoSelection());
        break;
    case CommandIDs::NewTrackFromSelection:
        if (this->getLassoSelection().getNumSelected() > 0)
        {
            const auto track = SequencerOperations::createPianoTrack(this->getLassoSelection());
            const ValueTree trackTemplate = track->serialize();
            auto inputDialog = ModalDialogInput::Presets::newTrack();
            inputDialog->onOk = [trackTemplate, this](const String &input)
            {
                SequencerOperations::deleteSelection(this->getLassoSelection(), true);
                this->project.getUndoStack()->perform(new PianoTrackInsertAction(this->project,
                    &this->project, trackTemplate, input));
            };

            App::Layout().showModalComponentUnowned(inputDialog.release());
        }
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
    case CommandIDs::CreateArpeggiatorFromSelection:
        if (this->selection.getNumSelected() >= 2)
        {
            Array<Note> selectedNotes;
            for (int i = 0; i < this->selection.getNumSelected(); ++i)
            {
                const auto nc = static_cast<NoteComponent *>(this->selection.getSelectedItem(i));
                selectedNotes.add(nc->getNote());
            }

            Note::Key contextKey = 0;
            Scale::Ptr contextScale = nullptr;
            if (SequencerOperations::findHarmonicContext(this->selection, this->activeClip,
                this->project.getTimeline()->getKeySignatures(), contextScale, contextKey))
            {
                auto newArpDialog = ModalDialogInput::Presets::newArpeggiator();
                newArpDialog->onOk = [contextScale, contextKey, selectedNotes](const String &name)
                {
                    Arpeggiator::Ptr arp(new Arpeggiator(name, contextScale, selectedNotes, contextKey));
                    App::Config().getArpeggiators()->updateUserResource(arp);
                };

                App::Layout().showModalComponentUnowned(newArpDialog.release());
            }
        }
        break;
    case CommandIDs::ShowArpeggiatorsPanel:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        if (auto *panel = ArpPreviewTool::createWithinContext(*this,
            this->project.getTimeline()->getKeySignatures()))
        {
            HelioCallout::emit(panel, this, true);
        }
        break;
    case CommandIDs::ShowRescalePanel:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        if (auto *panel = RescalePreviewTool::createWithinContext(*this,
            this->project.getTimeline()->getKeySignatures()))
        {
            HelioCallout::emit(panel, this, true);
        }
        break;
    case CommandIDs::ShowScalePanel:
        this->showChordTool(ScalePreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ShowChordPanel:
        this->showChordTool(ChordPreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ShowVolumePanel:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
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

    forEachEventComponent(this->patternMap, e)
    {
        const auto component = e.second.get();
        component->setFloatBounds(this->getEventBounds(component));
    }

    for (const auto component : this->ghostNotes)
    {
        component->setFloatBounds(this->getEventBounds(component));
    }

    if (this->knifeToolHelper != nullptr)
    {
        this->knifeToolHelper->updateBounds();
        this->knifeToolHelper->updateCutMarks();
    }

    HybridRoll::resized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::paint(Graphics &g)
{
    const auto *keysSequence = this->project.getTimeline()->getKeySignatures()->getSequence();
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = paintStartX + this->viewport.getViewWidth();

    static const float paintOffsetY = float(HYBRID_ROLL_HEADER_HEIGHT);

    int prevBarX = paintStartX;
    const HighlightingScheme *prevScheme = nullptr;
    const int y = this->viewport.getViewPositionY();
    const int h = this->viewport.getViewHeight();

    for (int nextKeyIdx = 0; nextKeyIdx < keysSequence->size(); ++nextKeyIdx)
    {
        const auto *key = static_cast<KeySignatureEvent *>(keysSequence->getUnchecked(nextKeyIdx));
        const int barX = int(((key->getBeat() / float(BEATS_PER_BAR)) - this->firstBar)  * this->barWidth);
        const int index = this->binarySearchForHighlightingScheme(key);

#if DEBUG
        if (index < 0)
        {
            DBG("Missing " + key->toString());
            jassert(index >= 0);
        }
#endif

        const auto *s = (prevScheme == nullptr) ? this->backgroundsCache.getUnchecked(index) : prevScheme;
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
        const auto *s = (prevScheme == nullptr) ? this->defaultHighlighting : prevScheme;
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
    this->addNote(draggingRow, draggingColumn, DEFAULT_NOTE_LENGTH, this->newNoteVolume);
}

void PianoRoll::startCuttingEvents(const MouseEvent &e)
{
    if (this->knifeToolHelper == nullptr)
    {
        this->deselectAll();
        this->knifeToolHelper = new KnifeToolHelper(*this);
        this->addAndMakeVisible(this->knifeToolHelper);
        this->knifeToolHelper->toBack();
        this->knifeToolHelper->fadeIn();
    }

    this->knifeToolHelper->setStartPosition(e.position);
    this->knifeToolHelper->setEndPosition(e.position);
}

void PianoRoll::continueCuttingEvents(const MouseEvent &event)
{
    if (this->knifeToolHelper != nullptr)
    {
        this->knifeToolHelper->setEndPosition(event.position);
        this->knifeToolHelper->updateBounds();

        bool addsPoint;
        Point<float> intersection;
        forEachEventComponent(this->patternMap, e)
        {
            addsPoint = false;
            auto *nc = e.second.get();
            if (nc->isActive())
            {
                const int h2 = nc->getHeight() / 2;
                const Line<float> noteLine(nc->getPosition().translated(0, h2).toFloat(),
                    nc->getPosition().translated(nc->getWidth(), h2).toFloat());

                if (this->knifeToolHelper->getLine().intersects(noteLine, intersection))
                {
                    const float relativeCutBeat = this->getRoundBeatByXPosition(int(intersection.getX()))
                        - this->activeClip.getBeat() - nc->getBeat();
 
                    if (relativeCutBeat > 0.f && relativeCutBeat < nc->getLength())
                    {
                        addsPoint = true;
                        this->knifeToolHelper->addOrUpdateCutPoint(nc, relativeCutBeat);
                    }
                }

                if (!addsPoint)
                {
                    this->knifeToolHelper->removeCutPointIfExists(nc->getNote());
                }
            }
        }
    }
}

void PianoRoll::endCuttingEventsIfNeeded()
{
    if (this->knifeToolHelper != nullptr)
    {
        Array<Note> notes;
        Array<float> beats;
        this->knifeToolHelper->getCutPoints(notes, beats);
        Array<Note> cutEventsToTheRight = SequencerOperations::cutEvents(notes, beats);
        // Now select all the new notes:
        forEachSequenceMapOfGivenTrack(this->patternMap, c, this->activeTrack)
        {
            auto &sequenceMap = *c.second.get();
            for (const auto &note : cutEventsToTheRight)
            {
                if (auto *component = sequenceMap[note].get())
                {
                    this->selectEvent(component, false);
                }
            }
        }

        this->applyEditModeUpdates(); // update behaviour of newly created note components
        this->knifeToolHelper = nullptr;
    }
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

void PianoRoll::changeListenerCallback(ChangeBroadcaster *source)
{
    this->endCuttingEventsIfNeeded();
    HybridRoll::changeListenerCallback(source);
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
        DBG("Added scheme " + key.toString());
    }
    else
    {
        DBG("Ignored duplicate " + key.toString());
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
        DBG("Removed scheme " + key.toString());
    }
    else
    {
        DBG("Failed to remove scheme " + key.toString());
        jassertfalse;
    }
#endif
}

Array<Image> PianoRoll::renderBackgroundCacheFor(const HighlightingScheme *const scheme) const
{
    Array<Image> result;
    const auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    for (int j = 0; j < PIANOROLL_MIN_ROW_HEIGHT; ++j)
    {
        result.add({});
    }
    for (int j = PIANOROLL_MIN_ROW_HEIGHT; j <= PIANOROLL_MAX_ROW_HEIGHT; ++j)
    {
        result.add(PianoRoll::renderRowsPattern(theme, scheme->getScale(), scheme->getRootKey(), j));
    }
    return result;
}

// pre-rendered tiles are used in paint() method to fill the background,
// but OpenGL doesn't work well with non-power-of-2 textures
// and the smaller the texture is, the uglier it is displayed,
// that's why I ended up rendering 6 octaves here instead of one:
#define NUM_ROWS_TO_RENDER (CHROMATIC_SCALE_SIZE * 6)

Image PianoRoll::renderRowsPattern(const HelioTheme &theme,
    const Scale::Ptr scale, int root, int height)
{
    if (height < PIANOROLL_MIN_ROW_HEIGHT)
    {
        return Image(Image::RGB, 1, 1, true);
    }

    Image patternImage(Image::RGB, 4, height * NUM_ROWS_TO_RENDER, false);
    Graphics g(patternImage);

    const Colour blackKey = theme.findColour(ColourIDs::Roll::blackKey);
    const Colour blackKeyOdd = theme.findColour(ColourIDs::Roll::blackKeyAlt);
    const Colour whiteKey = theme.findColour(ColourIDs::Roll::whiteKey);
    const Colour whiteKeyOdd = theme.findColour(ColourIDs::Roll::whiteKeyAlt);
    const Colour rootKey = whiteKey.brighter(0.1f);
    const Colour rootKeyOdd = whiteKeyOdd.brighter(0.1f);
    const Colour rowLine = theme.findColour(ColourIDs::Roll::rowLine);

    float currentHeight = float(height);
    float previousHeight = 0;
    float posY = patternImage.getHeight() - currentHeight;

    const int middleCOffset = scale->getBasePeriod() - (MIDDLE_C % scale->getBasePeriod());
    const int lastOctaveReminder = (128 % scale->getBasePeriod()) - root + middleCOffset;

    //g.setColour(whiteKeyOdd);
    //g.fillRect(patternImage.getBounds());

    // draw rows
    for (int i = lastOctaveReminder;
        (i < NUM_ROWS_TO_RENDER + lastOctaveReminder) && ((posY + previousHeight) >= 0.0f);
        i++)
    {
        const int noteNumber = (i % 12);
        const int octaveNumber = (i) / 12;
        const bool octaveIsOdd = ((octaveNumber % 2) > 0);

        previousHeight = currentHeight;

        if (noteNumber == 0)
        {
            const Colour c = octaveIsOdd ? rootKeyOdd : rootKey;
            g.setColour(c);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(c.brighter(0.025f));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }
        else if (scale->hasKey(noteNumber))
        {
            const Colour c = octaveIsOdd ? whiteKeyOdd : whiteKey;
            g.setColour(c);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(c.brighter(0.025f));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }
        else
        {
            g.setColour(octaveIsOdd ? blackKeyOdd : blackKey);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
        }

        // fill divider line
        g.setColour(rowLine);
        g.drawHorizontalLine(int(posY), 0.f, float(patternImage.getWidth()));

        currentHeight = float(height);
        posY -= currentHeight;
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

void PianoRoll::showChordTool(ToolType type, Point<int> position)
{
    auto *pianoSequence = dynamic_cast<PianoSequence *>(this->activeTrack->getSequence());
    jassert(pianoSequence);

    this->deselectAll();

    switch (type)
    {
    case PianoRoll::ScalePreview:
        if (pianoSequence != nullptr)
        {
            auto *popup = new ScalePreviewTool(this, pianoSequence);
            popup->setTopLeftPosition(position - Point<int>(popup->getWidth(), popup->getHeight()) / 2);
            App::Layout().addAndMakeVisible(popup);
        }
        break;
    case PianoRoll::ChordPreview:
        if (auto *harmonicContext =
            dynamic_cast<KeySignaturesSequence *>(this->project.getTimeline()->getKeySignatures()->getSequence()))
        {
            auto *popup = new ChordPreviewTool(*this, pianoSequence, this->activeClip, harmonicContext);
            popup->setTopLeftPosition(position - Point<int>(popup->getWidth(), popup->getHeight()) / 2);
            App::Layout().addAndMakeVisible(popup);
        }
        break;
    default:
        break;
    }
}
