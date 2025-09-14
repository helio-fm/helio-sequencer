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
#include "PatternRoll.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "PluginWindow.h"
#include "ModalCallout.h"
#include "MenuPanel.h"

#include "Pattern.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "LassoListeners.h"
#include "SmoothPanController.h"
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

#include "TempoDialog.h"
#include "TrackPropertiesDialog.h"
#include "TimeSignatureDialog.h"
#include "ProjectTimeline.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "ClipComponent.h"
#include "PianoClipComponent.h"
#include "AutomationCurveClipComponent.h"
#include "AutomationStepsClipComponent.h"

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

    this->contextMenuController->onWillShowMenu = [this]()
    {
        auto *componentUnderMouse = Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();
        if (auto *clipComponent = dynamic_cast<ClipComponent *>(componentUnderMouse))
        {
            if (!clipComponent->isSelected())
            {
                this->selectEvent(clipComponent, true);
                // listeners should react immediately, not asynchronously:
                this->selection.dispatchPendingMessages();
            }
        }
    };

    this->repaintBackgroundsCache();
    this->reloadRollContent();
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

    if (dynamic_cast<PianoSequence *>(sequence))
    {
        return new PianoClipComponent(project, sequence, roll, clip);
    }
    else if (dynamic_cast<AutomationSequence *>(sequence))
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
                    this->triggerBatchRepaintFor(clipComponent);
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

    // on small screens pretend we have a couple more rows
    // so that the canvas can be scrolled up to make the bottom rows
    // available if they are hidden by the bottom editor panels:
    const auto numRows = App::isRunningOnPhone() ?
         this->getNumRows() + 2 : this->getNumRows();

    const int h = Globals::UI::rollHeaderHeight +
        numRows * PatternRoll::rowHeight + addTrackHelper;

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
    return this->getEventBounds(cc->getClip());
}

Rectangle<float> PatternRoll::getEventBounds(const Clip &clip) const
{
    const auto *track = clip.getPattern()->getTrack();
    const auto *sequence = track->getSequence();
    jassert(sequence != nullptr);

    const auto grouping = this->project.getTrackGroupingMode();
    const auto trackGroupKey = track->getTrackGroupKey(grouping);
    const int trackIndex = this->rows.indexOfSorted(kStringSort, trackGroupKey);

    const float x = this->beatWidth * (sequence->getFirstBeat() + clip.getBeat() - this->firstBeat);
    const float y = float(trackIndex * PatternRoll::rowHeight);

    // - in case there are no events, still display an empty clip of some default length,
    // also make sure that the clip isn't too short even if the sequence is (e.g. first moments when recording);
    // - calculating the width like (lastBeat - floor(firstBeat)) instead of (beatWidth * sequenceLength)
    // to place clip edges precisely on bar lines on higher zoom levels and avoid nasty fluttering:
    const float w = sequence->isEmpty() ?
        (this->beatWidth * Globals::Defaults::emptyClipLength) :
        jmax(this->beatWidth * Globals::minClipLength,
            this->beatWidth * (sequence->getLastBeat() + clip.getBeat() - this->firstBeat) - floorf(x));

    constexpr auto verticalMargin = 1;
    return Rectangle<float>(x,
        Globals::UI::rollHeaderHeight + PatternRoll::trackHeaderHeight + y + verticalMargin,
        w,
        float(PatternRoll::clipHeight - (verticalMargin * 2)));
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
    if (track->getPattern() != nullptr)
    {
        //the track name could change here so we have to keep this array sorted by name:
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

    if (auto *pattern = track->getPattern())
    {
        for (int i = 0; i < pattern->size(); ++i)
        {
            const auto &clip = *pattern->getUnchecked(i);
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

    if (clip.isSoloed()) // solo flags affect everyone's appearance
    {
        for (const auto &it : this->clipComponents)
        {
            this->triggerBatchRepaintFor(it.second.get());
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

    if (clip.isSoloed() != newClip.isSoloed())
    {
        for (const auto &it : this->clipComponents)
        {
            this->triggerBatchRepaintFor(it.second.get());
        }
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

    if (clip.isSoloed())
    {
        for (const auto &it : this->clipComponents)
        {
            this->triggerBatchRepaintFor(it.second.get());
        }
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
// DrawableLassoSource
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
        if (component->isActiveAndEditable() &&
            component->getBeat() >= startBeat &&
            component->getBeat() < endBeat)
        {
            this->selection.addToSelection(component);
        }
    }
}

void PatternRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
    const Rectangle<int> &rectangle)
{
    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
        if (component->isActiveAndEditable() &&
            rectangle.intersects(component->getBounds()))
        {
            jassert(!itemsFound.contains(component));
            itemsFound.add(component);
        }
    }
}

void PatternRoll::findLassoItemsInPolygon(Array<SelectableComponent *> &itemsFound,
    const Rectangle<int> &bounds, const Array<Point<float>> &polygon)
{
    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
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

void PatternRoll::onLongTap(const Point<float> &position, const WeakReference<Component> &target)
{
    if (this->multiTouchController->hasMultiTouch())
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

    RollBase::onLongTap(position, target);
}

void PatternRoll::zoomRelative(const Point<float> &origin,
    const Point<float> &factor, bool isInertial)
{
    ROLL_BATCH_REPAINT_START
    RollBase::zoomRelative(origin, factor, isInertial);
    ROLL_BATCH_REPAINT_END
}

void PatternRoll::zoomAbsolute(const Rectangle<float> &proportion)
{
    jassert(!proportion.isEmpty());
    jassert(proportion.isFinite());

    ROLL_BATCH_REPAINT_START
    RollBase::zoomAbsolute(proportion);
    ROLL_BATCH_REPAINT_END
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void PatternRoll::mouseDown(const MouseEvent &e)
{
    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
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
    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
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
    if (this->isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
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
    case CommandIDs::ViewportPanUp:
        this->smoothPanController->panByOffset({ 0.f, -float(PatternRoll::rowHeight) });
        break;
    case CommandIDs::ViewportPanDown:
        this->smoothPanController->panByOffset({ 0.f, float(PatternRoll::rowHeight) });
        break;
    case CommandIDs::SelectAllClips:
        this->selectAll();
        break;
    case CommandIDs::RenameTrack:
        if (this->selection.getNumSelected() > 0)
        {
            Array<WeakReference<MidiTrack>> tracks;
            for (int i = 0; i < this->selection.getNumSelected(); ++i)
            {
                auto *track = this->selection.getItemAs<ClipComponent>(i)->getClip().getPattern()->getTrack();
                tracks.addIfNotAlreadyThere(track);
            }

            App::showModalComponent(make<TrackPropertiesDialog>(this->project, tracks));
        }
        break;
    case CommandIDs::SetTrackTimeSignature:
        if (this->selection.getNumSelected() == 1)
        {
            const auto clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            if (auto *track = this->project.findTrackById<MidiTrackNode>(clip.getTrackId()))
            {
                App::showModalComponent(TimeSignatureDialog::editingDialog(*this,
                    this->project, *track->getTimeSignatureOverride()));
            }
        }
        break;
    case CommandIDs::DuplicateTrack:        // the implementation for these two
    case CommandIDs::InstanceToUniqueTrack: // is pretty much the same code
        if (this->selection.getNumSelected() == 1)
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

            const auto undoActionId = altMode ?
                UndoActionIDs::MakeUniqueTrack : UndoActionIDs::DuplicateTrack;

            this->project.getUndoStack()->beginNewTransaction(undoActionId);

            if (altMode)
            {
                jassert(this->selection.getNumSelected() == 1);
                PatternOperations::deleteSelection(this->selection, this->project, false);
            }

            const bool generateNewName = !altMode;
            InteractiveActions::addNewTrack(this->project,
                move(trackPreset), clonedTrack->getTrackName(), generateNewName,
                undoActionId,
                altMode ? TRANS(I18n::Menu::trackMakeUnique) : TRANS(I18n::Menu::trackDuplicate),
                false);
        }
        break;
    // the next 3 commands will first try to change only the selected tempo tracks, if any
    case CommandIDs::TrackSetOneTempo:
        if (this->selection.getNumSelected() == 1)
        {
            const auto &clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            auto *track = clip.getPattern()->getTrack();
            if (!track->isTempoTrack())
            {
                this->postCommandMessage(CommandIDs::ProjectSetOneTempo);
                break;
            }
            if (auto *autoSequence = dynamic_cast<AutomationSequence *>(track->getSequence()))
            {
                const auto firstBeat = autoSequence->getFirstBeat();
                const auto lastBeat = jmax(autoSequence->getLastBeat(), firstBeat + Globals::Defaults::emptyClipLength);
                const auto avgValue = autoSequence->getAverageControllerValue();
                const auto avgMsPerQuarterNote = Transport::getTempoByControllerValue(avgValue) / 1000;
                const auto avgBpm = 60000 / jmax(1, avgMsPerQuarterNote);

                auto dialog = make<TempoDialog>(avgBpm);
                dialog->onOk = [firstBeat, lastBeat, track](int newBpmValue)
                {
                    SequencerOperations::setOneTempoForTrack(track, firstBeat, lastBeat, newBpmValue);
                };

                App::showModalComponent(move(dialog));
            }
            else
            {
                this->postCommandMessage(CommandIDs::ProjectSetOneTempo);
            }
        }
        else
        {
            this->postCommandMessage(CommandIDs::ProjectSetOneTempo);
        }
        break;
    case CommandIDs::TempoUp1Bpm:
        if (!PatternOperations::shiftTempo(this->selection, +1))
        {
            this->postCommandMessage(CommandIDs::ProjectTempoUp1Bpm);
        }
        break;
    case CommandIDs::TempoDown1Bpm:
        if (!PatternOperations::shiftTempo(this->selection, -1))
        {
            this->postCommandMessage(CommandIDs::ProjectTempoDown1Bpm);
        }
        break;
    case CommandIDs::EditCurrentInstrument:
    {
        auto currentInstrumentId = PatternOperations::getSelectedInstrumentId(this->selection);

        if (currentInstrumentId.isEmpty())
        {
            currentInstrumentId = this->lastShownInstrumentId;
        }

        if (currentInstrumentId.isEmpty() && !this->tracks.isEmpty())
        {
            currentInstrumentId = this->tracks.getFirst()->getTrackInstrumentId();
        }

        if (PluginWindow::showWindowFor(currentInstrumentId))
        {
            this->lastShownInstrumentId = currentInstrumentId;
        }
    }
        break;
    case CommandIDs::DeleteClips:
        PatternOperations::deleteSelection(this->selection, this->project);
        break;
    case CommandIDs::ZoomEntireClip:
        if (this->selection.getNumSelected() == 1)
        {
            const auto &clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            this->project.setEditableScope(clip, true);
        }
        else if (auto *clipComponentUnderMouse = dynamic_cast<PianoClipComponent *>
            (Desktop::getInstance().getMainMouseSource().getComponentUnderMouse()))
        {
            this->project.setEditableScope(clipComponentUnderMouse->getClip(), true);
        }
        break;
    case CommandIDs::ClipTransposeUp:
        PatternOperations::transposeClips(this->selection, 1);
        break;
    case CommandIDs::ClipTransposeDown:
        PatternOperations::transposeClips(this->selection, -1);
        break;
    case CommandIDs::ClipTransposeOctaveUp:
        PatternOperations::transposeClips(this->selection,
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave), true, true);
        break;
    case CommandIDs::ClipTransposeOctaveDown:
        PatternOperations::transposeClips(this->selection,
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectOctave), true, true);
        break;
    case CommandIDs::ClipTransposeFifthUp:
        PatternOperations::transposeClips(this->selection,
            this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth), true, true);
        break;
    case CommandIDs::ClipTransposeFifthDown:
        PatternOperations::transposeClips(this->selection,
            -this->temperament->getEquivalentOfTwelveToneInterval(Semitones::PerfectFifth), true, true);
        break;
    case CommandIDs::ClipVolumeUp:
        PatternOperations::tuneClips(this->selection, 1.f / 32.f);
        break;
    case CommandIDs::ClipVolumeDown:
        PatternOperations::tuneClips(this->selection, -1.f / 32.f);
        break;
    case CommandIDs::BeatShiftLeft:
        PatternOperations::shiftBeatRelative(this->selection, -this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::BeatShiftRight:
        PatternOperations::shiftBeatRelative(this->selection, this->getMinVisibleBeatForCurrentZoomLevel());
        break;
    case CommandIDs::Retrograde:
        PatternOperations::retrograde(this->project, this->selection);
        break;
    case CommandIDs::ToggleMuteClips:
        PatternOperations::toggleMuteClips(this->selection);
        break;
    case CommandIDs::ToggleSoloClips:
        PatternOperations::toggleSoloClips(this->selection);
        break;
    case CommandIDs::ToggleMuteModifiers:
        PatternOperations::toggleMuteModifiersStack(this->selection);
        break;
    case CommandIDs::ToggleLoopOverSelection:
        if (this->selection.getNumSelected() > 0)
        {
            const auto startBeat = PatternOperations::findStartBeat(this->selection);
            const auto endBeat = PatternOperations::findEndBeat(this->selection);
            this->getTransport().togglePlaybackLoop(startBeat, endBeat);
        }
        else // no selection, loop the entire project:
        {
            this->getTransport().togglePlaybackLoop(this->projectFirstBeat, this->projectLastBeat);
        }
        break;
    case CommandIDs::ShowNewTrackPanel:
        // inserting at the playhead position:
        this->showNewTrackMenu(this->lastPlayheadBeat.get());
        break;
    case CommandIDs::ToggleVolumePanel:
        App::Config().getUiFlags()->toggleEditorPanelVisibility();
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
        PatternOperations::quantize(this->selection, 1.f);
        break;
    case CommandIDs::QuantizeTo1_2:
        PatternOperations::quantize(this->selection, 2.f);
        break;
    case CommandIDs::QuantizeTo1_4:
        PatternOperations::quantize(this->selection, 4.f);
        break;
    case CommandIDs::QuantizeTo1_8:
        PatternOperations::quantize(this->selection, 8.f);
        break;
    case CommandIDs::QuantizeTo1_16:
        PatternOperations::quantize(this->selection, 16.f);
        break;
    case CommandIDs::QuantizeTo1_32:
        PatternOperations::quantize(this->selection, 32.f);
        break;
    default:
        break;
    }

    RollBase::handleCommandMessage(commandId);
}

void PatternRoll::resized()
{
    if (!this->isEnabled())
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

void PatternRoll::paint(Graphics &g) noexcept
{
    g.setImageResamplingQuality(Graphics::lowResamplingQuality);

    const auto viewArea = this->viewport.getViewArea();

    // just because we cannot rely on OpenGL tiling:
    for (int i = Globals::UI::rollHeaderHeight;
        i < viewArea.getBottom();
        i += (PatternRoll::rowPatternHeight / 2))
    {
        g.setFillType({ this->rowPattern, AffineTransform::translation(0.f, float(i)) });
        g.fillRect(viewArea.getX(), i, viewArea.getWidth(), PatternRoll::rowPatternHeight);
    }

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

void PatternRoll::updateAllSnapLines()
{
    // PianoRoll is happy with the base implementation of computeAllSnapLines,
    // but PatternRoll wants to append those snaps with clips' right edges:
    // most of the time it will have no visible effect, but may come in handy
    // e.g. when doing some complex rhythms and dragging the clips around:

    RollBase::updateAllSnapLines();

    const auto paintStartX = this->viewport.getViewPositionX();
    const auto paintEndX = paintStartX + this->viewport.getViewWidth();

    for (const auto &it : this->clipComponents)
    {
        const auto &clip = it.first;
        const auto clipEndX = it.second->getRight();

        if (clipEndX > paintStartX && clipEndX < paintEndX)
        {
            this->allSnaps.add(roundBeat(this->beatWidth *
                (clip.getPattern()->getTrack()->getSequence()->getLastBeat() + clip.getBeat() - this->firstBeat)));
        }
    }
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
        if (!cc->isActiveAndEditable() || !cc->isVisible())
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

    PatternOperations::deleteSelection(this->clipsToEraseOnMouseUp, this->project, true);

    this->clipsToEraseOnMouseUp.clearQuick();
}

//===----------------------------------------------------------------------===//
// Knife mode
//===----------------------------------------------------------------------===//

void PatternRoll::startCuttingClips(const Point<float> &mousePosition)
{
    ClipComponent *targetComponent = nullptr;
    for (const auto &cc : this->clipComponents)
    {
        if (cc.second.get()->getBounds().contains(mousePosition.toInt()))
        {
            targetComponent = cc.second.get();
            break;
        }
    }

    if (this->knifeToolHelper == nullptr && targetComponent != nullptr)
    {
        this->knifeToolHelper = make<ClipCutPointMark>(targetComponent, targetComponent->getClip());
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

    const float startBeat = float(root.getProperty(UI::startBeat, 0.f));
    const int x = this->getXPositionByBeat(startBeat);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);
}

void PatternRoll::reset() {}

//===----------------------------------------------------------------------===//
// Background image cache
//===----------------------------------------------------------------------===//

Image PatternRoll::renderRowsPattern(const HelioTheme &theme)
{
    const int shadowHeight = PatternRoll::trackHeaderHeight * 2;
    constexpr int width = 64;
    Image patternImage(Image::RGB, width, PatternRoll::rowPatternHeight, false);
    Graphics g(patternImage);

    g.setColour(theme.findColour(ColourIDs::Roll::patternRowFill));
    g.fillRect(patternImage.getBounds());

    HelioTheme::drawNoise(theme, g);

    const auto shadowColour = theme.findColour(ColourIDs::Roll::trackHeaderShadow);
    const auto shadowColourLight = shadowColour.withMultipliedAlpha(0.25f);

    int yBase = 0;
    while (yBase < PatternRoll::rowPatternHeight)
    {
        g.setColour(theme.findColour(ColourIDs::Roll::trackHeaderFill));
        g.fillRect(0, yBase, width, PatternRoll::trackHeaderHeight);

        g.setColour(theme.findColour(ColourIDs::Roll::trackHeaderBorderLight));
        g.drawHorizontalLine(yBase, 0.f, float(width));
        g.drawHorizontalLine(yBase + PatternRoll::trackHeaderHeight - 1, 0.f, float(width));

        g.setColour(theme.findColour(ColourIDs::Roll::trackHeaderBorderDark));
        g.drawHorizontalLine(yBase + PatternRoll::rowHeight - 1, 0.f, float(width));
        g.drawHorizontalLine(yBase + PatternRoll::trackHeaderHeight, 0.f, float(width));

        {
            float x = 0, y = float(yBase + PatternRoll::trackHeaderHeight);
            g.setGradientFill(ColourGradient(shadowColour, x, y,
                Colours::transparentBlack, x, float(y + shadowHeight), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + PatternRoll::trackHeaderHeight);
            g.setGradientFill(ColourGradient(shadowColour, x, y,
                Colours::transparentBlack, x, float(y + (shadowHeight / 2)), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + PatternRoll::rowHeight - shadowHeight);
            g.setGradientFill(ColourGradient(Colours::transparentBlack, x, y,
                shadowColourLight, x, float(y + shadowHeight), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        {
            float x = 0, y = float(yBase + PatternRoll::rowHeight - shadowHeight / 2);
            g.setGradientFill(ColourGradient(Colours::transparentBlack, x, y,
                shadowColourLight, x, float(y + (shadowHeight / 2)), false));
            g.fillRect(int(x), int(y), width, shadowHeight);
        }

        yBase += PatternRoll::rowHeight;
    }

    return patternImage;
}

void PatternRoll::repaintBackgroundsCache()
{
    const auto &theme = HelioTheme::getCurrentTheme();
    this->rowPattern = PatternRoll::renderRowsPattern(theme);
}

void PatternRoll::showNewTrackMenu(float beatToInsertAt)
{
    const auto instruments = App::Workspace().getAudioCore().getInstrumentsExceptInternal();
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
        ModalCallout::emit(panel.release(), this, true);
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
