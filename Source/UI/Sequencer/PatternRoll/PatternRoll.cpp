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
#include "PatternRoll.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "PluginWindow.h"
#include "HelioCallout.h"
#include "MenuPanel.h"

#include "Pattern.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "LassoListeners.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "SelectionComponent.h"
#include "RollHeader.h"
#include "CutPointMark.h"
#include "MergingEventsConnector.h"

#include "UndoStack.h"
#include "PatternOperations.h"
#include "SequencerOperations.h"
#include "InteractiveActions.h"

#include "TrackPropertiesDialog.h"
#include "TimeSignatureDialog.h"
#include "ProjectTimeline.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "ClipComponent.h"
#include "PianoClipComponent.h"
#include "AutomationCurveClipComponent.h"
#include "AutomationStepsClipComponent.h"
#include "DummyClipComponent.h"

#include "HelioTheme.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "ColourIDs.h"
#include "Config.h"

struct StringComparator final
{
    static int compareElements(const String &first, const String &second)
    {
        return first.compareNatural(second);
    }
};

static StringComparator kStringSort;

static void updateTrackRowPosition(Array<String> &rows,
    MidiTrack::Grouping grouping, const MidiTrack *const track)
{
    const auto &trackGroupKey = track->getTrackGroupKey(grouping);
    const auto indexOfSorted = rows.indexOfSorted(kStringSort, trackGroupKey);
    if (indexOfSorted < 0)
    {
        rows.addSorted(kStringSort, trackGroupKey);
    }
}

PatternRoll::PatternRoll(ProjectNode &parentProject,
    Viewport &viewportRef,
    WeakReference<AudioMonitor> clippingDetector) :
    RollBase(parentProject, viewportRef, clippingDetector, false, false, true)
{
    this->setComponentID(ComponentIDs::patternRollId);

    this->selectionListeners.add(new PatternRollSelectionMenuManager(&this->selection));
    this->selectionListeners.add(new PatternRollTimeSignaturePicker(&this->selection, parentProject));
    this->selectionListeners.add(new PatternRollRecordingTargetController(&this->selection, parentProject));
    this->selectionListeners.add(new PatternRollSelectionRangeIndicatorController(&this->selection, *this));

    this->repaintBackgroundsCache();
    this->reloadRollContent();
    this->setBeatRange(0, Globals::Defaults::projectLength);
}

void PatternRoll::selectAll()
{
    for (const auto &e : this->clipComponents)
    {
        this->selection.addToSelection(e.second.get());
    }
}

static ClipComponent *createClipComponentFor(MidiTrack *track,
    const Clip &clip, ProjectNode &project, RollBase &roll)
{
    auto *sequence = track->getSequence();
    jassert(sequence != nullptr);

    if (auto *pianoLayer = dynamic_cast<PianoSequence *>(sequence))
    {
        return new PianoClipComponent(project, sequence, roll, clip);
    }
    else if (auto *autoLayer = dynamic_cast<AutomationSequence *>(sequence))
    {
        if (track->isOnOffAutomationTrack())
        {
            return new AutomationStepsClipComponent(project, sequence, roll, clip);
        }
        else
        {
            return new AutomationCurveClipComponent(project, sequence, roll, clip);
        }
    }

    return nullptr;
}

void PatternRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->clipComponents.clear();
    this->tracks.clearQuick();
    this->rows.clearQuick();

    const auto grouping = this->project.getTrackGroupingMode();

    ROLL_BATCH_REPAINT_START

    for (auto *track : this->project.getTracks())
    {
        // Only show tracks with patterns (i.e. ignore timeline tracks)
        if (const auto *pattern = track->getPattern())
        {
            this->tracks.add(track);
            updateTrackRowPosition(this->rows, grouping, track);

            for (int j = 0; j < pattern->size(); ++j)
            {
                const Clip &clip = *pattern->getUnchecked(j);
                if (auto *clipComponent = createClipComponentFor(track, clip, this->project, *this))
                {
                    this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
                    this->addAndMakeVisible(clipComponent);
                }
            }
        }
    }

    this->repaint(this->viewport.getViewArea());

    ROLL_BATCH_REPAINT_END
}

int PatternRoll::getNumRows() const noexcept
{
    return this->rows.size();
}

void PatternRoll::reloadRowsGrouping()
{
    this->rows.clearQuick();
    const auto grouping = this->project.getTrackGroupingMode();
    for (const auto *track : this->tracks)
    {
        updateTrackRowPosition(this->rows, grouping, track);
    }
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void PatternRoll::onRecord()
{
    // mark the first piano component as recording clip:
    PianoClipComponent *singleSelectedTarget = nullptr;
    if (this->selection.getNumSelected() == 1)
    {
        auto *cc = this->selection.getFirstAs<ClipComponent>();
        singleSelectedTarget = dynamic_cast<PianoClipComponent *>(cc);
    }

    if (singleSelectedTarget != nullptr)
    {
        singleSelectedTarget->setShowRecordingMode(true);
    }
    else
    {
        this->deselectAll();
    }

    RollBase::onRecord();
}

void PatternRoll::onStop()
{
    for (int i = 0; i < this->selection.getNumSelected(); ++i)
    {
        auto *cc = this->selection.getFirstAs<ClipComponent>();
        if (auto *pianoClip = dynamic_cast<PianoClipComponent *>(cc))
        {
            pianoClip->setShowRecordingMode(false);
        }
    }

    RollBase::onStop();
}

//===----------------------------------------------------------------------===//
// RollBase
//===----------------------------------------------------------------------===//

void PatternRoll::setChildrenInteraction(bool interceptsMouse, MouseCursor cursor)
{
    for (const auto &e : this->clipComponents)
    {
        const auto child = e.second.get();
        child->setInterceptsMouseClicks(interceptsMouse, interceptsMouse);
        child->setMouseCursor(cursor);
    }
}

void PatternRoll::updateRollSize()
{
    constexpr auto addTrackHelper = PatternRoll::rowHeight;
    const int h = Globals::UI::rollHeaderHeight +
        this->getNumRows() * PatternRoll::rowHeight + addTrackHelper;
    this->setSize(this->getWidth(), jmax(h, this->viewport.getHeight()));
}

//===----------------------------------------------------------------------===//
// Clip management
//===----------------------------------------------------------------------===//

void PatternRoll::addClip(Pattern *pattern, float beat)
{
    pattern->checkpoint();
    Clip clip(pattern, beat);
    pattern->insert(clip, true);
}

Rectangle<float> PatternRoll::getEventBounds(FloatBoundsComponent *mc) const
{
    jassert(dynamic_cast<ClipComponent *>(mc));
    const auto *cc = static_cast<ClipComponent *>(mc);
    return this->getEventBounds(cc->getClip(), cc->getBeat());
}

Rectangle<float> PatternRoll::getEventBounds(const Clip &clip, float clipBeat) const
{
    const auto *track = clip.getPattern()->getTrack();
    const auto *sequence = track->getSequence();
    jassert(sequence != nullptr);

    const auto grouping = this->project.getTrackGroupingMode();
    const auto trackGroupKey = track->getTrackGroupKey(grouping);
    const int trackIndex = this->rows.indexOfSorted(kStringSort, trackGroupKey);

    // In case there are no events, display an empty clip of some default length,
    // if there are some really short events (e.g. the first moments in recording mode),
    // set the minimal limit for the clip bounds:
    const float sequenceLength = sequence->isEmpty() ? Globals::Defaults::emptyClipLength :
        jmax(sequence->getLengthInBeats(), Globals::minClipLength);

    const float w = this->beatWidth * sequenceLength;
    const float x = this->beatWidth * (sequence->getFirstBeat() + clipBeat - this->firstBeat);
    const float y = float(trackIndex * PatternRoll::rowHeight);

    return Rectangle<float>(x,
        Globals::UI::rollHeaderHeight + y + PatternRoll::trackHeaderHeight,
        w, float(PatternRoll::clipHeight - 1));
}

float PatternRoll::getBeatForClipByXPosition(const Clip &clip, float x) const
{
    // One trick here is that displayed clip position depends on a sequence's first beat as well:
    const auto *sequence = clip.getPattern()->getTrack()->getSequence();
    return this->getRoundBeatSnapByXPosition(int(x)) - sequence->getFirstBeat();
}

float PatternRoll::getBeatByMousePosition(const Pattern *pattern, int x) const
{
    const auto *sequence = pattern->getTrack()->getSequence();
    return this->getFloorBeatSnapByXPosition(x) - sequence->getFirstBeat();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PatternRoll::onAddTrack(MidiTrack *const track)
{
    if (auto *pattern = track->getPattern())
    {
        this->tracks.add(track);
        const auto grouping = this->project.getTrackGroupingMode();
        updateTrackRowPosition(this->rows, grouping, track);

        for (int j = 0; j < pattern->size(); ++j)
        {
            const Clip &clip = *pattern->getUnchecked(j);
            if (auto *clipComponent = createClipComponentFor(track, clip, this->project, *this))
            {
                this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
                this->addAndMakeVisible(clipComponent);

                if (this->isEnabled())
                {
                    this->selectEvent(clipComponent, true);
                }
            }
        }
    }

    // Roll size might need to be changed
    this->updateRollSize();

    // And clip component positions should be updated
    // (this may be doing the same job twice,
    // in case if updateRollSize have called resized as well)
    this->resized();
    this->applyEditModeUpdates();
}

//void debugTracksOrder(Array<MidiTrack *> tracks)
//{
//    DBG("---------------------");
//    for (const auto *track : tracks)
//    {
//        DBG(track->getTrackName());
//    }
//}

void PatternRoll::onChangeTrackProperties(MidiTrack *const track)
{
    if (Pattern *pattern = track->getPattern())
    {
        // track name could change here so we have to keep track array sorted by name:
        //debugTracksOrder(this->tracks);
        this->reloadRowsGrouping();
        //debugTracksOrder(this->tracks);

        // TODO only repaint clips of a changed track?
        for (const auto &e : this->clipComponents)
        {
            const auto component = e.second.get();
            component->updateColours();
        }
    }

    this->updateRollSize();
    this->resized();
}

void PatternRoll::onRemoveTrack(MidiTrack *const track)
{
    this->selection.deselectAll();

    this->tracks.removeAllInstancesOf(track);
    this->reloadRowsGrouping();

    if (Pattern *pattern = track->getPattern())
    {
        for (int i = 0; i < pattern->size(); ++i)
        {
            const Clip &clip = *pattern->getUnchecked(i);
            if (this->clipComponents.contains(clip))
            {
                this->clipComponents.erase(clip);
            }
        }
    }

    this->updateRollSize();
    this->resized();
}

void PatternRoll::onAddClip(const Clip &clip)
{
    auto *track = clip.getPattern()->getTrack();
    if (auto *clipComponent = createClipComponentFor(track, clip, this->project, *this))
    {
        this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
        this->addAndMakeVisible(clipComponent);
        clipComponent->toFront(false);

        this->fader.fadeIn(clipComponent, Globals::UI::fadeInLong);
        this->triggerBatchRepaintFor(clipComponent);

        if (this->addNewClipMode)
        {
            this->newClipDragging = clipComponent;
            this->addNewClipMode = false;
            this->selectEvent(this->newClipDragging, true); // clear previous selection
        }
    }
}

void PatternRoll::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (const auto component = this->clipComponents[clip].release())
    {
        this->clipComponents.erase(clip);
        this->clipComponents[newClip] = UniquePointer<ClipComponent>(component);
        this->triggerBatchRepaintFor(component);
    }

    RollBase::onChangeClip(clip, newClip);
}

void PatternRoll::onRemoveClip(const Clip &clip)
{
    if (const auto deletedComponent = this->clipComponents[clip].get())
    {
        this->fader.fadeOut(deletedComponent, Globals::UI::fadeOutLong);
        this->selection.deselect(deletedComponent);
        this->clipComponents.erase(clip);
    }
}

void PatternRoll::onPostRemoveClip(Pattern *const pattern) {}

void PatternRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    RollBase::onReloadProjectContent(tracks, meta); // updates temperament ref
    this->reloadRollContent();
}

//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

void PatternRoll::selectEventsInRange(float startBeat, float endBeat, bool shouldClearAllOthers)
{
    if (shouldClearAllOthers)
    {
        this->selection.deselectAll();
    }

    for (const auto &e : this->clipComponents)
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

void PatternRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
            jassert(!itemsFound.contains(component));
            itemsFound.add(component);
        }
    }
}

void PatternRoll::updateHighlightedInstances()
{
    for (const auto *track : this->tracks)
    {
        bool hasSelectedClip = false;
        const auto *pattern = track->getPattern();

        // find out if at least one instance in a pattern is selected
        for (const auto *clip : pattern->getClips())
        {
            const auto component = this->clipComponents.find(*clip);
            if (component != this->clipComponents.end() &&
                component.value()->isSelected())
            {
                hasSelectedClip = true;
                break;
            }
        }

        for (const auto *clip : pattern->getClips())
        {
            const auto component = this->clipComponents.find(*clip);
            if (component != this->clipComponents.end())
            {
                component.value()->setHighlightedAsInstance(hasSelectedClip);
            }
        }
    }
}

void PatternRoll::selectClip(const Clip &clip)
{
    auto it = this->clipComponents.find(clip);
    if (it != this->clipComponents.end())
    {
        this->selectEvent(it->second.get(), true);
    }
}

//===----------------------------------------------------------------------===//
// SmoothZoomListener
//===----------------------------------------------------------------------===//

float PatternRoll::getZoomFactorY() const noexcept
{
    const float viewHeight = float(this->viewport.getViewHeight());
    return viewHeight / float(this->getHeight());
}

void PatternRoll::longTapEvent(const Point<float> &position, const WeakReference<Component> &target)
{
    if (this->multiTouchController->hasMultitouch())
    {
        return;
    }

    if (target == this)
    {
        if (this->getEditMode().isMode(RollEditMode::knifeMode))
        {
            this->endCuttingClipsIfNeeded(false, false);
            this->getEditMode().setMode(RollEditMode::mergeMode);
            this->startMergingEvents(position);
        }
    }

    RollBase::longTapEvent(position, target);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PatternRoll::mouseDown(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (!this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, false);

        if (this->isAddEvent(e))
        {
            this->insertNewClipAt(e);
        }
        else if (this->isKnifeToolEvent(e))
        {
            this->startCuttingClips(e.position);
        }
    }

    RollBase::mouseDown(e);
}

void PatternRoll::mouseDrag(const MouseEvent &e)
{
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    if (this->newClipDragging)
    {
        if (this->newClipDragging->isDragging())
        {
            this->newClipDragging->mouseDrag(e.getEventRelativeTo(this->newClipDragging));
        }
        else
        {
            this->newClipDragging->startDragging();
            // a hack here. clip is technically created in two actions:
            // adding one and resizing it afterwards, so two checkpoints would happen
            // which we don't want, as adding a note should appear to user as a single transaction
            this->newClipDragging->setNoCheckpointNeededForNextAction();
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
        }
    }
    else if (this->isKnifeToolEvent(e))
    {
        this->continueCuttingClips(e.position);
    }

    RollBase::mouseDrag(e);
}

void PatternRoll::mouseUp(const MouseEvent &e)
{
    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }

    // Dismiss newClipDragging, if needed
    if (this->newClipDragging != nullptr)
    {
        this->newClipDragging->endDragging();
        this->setMouseCursor(this->project.getEditMode().getCursor());
        this->newClipDragging = nullptr;
    }

    this->endCuttingClipsIfNeeded(true, e.mods.isAnyModifierKeyDown());

    if (!this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, true);

        // process lasso selection logic
        RollBase::mouseUp(e);
    }
}

//===----------------------------------------------------------------------===//
// Keyboard shortcuts
//===----------------------------------------------------------------------===//

void PatternRoll::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::SelectAllClips:
        this->selectAll();
        break;
    case CommandIDs::RenameTrack:
        if (this->selection.getNumSelected() > 0)
        {
            const String trackId = this->selection.getFirstAs<ClipComponent>()->getClip().getTrackId();
            for (int i = 0; i < this->selection.getNumSelected(); ++i)
            {
                if (this->selection.getItemAs<ClipComponent>(i)->getClip().getTrackId() != trackId)
                {
                    return; // More then one track is selected
                }
            }

            auto *trackNode = this->project.findTrackById<MidiTrackNode>(trackId);
            App::showModalComponent(make<TrackPropertiesDialog>(this->project, trackNode));
        }
        break;
    case CommandIDs::SetTrackTimeSignature:
        if (this->getLassoSelection().getNumSelected() == 1)
        {
            const auto clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            if (auto *pianoTrack = this->project.findTrackById<PianoTrackNode>(clip.getTrackId()))
            {
                App::showModalComponent(TimeSignatureDialog::editingDialog(*this,
                    this->project.getUndoStack(), *pianoTrack->getTimeSignatureOverride()));
            }
        }
        break;
    case CommandIDs::DuplicateTrack:        // the implementation for these two
    case CommandIDs::InstanceToUniqueTrack: // is pretty much the same code
        if (this->getLassoSelection().getNumSelected() == 1)
        {
            const auto clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            auto *clonedTrack = this->project.findTrackById<MidiTrackNode>(clip.getTrackId());
            if (clonedTrack == nullptr)
            {
                jassertfalse;
                break;
            }

            UniquePointer<MidiTrackNode> trackPreset = nullptr;
            if (dynamic_cast<PianoTrackNode *>(clonedTrack))
            {
                const auto *cloneSource = static_cast<PianoSequence *>(clonedTrack->getSequence());
                trackPreset = SequencerOperations::createPianoTrack(cloneSource, clip);
            }
            else if (dynamic_cast<AutomationTrackNode *>(clonedTrack))
            {
                const auto *cloneSource = static_cast<AutomationSequence *>(clonedTrack->getSequence());
                trackPreset = SequencerOperations::createAutomationTrack(cloneSource, clip);
            }

            if (trackPreset == nullptr)
            {
                jassertfalse; // it should be either an automation track or a piano track
                break;
            }

            // the alternative mode to turn instance segment into a unique track,
            // which is similar to "duplicate track", but deletes the selected
            // instance segment and uses the same name for the new track;
            // and it only makes sense when the pattern has more than 1 clip,
            // if there's only 1 clip it'll do nothing useful, so fallback to duplicate:
            const bool altMode = clonedTrack->getPattern()->size() > 1 &&
                commandId == CommandIDs::InstanceToUniqueTrack;

            this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::DuplicateTrack);

            if (altMode)
            {
                jassert(this->getLassoSelection().getNumSelected() == 1);
                PatternOperations::deleteSelection(this->getLassoSelection(), this->project, false);
            }

            const bool generateNewName = !altMode;
            InteractiveActions::addNewTrack(this->project,
                move(trackPreset), clonedTrack->getTrackName(), generateNewName,
                UndoActionIDs::DuplicateTrack, TRANS(I18n::Menu::trackDuplicate),
                false);
        }
        break;
    case CommandIDs::EditCurrentInstrument:
        if (auto *window = PluginWindow::getWindowFor(PatternOperations::getSelectedInstrumentId(this->selection)))
        {
            window->toFront(true);
        }
        break;
    case CommandIDs::DeleteClips:
        PatternOperations::deleteSelection(this->getLassoSelection(), this->project);
        break;
    case CommandIDs::ZoomEntireClip:
        if (this->selection.getNumSelected() > 0)
        {
            const auto &clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            this->project.setEditableScope(clip, true);
        }
        break;
    case CommandIDs::ClipTransposeUp:
        PatternOperations::transposeClips(this->getLassoSelection(), 1);
        break;
    case CommandIDs::ClipTransposeDown:
        PatternOperations::transposeClips(this->getLassoSelection(), -1);
        break;
    case CommandIDs::ClipTransposeOctaveUp:
        PatternOperations::transposeClips(this->getLassoSelection(),
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave));
        break;
    case CommandIDs::ClipTransposeOctaveDown:
        PatternOperations::transposeClips(this->getLassoSelection(),
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave));
        break;
    case CommandIDs::ClipTransposeFifthUp:
        PatternOperations::transposeClips(this->getLassoSelection(),
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth));
        break;
    case CommandIDs::ClipTransposeFifthDown:
        PatternOperations::transposeClips(this->getLassoSelection(),
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth));
        break;
    case CommandIDs::ClipVolumeUp:
        PatternOperations::tuneClips(this->getLassoSelection(), 1.f / 32.f);
        break;
    case CommandIDs::ClipVolumeDown:
        PatternOperations::tuneClips(this->getLassoSelection(), -1.f / 32.f);
        break;
    case CommandIDs::BeatShiftLeft:
        PatternOperations::shiftBeatRelative(this->getLassoSelection(), -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::BeatShiftRight:
        PatternOperations::shiftBeatRelative(this->getLassoSelection(), this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::ToggleMuteClips:
        PatternOperations::toggleMuteClips(this->getLassoSelection());
        break;
    case CommandIDs::ToggleSoloClips:
        PatternOperations::toggleSoloClips(this->getLassoSelection());
        break;
    case CommandIDs::ToggleLoopOverSelection:
        if (this->selection.getNumSelected() > 0)
        {
            const auto startBeat = PatternOperations::findStartBeat(this->selection);
            const auto endBeat = PatternOperations::findEndBeat(this->selection);
            this->getTransport().togglePlaybackLoop(startBeat, endBeat);
        }
        else // no selection, just place the loop at the playhead position:
        {
            const auto startBeat = this->getTransport().getSeekBeat();
            const auto endBeat = startBeat + Globals::beatsPerBar * 4;
            this->getTransport().togglePlaybackLoop(startBeat, endBeat);
        }
        break;
    case CommandIDs::ShowNewTrackPanel:
        // inserting at the playhead position:
        this->showNewTrackMenu(this->lastTransportBeat.get());
        break;
    case CommandIDs::PatternsGroupByName:
        this->deselectAll();
        this->project.setTrackGroupingMode(MidiTrack::Grouping::GroupByName);
        this->reloadRowsGrouping();
        // Clip component positions should be updated:
        this->updateRollSize();
        this->resized();
        break;
    case CommandIDs::PatternsGroupByColour:
        this->deselectAll();
        this->project.setTrackGroupingMode(MidiTrack::Grouping::GroupByColour);
        this->reloadRowsGrouping();
        this->updateRollSize();
        this->resized();
        break;
    case CommandIDs::PatternsGroupByInstrument:
        this->deselectAll();
        this->project.setTrackGroupingMode(MidiTrack::Grouping::GroupByInstrument);
        this->reloadRowsGrouping();
        this->updateRollSize();
        this->resized();
        break;
    case CommandIDs::PatternsGroupById:
        this->deselectAll();
        this->project.setTrackGroupingMode(MidiTrack::Grouping::GroupByNameId);
        this->reloadRowsGrouping();
        this->updateRollSize();
        this->resized();
        break;
    case CommandIDs::QuantizeTo1_1:
        PatternOperations::quantize(this->getLassoSelection(), 1.f);
        break;
    case CommandIDs::QuantizeTo1_2:
        PatternOperations::quantize(this->getLassoSelection(), 2.f);
        break;
    case CommandIDs::QuantizeTo1_4:
        PatternOperations::quantize(this->getLassoSelection(), 4.f);
        break;
    case CommandIDs::QuantizeTo1_8:
        PatternOperations::quantize(this->getLassoSelection(), 8.f);
        break;
    case CommandIDs::QuantizeTo1_16:
        PatternOperations::quantize(this->getLassoSelection(), 16.f);
        break;
    case CommandIDs::QuantizeTo1_32:
        PatternOperations::quantize(this->getLassoSelection(), 32.f);
        break;
    default:
        break;
    }

    RollBase::handleCommandMessage(commandId);
}

void PatternRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    ROLL_BATCH_REPAINT_START

    for (const auto &e : this->clipComponents)
    {
        const auto c = e.second.get();
        c->setFloatBounds(this->getEventBounds(c));
    }

    if (this->knifeToolHelper != nullptr)
    {
        this->knifeToolHelper->updateBounds(true);
    }

    RollBase::resized();

    ROLL_BATCH_REPAINT_END
}

void PatternRoll::paint(Graphics &g)
{
    g.setTiledImageFill(this->rowPattern, 0, Globals::UI::rollHeaderHeight, 1.f);
    g.fillRect(this->viewport.getViewArea());
    g.setFont(Globals::UI::Fonts::XS); // so that clips don't have to do it
    RollBase::paint(g);
}

void PatternRoll::parentSizeChanged()
{
    this->updateRollSize();
}

float PatternRoll::findNextAnchorBeat(float beat) const
{
    float result = this->getLastBeat();
    for (const auto *track : this->tracks)
    {
        const float sequenceStart = track->getSequence()->getFirstBeat();
        for (const auto *clip : track->getPattern()->getClips())
        {
            const auto clipStart = clip->getBeat() + sequenceStart;
            if (clipStart > beat)
            {
                result = jmin(clipStart, result);
            }
        }
    }

    return result;
}

float PatternRoll::findPreviousAnchorBeat(float beat) const
{
    float result = this->getFirstBeat();
    for (const auto *track : this->tracks)
    {
        const float sequenceStart = track->getSequence()->getFirstBeat();
        for (const auto *clip : track->getPattern()->getClips())
        {
            const auto clipStart = clip->getBeat() + sequenceStart;
            if (clipStart < beat)
            {
                result = jmax(clipStart, result);
            }
        }
    }

    return result;
}

void PatternRoll::insertNewClipAt(const MouseEvent &e)
{
    const int rowNumber = jmax(0, (e.y - Globals::UI::rollHeaderHeight) / PatternRoll::rowHeight);
    if (rowNumber >= this->getNumRows())
    {
        this->deselectAll(); // just in case
        this->showNewTrackMenu(this->getBeatByXPosition(float(e.x)));
        return;
    }

    const auto &rowKey = this->rows.getReference(rowNumber);
    const auto grouping = this->project.getTrackGroupingMode();

    float nearestClipdistance = FLT_MAX;
    Pattern *targetPattern = nullptr;
    for (const auto *track : this->tracks)
    {
        const auto &trackKey = track->getTrackGroupKey(grouping);
        if (trackKey == rowKey)
        {
            const float clickBeat = this->getBeatByMousePosition(track->getPattern(), e.x);
            for (const auto *clip : track->getPattern()->getClips())
            {
                const auto clipStartDistance = fabs(clickBeat - clip->getBeat());
                if (nearestClipdistance > clipStartDistance)
                {
                    nearestClipdistance = clipStartDistance;
                    targetPattern = track->getPattern();
                }
            }
        }
    }

    if (targetPattern != nullptr)
    {
        const float draggingBeat = this->getBeatByMousePosition(targetPattern, e.x);
        this->addNewClipMode = true;
        this->addClip(targetPattern, draggingBeat);
    }
}

//===----------------------------------------------------------------------===//
// Erasing mode
//===----------------------------------------------------------------------===//

// see the comment above PianoRoll::startErasingEvents for
// the explanation of what's happening in these three methods and why:

void PatternRoll::startErasingEvents(const Point<float> &mousePosition)
{
    this->clipsToEraseOnMouseUp.clearQuick();
    // if we are already pointing at a clip:
    this->continueErasingEvents(mousePosition);
}

void PatternRoll::continueErasingEvents(const Point<float> &mousePosition)
{
    for (const auto &it : this->clipComponents)
    {
        auto *cc = it.second.get();
        if (!cc->isActive() || !cc->isVisible())
        {
            continue;
        }

        if (!cc->getBounds().contains(mousePosition.toInt()))
        {
            continue;
        }

        // duplicates the behavior in onRemoveClip
        this->fader.fadeOut(cc, Globals::UI::fadeOutLong);
        this->selection.deselect(cc);

        // but sets invisible instead of removing
        cc->setVisible(false);
        this->clipsToEraseOnMouseUp.add(cc->getClip());
    }
}

void PatternRoll::endErasingEvents()
{
    if (this->clipsToEraseOnMouseUp.isEmpty())
    {
        return;
    }

    this->project.checkpoint();
    for (const auto &clip : this->clipsToEraseOnMouseUp)
    {
        clip.getPattern()->remove(clip, true);
    }

    this->clipsToEraseOnMouseUp.clearQuick();
}

//===----------------------------------------------------------------------===//
// Knife mode
//===----------------------------------------------------------------------===//

void PatternRoll::startCuttingClips(const Point<float> &mousePosition)
{
    ClipComponent *targetClip = nullptr;
    for (const auto &cc : this->clipComponents)
    {
        if (cc.second.get()->getBounds().contains(mousePosition.toInt()))
        {
            targetClip = cc.second.get();
            break;
        }
    }

    if (this->knifeToolHelper == nullptr && targetClip != nullptr)
    {
        this->knifeToolHelper = make<ClipCutPointMark>(targetClip);
        this->addAndMakeVisible(this->knifeToolHelper.get());

        const float cutBeat = this->getRoundBeatSnapByXPosition(int(mousePosition.x));
        const int beatX = this->getXPositionByBeat(cutBeat);
        this->knifeToolHelper->updatePositionFromMouseEvent(beatX, int(mousePosition.y));
        this->knifeToolHelper->toFront(false);
        this->knifeToolHelper->fadeIn();
    }
}

void PatternRoll::continueCuttingClips(const Point<float> &mousePosition)
{
    if (this->knifeToolHelper == nullptr)
    {
        jassertfalse;
        return;
    }

    const float cutBeat = this->getRoundBeatSnapByXPosition(int(mousePosition.x));
    const int beatX = this->getXPositionByBeat(cutBeat);
    this->knifeToolHelper->updatePositionFromMouseEvent(beatX, int(mousePosition.y));
}

void PatternRoll::endCuttingClipsIfNeeded(bool shouldCut, bool shouldRenameNewTracks)
{
    if (this->knifeToolHelper == nullptr)
    {
        return;
    }

    if (shouldCut)
    {
        const float cutPos = this->knifeToolHelper->getCutPosition();
        const auto *cc = dynamic_cast<ClipComponent *>(this->knifeToolHelper->getComponent());
        if (cc != nullptr && cutPos > 0.f && cutPos < 1.f)
        {
            const float cutBeat = this->getRoundBeatSnapByXPosition(cc->getX() + int(cc->getWidth() * cutPos));
            PatternOperations::cutClip(this->project, cc->getClip(), cutBeat, shouldRenameNewTracks);
        }
    }

    this->knifeToolHelper->updatePosition(-1.f);
    this->knifeToolHelper = nullptr;
}

//===----------------------------------------------------------------------===//
// Merge clips mode
//===----------------------------------------------------------------------===//

void PatternRoll::startMergingEvents(const Point<float> &mousePosition)
{
    this->deselectAll();

    ClipComponent *targetClip = nullptr;
    for (const auto &it : this->clipComponents)
    {
        auto *cc = it.second.get();
        cc->setHighlightedAsMergeTarget(false);
        if (cc->getBounds().contains(mousePosition.toInt()))
        {
            targetClip = cc;
        }
    }

    if (this->mergeToolHelper == nullptr && targetClip != nullptr)
    {
        targetClip->setHighlightedAsMergeTarget(true);
        const auto position = mousePosition / this->getLocalBounds().getBottomRight().toFloat();
        this->mergeToolHelper = make<MergingClipsConnector>(targetClip, position);
        this->addAndMakeVisible(this->mergeToolHelper.get());
    }
}

void PatternRoll::continueMergingEvents(const Point<float> &mousePosition)
{
    if (this->mergeToolHelper == nullptr)
    {
        return;
    }

    ClipComponent *targetClip = nullptr;
    for (const auto &it : this->clipComponents)
    {
        auto *cc = it.second.get();
        cc->setHighlightedAsMergeTarget(false);
        if (cc->getBounds().contains(mousePosition.toInt()) &&
            this->mergeToolHelper->canMergeInto(cc))
        {
            targetClip = cc;
        }
    }

    if (targetClip != nullptr)
    {
        targetClip->setHighlightedAsMergeTarget(true);
    }

    if (auto *sourceClip = dynamic_cast<ClipComponent *>(this->mergeToolHelper->getSourceComponent()))
    {
        sourceClip->setHighlightedAsMergeTarget(true);
    }

    const auto position = mousePosition / this->getLocalBounds().getBottomRight().toFloat();
    this->mergeToolHelper->setTargetComponent(targetClip);
    this->mergeToolHelper->setEndPosition(position);
}

void PatternRoll::endMergingEvents()
{
    if (this->mergeToolHelper == nullptr)
    {
        return;
    }

    for (const auto &it : this->clipComponents)
    {
        it.second.get()->setHighlightedAsMergeTarget(false);
    }

    const auto *sourceCC = dynamic_cast<ClipComponent *>(this->mergeToolHelper->getSourceComponent());
    const auto *targetCC = dynamic_cast<ClipComponent *>(this->mergeToolHelper->getTargetComponent());
    this->mergeToolHelper = nullptr;

    if (sourceCC == nullptr || targetCC == nullptr)
    {
        return;
    }

    PatternOperations::mergeClips(this->project, targetCC->getClip(), { sourceCC->getClip() }, true);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData PatternRoll::serialize() const
{
    using namespace Serialization;
    SerializedData tree(UI::patternRoll);

    tree.setProperty(UI::beatWidth, roundf(this->beatWidth));

    tree.setProperty(UI::startBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX())));

    tree.setProperty(UI::endBeat,
        roundf(this->getRoundBeatSnapByXPosition(this->getViewport().getViewPositionX() +
            this->getViewport().getViewWidth())));

    tree.setProperty(UI::viewportPositionY, this->getViewport().getViewPositionY());

    // m?
    //tree.setProperty(UI::selection, this->getLassoSelection().serialize(), nullptr);

    return tree;
}

void PatternRoll::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    const auto root =
        data.hasType(UI::patternRoll) ?
        data : data.getChildWithName(UI::patternRoll);

    if (!root.isValid())
    {
        return;
    }

    this->setBeatWidth(float(root.getProperty(UI::beatWidth, this->beatWidth)));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBeat = float(root.getProperty(UI::startBeat, 0.f));
    const int x = this->getXPositionByBeat(startBeat);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PatternRoll::reset() {}

//===----------------------------------------------------------------------===//
// Background image cache
//===----------------------------------------------------------------------===//

Image PatternRoll::renderRowsPattern(const HelioTheme &theme, int height)
{
    static const int width = 8;
    const int shadowHeight = PatternRoll::trackHeaderHeight * 2;
    Image patternImage(Image::RGB, width, height, false);
    Graphics g(patternImage);

    const Colour fillColour = theme.findColour(ColourIDs::Roll::blackKey).brighter(0.03f);
    g.setColour(fillColour);
    g.fillRect(patternImage.getBounds());

    // FIXME no hard-coded colours please
    const Colour shadowColour(Colours::black.withAlpha(0.125f));

    int yBase = 0;
    while (yBase < height)
    {
        g.setColour(theme.findColour(ColourIDs::Roll::trackHeaderFill));
        g.fillRect(0, yBase, width, PatternRoll::trackHeaderHeight);

        g.setColour(theme.findColour(ColourIDs::Roll::trackHeaderBorder));
        g.drawHorizontalLine(yBase, 0.f, float(width));
        g.drawHorizontalLine(yBase + PatternRoll::trackHeaderHeight - 1, 0.f, float(width));

        {
            float x = 0, y = float(yBase + PatternRoll::trackHeaderHeight);
            g.setGradientFill(ColourGradient(shadowColour, x, y,
                Colours::transparentBlack, x, float(shadowHeight + y), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + PatternRoll::trackHeaderHeight);
            g.setGradientFill(ColourGradient(shadowColour, x, y,
                Colours::transparentBlack, x, float((shadowHeight / 2) + y), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + height - shadowHeight);
            g.setGradientFill(ColourGradient(Colours::transparentBlack, x, y,
                shadowColour, x, float(height), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + height - shadowHeight / 2);
            g.setGradientFill(ColourGradient(Colours::transparentBlack, x, y,
                shadowColour, x, float(height), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        yBase += PatternRoll::rowHeight;
    }

    HelioTheme::drawNoise(theme, g, 1.75f);
    return patternImage;
}

void PatternRoll::repaintBackgroundsCache()
{
    const auto &theme = HelioTheme::getCurrentTheme();
    this->rowPattern = PatternRoll::renderRowsPattern(theme, PatternRoll::rowHeight * 8);
}

void PatternRoll::showNewTrackMenu(float beatToInsertAt)
{
    const auto &instruments = App::Workspace().getAudioCore().getInstruments();
    if (instruments.size() > 1)
    {
        MenuPanel::Menu menu;
        for (const auto *instrument : instruments)
        {
            const String instrumentId = instrument->getIdAndHash();
            menu.add(MenuItem::item(Icons::pianoTrack,
                TRANS(I18n::Menu::Project::addTrack) + ": " + instrument->getName())->
                withAction([this, instrumentId, beatToInsertAt]()
            {
                if (auto *modal = Component::getCurrentlyModalComponent(0))
                {
                    modal->exitModalState(0);
                }

                this->showNewTrackDialog(instrumentId, beatToInsertAt);
            }));
        }

        auto panel = make<MenuPanel>();
        panel->updateContent(menu, MenuPanel::AnimationType::Fading);
        HelioCallout::emit(panel.release(), this, true);
    }
    else
    {
        this->showNewTrackDialog(instruments.getFirst()->getIdAndHash(), beatToInsertAt);
    }
}

void PatternRoll::showNewTrackDialog(const String &instrumentId, float beatToInsertAt)
{
    this->project.getUndoStack()->beginNewTransaction(UndoActionIDs::AddNewTrack);

    String outTrackId;
    const auto trackTemplate =
        SequencerOperations::createPianoTrackTemplate(project,
            TRANS(I18n::Defaults::midiTrackName),
            beatToInsertAt, instrumentId, outTrackId);

    InteractiveActions::addNewTrack<PianoTrackInsertAction>(this->project,
        trackTemplate, outTrackId, TRANS(I18n::Defaults::midiTrackName), true,
        UndoActionIDs::AddNewTrack, TRANS(I18n::Dialog::addTrackCaption),
        false);
}
