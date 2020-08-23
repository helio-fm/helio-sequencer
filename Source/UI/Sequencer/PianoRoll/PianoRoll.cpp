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
#include "PluginWindow.h"
#include "Pattern.h"
#include "MidiSequence.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "AnnotationsSequence.h"
#include "KeySignaturesSequence.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "VersionControlNode.h"
#include "ModalDialogInput.h"
#include "TrackPropertiesDialog.h"
#include "ProjectTimeline.h"
#include "ProjectMetadata.h"
#include "Note.h"
#include "NoteComponent.h"
#include "NoteNameGuidesBar.h"
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
#include "PatternOperations.h"
#include "SerializationKeys.h"
#include "Arpeggiator.h"
#include "HeadlineItemDataSource.h"
#include "CommandPaletteChordConstructor.h"
#include "LassoListeners.h"
#include "UndoStack.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "HelioTheme.h"
#include "ComponentIDs.h"
#include "ColourIDs.h"
#include "Config.h"
#include "Icons.h"

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

#if HELIO_DESKTOP
#   define PIANOROLL_HAS_NOTE_RESIZERS 0
#elif HELIO_MOBILE
#   define PIANOROLL_HAS_NOTE_RESIZERS 1
#endif

PianoRoll::PianoRoll(ProjectNode &project, Viewport &viewport, WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(project, viewport, clippingDetector)
{
    this->setComponentID(ComponentIDs::pianoRollId);

    this->selectedNotesMenuManager = make<PianoRollSelectionMenuManager>(&this->selection, this->project);

    this->draggingHelper = make<HelperRectangleHorizontal>();
    this->addChildComponent(this->draggingHelper.get());

    this->consoleChordConstructor = make<CommandPaletteChordConstructor>(*this);

    const auto *uiFlags = App::Config().getUiFlags();
    this->scalesHighlightingEnabled = uiFlags->isScalesHighlightingEnabled();
    const bool noteNameGuidesEnabled = uiFlags->isNoteNameGuidesEnabled();

    this->noteNameGuides = make<NoteNameGuidesBar>(*this);
    this->addChildComponent(this->noteNameGuides.get());
    this->noteNameGuides->setVisible(noteNameGuidesEnabled);

    this->reloadRollContent();

    // finally, when the bg caches are rendered:
    this->setRowHeight(PianoRoll::defaultRowHeight);
    this->setBeatRange(0, Globals::Defaults::projectLength);
}

PianoRoll::~PianoRoll() {}

void PianoRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->patternMap.clear();

    HYBRID_ROLL_BULK_REPAINT_START

    for (const auto *track : this->project.getTracks())
    {
        this->loadTrack(track);
    }

    this->updateBackgroundCachesAndRepaint();

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

        auto *sequenceMap = new SequenceMap();
        this->patternMap[*clip] = UniquePointer<SequenceMap>(sequenceMap);

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Note))
            {
                const Note *note = static_cast<const Note *>(event);
                auto *nc = new NoteComponent(*this, *note, *clip);
                (*sequenceMap)[*note] = UniquePointer<NoteComponent>(nc);
                const bool isActive = nc->belongsTo(this->activeTrack, this->activeClip);
                nc->setActive(isActive, true);
                this->addAndMakeVisible(nc);
                nc->setFloatBounds(this->getEventBounds(nc));
            }
        }
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

void PianoRoll::setDefaultNoteLength(float length) noexcept
{
    // no less than 0.25f, as it can become pretty much unusable:
    this->newNoteLength = jmax(0.25f, length);
}

void PianoRoll::setRowHeight(int newRowHeight)
{
    if (newRowHeight == this->rowHeight) { return; }
    this->rowHeight = jlimit(PianoRoll::minRowHeight, PianoRoll::maxRowHeight, newRowHeight);
    this->updateSize();
}

void PianoRoll::updateSize()
{
    this->setSize(this->getWidth(), HybridRoll::headerHeight + this->getNumKeys() * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// HybridRoll
//===----------------------------------------------------------------------===//

void PianoRoll::selectAll()
{
    forEachEventComponent(this->patternMap, e)
    {
        auto *childComponent = e.second.get();
        if (childComponent->belongsTo(this->activeTrack, activeClip))
        {
            this->selectEvent(childComponent, false);
        }
    }
}

void PianoRoll::setChildrenInteraction(bool interceptsMouse, MouseCursor cursor)
{
    forEachEventComponent(this->patternMap, e)
    {
        auto *childComponent = e.second.get();
        childComponent->setInterceptsMouseClicks(interceptsMouse, interceptsMouse);
        childComponent->setMouseCursor(cursor);
    }
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PianoRoll::showGhostNoteFor(NoteComponent *target)
{
    auto *component = new NoteComponent(*this, target->getNote(), target->getClip());
    component->setEnabled(false);

    //component->setAlpha(0.2f); // setAlpha makes everything slower
    component->setGhostMode(); // use this, Luke.

    this->addAndMakeVisible(component);
    this->ghostNotes.add(component);

    this->triggerBatchRepaintFor(component);
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
// Input Listeners
//===----------------------------------------------------------------------===//

void PianoRoll::longTapEvent(const Point<float> &position,
    const WeakReference<Component> &target)
{
    // try to switch to selected note's track:
    if (!this->multiTouchController->hasMultitouch() &&
        !this->getEditMode().forbidsSelectionMode())
    {
        const auto *nc = dynamic_cast<NoteComponent *>(target.get());
        if (nc != nullptr && !nc->isActive())
        {
            auto *track = nc->getNote().getSequence()->getTrack();
            this->project.setEditableScope(track, nc->getClip(), false);
            return;
        }
    }

    // else - start dragging lasso, if needed:
    HybridRoll::longTapEvent(position, target);
}

void PianoRoll::zoomRelative(const Point<float> &origin, const Point<float> &factor)
{
    static const float yZoomThreshold = 0.035f;

    if (fabs(factor.getY()) > yZoomThreshold)
    {
        const Point<float> oldViewPosition = this->viewport.getViewPosition().toFloat();
        const Point<float> absoluteOrigin = oldViewPosition + origin;
        const float oldHeight = float(this->getHeight());

        int newRowHeight = this->getRowHeight();
        newRowHeight = (factor.getY() < -yZoomThreshold) ? (newRowHeight - 1) : newRowHeight;
        newRowHeight = (factor.getY() > yZoomThreshold) ? (newRowHeight + 1) : newRowHeight;

        const float estimatedNewHeight = float(newRowHeight * this->getNumKeys());

        if (estimatedNewHeight < this->viewport.getViewHeight() ||
            newRowHeight > PianoRoll::maxRowHeight ||
            newRowHeight < PianoRoll::minRowHeight)
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
    const float newHeight = (this->getNumKeys() * PianoRoll::maxRowHeight) * zoom.getY();
    const float rowsOnNewScreen = float(newHeight / PianoRoll::maxRowHeight);
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

    constexpr auto margin = Globals::twelveTonePeriodSize;
    const float numKeysToFit = float(maxKey - minKey + (margin * 2));
    const float heightToFit = float(this->viewport.getViewHeight());
    this->setRowHeight(int(heightToFit / numKeysToFit));

    const int maxKeyY = this->getRowHeight() * (this->getNumKeys() - maxKey - margin);
    this->viewport.setViewPosition(this->viewport.getViewPositionY() - HybridRoll::headerHeight, maxKeyY);

    HybridRoll::zoomToArea(minBeat, maxBeat);
}

//===----------------------------------------------------------------------===//
// Note management
//===----------------------------------------------------------------------===//

Rectangle<float> PianoRoll::getEventBounds(FloatBoundsComponent *mc) const
{
    //jassert(dynamic_cast<NoteComponent *>(mc));
    const auto *nc = static_cast<NoteComponent *>(mc);
    return this->getEventBounds(nc->getKey() + nc->getClip().getKey(),
        nc->getBeat() + nc->getClip().getBeat(), nc->getLength());
}

Rectangle<float> PianoRoll::getEventBounds(int key, float beat, float length) const
{
    // todo jassert depending on temperament used
    //jassert(key >= -Globals::maxNoteKey && key <= Globals::maxNoteKey);

    const float x = this->beatWidth * (beat - this->firstBeat);
    const float w = this->beatWidth * length;
    const float yPosition = float(this->getYPositionByKey(key));

    return { x, yPosition + 1.f, w, float(this->rowHeight - 1) };
}

bool PianoRoll::isNoteVisible(int key, float beat, float length) const
{
    const auto view = this->viewport.getViewArea().toFloat();
    const auto firstViewportBeat = this->getBeatByXPosition(view.getX());
    const auto lastViewportBeat = this->getBeatByXPosition(view.getRight());
    const auto firstViewportKey = int(this->getHeight() - view.getBottom()) / this->getRowHeight();
    const auto lastViewportKey = int(this->getHeight() - view.getY()) / this->getRowHeight();

    return (key > firstViewportKey && key < lastViewportKey) &&
        ((beat > firstViewportBeat && beat < lastViewportBeat) ||
            (beat + length > firstViewportBeat && beat + length < lastViewportBeat));
}

void PianoRoll::getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getRoundBeatSnapByXPosition(int(x)) - this->activeClip.getBeat(); /* - 0.5f ? */
    noteNumber = int((this->getHeight() - y) / this->rowHeight) - this->activeClip.getKey();
    noteNumber = jlimit(0, this->getNumKeys(), noteNumber);
}

void PianoRoll::getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const
{
    beatNumber = this->getFloorBeatSnapByXPosition(x) - this->activeClip.getBeat();
    noteNumber = int((this->getHeight() - y) / this->rowHeight) - this->activeClip.getKey();
    noteNumber = jlimit(0, this->getNumKeys(), noteNumber);
}

int PianoRoll::getYPositionByKey(int targetKey) const
{
    return (this->getHeight() - this->rowHeight) - (targetKey * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// Drag helpers
//===----------------------------------------------------------------------===//

void PianoRoll::showDragHelpers()
{
    if (!this->draggingHelper->isVisible())
    {
        this->selection.needsToCalculateSelectionBounds();
        this->moveDragHelpers(0.f, 0);
        this->draggingHelper->setAlpha(1.f);
        this->draggingHelper->setVisible(true);
    }
}

void PianoRoll::hideDragHelpers()
{
    if (this->draggingHelper->isVisible())
    {
        this->fader.fadeOut(this->draggingHelper.get(), 150);
    }
}

void PianoRoll::moveDragHelpers(const float deltaBeat, const int deltaKey)
{
    const Rectangle<int> selectionBounds = this->selection.getSelectionBounds();
    const Rectangle<float> delta = this->getEventBounds(deltaKey - 1, deltaBeat + this->firstBeat, 1.f);

    const int deltaX = int(delta.getTopLeft().getX());
    const int deltaY = int(delta.getTopLeft().getY() - this->getHeight() - 1);
    const Rectangle<int> selectionTranslated = selectionBounds.translated(deltaX, deltaY);

    const int vX = this->viewport.getViewPositionX();
    const int vW = this->viewport.getViewWidth();
    this->draggingHelper->setBounds(selectionTranslated.withLeft(vX).withWidth(vW));
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoRoll::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (oldEvent.isTypeOf(MidiEvent::Type::Note))
    {
        const auto &note = static_cast<const Note &>(oldEvent);
        const auto &newNote = static_cast<const Note &>(newEvent);
        const auto *track = newEvent.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            if (auto *component = sequenceMap[note].release())
            {
                // Pass ownership to another key:
                sequenceMap.erase(note);
                // Hitting this assert means that a track somehow contains events
                // with duplicate id's. This should never, ever happen.
                jassert(!sequenceMap.contains(newNote));
                // Always erase before updating, as it may happen both events have the same hash code:
                sequenceMap[newNote] = UniquePointer<NoteComponent>(component);
                // Schedule to be repainted later:
                this->triggerBatchRepaintFor(component);
            }
        }

        // FIXME someday please: this is a kind of a really nasty hack,
        // and instead the guides bar should subscribe on project changes on its own,
        // and keep track of selected notes and change visible note guides positions,
        // BUT I'm too lazy, and this is also way less code and should work a bit faster,
        // so here we go. Yet this particular line is a piece of shit:
        this->noteNameGuides->syncWithSelection(&this->selection);
    }
    else if (oldEvent.isTypeOf(MidiEvent::Type::KeySignature))
    {
        const auto &oldKey = static_cast<const KeySignatureEvent &>(oldEvent);
        const auto &newKey = static_cast<const KeySignatureEvent &>(newEvent);
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
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

        forEachSequenceMapOfGivenTrack(this->patternMap, c, track)
        {
            auto &sequenceMap = *c.second.get();
            const auto *targetParams = &c.first;
            const int i = track->getPattern()->indexOfSorted(targetParams);
            jassert(i >= 0);
            
            const Clip *realClip = track->getPattern()->getUnchecked(i);
            auto *component = new NoteComponent(*this, note, *realClip);
            sequenceMap[note] = UniquePointer<NoteComponent>(component);
            this->addAndMakeVisible(component);

            this->fader.fadeIn(component, 150);

            // TODO check this in a more elegant way
            // (needed not to break shift+drag note copying)
            const bool isCurrentlyDraggingNote = this->draggingHelper->isVisible();

            const bool isActive = component->belongsTo(this->activeTrack, this->activeClip);
            component->setActive(isActive, true);

            this->triggerBatchRepaintFor(component);

            // arpeggiators preview cannot work without that:
            if (isActive && !isCurrentlyDraggingNote)
            {
                this->selectEvent(component, false);
            }

            if (this->addNewNoteMode && isActive)
            {
                this->newNoteDragging = component;
                this->addNewNoteMode = false;
                this->selectEvent(this->newNoteDragging, true); // clear prev selection
            }
        }
    }
    else if (event.isTypeOf(MidiEvent::Type::KeySignature))
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
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        this->hideDragHelpers();
        this->hideAllGhostNotes(); // Avoids crash

        const Note &note = static_cast<const Note &>(event);
        const auto *track = note.getSequence()->getTrack();

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
    else if (event.isTypeOf(MidiEvent::Type::KeySignature))
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

    auto *sequenceMap = new SequenceMap();
    this->patternMap[clip] = UniquePointer<SequenceMap>(sequenceMap);

    for (const auto &e : *referenceMap)
    {
        // reference the same note as neighbor components:
        const auto &note = e.second.get()->getNote();
        auto *component = new NoteComponent(*this, note, clip);
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
        // the parameters of the clip have changed;
        // keeping track of the active clip changes this way is very ugly,
        // but refactoring to keep a weak reference instead is too bloody;
        // please fixme someday (maybe just keep a raw pointer?)
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
        if (event->isTypeOf(MidiEvent::Type::KeySignature))
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

    this->hideDragHelpers();
    this->hideAllGhostNotes(); // Avoids crash

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const auto *event = track->getSequence()->getUnchecked(i);
        if (event->isTypeOf(MidiEvent::Type::KeySignature))
        {
            const auto &key = static_cast<const KeySignatureEvent &>(*event);
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

void PianoRoll::onChangeProjectInfo(const ProjectMetadata *info)
{
    // note: not calling HybridRoll::onChangeProjectInfo here,
    // because it only updates temperament, and we have a very own logic here
    if (this->temperament != info->getTemperament())
    {
        this->temperament = info->getTemperament();
        this->noteNameGuides->syncWithTemperament(this->temperament);
        this->updateBackgroundCachesAndRepaint();
        this->updateSize(); // might have changed by due to different temperament
        this->updateChildrenPositions();
    }
}

void PianoRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    HybridRoll::onReloadProjectContent(tracks); // updates temperament
    this->noteNameGuides->syncWithTemperament(this->temperament);
    this->reloadRollContent(); // will updateBackgroundCachesAndRepaint
    this->updateSize(); // might have changed by due to different temperament
    this->updateChildrenPositions();
}

void PianoRoll::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->updateActiveRangeIndicator();
    HybridRoll::onChangeProjectBeatRange(firstBeat, lastBeat);
}

void PianoRoll::onChangeViewEditableScope(MidiTrack *const newActiveTrack,
    const Clip &newActiveClip, bool shouldFocus)
{
    this->contextMenuController->cancelIfPending();

    if (!shouldFocus &&
        this->activeClip == newActiveClip &&
        this->activeTrack == newActiveTrack)
    {
        return;
    }

    if (this->lassoComponent->isDragging())
    {
        this->lassoComponent->endLasso();
    }

    this->selection.deselectAll();

    this->activeTrack = newActiveTrack;
    this->activeClip = newActiveClip;

    int focusMinKey = INT_MAX;
    int focusMaxKey = 0;
    float focusMinBeat = FLT_MAX;
    float focusMaxBeat = -FLT_MAX;
    bool hasComponentsToFocusOn = false;

    forEachEventComponent(this->patternMap, e)
    {
        auto *nc = e.second.get();
        const bool isActive = nc->belongsTo(this->activeTrack, this->activeClip);
        const auto key = nc->getKey() + this->activeClip.getKey();
        nc->setActive(isActive, true);

        if (shouldFocus && isActive)
        {
            hasComponentsToFocusOn = true;
            focusMinKey = jmin(focusMinKey, key);
            focusMaxKey = jmax(focusMaxKey, key);
            focusMinBeat = jmin(focusMinBeat, nc->getBeat());
            focusMaxBeat = jmax(focusMaxBeat, nc->getBeat() + nc->getLength());
        }
    }

    this->updateActiveRangeIndicator();

    if (shouldFocus)
    {
        // hardcoded zoom settings for empty tracks:
        if (!hasComponentsToFocusOn)
        {
            focusMinKey = 44;
            focusMaxKey = 84;
            focusMinBeat = 0;
            focusMaxBeat = Globals::Defaults::emptyClipLength;
        }

        this->zoomToArea(focusMinKey, focusMaxKey,
            focusMinBeat + this->activeClip.getBeat(),
            focusMaxBeat + this->activeClip.getBeat());
    }
    else
    {
        this->repaint(this->viewport.getViewArea());
    }

    this->noteNameGuides->toFront(false);
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
        auto *component = e.second.get();
        if (component->isActive() &&
            (component->getNote().getBeat() + component->getClip().getBeat()) >= startBeat &&
            (component->getNote().getBeat() + component->getClip().getBeat()) < endBeat)
        {
            this->selectEvent(component, false);
        }
    }
}

void PianoRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    forEachEventComponent(this->patternMap, e)
    {
        auto *component = e.second.get();
        component->setSelected(false);
    }

    for (auto *component : this->selection)
    {
        component->setSelected(true);
    }
    
    forEachEventComponent(this->patternMap, e)
    {
        auto *component = e.second.get();
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
            component->setSelected(true);
            jassert(!itemsFound.contains(component));
            itemsFound.add(component);
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
        this->showChordTool(e.mods.isAnyModifierKeyDown() ?
            ToolType::ScalePreview : ToolType::ChordPreview, e2.getPosition());
    }
}

void PianoRoll::mouseDrag(const MouseEvent &e)
{
    // can show menus
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->newNoteDragging != nullptr)
    {
        if (this->newNoteDragging->isInEditMode())
        {
            this->newNoteDragging->mouseDrag(e.getEventRelativeTo(this->newNoteDragging));
        }
        else
        {
            const auto noteEvent = e.getEventRelativeTo(this->newNoteDragging);
            const auto editingCursor = this->newNoteDragging->startEditingNewNote(noteEvent);
            this->setMouseCursor(editingCursor);
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
        this->newNoteDragging->mouseUp(e.getEventRelativeTo(this->newNoteDragging));
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
        this->project.setEditableScope(this->activeTrack, this->activeClip, true);
        this->zoomOutImpulse(0.25f); // A bit of fancy animation
        break;
    case CommandIDs::RenameTrack:
        if (auto *trackNode = dynamic_cast<MidiTrackNode *>(this->project.findActiveNode()))
        {
            App::showModalComponent(make<TrackPropertiesDialog>(this->project, trackNode));
        }
        break;
    case CommandIDs::EditCurrentInstrument:
        if (auto *window = PluginWindow::getWindowFor(this->activeTrack->getTrackInstrumentId()))
        {
            window->toFront(true);
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
        const auto playheadPos = this->project.getTransport().getSeekBeat();
        const float playheadBeat = playheadPos - this->activeClip.getBeat();
        SequencerOperations::pasteFromClipboard(App::Clipboard(), this->project, this->getActiveTrack(), playheadBeat);
    }
        break;
    case CommandIDs::DeleteEvents:
        SequencerOperations::deleteSelection(this->getLassoSelection());
        break;
    case CommandIDs::DeleteTrack:
    {
        this->project.checkpoint();
        this->project.removeTrack(*this->activeTrack);
        return;
    }
    case CommandIDs::NewTrackFromSelection:
        if (this->getLassoSelection().getNumSelected() > 0)
        {
            this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::AddNewTrack);
            const auto trackPreset = SequencerOperations::createPianoTrack(this->getLassoSelection());
            
            // false == we already have the correct checkpoint
            SequencerOperations::deleteSelection(this->getLassoSelection(), false);

            this->addTrackInteractively(trackPreset.get(),
                UndoActionIDs::AddNewTrack, true, this->activeTrack->getTrackName(),
                TRANS(I18n::Menu::Selection::notesToTrack), TRANS(I18n::Dialog::addTrackProceed));
        }
        break;
    case CommandIDs::DuplicateTrack:
    {
        this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::AddNewTrack);
        const auto *cloneSource = static_cast<PianoSequence *>(this->activeTrack->getSequence());
        const auto trackPreset = SequencerOperations::createPianoTrack(cloneSource, this->activeClip);
        this->addTrackInteractively(trackPreset.get(),
            UndoActionIDs::AddNewTrack, true, this->activeTrack->getTrackName(),
            TRANS(I18n::Menu::trackDuplicate), TRANS(I18n::Dialog::addTrackProceed));
    }
    break;
    case CommandIDs::BeatShiftLeft:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::BeatShiftRight:
        SequencerOperations::shiftBeatRelative(this->getLassoSelection(), this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::LengthIncrease:
        SequencerOperations::shiftLengthRelative(this->getLassoSelection(), this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::LengthDecrease:
        SequencerOperations::shiftLengthRelative(this->getLassoSelection(), -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::KeyShiftUp:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), 1, true, &this->getTransport());
        break;
    case CommandIDs::KeyShiftDown:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), -1, true, &this->getTransport());
        break;
    case CommandIDs::OctaveShiftUp:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), this->getPeriodSize(), true, &this->getTransport());
        break;
    case CommandIDs::OctaveShiftDown:
        SequencerOperations::shiftKeyRelative(this->getLassoSelection(), -this->getPeriodSize(), true, &this->getTransport());
        break;
    case CommandIDs::CleanupOverlaps:
        SequencerOperations::cleanupOverlaps(this->getLassoSelection());
        break;
    case CommandIDs::MelodicInversion:
        SequencerOperations::melodicInversion(this->getLassoSelection());
        break;
    case CommandIDs::Retrograde:
        SequencerOperations::retrograde(this->getLassoSelection());
        break;
    case CommandIDs::InvertChordUp:
        SequencerOperations::invertChord(this->getLassoSelection(), this->getPeriodSize(), true, &this->getTransport());
        break;
    case CommandIDs::InvertChordDown:
        SequencerOperations::invertChord(this->getLassoSelection(), -this->getPeriodSize(), true, &this->getTransport());
        break;
    case CommandIDs::ToggleMuteClips:
        PatternOperations::toggleMuteClip(this->activeClip);
        break;
    case CommandIDs::ToggleSoloClips:
        PatternOperations::toggleSoloClip(this->activeClip);
        break;
    case CommandIDs::ToggleScalesHighlighting:
        App::Config().getUiFlags()->setScalesHighlightingEnabled(!this->scalesHighlightingEnabled);
        break;
    case CommandIDs::ToggleNoteNameGuides:
        App::Config().getUiFlags()->setNoteNameGuidesEnabled(!this->noteNameGuides->isVisible());
        break;
    case CommandIDs::ToggleLoopOverSelection:
        if (this->selection.getNumSelected() > 0)
        {
            const auto clipOffset = this->activeClip.getBeat();
            const auto startBeat = SequencerOperations::findStartBeat(this->selection);
            const auto endBeat = SequencerOperations::findEndBeat(this->selection);
            this->getTransport().toggleLoopPlayback(clipOffset + startBeat, clipOffset + endBeat);
        }
        else
        {
            jassert(this->activeTrack != nullptr);
            const auto clipOffset = this->activeClip.getBeat();
            const auto startBeat = this->activeTrack->getSequence()->getFirstBeat();
            const auto endBeat = this->activeTrack->getSequence()->getLastBeat();
            this->getTransport().toggleLoopPlayback(clipOffset + startBeat, clipOffset + endBeat);
        }
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
                newArpDialog->onOk = [this, contextScale, contextKey, selectedNotes](const String &name)
                {
                    Arpeggiator::Ptr arp(new Arpeggiator(name,
                        this->temperament, contextScale, selectedNotes, contextKey));

                    App::Config().getArpeggiators()->updateUserResource(arp);
                };

                App::showModalComponent(move(newArpDialog));
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
        if (auto *panel = RescalePreviewTool::createWithinSelectionAndContext(this,
            this->project.getTimeline()->getKeySignatures()))
        {
            HelioCallout::emit(panel, this, true);
        }
        break;
    case CommandIDs::ShowScalePanel:
        this->showChordTool(ToolType::ScalePreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ShowChordPanel:
        this->showChordTool(ToolType::ChordPreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ShowVolumePanel:
        App::Config().getUiFlags()->toggleVelocityMapVisibility();
        // TODO if shift is pressed:
        //if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        //HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
        break;
    case CommandIDs::NotesVolumeRandom:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::randomizeVolume(this->getLassoSelection(), 0.1f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::NotesVolumeFadeOut:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::fadeOutVolume(this->getLassoSelection(), 0.35f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::NotesVolumeUp:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::tuneVolume(this->getLassoSelection(), 1.f / 32.f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::NotesVolumeDown:
        HYBRID_ROLL_BULK_REPAINT_START
        SequencerOperations::tuneVolume(this->getLassoSelection(), -1.f / 32.f);
        HYBRID_ROLL_BULK_REPAINT_END
        break;
    case CommandIDs::Tuplet1:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 1);
        break;
    case CommandIDs::Tuplet2:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 2);
        break;
    case CommandIDs::Tuplet3:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 3);
        break;
    case CommandIDs::Tuplet4:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 4);
        break;
    case CommandIDs::Tuplet5:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 5);
        break;
    case CommandIDs::Tuplet6:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 6);
        break;
    case CommandIDs::Tuplet7:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 7);
        break;
    case CommandIDs::Tuplet8:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 8);
        break;
    case CommandIDs::Tuplet9:
        SequencerOperations::applyTuplets(this->getLassoSelection(), 9);
        break;
    case CommandIDs::QuantizeTo1_1:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 1.f);
        break;
    case CommandIDs::QuantizeTo1_2:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 2.f);
        break;
    case CommandIDs::QuantizeTo1_4:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 4.f);
        break;
    case CommandIDs::QuantizeTo1_8:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 8.f);
        break;
    case CommandIDs::QuantizeTo1_16:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 16.f);
        break;
    case CommandIDs::QuantizeTo1_32:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->getLassoSelection(), 32.f);
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

    static constexpr auto paintOffsetY = HybridRoll::headerHeight;

    int prevBeatX = paintStartX;
    const HighlightingScheme *prevScheme = nullptr;
    const int y = this->viewport.getViewPositionY();
    const int h = this->viewport.getViewHeight();

    const auto periodHeight = this->rowHeight * this->getPeriodSize();
    const auto numPeriodsToSkip = (y - paintOffsetY) / periodHeight;
    const auto paintStartY = paintOffsetY + numPeriodsToSkip * periodHeight;

    //g.setImageResamplingQuality(Graphics::lowResamplingQuality);

    for (int nextKeyIdx = 0; this->scalesHighlightingEnabled && nextKeyIdx < keysSequence->size(); ++nextKeyIdx)
    {
        const auto *key = static_cast<KeySignatureEvent *>(keysSequence->getUnchecked(nextKeyIdx));
        const int beatX = int((key->getBeat() - this->firstBeat)  * this->beatWidth);
        const int index = this->binarySearchForHighlightingScheme(key);

        jassert(index >= 0);

        const auto *s = (prevScheme == nullptr) ? this->backgroundsCache.getUnchecked(index) : prevScheme;
        const auto fillImage = s->getUnchecked(this->rowHeight);

        if (beatX >= paintStartX)
        {
            /*
                You might be thinking: why the do we need this ugly loop here?
                Given a tiled texture, we can just fill it all at once with a single fillRect()?

                Well yes, but actually no.
                OpenGmotherfuckingL does suck real hard, and it can't seem to do
                texture tiling correctly unless the stars are well aligned (they are not),
                so there is always some weird offset, and it's getting weirder at each next tile.

                For horizontal tiling, we don't care, but for vertical tiling, this means that
                sequencer rows are messed up, so we have to say explicitly where to fill each period.
            */

            for (int i = paintStartY; i < y + h; i += periodHeight)
            {
                g.setFillType({ fillImage, AffineTransform::translation(0.f, float(i)) });
                g.fillRect(prevBeatX, i, beatX - prevBeatX, periodHeight);
            }
        }

        if (beatX >= paintEndX)
        {
            HybridRoll::paint(g);
            return;
        }

        prevBeatX = beatX;
        prevScheme = this->backgroundsCache.getUnchecked(index);
    }

    if (prevBeatX < paintEndX)
    {
        const auto *s = (prevScheme == nullptr) ? this->defaultHighlighting.get() : prevScheme;
        const auto fillImage = s->getUnchecked(this->rowHeight);

        // just because we cannot rely on OpenGL tiling:
        for (int i = paintStartY; i < y + h; i += periodHeight)
        {
            g.setFillType({ fillImage, AffineTransform::translation(0.f, float(i)) });
            g.fillRect(prevBeatX, i, paintEndX - prevBeatX, periodHeight);
        }

        HybridRoll::paint(g);
    }
}

void PianoRoll::insertNewNoteAt(const MouseEvent &e)
{
    int draggingRow = 0;
    float draggingColumn = 0.f;
    this->getRowsColsByMousePosition(e.x, e.y, draggingRow, draggingColumn);

    auto *activeSequence = static_cast<PianoSequence *>(this->activeTrack->getSequence());
    activeSequence->checkpoint();

    // thing is, we can't have a pointer to this note's component,
    // since it is not created yet at this point; so we set a flag,
    // and in onAddMidiEvent/1 callback we store that pointer,
    // so that the new note can be dragged, or resized, or whatever
    this->addNewNoteMode = true;
    activeSequence->insert(Note(activeSequence, draggingRow, draggingColumn,
        this->newNoteLength, this->newNoteVolume), true);
}

void PianoRoll::startCuttingEvents(const MouseEvent &e)
{
    if (this->knifeToolHelper == nullptr)
    {
        this->deselectAll();
        this->knifeToolHelper.reset(new KnifeToolHelper(*this));
        this->addAndMakeVisible(this->knifeToolHelper.get());
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
                    const float relativeCutBeat = this->getRoundBeatSnapByXPosition(int(intersection.getX()))
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
        this->noteResizerLeft.reset(new NoteResizerLeft(*this));
        this->addAndMakeVisible(this->noteResizerLeft.get());
    }

    if (this->selection.getNumSelected() > 0 &&
        this->noteResizerRight == nullptr)
    {
        this->noteResizerRight.reset(new NoteResizerRight(*this));
        this->addAndMakeVisible(this->noteResizerRight.get());
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

    if (this->noteNameGuides->isVisible())
    {
        this->noteNameGuides->updateBounds();
    }

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

    if (this->noteNameGuides->isVisible())
    {
        this->noteNameGuides->updatePosition();
    }

    HybridRoll::updateChildrenPositions();
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> PianoRoll::getCommandPaletteActionProviders() const
{
    if (this->getPeriodSize() == Globals::twelveTonePeriodSize)
    {
        // the chord constructor will only work for 12-EDO:
        return { this->consoleChordConstructor.get() };
    }

    return {};
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData PianoRoll::serialize() const
{
    using namespace Serialization;
    SerializedData tree(UI::pianoRoll);
    
    tree.setProperty(UI::beatWidth, roundf(this->beatWidth));
    tree.setProperty(UI::rowHeight, this->getRowHeight());

    tree.setProperty(UI::startBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX())));

    tree.setProperty(UI::endBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX() +
            this->getViewport().getViewWidth())));

    tree.setProperty(UI::viewportPositionY, this->getViewport().getViewPositionY());

    // m?
    //tree.setProperty(UI::selection, this->getLassoSelection().serialize());

    return tree;
}

void PianoRoll::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    const auto root = data.hasType(UI::pianoRoll) ?
        data : data.getChildWithName(UI::pianoRoll);

    if (!root.isValid())
    {
        return;
    }
    
    this->setBeatWidth(float(root.getProperty(UI::beatWidth, this->beatWidth)));
    this->setRowHeight(root.getProperty(UI::rowHeight, this->getRowHeight()));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBeat = float(root.getProperty(UI::startBeat, 0.f));
    const int x = this->getXPositionByBeat(startBeat);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PianoRoll::reset() {}

//===----------------------------------------------------------------------===//
// Background pattern images cache
//===----------------------------------------------------------------------===//

void PianoRoll::updateBackgroundCachesAndRepaint()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const auto highlightingScale = App::Config().getTemperaments()->findHighlightingFor(this->temperament);
    this->defaultHighlighting = make<HighlightingScheme>(0, highlightingScale);
    this->defaultHighlighting->renderBackgroundCache(this->temperament);

    this->backgroundsCache.clear();

    for (const auto *track : this->project.getTracks())
    {
        // Re-render backgrounds for all key signatures:
        for (int i = 0; i < track->getSequence()->size(); ++i)
        {
            const auto *event = track->getSequence()->getUnchecked(i);
            if (event->isTypeOf(MidiEvent::Type::KeySignature))
            {
                const auto &key = static_cast<const KeySignatureEvent &>(*event);
                this->updateBackgroundCacheFor(key);
            }
        }
    }

    this->repaint(this->viewport.getViewArea());

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::updateBackgroundCacheFor(const KeySignatureEvent &key)
{
    int duplicateSchemeIndex = this->binarySearchForHighlightingScheme(&key);
    if (duplicateSchemeIndex < 0)
    {
        auto scheme = make<HighlightingScheme>(key.getRootKey(), key.getScale());
        scheme->renderBackgroundCache(this->temperament);
        this->backgroundsCache.addSorted(*this->defaultHighlighting, scheme.release());
    }
}

void PianoRoll::removeBackgroundCacheFor(const KeySignatureEvent &key)
{
    const auto keySignatures = this->project.getTimeline()->getKeySignatures()->getSequence();
    for (int i = 0; i < keySignatures->size(); ++i)
    {
        const auto *k = static_cast<KeySignatureEvent *>(keySignatures->getUnchecked(i));
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

    jassert(index >= 0);
}

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
    case ToolType::ScalePreview:
        if (pianoSequence != nullptr)
        {
            auto *popup = new ScalePreviewTool(this, pianoSequence);
            popup->setTopLeftPosition(position - Point<int>(popup->getWidth(), popup->getHeight()) / 2);
            App::Layout().addAndMakeVisible(popup);
        }
        break;
    case ToolType::ChordPreview:
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

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void PianoRoll::onScalesHighlightingFlagChanged(bool enabled)
{
    this->scalesHighlightingEnabled = enabled;
    this->repaint();
}

void PianoRoll::onNoteNameGuidesFlagChanged(bool enabled)
{
    this->noteNameGuides->setVisible(enabled);
    this->noteNameGuides->toFront(false);
    this->updateChildrenBounds();
    this->repaint();
}
