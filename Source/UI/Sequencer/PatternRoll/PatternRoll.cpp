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
#include "MainLayout.h"
#include "PatternRoll.h"
#include "HybridRollHeader.h"
#include "MidiTrackHeader.h"
#include "Pattern.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "ProjectTreeItem.h"
#include "ProjectTimeline.h"
#include "ClipComponent.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "HelioTheme.h"
#include "ChordBuilder.h"
#include "HybridLassoComponent.h"
#include "HybridRollEditMode.h"
#include "SerializationKeys.h"
#include "Icons.h"
#include "InternalClipboard.h"
#include "HelioCallout.h"
#include "NotesTuningPanel.h"
#include "PianoRollToolbox.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "PianoSequence.h"
#include "PianoClipComponent.h"
#include "AutomationSequence.h"
#include "AutomationClipComponent.h"
#include "DummyClipComponent.h"
#include "ComponentIDs.h"
#include "ColourIDs.h"

#define ROWS_OF_TWO_OCTAVES 24
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
    HybridRoll(parentProject, viewportRef, clippingDetector, false, false, true)
{
    // TODO: pattern roll doesn't need neither annotations track map nor key signatures track map

    this->setComponentID(ComponentIDs::patternRollId);

    this->insertTrackHelper = new MidiTrackHeader(nullptr);
    this->addAndMakeVisible(this->insertTrackHelper);

    this->repaintBackgroundsCache();
    this->reloadRollContent();
    this->setBarRange(0, 8);
}

void PatternRoll::deleteSelection()
{
    if (this->selection.getNumSelected() == 0)
    {
        return;
    }
    
    // Avoids crash
    this->hideAllGhostClips();

    OwnedArray<Array<Clip>> selections;

    for (int i = 0; i < this->selection.getNumSelected(); ++i)
    {
        const Clip clip = this->selection.getItemAs<ClipComponent>(i)->getClip();
        Pattern *ownerPattern = clip.getPattern();
        Array<Clip> *arrayToAddTo = nullptr;

        for (int j = 0; j < selections.size(); ++j)
        {
            if (selections.getUnchecked(j)->size() > 0)
            {
                if (selections.getUnchecked(j)->getUnchecked(0).getPattern() == ownerPattern)
                {
                    arrayToAddTo = selections.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Clip>();
            selections.add(arrayToAddTo);
        }

        arrayToAddTo->add(clip);
    }

    bool didCheckpoint = false;

    for (int i = 0; i < selections.size(); ++i)
    {
        Pattern *pattern = (selections.getUnchecked(i)->getUnchecked(0).getPattern());

        if (! didCheckpoint)
        {
            didCheckpoint = true;
            pattern->checkpoint();
        }

        for (Clip &c : *selections.getUnchecked(i))
        {
            pattern->remove(c, true);
        }
    }
}

void PatternRoll::selectAll()
{
    // TODO
}

void PatternRoll::reloadRollContent()
{
    this->selection.deselectAll();

    this->trackHeaders.clear();
    this->clipComponents.clear();

    this->tracks.clearQuick();

    for (auto track : this->project.getTracks())
    {
        auto sequence = track->getSequence();
        jassert(sequence != nullptr);

        // Only show tracks with patterns (i.e. ignore timeline tracks)
        if (auto pattern = track->getPattern())
        {
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
}

void PatternRoll::updateChildrenPositions()
{
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
    ClipComponent *nc = static_cast<ClipComponent *>(mc);
    return this->getEventBounds(nc->getClip(), nc->getBeat());
}

Rectangle<float> PatternRoll::getEventBounds(const Clip &clip, float clipBeat) const
{
    const Pattern *pattern = clip.getPattern();
    const MidiTrack *track = pattern->getTrack();
    const MidiSequence *sequence = track->getSequence();
    jassert(sequence != nullptr);

    const float viewStartOffsetBeat = float(this->firstBar * BEATS_PER_BAR);
    const int trackIndex = this->tracks.indexOfSorted(*track, track);
    const float sequenceLength = sequence->getLengthInBeats();
    const float sequenceStartBeat = sequence->getFirstBeat();

    const float w = this->barWidth * sequenceLength / BEATS_PER_BAR;
    const float x = this->barWidth *
        (sequenceStartBeat + clipBeat - viewStartOffsetBeat) / BEATS_PER_BAR;

    const float y = float(trackIndex * rowHeight());
    return Rectangle<float> (x, HYBRID_ROLL_HEADER_HEIGHT + y + PATTERN_ROLL_TRACK_HEADER_HEIGHT,
        w, float(PATTERN_ROLL_CLIP_HEIGHT));
}

float PatternRoll::getBeatByComponentPosition(float x) const
{
    return this->getRoundBeatByXPosition(int(x)); /* - 0.5f ? */
}

float PatternRoll::getBeatByMousePosition(int x) const
{
    return this->getFloorBeatByXPosition(x);
}

Pattern *PatternRoll::getPatternByMousePosition(int y) const
{
    const int patternIndex = jlimit(0, this->getNumRows() - 1, int(y / rowHeight()));
    return this->tracks.getUnchecked(patternIndex)->getPattern();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PatternRoll::onAddMidiEvent(const MidiEvent &event)
{
    // the question is:
    // is pattern roll supposed to monitor single event changes?
    // or it just reloads the whole sequence on show?0
    //this->reloadRollContent();
}

void PatternRoll::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    //
}

void PatternRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    //
}

void PatternRoll::onPostRemoveMidiEvent(MidiSequence *const layer)
{
    //
}

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
    }
}

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
        //this->tracks.removeAllInstancesOf(track);
        //this->tracks.addSorted(*track, track);
        this->tracks.sort();

        // TODO only repaint clips of a changed track?
        for (const auto &e : this->clipComponents)
        {
            const auto component = e.second.get();
            component->updateColours();
        }
    }

    this->repaint();
}

void PatternRoll::onRemoveTrack(MidiTrack *const track)
{
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

        this->batchRepaintList.add(clipComponent);
        this->triggerAsyncUpdate();

        clipComponent->toFront(false);

        this->fader.fadeIn(clipComponent, 150);
        this->selectEvent(clipComponent, false);
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
// ClipboardOwner
//===----------------------------------------------------------------------===//

ValueTree PatternRoll::clipboardCopy() const
{
    ValueTree tree(Serialization::Clipboard::clipboard);

    float firstBeat = FLT_MAX;
    float lastBeat = -FLT_MAX;

    for (const auto &s : selection.getGroupedSelections())
    {
        const auto patternSelection(s.second);
        const String patternId(s.first);

        // create xml parent with layer id
        ValueTree patternIdParent(Serialization::Clipboard::pattern);
        patternIdParent.setProperty(Serialization::Clipboard::patternId, patternId, nullptr);
        tree.appendChild(patternIdParent, nullptr);

        for (int i = 0; i < patternSelection->size(); ++i)
        {
            if (const ClipComponent *clipComponent =
                dynamic_cast<ClipComponent *>(patternSelection->getUnchecked(i)))
            {
                patternIdParent.appendChild(clipComponent->getClip().serialize(), nullptr);
                firstBeat = jmin(firstBeat, clipComponent->getBeat());
                lastBeat = jmax(lastBeat, clipComponent->getBeat());
            }
        }
    }

    tree.setProperty(Serialization::Clipboard::firstBeat, firstBeat, nullptr);
    tree.setProperty(Serialization::Clipboard::lastBeat, lastBeat, nullptr);

    return tree;
}

void PatternRoll::clipboardPaste(const ValueTree &tree)
{
    const auto root =
        tree.hasType(Serialization::Clipboard::clipboard) ?
        tree : tree.getChildWithName(Serialization::Clipboard::clipboard);

    if (!root.isValid()) { return; }

    bool didCheckpoint = false;

    const float indicatorRoughBeat = this->getBeatByTransportPosition(this->project.getTransport().getSeekPosition());
    const float indicatorBeat = roundf(indicatorRoughBeat * 1000.f) / 1000.f;

    const double firstBeat = root.getProperty(Serialization::Clipboard::firstBeat);
    const double lastBeat = root.getProperty(Serialization::Clipboard::lastBeat);
    const bool indicatorIsWithinSelection = (indicatorBeat >= firstBeat) && (indicatorBeat < lastBeat);
    const float startBeatAligned = roundf(float(firstBeat));
    const float deltaBeat = (indicatorBeat - startBeatAligned);

    this->deselectAll();

    forEachValueTreeChildWithType(root, patternElement, Serialization::Midi::pattern)
    {
        Array<Clip> pastedClips;
        const String patternId = patternElement.getProperty(Serialization::Clipboard::patternId);
        
        if (nullptr != this->project.findPatternByTrackId(patternId))
        {
            Pattern *targetPattern = this->project.findPatternByTrackId(patternId);
            
            forEachValueTreeChildWithType(patternElement, clipElement, Serialization::Midi::clip)
            {
                Clip &&c = Clip(targetPattern).withParameters(clipElement).copyWithNewId();
                pastedClips.add(c.withDeltaBeat(deltaBeat));
            }
            
            if (pastedClips.size() > 0)
            {
                if (! didCheckpoint)
                {
                    targetPattern->checkpoint();
                    didCheckpoint = true;
                    
                    // also insert space if needed
                    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
                    if (isShiftPressed)
                    {
                        const float changeDelta = float(lastBeat - firstBeat);
                        PianoRollToolbox::shiftEventsToTheRight(this->project.getTracks(), indicatorBeat, changeDelta, false);
                    }
                }
                
                for (Clip &c : pastedClips)
                {
                    targetPattern->insert(c, true);
                }
            }
        }
    }

    return;
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
    // can show menus
    if (this->multiTouchController->hasMultitouch() || (e.source.getIndex() > 0))
    {
        return;
    }

    HybridRoll::mouseDrag(e);
}

void PatternRoll::mouseUp(const MouseEvent &e)
{
    if (const bool hasMultitouch = (e.source.getIndex() > 0))
    {
        return;
    }
    
    // Due to weird modal component behavior,
    // a component can receive mouseUp event without receiving a mouseDown event before.

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
void PatternRoll::handleCommandMessage(int commandId)
{
    // TODO switch
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
    float draggingBeat = this->getBeatByMousePosition(e.x);
    const auto pattern = this->getPatternByMousePosition(e.y);
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
    Image patternImage(Image::RGB, 128, height * ROWS_OF_TWO_OCTAVES, false);
    Graphics g(patternImage);

    const Colour whiteKey = theme.findColour(ColourIDs::Roll::whiteKey);

    g.setColour(whiteKey);
    g.fillRect(patternImage.getBounds());

    HelioTheme::drawNoise(theme, g, 1.75f);

    return patternImage;
}

void PatternRoll::repaintBackgroundsCache()
{
    const HelioTheme &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    this->rowPattern = PatternRoll::renderRowsPattern(theme, rowHeight());
}
