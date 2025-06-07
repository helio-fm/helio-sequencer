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
#include "NotesDraggingGuide.h"
#include "RollHeader.h"
#include "KnifeToolHelper.h"
#include "MergingEventsConnector.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "SelectionComponent.h"
#include "ModalCallout.h"
#include "RescalePreviewTool.h"
#include "ChordPreviewTool.h"
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

    this->selectionListeners.add(new PianoRollSelectionMenuManager(&this->selection, *this));
    this->selectionListeners.add(new PianoRollSelectionRangeIndicatorController(&this->selection, *this));

    this->draggingHelper = make<NotesDraggingGuide>();
    this->addChildComponent(this->draggingHelper.get());

    const auto *uiFlags = App::Config().getUiFlags();
    this->scalesHighlightingEnabled = uiFlags->isScalesHighlightingEnabled();

    this->noteNameGuides = make<NoteNameGuidesBar>(*this, project.getTimeline()->getKeySignatures());
    this->addChildComponent(this->noteNameGuides.get());
    this->noteNameGuides->setVisible(uiFlags->areNoteNameGuidesEnabled());
}

PianoRoll::~PianoRoll() = default;

void PianoRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->generatedNotes.clear();
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
        const bool isActive = this->activeClip == *clip;

        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            const auto *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Type::Note))
            {
                const auto *note = static_cast<const Note *>(event);
                auto *nc = new NoteComponent(*this, *note, *clip);
                (*sequenceMap)[*note] = UniquePointer<NoteComponent>(nc);
                nc->setActive(isActive, true);
                this->addAndMakeVisible(nc);
                // project/view ranges may change right after reloading, so:
                this->triggerBatchRepaintFor(nc);
            }
        }
    }
}

void PianoRoll::updateClipRangeIndicator() const
{
    if (this->activeClip.isValid())
    {
        this->header->updateClipRangeIndicators(this->activeClip);
    }
}

WeakReference<MidiTrack> PianoRoll::getActiveTrack() const noexcept { return this->activeTrack; }
const Clip &PianoRoll::getActiveClip() const noexcept { return this->activeClip; }

void PianoRoll::setDefaultNoteVolume(float volume) noexcept
{
    this->newNoteVolume = volume;
}

float PianoRoll::getDefaultNoteVolume() const noexcept
{
    return this->newNoteVolume;
}

void PianoRoll::setDefaultNoteLength(float length) noexcept
{
    // no less than 0.25f, as it can become pretty much unusable:
    this->newNoteLength = jmax(0.25f, length);
}

float PianoRoll::getDefaultNoteLength() const noexcept
{
    return this->newNoteLength;
}

void PianoRoll::setRowHeight(int newRowHeight)
{
    if (newRowHeight == this->rowHeight) { return; }
    this->rowHeight = jlimit(PianoRoll::minRowHeight, PianoRoll::maxRowHeight, newRowHeight);
    this->updateHeight();
}

void PianoRoll::updateHeight()
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
        if (childComponent->belongsTo(this->activeClip))
        {
            jassert(childComponent->isActiveAndEditable());
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
// MultiTouchListener
//===----------------------------------------------------------------------===//

void PianoRoll::multiTouchStartZooming()
{
    this->rowHeightAnchor = this->rowHeight;
    RollBase::multiTouchStartZooming();
}

void PianoRoll::multiTouchContinueZooming(
    const Rectangle<float> &relativePositions,
    const Rectangle<float> &relativeAnchor,
    const Rectangle<float> &absoluteAnchor)
{
    // when zooming horizontally, try to avoid vertical zooming glitches:
    const auto minSensitivity = float(this->getViewport().getViewHeight()) / 8.f;

    const float newRowHeight = float(this->rowHeightAnchor) *
        (jmax(minSensitivity, relativePositions.getHeight()) / jmax(minSensitivity, relativeAnchor.getHeight()));

    this->setRowHeight(int(roundf(newRowHeight)));

    RollBase::multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);

    this->updateDragHelpers();
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PianoRoll::showGhostNoteFor(NoteComponent *target)
{
    auto *component = new NoteComponent(*this, target->getNote(), target->getClip(), false);
    component->setDisplayAsGhost(true);
    component->toBack();

    this->addAndMakeVisible(component);
    this->ghostNotes.add(component);
}

void PianoRoll::hideAllGhostNotes()
{
    for (auto *ghostNote : this->ghostNotes)
    {
        this->fader.fadeOut(ghostNote, Globals::UI::fadeOutShort);
    }

    this->ghostNotes.clear();
}

//===----------------------------------------------------------------------===//
// Input Listeners
//===----------------------------------------------------------------------===//

void PianoRoll::onLongTap(const Point<float> &position,
    const WeakReference<Component> &target)
{
    // try to switch to selected note's track:
    if (!this->multiTouchController->hasMultiTouch() &&
        !this->getEditMode().forbidsSelectionMode({}))
    {
        const auto *nc = dynamic_cast<NoteComponent *>(target.get());
        if (nc != nullptr && !nc->isActiveAndEditable())
        {
            this->project.setEditableScope(nc->getClip(), false);
            return;
        }
    }

    // else - start dragging lasso, if needed:
    RollBase::onLongTap(position, target);
}

void PianoRoll::zoomRelative(const Point<float> &origin,
    const Point<float> &factor, bool isInertial)
{
    if (this->zoomLevelLocked)
    {
        return;
    }

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

    if (!this->zoomLevelLocked)
    {
        const float heightToFit = float(this->viewport.getViewHeight());
        const auto numKeysToFit = jmax(1.f, float(keysTotal) * proportion.getHeight());
        this->setRowHeight(int(heightToFit / numKeysToFit));
    }

    const auto firstKey = int(float(keysTotal) * proportion.getY());
    const int firstKeyY = this->getRowHeight() * firstKey;
    this->viewport.setViewPosition(this->viewport.getViewPositionY() -
        Globals::UI::rollHeaderHeight, firstKeyY);

    RollBase::zoomAbsolute(proportion);
}

void PianoRoll::zoomToArea(int minKey, int maxKey, float minBeat, float maxBeat)
{
    jassert(minKey >= 0);
    jassert(maxKey >= minKey);

    if (!this->zoomLevelLocked)
    {
        const auto keyMargin = App::isRunningOnPhone() ?
            Globals::twelveTonePeriodSize / 2 : Globals::twelveTonePeriodSize;
        const float numKeysToFit = float(maxKey - minKey + (keyMargin * 2));
        const float heightToFit = float(this->viewport.getViewHeight());
        this->setRowHeight(int(heightToFit / numKeysToFit));
    }

    const int centerY = this->getHeight() - this->getRowHeight() * ((maxKey + minKey) / 2);
    this->viewport.setViewPosition(this->viewport.getViewPositionX(),
        centerY - Globals::UI::rollHeaderHeight - (this->viewport.getViewHeight() / 2));

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

void PianoRoll::getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber, bool snap) const
{
    if (snap)
    {
        beatNumber = this->getRoundBeatSnapByXPosition(int(x)) - this->activeClip.getBeat();
    }
    else
    {
        beatNumber = this->getBeatByXPosition(x) - this->activeClip.getBeat();
    }

    noteNumber = int((this->getHeight() - y) / this->rowHeight) - this->activeClip.getKey();
    noteNumber = jlimit(0, this->getNumKeys(), noteNumber);
}

void PianoRoll::getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber, bool snap) const
{
    if (snap)
    {
        beatNumber = this->getFloorBeatSnapByXPosition(x) - this->activeClip.getBeat();
    }
    else
    {
        beatNumber = this->getBeatByXPosition(float(x)) - this->activeClip.getBeat();
    }
    
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
    this->isDraggingAnyNotes = true;

    if (!this->draggingHelper->isVisible())
    {
        this->draggingHelper->resetAnchor(this->selection);

        // only display the dragging helper if there's more than one key in the selection
        if (this->draggingHelper->getKeyRange().getLength() > 0)
        {
            this->draggingHelper->setAlpha(1.f);
            this->draggingHelper->setVisible(true);
        }
    }
}

void PianoRoll::hideDragHelpers()
{
    this->isDraggingAnyNotes = false;

    if (this->draggingHelper->isVisible())
    {
        this->fader.fadeOut(this->draggingHelper.get(), Globals::UI::fadeOutShort);
    }
}

void PianoRoll::updateDragHelpers()
{
    if (!this->draggingHelper->isVisible())
    {
        return;
    }

    const auto keyRange = this->draggingHelper->getKeyRange();
    const auto lowerKeyBounds = this->getEventBounds(keyRange.getStart(), this->firstBeat, 1.f);
    const auto upperKeyBounds = this->getEventBounds(keyRange.getEnd(), this->firstBeat, 1.f);

    this->draggingHelper->setBounds(this->viewport.getViewPositionX(),
        int(upperKeyBounds.getY() - 2),
        this->viewport.getViewWidth(),
        int(lowerKeyBounds.getBottom() - upperKeyBounds.getY() + 4));
}

void PianoRoll::updateDragHelpers(int keyDelta)
{
    if (this->draggingHelper->isVisible())
    {
        this->draggingHelper->setKeyDelta(keyDelta);
        this->updateDragHelpers();
    }
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
        this->noteNameGuides->triggerAsyncUpdate(); // possibly update key names
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
            const auto *targetClipId = &c.first;
            const int i = track->getPattern()->indexOfSorted(targetClipId);
            jassert(i >= 0);
            
            const auto *clip = track->getPattern()->getUnchecked(i);
            auto *component = new NoteComponent(*this, note, *clip);
            sequenceMap[note] = UniquePointer<NoteComponent>(component);
            this->addAndMakeVisible(component);

            this->fader.fadeIn(component, Globals::UI::fadeInLong);

            const bool isActive = component->belongsTo(this->activeClip);
            component->setActive(isActive, true);

            if (isActive && !this->isDraggingAnyNotes)
            {
                // arpeggiators preview cannot work without that:
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
        const auto &key = static_cast<const KeySignatureEvent &>(event);
        this->updateBackgroundCacheFor(key);
        this->noteNameGuides->triggerAsyncUpdate(); // possibly update key names
        this->repaint();
    }

    RollBase::onAddMidiEvent(event);
}

void PianoRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::Note))
    {
        this->hideDragHelpers();
        this->hideAllGhostNotes();

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
        this->noteNameGuides->triggerAsyncUpdate(); // possibly update key names
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

        const bool isActive = component->belongsTo(this->activeClip);
        component->setActive(isActive);
    }

    this->triggerAsyncUpdate();
}

void PianoRoll::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->activeClip == clip) // same id
    {
        this->activeClip = newClip; // new parameters
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

void PianoRoll::onReloadGeneratedSequence(const Clip &clip,
    MidiSequence *const generatedSequence)
{
    if (generatedSequence == nullptr ||
        generatedSequence->isEmpty() ||
        !clip.hasEnabledModifiers())
    {
        this->generatedNotes.erase(clip);
        return;
    }

    ROLL_BATCH_REPAINT_START

    auto &newComponents = this->generatedNotes[clip];
    newComponents.clearQuick(true);

    const bool isActive = clip == this->activeClip;

    for (const auto *event : *generatedSequence)
    {
        // only notes are supported at the moment
        jassert(dynamic_cast<const Note *>(event));
        const auto *note = static_cast<const Note *>(event);

        auto *component = new NoteComponent(*this, *note, clip);
        component->setDisplayAsGenerated(true);
        component->setActive(isActive);
        component->setEditable(false);
        this->addAndMakeVisible(component, 0);

        newComponents.add(component);
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

        for (const auto &generatedMap : this->generatedNotes)
        {
            for (auto *component : generatedMap.second)
            {
                component->updateColours();
            }
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

    for (const auto *event : *track->getSequence())
    {
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
    this->hideAllGhostNotes();

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
        // try to preserve the canvas y position when changing temperaments
        const auto viewCentreY =
            float(this->getViewport().getViewArea().getCentreY()) / float(jmax(1, this->getHeight()));

        this->temperament = info->getTemperament();
        this->noteNameGuides->syncWithTemperament(this->temperament);
        this->updateBackgroundCachesAndRepaint();
        this->updateHeight(); // might have changed by due to different temperament

        this->getViewport().setViewPosition(this->getViewport().getViewPositionX(),
            int(float(this->getHeight()) * viewCentreY - this->getViewport().getViewHeight() / 2));

        this->updateChildrenPositions();
    }
}

void PianoRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks, const ProjectMetadata *meta)
{
    RollBase::onReloadProjectContent(tracks, meta); // updates temperament
    this->noteNameGuides->syncWithTemperament(this->temperament);
    this->reloadRollContent(); // will updateBackgroundCachesAndRepaint
    this->updateHeight(); // might have changed by due to different temperament
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

    for (const auto &generatedMap : this->generatedNotes)
    {
        const bool belongsToActiveClip =
            generatedMap.first == this->activeClip; // same id

        for (auto *component : generatedMap.second)
        {
            component->setActive(belongsToActiveClip);
        }
    }

    for (const auto &sequenceMap : this->patternMap)
    {
        const bool isActive =
            sequenceMap.first == this->activeClip; // same id

        for (const auto &components : *sequenceMap.second)
        {
            auto *nc = components.second.get();
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
// DrawableLassoSource
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
        if (component->isActiveAndEditable() &&
            (component->getNote().getBeat() + component->getClip().getBeat()) >= startBeat &&
            (component->getNote().getBeat() + component->getClip().getBeat()) < endBeat)
        {
            this->selectEvent(component, false);
        }
    }
}

void PianoRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
    const Rectangle<int> &rectangle)
{
    forEachEventComponent(this->patternMap, e)
    {
        auto *component = e.second.get();
        if (component->isActiveAndEditable() &&
            rectangle.intersects(component->getBounds()))
        {
            jassert(!itemsFound.contains(component));
            itemsFound.add(component);
        }
    }
}

void PianoRoll::findLassoItemsInPolygon(Array<SelectableComponent *> &itemsFound,
    const Rectangle<int> &bounds, const Array<Point<float>> &polygon)
{
    forEachEventComponent(this->patternMap, e)
    {
        auto *component = e.second.get();
        if (!component->isActiveAndEditable() ||
            !bounds.intersects(component->getBounds())) // fast path
        {
            continue;
        }

        if (DrawableLassoSource::boundsIntersectPolygon(component->getFloatBounds(), polygon))
        {
            jassert(!itemsFound.contains(component));
            itemsFound.add(component);
        }
    }
}

void PianoRoll::selectEvents(const Array<Note> &notes, bool shouldDeselectAllOthers)
{
    if (shouldDeselectAllOthers)
    {
        this->deselectAll();
    }

    for (const auto &it : this->patternMap)
    {
        if (it.first == this->activeClip)
        {
            auto &sequenceMap = *it.second.get();
            for (const auto &note : notes)
            {
                if (auto *component = sequenceMap[note].get())
                {
                    jassert(component->isActiveAndEditable());
                    this->selectEvent(component, false);
                }
            }
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

const NoteListBase &PianoRoll::getLassoOrEntireSequence() const
{
    if (this->selection.size() > 0)
    {
        return this->selection;
    }

    const auto *sequence = dynamic_cast<PianoSequence *>(this->activeTrack->getSequence());
    jassert(sequence != nullptr);
    return *sequence;
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void PianoRoll::onPlay()
{
    for (const auto &generatedMap : this->generatedNotes)
    {
        jassert(this->patternMap.contains(generatedMap.first));
        if (auto *sequenceMap = this->patternMap[generatedMap.first].get())
        {
            for (const auto &it : *sequenceMap)
            {
                it.second->setDisplayAsGenerated(true);
            }
        }

        for (auto *component : generatedMap.second)
        {
            component->setDisplayAsGenerated(false);
        }
    }

    this->repaint();

    RollBase::onPlay();
}

void PianoRoll::onStop()
{
    for (const auto &generatedMap : this->generatedNotes)
    {
        jassert(this->patternMap.contains(generatedMap.first));
        if (auto *sequenceMap = this->patternMap[generatedMap.first].get())
        {
            for (const auto &it : *sequenceMap)
            {
                it.second->setDisplayAsGenerated(false);
            }
        }

        for (auto *component : generatedMap.second)
        {
            component->setDisplayAsGenerated(true);
        }
    }

    this->repaint();

    RollBase::onStop();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PianoRoll::mouseDown(const MouseEvent &e)
{
    if (this->isMultiTouchEvent(e))
    {
        return;
    }

    const bool snap = !e.mods.isAltDown(); // holding alt disables snapping to barlines

    if (! this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, false);

        if (this->isAddEvent(e))
        {
            this->insertNewNoteAt(e, snap);
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
    if (this->isMultiTouchEvent(e))
    {
        return;
    }

    if (!this->project.getEditMode().forbidsAddingEvents({}))
    {
        this->showChordTool(e.getEventRelativeTo(&App::Layout()).getPosition());
    }
}

void PianoRoll::mouseDrag(const MouseEvent &e)
{
    // can show menus
    if (this->isMultiTouchEvent(e))
    {
        return;
    }

    if (this->newNoteDragging != nullptr)
    {
        if (this->selection.size() != 1)
        {
            // normally this shouldn't happen, bail out
            // (switched to another track while drawing a note or something?)
            jassertfalse;
            this->newNoteDragging = nullptr;
            this->setMouseCursor(this->project.getEditMode().getCursor());
        }
        else if (this->newNoteDragging->isInEditMode())
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
    if (this->isMultiTouchEvent(e))
    {
        return;
    }
    
    if (this->newNoteDragging != nullptr)
    {
        if (this->selection.size() != 1)
        {
            jassertfalse; // same check in mouseDrag
            this->setMouseCursor(this->project.getEditMode().getCursor());
        }
        else if (this->newNoteDragging->isInEditMode())
        {
            this->newNoteDragging->mouseUp(e.getEventRelativeTo(this->newNoteDragging));
            this->setMouseCursor(this->project.getEditMode().getCursor());
        }

        this->newNoteDragging = nullptr;

        // all the following checks are irrelevant,
        // we also want to skip the deselectAll call later:
        return;
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
    // some refactoring operations will use getLassoOrEntireSequence() to affect
    // either the selection or the entire sequence when nothing is selected;
    // the simplest transformations will still work on selection only
    // (shifting the entire sequence on pressing arrows seems misleading)

    switch (commandId)
    {
    case CommandIDs::SelectAllEvents:
        this->selectAll();
        break;
    case CommandIDs::ZoomEntireClip:
        this->project.setEditableScope(this->activeClip, true);
        #if PLATFORM_DESKTOP
        this->zoomOutImpulse(0.35f);
        #endif
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
        if (this->selection.size() > 0)
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
    case CommandIDs::TempoUp1Bpm:
        SequencerOperations::shiftTempoForProject(this->getProject(), +1);
        break;
    case CommandIDs::TempoDown1Bpm:
        SequencerOperations::shiftTempoForProject(this->getProject(), -1);
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
        SequencerOperations::shiftKeyRelative(this->selection, 1, true, true, false);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeDown:
        SequencerOperations::shiftKeyRelative(this->selection, -1, true, true, false);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeScaleKeyUp:
        // these two commands were supposed to mean in-scale transposition,
        // i.e. using scale(s) from the timeline, but when the scales highlighting flag is off,
        // they feel misleading, so let's make them work as "transposition using highlighted rows":
        SequencerOperations::shiftInScaleKeyRelative(this->getLassoOrEntireSequence(), this->activeClip,
            this->scalesHighlightingEnabled ? this->project.getTimeline()->getKeySignaturesSequence() : nullptr,
            this->temperament->getHighlighting(), 1, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeScaleKeyDown:
        SequencerOperations::shiftInScaleKeyRelative(this->getLassoOrEntireSequence(), this->activeClip,
            this->scalesHighlightingEnabled ? this->project.getTimeline()->getKeySignaturesSequence() : nullptr,
            this->temperament->getHighlighting(), -1, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::AlignToScale:
        SequencerOperations::shiftInScaleKeyRelative(this->getLassoOrEntireSequence(), this->activeClip,
            this->scalesHighlightingEnabled ? this->project.getTimeline()->getKeySignaturesSequence() : nullptr,
            this->temperament->getHighlighting(), 0, true, true);
        break;
    case CommandIDs::TransposeOctaveUp:
        SequencerOperations::shiftKeyRelative(this->getLassoOrEntireSequence(),
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave), true, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeOctaveDown:
        SequencerOperations::shiftKeyRelative(this->getLassoOrEntireSequence(),
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave), true, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeFifthUp:
        SequencerOperations::shiftKeyRelative(this->getLassoOrEntireSequence(),
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth), true, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::TransposeFifthDown:
        SequencerOperations::shiftKeyRelative(this->getLassoOrEntireSequence(),
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth), true, true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::MakeStaccato:
        SequencerOperations::makeStaccato(this->getLassoOrEntireSequence(), Globals::minNoteLength * 2.f, true);
        break;
    case CommandIDs::MakeStaccatissimo:
        SequencerOperations::makeStaccato(this->getLassoOrEntireSequence(), Globals::minNoteLength, true);
        break;
    case CommandIDs::MakeLegato:
        SequencerOperations::makeLegato(this->getLassoOrEntireSequence(), 0.f);
        break;
    case CommandIDs::MakeLegatoOverlapping:
        SequencerOperations::makeLegato(this->getLassoOrEntireSequence(), Globals::minNoteLength);
        break;
    case CommandIDs::CleanupOverlaps:
        SequencerOperations::cleanupOverlaps(this->getLassoOrEntireSequence());
        break;
    case CommandIDs::MelodicInversion:
        SequencerOperations::melodicInversion(this->getLassoOrEntireSequence());
        break;
    case CommandIDs::Retrograde:
        SequencerOperations::retrograde(this->getLassoOrEntireSequence());
        break;
    case CommandIDs::InvertChordUp:
        SequencerOperations::invertChord(this->getLassoOrEntireSequence(), this->getPeriodSize(), true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::InvertChordDown:
        SequencerOperations::invertChord(this->getLassoOrEntireSequence(), -this->getPeriodSize(), true, true);
        SequencerOperations::previewSelection(this->selection, this->getTransport());
        break;
    case CommandIDs::ToggleMuteClips:
        PatternOperations::toggleMuteClip(this->activeClip);
        break;
    case CommandIDs::ToggleSoloClips:
        PatternOperations::toggleSoloClip(this->activeClip);
        break;
    case CommandIDs::ToggleMuteModifiers:
        PatternOperations::toggleMuteModifiersStack(this->activeClip);
        break;
    case CommandIDs::ToggleScalesHighlighting:
        App::Config().getUiFlags()->setScalesHighlightingEnabled(!this->scalesHighlightingEnabled);
        break;
    case CommandIDs::ToggleNoteNameGuides:
        App::Config().getUiFlags()->setNoteNameGuidesEnabled(!this->noteNameGuides->isVisible());
        break;
    case CommandIDs::ToggleLoopOverSelection:
        if (this->selection.size() > 0)
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
        if (this->selection.size() >= 3)
        {
            Scale::Ptr scale = nullptr;
            Note::Key scaleRootKey = 0;
            String scaleRootKeyName;
            if (!SequencerOperations::findHarmonicContext(this->selection, this->activeClip,
                this->project.getTimeline()->getKeySignatures(), scale, scaleRootKey, scaleRootKeyName))
            {
                jassertfalse;
                break;
            }

            // try to guess the arp's name:
            auto name = SequencerOperations::findClosestOverlappingAnnotation(this->selection,
                this->project.getTimeline()->getAnnotations());
            if (name.isEmpty())
            {
                name = this->activeTrack->getTrackName();
            }

            auto newArpDialog = ModalDialogInput::Presets::newArpeggiator(name);
            newArpDialog->onOk = [this, scale, scaleRootKey](const String &name)
            {
                auto arp = SequencerOperations::makeArpeggiator(name,
                    this->selection, this->temperament, scale, scaleRootKey,
                    this->project.getTimeline()->getTimeSignaturesAggregator());

                App::Config().getArpeggiators()->updateUserResource(arp);
            };

            App::showModalComponent(move(newArpDialog));
        }
        break;
    case CommandIDs::ShowArpeggiatorsPanel:
        ModalCallout::emit(new ArpPreviewTool(*this,
            this->project.getTimeline()->getKeySignaturesSequence(),
            this->project.getTimeline()->getTimeSignaturesAggregator()), this, true);
        break;
    case CommandIDs::ShowRescalePanel:
        ModalCallout::emit(new RescalePreviewTool(*this,
            this->project.getTimeline()->getKeySignaturesSequence()), this, true);
        break;
    case CommandIDs::ShowChordPanel:
        this->showChordTool(this->getDefaultPositionForPopup());
        break;
    case CommandIDs::ToggleVolumePanel:
        App::Config().getUiFlags()->toggleEditorPanelVisibility();
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
        if (this->selection.size() > 0)
        {
            ROLL_BATCH_REPAINT_START
            SequencerOperations::tuneVolume(this->selection, 1.f / 64.f);
            this->setDefaultNoteVolume(this->selection.getFirstAs<NoteComponent>()->getVelocity());
            ROLL_BATCH_REPAINT_END
        }
        break;
    case CommandIDs::NotesVolumeDown:
        if (this->selection.size() > 0)
        {
            ROLL_BATCH_REPAINT_START
            SequencerOperations::tuneVolume(this->selection, -1.f / 64.f);
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
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 1.f);
        break;
    case CommandIDs::QuantizeTo1_2:
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 2.f);
        break;
    case CommandIDs::QuantizeTo1_4:
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 4.f);
        break;
    case CommandIDs::QuantizeTo1_8:
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 8.f);
        break;
    case CommandIDs::QuantizeTo1_16:
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 16.f);
        break;
    case CommandIDs::QuantizeTo1_32:
        SequencerOperations::quantize(this->getLassoOrEntireSequence(), 32.f);
        break;
    default:
        break;
    }

    RollBase::handleCommandMessage(commandId);
}

void PianoRoll::resized()
{
    if (!this->isEnabled())
    {
        return;
    }

    ROLL_BATCH_REPAINT_START

    forEachEventComponent(this->patternMap, e)
    {
        const auto component = e.second.get();
        component->setFloatBounds(this->getEventBounds(component));
    }

    for (auto *component : this->ghostNotes)
    {
        component->setFloatBounds(this->getEventBounds(component));
    }

    for (const auto &components : this->generatedNotes)
    {
        for (auto *component : components.second)
        {
            component->setFloatBounds(this->getEventBounds(component));
        }
    }

    if (this->knifeToolHelper != nullptr)
    {
        this->knifeToolHelper->updateBounds();
        this->knifeToolHelper->updateCutMarks();
    }

    RollBase::resized();

    ROLL_BATCH_REPAINT_END
}

void PianoRoll::paint(Graphics &g) noexcept
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

            const auto tileHeight = periodHeight * HighlightingScheme::periodsInTile;
            for (int i = paintStartY; i < y + h; i += tileHeight)
            {
                g.setFillType({ fillImage, AffineTransform::translation(0.f, float(i)) });
                g.fillRect(prevBeatX, i, beatX - prevBeatX, tileHeight);
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

        const auto tileHeight = periodHeight * HighlightingScheme::periodsInTile;
        for (int i = paintStartY; i < y + h; i += tileHeight)
        {
            g.setFillType({ fillImage, AffineTransform::translation(0.f, float(i)) });
            g.fillRect(prevBeatX, i, paintEndX - prevBeatX, tileHeight);
        }

        RollBase::paint(g);
    }
}

void PianoRoll::insertNewNoteAt(const MouseEvent &e, bool snap)
{
    int key = 0;
    float beat = 0.f;
    constexpr int xOffset = 5;
    this->getRowsColsByMousePosition(e.x + xOffset, e.y, key, beat, snap); // Pretend the mouse is a little to the right of where it actually is. Boosts accuracy when placing notes - RPM

    auto *activeSequence = static_cast<PianoSequence *>(this->activeTrack->getSequence());
    activeSequence->checkpoint();

    // thing is, we can't have a pointer to this note's component,
    // since it is not created yet at this point; so we set a flag,
    // and in onAddMidiEvent/1 callback we store that pointer,
    // so that the new note can be dragged, or resized, or whatever

    this->addNewNoteMode = true;
    activeSequence->insert(Note(activeSequence, key, beat,
        this->newNoteLength, this->newNoteVolume), true);

    this->getTransport().previewKey(this->activeTrack->getTrackId(),
        this->activeTrack->getTrackChannel(),
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
        if (!nc->isActiveAndEditable() || !nc->isVisible())
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
            if (!nc->isActiveAndEditable())
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
        this->selectEvents(cutEventsToTheRight, false);
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
        if (nc->isActiveAndEditable() &&
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
        if (nc->isActiveAndEditable() &&
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
    if (this->selection.getNumSelected() > 0)
    {
        if (this->noteResizerLeft == nullptr)
        {
            this->noteResizerLeft = make<NoteResizerLeft>(*this);
            this->addAndMakeVisible(this->noteResizerLeft.get());
        }

        if (this->noteResizerRight == nullptr)
        {
            this->noteResizerRight = make<NoteResizerRight>(*this);
            this->addAndMakeVisible(this->noteResizerRight.get());
        }

        this->noteResizerLeft->updateBounds();
        this->noteResizerRight->updateBounds();
    }
    else
    {
        this->noteResizerLeft = nullptr;
        this->noteResizerRight = nullptr;
    }
#endif

    RollBase::handleAsyncUpdate();
}

void PianoRoll::onChangeEditMode(const RollEditMode &mode)
{
    this->endCuttingEventsIfNeeded();
    RollBase::onChangeEditMode(mode);
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

    RollBase::updateChildrenBounds();
}

void PianoRoll::updateChildrenPositions()
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

    RollBase::updateChildrenPositions();
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> PianoRoll::getCommandPaletteActionProviders()
{
    Array<CommandPaletteActionsProvider *> result;

    // recreates actions every time they are needed to avoid keeping their state, like undo flags
    if (this->getPeriodSize() == Globals::twelveTonePeriodSize)
    {
        this->consoleChordConstructor = make<CommandPaletteChordConstructor>(*this);
        result.add(this->consoleChordConstructor.get());
    }

    if (this->selection.getNumSelected() > 0 &&
        this->project.findChildrenOfType<PianoTrackNode>().size() > 1)
    {
        this->consoleMoveNotesMenu = make<CommandPaletteMoveNotesMenu>(*this, this->project);
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
    SerializedData data(UI::pianoRoll);
    
    data.setProperty(UI::beatWidth, roundf(this->beatWidth));
    data.setProperty(UI::rowHeight, this->getRowHeight());

    data.setProperty(UI::startBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX())));

    data.setProperty(UI::endBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX() +
            this->getViewport().getViewWidth())));

    data.setProperty(UI::viewportPositionY, this->getViewport().getViewPositionY());

    data.setProperty(UI::defaultNoteLength, this->newNoteLength);
    data.setProperty(UI::defaultNoteVolume, this->newNoteVolume);

    return data;
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

    this->newNoteLength = root.getProperty(UI::defaultNoteLength, this->newNoteLength);
    this->newNoteVolume = root.getProperty(UI::defaultNoteVolume, this->newNoteVolume);

    const float startBeat = float(root.getProperty(UI::startBeat, 0.f));
    const int x = this->getXPositionByBeat(startBeat);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);
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

void PianoRoll::showChordTool(Point<int> position)
{
    auto *pianoSequence = dynamic_cast<PianoSequence *>(this->activeTrack->getSequence());
    jassert(pianoSequence);

    this->deselectAll();

    auto *timeContext = this->project.getTimeline()->getTimeSignaturesAggregator();
    auto *harmonicContext = this->project.getTimeline()->getKeySignaturesSequence();
    auto *popup = new ChordPreviewTool(*this, pianoSequence, this->activeClip, harmonicContext, timeContext);
    popup->setTopLeftPosition(position - Point<int>(popup->getWidth(), popup->getHeight()) / 2);
    App::Layout().addAndMakeVisible(popup);
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
    if (enabled && !this->noteNameGuides->isVisible())
    {
        this->fader.fadeIn(this->noteNameGuides.get(), Globals::UI::fadeInShort);
    }
    else
    {
        this->noteNameGuides->setVisible(enabled);
    }

    this->repaint();
}
