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
#include "HybridRollHeader.h"
#include "MidiTrackHeader.h"
#include "Pattern.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "VersionControlTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "ProjectTreeItem.h"
#include "ProjectTimeline.h"
#include "ClipComponent.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "HelioTheme.h"
#include "ChordBuilder.h"
#include "SelectionComponent.h"
#include "HybridRollEditMode.h"
#include "SerializationKeys.h"
#include "ModalDialogInput.h"
#include "NotesTuningPanel.h"
#include "SequencerOperations.h"
#include "SerializationKeys.h"
#include "PianoSequence.h"
#include "PianoClipComponent.h"
#include "AutomationSequence.h"
#include "AutomationClipComponent.h"
#include "DummyClipComponent.h"
#include "LassoListeners.h"

#include "ComponentIDs.h"
#include "ColourIDs.h"
#include "Config.h"
#include "Icons.h"
#include "App.h"

#define DEFAULT_CLIP_LENGTH 1.0f

inline static constexpr int rowHeight()
{
    return PATTERN_ROLL_CLIP_HEIGHT + PATTERN_ROLL_TRACK_HEADER_HEIGHT;
}

//void dumpDebugInfo(Array<MidiTrack *> tracks)
//{
//    Logger::writeToLog("--- tracks:");
//    for (int i = 0; i < tracks.size(); i++)
//    {
//        Logger::writeToLog(tracks[i]->getTrackName());
//    }
//}

PatternRoll::PatternRoll(ProjectTreeItem &parentProject,
    Viewport &viewportRef,
    WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(parentProject, viewportRef, clippingDetector, false, false, true),
    newClipDragging(nullptr),
    addNewClipMode(false)
{
    // TODO: pattern roll doesn't need neither annotations track map nor key signatures track map
    this->selectedClipsMenuManager = new PatternRollSelectionMenuManager(&this->selection);

    this->setComponentID(ComponentIDs::patternRollId);

    this->insertTrackHelper = new MidiTrackHeader(nullptr);
    this->addAndMakeVisible(this->insertTrackHelper);

    this->repaintBackgroundsCache();
    this->reloadRollContent();
    this->setBarRange(0, 8);
}

void PatternRoll::selectAll()
{
    for (const auto &e : this->clipComponents)
    {
        this->selection.addToSelection(e.second.get());
    }
}

void PatternRoll::zoomViewToClip(const Clip &clip) const
{
    // TODO
    //const Pattern *pattern = clip.getPattern();
    //const MidiTrack *track = pattern->getTrack();
    //const MidiSequence *sequence = track->getSequence();
}

void PatternRoll::reloadRollContent()
{
    this->selection.deselectAll();

    this->trackHeaders.clear();
    this->clipComponents.clear();

    this->tracks.clearQuick();

    for (auto *track : this->project.getTracks())
    {
        auto *sequence = track->getSequence();
        jassert(sequence != nullptr);

        // Only show tracks with patterns (i.e. ignore timeline tracks)
        if (const auto *pattern = track->getPattern())
        {
            this->tracks.addSorted(*track, track);

            MidiTrackHeader *const trackHeader = new MidiTrackHeader(track);
            this->trackHeaders[track] = UniquePointer<MidiTrackHeader>(trackHeader);
            this->addAndMakeVisible(trackHeader);

            for (int j = 0; j < pattern->size(); ++j)
            {
                const Clip &clip = *pattern->getUnchecked(j);
                ClipComponent *clipComponent = nullptr;

                if (auto *pianoLayer = dynamic_cast<PianoSequence *>(sequence))
                {
                    clipComponent = new PianoClipComponent(track, *this, clip);
                }
                else if (auto *autoLayer = dynamic_cast<AutomationSequence *>(sequence))
                {
                    clipComponent = new AutomationClipComponent(track, *this, clip);
                }

                if (clipComponent != nullptr)
                {
                    this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
                    this->addAndMakeVisible(clipComponent);
                }
            }
        }
    }

    this->updateRollSize();
    this->repaint(this->viewport.getViewArea());
}

int PatternRoll::getNumRows() const noexcept
{
    return this->tracks.size();
}

//===----------------------------------------------------------------------===//
// HybridRoll
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
    const int addTrackHelper = PATTERN_ROLL_TRACK_HEADER_HEIGHT;
    const int h = HYBRID_ROLL_HEADER_HEIGHT + this->getNumRows() * rowHeight() + addTrackHelper;
    this->setSize(this->getWidth(), jmax(h, this->viewport.getHeight()));
}

void PatternRoll::updateChildrenBounds()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const int viewX = this->getViewport().getViewPositionX();
    const int viewW = this->getViewport().getViewWidth();

    const int insertTrackHelperY = this->tracks.size() * rowHeight();
    this->insertTrackHelper->setBounds(viewX,
        HYBRID_ROLL_HEADER_HEIGHT + insertTrackHelperY,
        viewW, PATTERN_ROLL_TRACK_HEADER_HEIGHT);

    for (const auto &e : this->trackHeaders)
    {
        const auto component = e.second.get();
        const auto track = component->getTrack();
        const int trackIndex = this->tracks.indexOfSorted(*track, track);
        const int y = trackIndex * rowHeight();
        component->setBounds(viewX, HYBRID_ROLL_HEADER_HEIGHT + y,
            viewW, PATTERN_ROLL_TRACK_HEADER_HEIGHT);
    }

    HybridRoll::updateChildrenBounds();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PatternRoll::updateChildrenPositions()
{
    HYBRID_ROLL_BULK_REPAINT_START

    const int viewX = this->getViewport().getViewPositionX();

    const int insertTrackHelperY = this->tracks.size() * rowHeight();
    this->insertTrackHelper->setTopLeftPosition(viewX,
        HYBRID_ROLL_HEADER_HEIGHT + insertTrackHelperY);

    for (const auto &e : this->trackHeaders)
    {
        const auto component = e.second.get();
        const auto track = component->getTrack();
        const int trackIndex = this->tracks.indexOfSorted(*track, track);
        const int y = trackIndex * rowHeight();
        component->setTopLeftPosition(viewX, HYBRID_ROLL_HEADER_HEIGHT + y);
    }

    HybridRoll::updateChildrenPositions();

    HYBRID_ROLL_BULK_REPAINT_END
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PatternRoll::showGhostClipFor(ClipComponent *targetClipComponent)
{
    auto component = new DummyClipComponent(*this, targetClipComponent->getClip());
    component->setEnabled(false);
    component->setGhostMode();

    this->addAndMakeVisible(component);
    this->ghostClips.add(component);

    this->batchRepaintList.add(component);
    this->triggerAsyncUpdate();
}

void PatternRoll::hideAllGhostClips()
{
    for (int i = 0; i < this->ghostClips.size(); ++i)
    {
        this->fader.fadeOut(this->ghostClips.getUnchecked(i), 100);
    }

    this->ghostClips.clear();
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
    ClipComponent *cc = static_cast<ClipComponent *>(mc);
    return this->getEventBounds(cc->getClip(), cc->getBeat());
}

Rectangle<float> PatternRoll::getEventBounds(const Clip &clip, float clipBeat) const
{
    const Pattern *pattern = clip.getPattern();
    const MidiTrack *track = pattern->getTrack();
    const MidiSequence *sequence = track->getSequence();
    jassert(sequence != nullptr);

    const int trackIndex = this->tracks.indexOfSorted(*track, track);
    const float viewStartOffsetBeat = float(this->firstBar * BEATS_PER_BAR);
    const float sequenceOffset = sequence->size() > 0 ? sequence->getFirstBeat() : 0.f;

    // In case there are no events, still display a clip of some default length:
    const float sequenceLength = jmax(sequence->getLengthInBeats(), float(BEATS_PER_BAR));

    const float w = this->barWidth * sequenceLength / BEATS_PER_BAR;
    const float x = this->barWidth * (sequenceOffset + clipBeat - viewStartOffsetBeat) / BEATS_PER_BAR;
    const float y = float(trackIndex * rowHeight());

    return Rectangle<float>(x,
        HYBRID_ROLL_HEADER_HEIGHT + y + PATTERN_ROLL_TRACK_HEADER_HEIGHT,
        w, float(PATTERN_ROLL_CLIP_HEIGHT - 1));
}

float PatternRoll::getBeatForClipByXPosition(const Clip &clip, float x) const
{
    // One trick here is that displayed clip position depends on a sequence's first beat as well:
    const Pattern *pattern = clip.getPattern();
    const MidiTrack *track = pattern->getTrack();
    const MidiSequence *sequence = track->getSequence();
    const float sequenceOffset = sequence->size() > 0 ? sequence->getFirstBeat() : 0.f;
    return this->getRoundBeatByXPosition(int(x)) - sequenceOffset; /* - 0.5f ? */
}

float PatternRoll::getBeatByMousePosition(const Pattern *pattern, int x) const
{
    const MidiTrack *track = pattern->getTrack();
    const MidiSequence *sequence = track->getSequence();
    const float sequenceOffset = sequence->size() > 0 ? sequence->getFirstBeat() : 0.f;
    return this->getFloorBeatByXPosition(x) - sequenceOffset;
}

Pattern *PatternRoll::getPatternByMousePosition(int y) const
{
    const int patternIndex = jlimit(0,
        this->getNumRows() - 1,
        (y - HYBRID_ROLL_HEADER_HEIGHT) / rowHeight());

    return this->tracks.getUnchecked(patternIndex)->getPattern();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PatternRoll::onAddTrack(MidiTrack *const track)
{
    if (Pattern *pattern = track->getPattern())
    {
        auto sequence = track->getSequence();
        jassert(sequence != nullptr);

        this->tracks.addSorted(*track, track);

        MidiTrackHeader *const trackHeader = new MidiTrackHeader(track);
        this->trackHeaders[track] = UniquePointer<MidiTrackHeader>(trackHeader);
        this->addAndMakeVisible(trackHeader);

        for (int j = 0; j < pattern->size(); ++j)
        {
            const Clip &clip = *pattern->getUnchecked(j);
            ClipComponent *clipComponent = nullptr;

            if (auto pianoLayer = dynamic_cast<PianoSequence *>(sequence))
            {
                clipComponent = new PianoClipComponent(track, *this, clip);
            }
            else if (auto autoLayer = dynamic_cast<AutomationSequence *>(sequence))
            {
                clipComponent = new AutomationClipComponent(track, *this, clip);
            }

            if (clipComponent != nullptr)
            {
                this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
                this->addAndMakeVisible(clipComponent);
            }
        }

        this->resized();
    }
}

//void debugTracksOrder(Array<MidiTrack *> tracks)
//{
//    Logger::writeToLog("---------------------");
//    for (const auto *track : tracks)
//    {
//        Logger::writeToLog(track->getTrackName());
//    }
//}

void PatternRoll::onChangeTrackProperties(MidiTrack *const track)
{
    if (MidiTrackHeader *header = this->trackHeaders[track].get())
    {
        if (header->getTrack() == track)
        {
            header->updateContent();
        }
    }

    if (Pattern *pattern = track->getPattern())
    {
        // track name could change here so we have to keep track array sorted by name:
        //debugTracksOrder(this->tracks);
        this->tracks.sort(*track);
        //debugTracksOrder(this->tracks);

        // TODO only repaint clips of a changed track?
        for (const auto &e : this->clipComponents)
        {
            const auto component = e.second.get();
            component->updateColours();
        }
    }

    this->resized();
}

void PatternRoll::onRemoveTrack(MidiTrack *const track)
{
    this->selection.deselectAll();
    this->hideAllGhostClips();

    this->tracks.removeAllInstancesOf(track);

    if (MidiTrackHeader *deletedHeader = this->trackHeaders[track].get())
    {
        this->trackHeaders.erase(track);
    }

    if (Pattern *pattern = track->getPattern())
    {
        for (int i = 0; i < pattern->size(); ++i)
        {
            const Clip &clip = *pattern->getUnchecked(i);
            if (const auto deletedComponent = this->clipComponents[clip].get())
            {
                this->selection.deselect(deletedComponent);
                this->clipComponents.erase(clip);
            }
        }

        this->updateRollSize();
    }
}

void PatternRoll::onAddClip(const Clip &clip)
{
    ClipComponent *clipComponent = nullptr;
    auto track = clip.getPattern()->getTrack();
    auto sequence = track->getSequence();

    if (dynamic_cast<PianoSequence *>(sequence))
    {
        clipComponent = new PianoClipComponent(track, *this, clip);
    }
    else if (dynamic_cast<AutomationSequence *>(sequence))
    {
        clipComponent = new AutomationClipComponent(track, *this, clip);
    }

    if (clipComponent != nullptr)
    {
        this->clipComponents[clip] = UniquePointer<ClipComponent>(clipComponent);
        this->addAndMakeVisible(clipComponent);
        clipComponent->toFront(false);

        this->fader.fadeIn(clipComponent, 150);

        this->batchRepaintList.add(clipComponent);
        this->triggerAsyncUpdate();

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

        this->batchRepaintList.add(component);
        this->triggerAsyncUpdate();
    }
}

void PatternRoll::onRemoveClip(const Clip &clip)
{
    if (const auto deletedComponent = this->clipComponents[clip].get())
    {
        this->hideAllGhostClips();

        this->fader.fadeOut(deletedComponent, 150);
        this->selection.deselect(deletedComponent);
        this->clipComponents.erase(clip);
    }
}

void PatternRoll::onPostRemoveClip(Pattern *const pattern)
{
    //
}

void PatternRoll::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
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
    bool shouldInvalidateSelectionCache = false;

    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
        component->setSelected(this->selection.isSelected(component));
    }

    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
        if (rectangle.intersects(component->getBounds()) && component->isActive())
        {
            shouldInvalidateSelectionCache = true;
            itemsFound.addIfNotAlreadyThere(component);
        }
    }

    if (shouldInvalidateSelectionCache)
    {
        this->selection.invalidateCache();
    }
}

//===----------------------------------------------------------------------===//
// SmoothZoomListener
//===----------------------------------------------------------------------===//

void PatternRoll::zoomRelative(const Point<float> &origin, const Point<float> &factor)
{
    const float yZoomThreshold = 0.005f;
    if (fabs(factor.getY()) > yZoomThreshold)
    {
        // TODO: should we zoom rows?
    }

    HybridRoll::zoomRelative(origin, factor);
}

void PatternRoll::zoomAbsolute(const Point<float> &zoom)
{
    // TODO: should we zoom rows?
    HybridRoll::zoomAbsolute(zoom);
}

float PatternRoll::getZoomFactorY() const
{
    const float &viewHeight = float(this->viewport.getViewHeight());
    return (viewHeight / float(this->getHeight()));
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
    
    if (! this->isUsingSpaceDraggingMode())
    {
        this->setInterceptsMouseClicks(true, false);

        if (this->isAddEvent(e))
        {
            this->insertNewClipAt(e);
        }
    }

    HybridRoll::mouseDown(e);
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
            this->setMouseCursor(MouseCursor(MouseCursor::DraggingHandCursor));
        }
    }

    HybridRoll::mouseDrag(e);
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

void PatternRoll::handleCommandMessage(int commandId)
{
    // TODO pattern roll-specific commands switch
    switch (commandId)
    {
    case CommandIDs::RenameTrack:
        if (auto *patternNode = dynamic_cast<PatternEditorTreeItem *>(this->project.findPrimaryActiveItem()))
        {
            // TODO check if all items in selection belong to one track
            // and if they do, find according tree node and rename it
            //this->selection.getFirstAs()

            //auto inputDialog = ModalDialogInput::Presets::renameTrack(patternNode->getXPath());
            //inputDialog->onOk = trackNode->getRenameCallback();
            //App::Layout().showModalComponentUnowned(inputDialog.release());
        }
        break;
    case CommandIDs::DeleteClips:
        PatternOperations::deleteSelection(this->getLassoSelection());
        break;
    case CommandIDs::EditClip:
        if (this->selection.getNumSelected() > 0)
        {
            const auto &clip = this->selection.getFirstAs<ClipComponent>()->getClip();
            this->project.setEditableScope(clip.getPattern()->getTrack(), clip, true);
        }
        break;
    //case CommandIDs::BeatShiftLeft:
    //    PatternOperations::shiftBeatRelative(this->getLassoSelection(), -1.f / BEATS_PER_BAR);
    //    break;
    //case CommandIDs::BeatShiftRight:
    //    PatternOperations::shiftBeatRelative(this->getLassoSelection(), 1.f / BEATS_PER_BAR);
    //    break;
    //case CommandIDs::BarShiftLeft:
    //    PatternOperations::shiftBeatRelative(this->getLassoSelection(), -1.f);
    //    break;
    //case CommandIDs::BarShiftRight:
    //    PatternOperations::shiftBeatRelative(this->getLassoSelection(), 1.f);
    //    break;
    default:
        break;
    }

    HybridRoll::handleCommandMessage(commandId);
}

void PatternRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    HYBRID_ROLL_BULK_REPAINT_START

    for (const auto &e : this->clipComponents)
    {
        const auto component = e.second.get();
        component->setFloatBounds(this->getEventBounds(component));
    }

    HybridRoll::resized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PatternRoll::paint(Graphics &g)
{
    g.setTiledImageFill(this->rowPattern, 0, HYBRID_ROLL_HEADER_HEIGHT, 1.f);
    g.fillRect(this->viewport.getViewArea());
    HybridRoll::paint(g);
}

void PatternRoll::parentSizeChanged()
{
    this->updateRollSize();
}

void PatternRoll::insertNewClipAt(const MouseEvent &e)
{
    auto *pattern = this->getPatternByMousePosition(e.y);
    const float draggingBeat = this->getBeatByMousePosition(pattern, e.x);
    this->addNewClipMode = true;
    this->addClip(pattern, draggingBeat);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree PatternRoll::serialize() const
{
    using namespace Serialization;
    ValueTree tree(UI::patternRoll);

    tree.setProperty(UI::barWidth, roundf(this->getBarWidth()), nullptr);

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

void PatternRoll::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;

    const auto root =
        tree.hasType(UI::patternRoll) ?
        tree : tree.getChildWithName(UI::patternRoll);

    if (!root.isValid())
    { return; }

    this->setBarWidth(float(root.getProperty(UI::barWidth, this->getBarWidth())));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBar = float(root.getProperty(UI::startBar, 0.0));
    const int x = this->getXPositionByBar(startBar);
    const int y = root.getProperty(UI::viewportPositionY);
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PatternRoll::reset() {}

//===----------------------------------------------------------------------===//
// Background image cache
//===----------------------------------------------------------------------===//

Image PatternRoll::renderRowsPattern(const HelioTheme &theme, int height) const
{
    const int width = 128;
    const int shadowHeight = 16;
    Image patternImage(Image::RGB, width, height, false);
    Graphics g(patternImage);

    const Colour fillColour = theme.findColour(ColourIDs::Roll::blackKey).brighter(0.035f);
    g.setColour(fillColour);
    g.fillRect(patternImage.getBounds());

    {
        float x = 0, y = PATTERN_ROLL_TRACK_HEADER_HEIGHT;
        g.setGradientFill(ColourGradient(Colour(0x0c000000),
            x, y,
            Colours::transparentBlack,
            x, float(shadowHeight + y),
            false));
        g.fillRect(int(x), int(y), width, shadowHeight);
    }

    {
        float x = 0, y = PATTERN_ROLL_TRACK_HEADER_HEIGHT;
        g.setGradientFill(ColourGradient(Colour(0x0c000000),
            x, y,
            Colours::transparentBlack,
            x, float((shadowHeight / 2) + y),
            false));
        g.fillRect(int(x), int(y), width, shadowHeight);
    }

    HelioTheme::drawNoise(theme, g, 1.75f);

    return patternImage;
}

void PatternRoll::repaintBackgroundsCache()
{
    const HelioTheme &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    this->rowPattern = PatternRoll::renderRowsPattern(theme, rowHeight());
}
