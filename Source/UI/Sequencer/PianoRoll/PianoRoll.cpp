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
#include "PianoSequence.h"
#include "KeySignaturesSequence.h"
#include "PianoTrackNode.h"
#include "ModalDialogInput.h"
#include "TrackPropertiesDialog.h"
#include "TimeSignatureDialog.h"
#include "ProjectTimeline.h"
#include "ProjectMetadata.h"
#include "Note.h"
#include "NoteComponent.h"
#include "NoteNameGuidesBar.h"
#include "NotesTuningPanel.h"
#include "HelperRectangle.h"
#include "RollHeader.h"
#include "KnifeToolHelper.h"
#include "MergingEventsConnector.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "SelectionComponent.h"
#include "ModalCallout.h"
#include "RescalePreviewTool.h"
#include "ChordPreviewTool.h"
#include "ScalePreviewTool.h"
#include "ArpPreviewTool.h"
#include "SequencerOperations.h"
#include "PatternOperations.h"
#include "InteractiveActions.h"
#include "SerializationKeys.h"
#include "Arpeggiator.h"
#include "CommandPaletteChordConstructor.h"
#include "CommandPaletteMoveNotesMenu.h"
#include "LassoListeners.h"
#include "UndoStack.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "ComponentIDs.h"
#include "Config.h"

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

#if PLATFORM_DESKTOP
#   define PIANOROLL_HAS_NOTE_RESIZERS 0
#elif PLATFORM_MOBILE
#   define PIANOROLL_HAS_NOTE_RESIZERS 1
#endif

PianoRoll::PianoRoll(ProjectNode &project, Viewport &viewport, WeakReference<AudioMonitor> clippingDetector) :
    RollBase(project, viewport, clippingDetector)
{
    this->setComponentID(ComponentIDs::pianoRollId);

    this->selectionListeners.add(new PianoRollSelectionMenuManager(&this->selection, this->project));
    this->selectionListeners.add(new PianoRollSelectionRangeIndicatorController(&this->selection, *this));

    this->draggingHelper = make<HelperRectangleHorizontal>();
    this->addChildComponent(this->draggingHelper.get());

    this->consoleMoveNotesMenu = make<CommandPaletteMoveNotesMenu>(*this, this->project);
    this->consoleChordConstructor = make<CommandPaletteChordConstructor>(*this);

    const auto *uiFlags = App::Config().getUiFlags();
    this->scalesHighlightingEnabled = uiFlags->isScalesHighlightingEnabled();
    const bool noteNameGuidesEnabled = uiFlags->isNoteNameGuidesEnabled();

    this->noteNameGuides = make<NoteNameGuidesBar>(*this);
    this->addChildComponent(this->noteNameGuides.get());
    this->noteNameGuides->setVisible(noteNameGuidesEnabled);
}

PianoRoll::~PianoRoll() = default;

void PianoRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->patternMap.clear();

    ROLL_BATCH_REPAINT_START

    for (const auto *track : this->project.getTracks())
    {
        this->loadTrack(track);
    }

    this->updateBackgroundCachesAndRepaint();
    this->applyEditModeUpdates();

    ROLL_BATCH_REPAINT_END
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

void PianoRoll::updateClipRangeIndicator() const
{
    if (this->activeTrack != nullptr)
    {
        const auto clipBeat = this->activeClip.getBeat();
        const auto *sequence = this->activeTrack->getSequence();
        const auto firstBeat = sequence->getFirstBeat();
        const auto lastBeat = sequence->isEmpty() ?
            firstBeat + Globals::Defaults::emptyClipLength :
            sequence->getLastBeat();

        this->header->updateClipRangeIndicator(this->activeTrack->getTrackColour(),
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
    this->setSize(this->getWidth(),
        Globals::UI::rollHeaderHeight + this->getNumKeys() * this->rowHeight);
}

//===----------------------------------------------------------------------===//
// RollBase
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

float PianoRoll::findNextAnchorBeat(float beat) const
{
    const auto nearestTimelineAnchor =
        this->project.getTimeline()->findNextAnchorBeat(beat);

    const auto activeTrackStart = this->activeClip.getBeat() +
        this->activeTrack->getSequence()->getFirstBeat();

    if (activeTrackStart > beat)
    {
        return jmin(nearestTimelineAnchor, activeTrackStart);
    }

    return nearestTimelineAnchor;
}

float PianoRoll::findPreviousAnchorBeat(float beat) const
{
    const auto nearestTimelineAnchor =
        this->project.getTimeline()->findPreviousAnchorBeat(beat);

    const auto activeTrackStart = this->activeClip.getBeat() +
        this->activeTrack->getSequence()->getFirstBeat();

    if (activeTrackStart < beat)
    {
        return jmax(nearestTimelineAnchor, activeTrackStart);
    }

    return nearestTimelineAnchor;
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
        this->fader.fadeOut(this->ghostNotes.getUnchecked(i), Globals::UI::fadeOutShort);
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
        !this->getEditMode().forbidsSelectionMode({}))
    {
        const auto *nc = dynamic_cast<NoteComponent *>(target.get());
        if (nc != nullptr && !nc->isActive())
        {
            this->project.setEditableScope(nc->getClip(), false);
            return;
        }
    }

    // else - start dragging lasso, if needed:
    RollBase::longTapEvent(position, target);
}

void PianoRoll::zoomRelative(const Point<float> &origin,
    const Point<float> &factor, bool isInertial)
{
    static const float yZoomThreshold = 0.035f;

    if (fabs(factor.getY()) > yZoomThreshold)
    {
        const auto oldViewPosition = this->viewport.getViewPosition().toFloat();
        const auto absoluteOrigin = oldViewPosition + origin;
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
        const float newViewPositionY = (absoluteOrigin.getY() * newHeight / oldHeight) - origin.getY();
        this->viewport.setViewPosition(int(oldViewPosition.getX()), int(newViewPositionY + 0.5f));
    }

    RollBase::zoomRelative(origin, factor, isInertial);
}

void PianoRoll::zoomAbsolute(const Rectangle<float> &proportion)
{
    jassert(!proportion.isEmpty());
    jassert(proportion.isFinite());

    const auto keysTotal = this->getNumKeys();
    const float heightToFit = float(this->viewport.getViewHeight());
    const auto numKeysToFit = jmax(1.f, float(keysTotal) * proportion.getHeight());
    this->setRowHeight(int(heightToFit / numKeysToFit));

    const auto firstKey = int(float(keysTotal) * proportion.getY());
    const int firstKeyY = this->getRowHeight() * firstKey;
    this->viewport.setViewPosition(this->viewport.getViewPositionY() -
        Globals::UI::rollHeaderHeight, firstKeyY);

    RollBase::zoomAbsolute(proportion);
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
    this->viewport.setViewPosition(this->viewport.getViewPositionY() -
        Globals::UI::rollHeaderHeight, maxKeyY);

    RollBase::zoomToArea(minBeat, maxBeat);
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
    beatNumber = this->getRoundBeatSnapByXPosition(int(x)) - this->activeClip.getBeat();
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
        this->fader.fadeOut(this->draggingHelper.get(), Globals::UI::fadeOutShort);
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

    RollBase::onChangeMidiEvent(oldEvent, newEvent);
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

            this->fader.fadeIn(component, Globals::UI::fadeInLong);

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
        const auto &key = static_cast<const KeySignatureEvent &>(event);
        this->updateBackgroundCacheFor(key);
        this->repaint();
    }

    RollBase::onAddMidiEvent(event);
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
                this->fader.fadeOut(deletedComponent, Globals::UI::fadeOutLong);
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

    RollBase::onRemoveMidiEvent(event);
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
            this->updateClipRangeIndicator();
        }

        // Schedule batch repaint
        this->triggerAsyncUpdate();
    }

    RollBase::onChangeClip(clip, newClip);
}

void PianoRoll::onRemoveClip(const Clip &clip)
{
    ROLL_BATCH_REPAINT_START

    if (this->patternMap.contains(clip))
    {
        this->patternMap.erase(clip);
    }

    ROLL_BATCH_REPAINT_END
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

        this->updateClipRangeIndicator(); // colour might have changed
        this->repaint();
    }
}

void PianoRoll::onChangeTrackBeatRange(MidiTrack *const track)
{
    if (track == this->activeTrack)
    {
        this->updateClipRangeIndicator();
    }
}

void PianoRoll::onAddTrack(MidiTrack *const track)
{
    ROLL_BATCH_REPAINT_START

    this->loadTrack(track);
    this->applyEditModeUpdates();

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

    ROLL_BATCH_REPAINT_END
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
    // note: not calling RollBase::onChangeProjectInfo here,
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

void PianoRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks, const ProjectMetadata *meta)
{
    RollBase::onReloadProjectContent(tracks, meta); // updates temperament
    this->noteNameGuides->syncWithTemperament(this->temperament);
    this->reloadRollContent(); // will updateBackgroundCachesAndRepaint
    this->updateSize(); // might have changed by due to different temperament
    this->updateChildrenPositions();

    // if this happens to be a new project, focus somewhere in the centre:
    if (this->getViewport().getViewPositionY() == 0)
    {
        const int defaultY = (this->getHeight() / 2 - this->getViewport().getViewHeight() / 2);
        this->getViewport().setViewPosition(this->getViewport().getViewPositionX(), defaultY);
    }
}

void PianoRoll::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->updateClipRangeIndicator();
    RollBase::onChangeProjectBeatRange(firstBeat, lastBeat);
}

void PianoRoll::onChangeViewEditableScope(MidiTrack *const newActiveTrack,
    const Clip &newActiveClip, bool shouldFocus)
{
    this->contextMenuController->cancelIfPending();

    this->project.getTimeline()->getTimeSignaturesAggregator()->setActiveScope({newActiveTrack});

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

    this->updateClipRangeIndicator();

    if (shouldFocus)
    {
        if (!hasComponentsToFocusOn)
        {
            // zoom settings for empty tracks:
            const auto middleC = this->getTemperament()->getMiddleC();
            const auto periodSize = this->getTemperament()->getPeriodSize();
            focusMinKey = middleC - periodSize * 2;
            focusMaxKey = middleC + periodSize * 2;
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
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
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
            this->startCuttingEvents(e.position);
        }
    }

    RollBase::mouseDown(e);
}

void PianoRoll::mouseDoubleClick(const MouseEvent &e)
{
    if (!this->project.getEditMode().forbidsAddingEvents({}))
    {
        const auto e2 = e.getEventRelativeTo(&App::Layout());
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
        this->continueCuttingEvents(e.position);
    }

    RollBase::mouseDrag(e);
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
        RollBase::mouseUp(e);
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
        this->project.setEditableScope(this->activeClip, true);
        this->zoomOutImpulse(0.35f);
        break;
    case CommandIDs::SwitchToClipInViewport:
        this->switchToClipInViewport();
        break;
    case CommandIDs::RenameTrack:
        jassert(this->activeTrack != nullptr);
        App::showModalComponent(make<TrackPropertiesDialog>(this->project, this->activeTrack));
        break;
    case CommandIDs::SetTrackTimeSignature:
        jassert(dynamic_cast<PianoTrackNode *>(this->activeTrack.get()));
        App::showModalComponent(TimeSignatureDialog::editingDialog(*this,
            this->project, *this->activeTrack->getTimeSignatureOverride()));
        break;
    case CommandIDs::EditCurrentInstrument:
        PluginWindow::showWindowFor(this->activeTrack->getTrackInstrumentId());
        break;
    case CommandIDs::CopyEvents:
        SequencerOperations::copyToClipboard(App::Clipboard(), this->selection);
        break;
    case CommandIDs::CutEvents:
        SequencerOperations::copyToClipboard(App::Clipboard(), this->selection);
        SequencerOperations::deleteSelection(this->selection);
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
        SequencerOperations::deleteSelection(this->selection);
        break;
    case CommandIDs::DeleteTrack:
    {
        this->project.checkpoint();
        this->project.removeTrack(*this->activeTrack);
        return;
    }
    case CommandIDs::NewTrackFromSelection:
        if (this->selection.getNumSelected() > 0)
        {
            this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::AddNewTrack);
            auto trackPreset = SequencerOperations::createPianoTrack(this->selection);
            // false == we already have the correct checkpoint:
            SequencerOperations::deleteSelection(this->selection, false);
            InteractiveActions::addNewTrack(this->project,
                move(trackPreset), this->activeTrack->getTrackName(), true,
                UndoActionIDs::AddNewTrack, TRANS(I18n::Menu::Selection::notesToTrack),
                true);
        }
        break;
    case CommandIDs::DuplicateTrack:
    {
        this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::DuplicateTrack);
        const auto *cloneSource = static_cast<PianoSequence *>(this->activeTrack->getSequence());
        auto trackPreset = SequencerOperations::createPianoTrack(cloneSource, this->activeClip);
        InteractiveActions::addNewTrack(this->project,
            move(trackPreset), this->activeTrack->getTrackName(), true,
            UndoActionIDs::DuplicateTrack, TRANS(I18n::Menu::trackDuplicate),
            true);
    }
    break;
    case CommandIDs::BeatShiftLeft:
        SequencerOperations::shiftBeatRelative(this->selection,
            -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::BeatShiftRight:
        SequencerOperations::shiftBeatRelative(this->selection,
            this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::LengthIncrease:
        SequencerOperations::shiftLengthRelative(this->selection,
            this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::LengthDecrease:
        SequencerOperations::shiftLengthRelative(this->selection,
            -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::TransposeUp:
        SequencerOperations::shiftKeyRelative(this->selection, 1, &this->getTransport(), true);
        break;
    case CommandIDs::TransposeDown:
        SequencerOperations::shiftKeyRelative(this->selection, -1, &this->getTransport(), true);
        break;
    case CommandIDs::TransposeScaleKeyUp:
        // these two commands were supposed to mean in-scale transposition,
        // i.e. using scale(s) from the timeline, but when the scales highlighting flag is off,
        // they feel misleading, so let's make them work as "transposition using highlighted rows":
        SequencerOperations::shiftInScaleKeyRelative(this->selection,
            this->scalesHighlightingEnabled ? this->project.getTimeline()->getKeySignatures() : nullptr,
            this->temperament->getHighlighting(),
            1, &this->getTransport(), true);
        break;
    case CommandIDs::TransposeScaleKeyDown:
        SequencerOperations::shiftInScaleKeyRelative(this->selection,
            this->scalesHighlightingEnabled ? this->project.getTimeline()->getKeySignatures() : nullptr,
            this->temperament->getHighlighting(),
            -1, &this->getTransport(), true);
        break;
    case CommandIDs::TransposeOctaveUp:
        SequencerOperations::shiftKeyRelative(this->selection,
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave),
            &this->getTransport(), true);
        break;
    case CommandIDs::TransposeOctaveDown:
        SequencerOperations::shiftKeyRelative(this->selection,
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave),
            &this->getTransport(), true);
        break;
    case CommandIDs::TransposeFifthUp:
        SequencerOperations::shiftKeyRelative(this->selection,
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth),
            &this->getTransport(), true);
        break;
    case CommandIDs::TransposeFifthDown:
        SequencerOperations::shiftKeyRelative(this->selection,
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth),
            &this->getTransport(), true);
        break;
    case CommandIDs::MakeStaccato:
        DBG("making notes staccato");
        SequencerOperations::makeStaccato(this->selection, 0.125f, true);
        break;
    case CommandIDs::MakeStaccatissimo:
        DBG("making notes staccatissimo");
        SequencerOperations::makeStaccato(this->selection, 0.0625f, true);
        break;
    case CommandIDs::MakeLegato:
        DBG("making selection legato");
        SequencerOperations::makeLegato(this->getLassoSelection(), 0.0f);
        break;
    case CommandIDs::MakeLegatoOverlapping:
        DBG("making selection legato (with overlaps)");
        SequencerOperations::makeLegato(this->getLassoSelection(), 0.0625f); //16'th note is the overlap amount.
        break;
    case CommandIDs::CleanupOverlaps:
        SequencerOperations::cleanupOverlaps(this->selection);
        break;
    case CommandIDs::MelodicInversion:
        SequencerOperations::melodicInversion(this->selection);
        break;
    case CommandIDs::Retrograde:
        SequencerOperations::retrograde(this->selection);
        break;
    case CommandIDs::InvertChordUp:
        SequencerOperations::invertChord(this->selection,
            this->getPeriodSize(), true, &this->getTransport());
        break;
    case CommandIDs::InvertChordDown:
        SequencerOperations::invertChord(this->selection,
            -this->getPeriodSize(), true, &this->getTransport());
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
            this->getTransport().togglePlaybackLoop(clipOffset + startBeat, clipOffset + endBeat);
        }
        else
        {
            jassert(this->activeTrack != nullptr);
            const auto clipOffset = this->activeClip.getBeat();
            const auto *sequence = this->activeTrack->getSequence();
            const auto startBeat = sequence->getFirstBeat();
            const auto endBeat = sequence->isEmpty() ?
                startBeat + Globals::Defaults::emptyClipLength :
                sequence->getLastBeat();

            this->getTransport().togglePlaybackLoop(clipOffset + startBeat, clipOffset + endBeat);
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
            ModalCallout::emit(panel, this, true);
        }
        break;
    case CommandIDs::ShowRescalePanel:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        if (auto *panel = RescalePreviewTool::createWithinSelectionAndContext(this,
            this->project.getTimeline()->getKeySignatures()))
        {
            ModalCallout::emit(panel, this, true);
        }
        break;
    case CommandIDs::ShowScalePanel:
        this->showChordTool(ToolType::ScalePreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ShowChordPanel:
        this->showChordTool(ToolType::ChordPreview, this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ToggleVolumePanel:
        if (Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown())
        {
            // alternative mode:
            if (this->selection.getNumSelected() == 0) { this->selectAll(); }
            ModalCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
        }
        else
        {
            App::Config().getUiFlags()->toggleVelocityMapVisibility();
        }
        break;
    case CommandIDs::NotesVolumeRandom:
        ROLL_BATCH_REPAINT_START
        SequencerOperations::randomizeVolume(this->selection, 0.1f);
        ROLL_BATCH_REPAINT_END
        break;
    case CommandIDs::NotesVolumeFadeOut:
        ROLL_BATCH_REPAINT_START
        SequencerOperations::fadeOutVolume(this->selection, 0.35f);
        ROLL_BATCH_REPAINT_END
        break;
    case CommandIDs::NotesVolumeUp:
        if (this->selection.getNumSelected() > 0)
        {
            ROLL_BATCH_REPAINT_START
            SequencerOperations::tuneVolume(this->selection, 1.f / 32.f);
            this->setDefaultNoteVolume(this->selection.getFirstAs<NoteComponent>()->getVelocity());
            ROLL_BATCH_REPAINT_END
        }
        break;
    case CommandIDs::NotesVolumeDown:
        if (this->selection.getNumSelected() > 0)
        {
            ROLL_BATCH_REPAINT_START
            SequencerOperations::tuneVolume(this->selection, -1.f / 32.f);
            this->setDefaultNoteVolume(this->selection.getFirstAs<NoteComponent>()->getVelocity());
            ROLL_BATCH_REPAINT_END
        }
        break;
    case CommandIDs::Tuplet1:
        SequencerOperations::applyTuplets(this->selection, 1);
        break;
    case CommandIDs::Tuplet2:
        SequencerOperations::applyTuplets(this->selection, 2);
        break;
    case CommandIDs::Tuplet3:
        SequencerOperations::applyTuplets(this->selection, 3);
        break;
    case CommandIDs::Tuplet4:
        SequencerOperations::applyTuplets(this->selection, 4);
        break;
    case CommandIDs::Tuplet5:
        SequencerOperations::applyTuplets(this->selection, 5);
        break;
    case CommandIDs::Tuplet6:
        SequencerOperations::applyTuplets(this->selection, 6);
        break;
    case CommandIDs::Tuplet7:
        SequencerOperations::applyTuplets(this->selection, 7);
        break;
    case CommandIDs::Tuplet8:
        SequencerOperations::applyTuplets(this->selection, 8);
        break;
    case CommandIDs::Tuplet9:
        SequencerOperations::applyTuplets(this->selection, 9);
        break;
    case CommandIDs::QuantizeTo1_1:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 1.f);
        break;
    case CommandIDs::QuantizeTo1_2:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 2.f);
        break;
    case CommandIDs::QuantizeTo1_4:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 4.f);
        break;
    case CommandIDs::QuantizeTo1_8:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 8.f);
        break;
    case CommandIDs::QuantizeTo1_16:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 16.f);
        break;
    case CommandIDs::QuantizeTo1_32:
        if (this->selection.getNumSelected() == 0) { this->selectAll(); }
        SequencerOperations::quantize(this->selection, 32.f);
        break;
    default:
        break;
    }

    RollBase::handleCommandMessage(commandId);
}

void PianoRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    ROLL_BATCH_REPAINT_START

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

    RollBase::resized();

    ROLL_BATCH_REPAINT_END
}

void PianoRoll::paint(Graphics &g)
{
    jassert(this->defaultHighlighting != nullptr); // trying to paint before the content is ready

    const auto *keysSequence = this->project.getTimeline()->getKeySignatures()->getSequence();
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = paintStartX + this->viewport.getViewWidth();

    static constexpr auto paintOffsetY = Globals::UI::rollHeaderHeight;

    int prevBeatX = paintStartX;
    const HighlightingScheme *prevScheme = nullptr;
    const int y = this->viewport.getViewPositionY();
    const int h = this->viewport.getViewHeight();

    const auto periodHeight = this->rowHeight * this->getPeriodSize();
    const auto numPeriodsToSkip = (y - paintOffsetY) / periodHeight;
    const auto paintStartY = paintOffsetY + numPeriodsToSkip * periodHeight;

    g.setImageResamplingQuality(Graphics::lowResamplingQuality);

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

                Well yes, but actually no, OpenGL can't seem to do texture tiling correctly
                unless the stars are well aligned (they are not), so there is always some weird offset,
                and it's getting weirder at each next tile.

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
            RollBase::paint(g);
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

        RollBase::paint(g);
    }
}

void PianoRoll::insertNewNoteAt(const MouseEvent &e)
{
    int key = 0;
    float beat = 0.f;
    int xOffset = 5;
    this->getRowsColsByMousePosition(e.x + xOffset, e.y, key, beat);    //Pretend the mouse is a little to the right of where it actually is. Boosts accuracy when placing notes - RPM

    auto *activeSequence = static_cast<PianoSequence *>(this->activeTrack->getSequence());
    activeSequence->checkpoint();

    // thing is, we can't have a pointer to this note's component,
    // since it is not created yet at this point; so we set a flag,
    // and in onAddMidiEvent/1 callback we store that pointer,
    // so that the new note can be dragged, or resized, or whatever
    this->addNewNoteMode = true;
    activeSequence->insert(Note(activeSequence, key, beat,
        this->newNoteLength, this->newNoteVolume), true);

    this->getTransport().previewKey(activeSequence->getTrackId(),
        key + this->activeClip.getKey(),
        this->newNoteVolume * this->activeClip.getVelocity(),
        this->newNoteLength);
}

void PianoRoll::switchToClipInViewport() const
{
    const auto fullArea = this->viewport.getViewArea();
    // we'll try to be smart about what clip to switch to,
    // and favor clips with notes in the centre of the viewport;
    const auto centreArea = fullArea.reduced(fullArea.getWidth() / 4, fullArea.getHeight() / 4);

    FlatHashMap<Clip, int, ClipHash> visibilityWeights;

    forEachEventComponent(this->patternMap, e)
    {
        auto *nc = e.second.get();
        if (nc->getBounds().intersects(centreArea))
        {
            visibilityWeights[nc->getClip()] += 4;
        }
        else if (nc->getBounds().intersects(fullArea))
        {
            visibilityWeights[nc->getClip()] += 1;
        }
    }

    Clip clipToFocus;
    int maxWeight = 0;
    for (const auto &it : visibilityWeights)
    {
        if (maxWeight < it.second)
        {
            maxWeight = it.second;
            clipToFocus = it.first;
        }
    }

    if (clipToFocus.isValid())
    {
        //DBG("Switching to " + String(clipToFocus.getId()));
        this->project.setEditableScope(clipToFocus, false);
    }
}

//===----------------------------------------------------------------------===//
// Erasing mode
//===----------------------------------------------------------------------===//

// so what's happening in these three methods and why is it so ugly:

// we want to be able to delete multiple events with rclick-&-drag,
// when the first mouseDown can happen either at the canvas of the roll,
// or at any note component, in which case we cannot just start deleting
// the components, because once the component which has been clicked at is deleted,
// the corresponding mouseDrag and mouseUp events will not be sent to anyone,
// which makes sense, but breaks our click-&-drag event erasing scheme;
// to workaround this, we "fake" deletions, i.e. just hide the notes/clips
// to delete them all at once on mouse up; in case th first mouseDown came at
// note/clip component, it will "pass" mouseDrag and mouseUp events to the roll

void PianoRoll::startErasingEvents(const Point<float> &mousePosition)
{
    // just in case:
    this->notesToEraseOnMouseUp.clearQuick();
    // if we are already pointing at a note:
    this->continueErasingEvents(mousePosition);
}

void PianoRoll::continueErasingEvents(const Point<float> &mousePosition)
{
    forEachEventComponent(this->patternMap, it)
    {
        auto *nc = it.second.get();
        if (!nc->isActive() || !nc->isVisible())
        {
            continue;
        }

        if (!nc->getBounds().contains(mousePosition.toInt()))
        {
            continue;
        }

        // duplicates the behavior in onRemoveMidiEvent
        this->fader.fadeOut(nc, Globals::UI::fadeOutLong);
        this->selection.deselect(nc);
        // but sets invisible instead of removing
        nc->setVisible(false);

        this->notesToEraseOnMouseUp.add(nc->getNote());
    }
}

void PianoRoll::endErasingEvents()
{
    if (this->notesToEraseOnMouseUp.isEmpty())
    {
        return;
    }

    this->project.checkpoint();
    auto *sequence = static_cast<PianoSequence *>(this->notesToEraseOnMouseUp.getFirst().getSequence());
    sequence->removeGroup(this->notesToEraseOnMouseUp, true);
    this->notesToEraseOnMouseUp.clearQuick();
}

//===----------------------------------------------------------------------===//
// Knife mode
//===----------------------------------------------------------------------===//

void PianoRoll::startCuttingEvents(const Point<float> &mousePosition)
{
    if (this->knifeToolHelper == nullptr)
    {
        this->deselectAll();
        this->knifeToolHelper = make<KnifeToolHelper>(*this);
        this->addAndMakeVisible(this->knifeToolHelper.get());
        this->knifeToolHelper->toBack();
        this->knifeToolHelper->fadeIn();
    }

    this->knifeToolHelper->setStartPosition(mousePosition);
    this->knifeToolHelper->setEndPosition(mousePosition);
}

void PianoRoll::continueCuttingEvents(const Point<float> &mousePosition)
{
    if (this->knifeToolHelper != nullptr)
    {
        this->knifeToolHelper->setEndPosition(mousePosition);
        this->knifeToolHelper->updateBounds();

        bool addsPoint;
        Point<float> intersection;
        forEachEventComponent(this->patternMap, e)
        {
            addsPoint = false;
            auto *nc = e.second.get();
            if (!nc->isActive())
            {
                continue;
            }

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

void PianoRoll::endCuttingEventsIfNeeded()
{
    if (this->knifeToolHelper != nullptr)
    {
        Array<Note> notes;
        Array<float> beats;
        this->knifeToolHelper->getCutPoints(notes, beats);
        Array<Note> cutEventsToTheRight = SequencerOperations::cutNotes(notes, beats);
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

        this->knifeToolHelper = nullptr;
    }
}

//===----------------------------------------------------------------------===//
// Merge notes mode
//===----------------------------------------------------------------------===//

void PianoRoll::startMergingEvents(const Point<float> &mousePosition)
{
    this->deselectAll();

    NoteComponent *targetNote = nullptr;
    forEachEventComponent(this->patternMap, e)
    {
        auto *nc = e.second.get();
        if (nc->isActive() &&
            nc->getBounds().contains(mousePosition.toInt()))
        {
            targetNote = nc;
        }
    }

    if (this->mergeToolHelper == nullptr && targetNote != nullptr)
    {
        const auto position = mousePosition / this->getLocalBounds().getBottomRight().toFloat();
        this->mergeToolHelper = make<MergingNotesConnector>(targetNote, position);
        this->addAndMakeVisible(this->mergeToolHelper.get());
    }
}

void PianoRoll::continueMergingEvents(const Point<float> &mousePosition)
{
    if (this->mergeToolHelper == nullptr)
    {
        return;
    }

    NoteComponent *targetNote = nullptr;
    forEachEventComponent(this->patternMap, e)
    {
        auto *nc = e.second.get();
        if (nc->isActive() &&
            nc->getBounds().contains(mousePosition.toInt()) &&
            this->mergeToolHelper->canMergeInto(nc))
        {
            targetNote = nc;
        }
    }

    const auto position = mousePosition / this->getLocalBounds().getBottomRight().toFloat();
    this->mergeToolHelper->setTargetComponent(targetNote);
    this->mergeToolHelper->setEndPosition(position);
}

void PianoRoll::endMergingEvents()
{
    if (this->mergeToolHelper == nullptr)
    {
        return;
    }

    const auto *sourceNC = dynamic_cast<NoteComponent *>(this->mergeToolHelper->getSourceComponent());
    const auto *targetNC = dynamic_cast<NoteComponent *>(this->mergeToolHelper->getTargetComponent());
    this->mergeToolHelper = nullptr;

    if (sourceNC == nullptr || targetNC == nullptr)
    {
        return;
    }

    SequencerOperations::mergeNotes(targetNC->getNote(), sourceNC->getNote(), true);
    this->deselectAll();
}

//===----------------------------------------------------------------------===//
// RollBase
//===----------------------------------------------------------------------===//

void PianoRoll::handleAsyncUpdate()
{
#if PIANOROLL_HAS_NOTE_RESIZERS
    // resizers for the mobile version
    if (this->selection.getNumSelected() > 0 &&
        this->noteResizerLeft == nullptr)
    {
        this->noteResizerLeft = make<NoteResizerLeft>(*this);
        this->addAndMakeVisible(this->noteResizerLeft.get());
    }

    if (this->selection.getNumSelected() > 0 &&
        this->noteResizerRight == nullptr)
    {
        this->noteResizerRight = make<NoteResizerRight>(*this);
        this->addAndMakeVisible(this->noteResizerRight.get());
    }

    if (this->selection.getNumSelected() == 0)
    {
        this->noteResizerLeft = nullptr;
        this->noteResizerRight = nullptr;
    }

    if (this->batchRepaintList.size() > 0)
    {
        ROLL_BATCH_REPAINT_START

        if (this->noteResizerLeft != nullptr)
        {
            this->noteResizerLeft->updateBounds();
        }

        if (this->noteResizerRight != nullptr)
        {
            this->noteResizerRight->updateBounds();
        }

        ROLL_BATCH_REPAINT_END
    }
#endif

    RollBase::handleAsyncUpdate();
}

void PianoRoll::changeListenerCallback(ChangeBroadcaster *source)
{
    this->endCuttingEventsIfNeeded();
    RollBase::changeListenerCallback(source);
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

    RollBase::updateChildrenBounds();
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

    RollBase::updateChildrenPositions();
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> PianoRoll::getCommandPaletteActionProviders() const
{
    Array<CommandPaletteActionsProvider *> result;

    if (this->getPeriodSize() == Globals::twelveTonePeriodSize)
    {
        result.add(this->consoleChordConstructor.get());
    }

    if (this->selection.getNumSelected() > 0 &&
        this->project.findChildrenOfType<PianoTrackNode>().size() > 1)
    {
        result.add(this->consoleMoveNotesMenu.get());
    }

    return result;
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
    ROLL_BATCH_REPAINT_START

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

    ROLL_BATCH_REPAINT_END
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
