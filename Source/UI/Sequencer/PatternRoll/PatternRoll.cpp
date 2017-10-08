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

#define ROWS_OF_TWO_OCTAVES 24
#define DEFAULT_CLIP_LENGTH 1.0f

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
    HybridRoll(parentProject, viewportRef, clippingDetector)
{
    this->header->toFront(false);
    this->indicator->toFront(false);
    //this->reloadRollContent();
}

PatternRoll::~PatternRoll()
{
}


void PatternRoll::deleteSelection()
{
    if (this->selection.getNumSelected() == 0)
    {
        return;
    }
    
    // Avoids crash
    this->hideAllGhostClips();

    // раскидать this->selection по массивам
    OwnedArray< Array<Clip> > selections;

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

    this->grabKeyboardFocus(); // not working?
}

void PatternRoll::selectAll()
{
    // TODO
}

void PatternRoll::reloadRollContent()
{
    this->selection.deselectAll();

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->removeChildComponent(this->eventComponents.getUnchecked(i));
    }

    this->eventComponents.clear();
    this->componentsHashTable.clear();

    for (auto track : this->tracks)
    {
        auto sequence = track->getSequence();
        auto pattern = track->getPattern();
        jassert(sequence != nullptr);

        for (int j = 0; j < pattern->size(); ++j)
        {
            const Clip &clip = pattern->getUnchecked(j);
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
                this->eventComponents.add(clipComponent);
                this->componentsHashTable.set(clip, clipComponent);
                this->addAndMakeVisible(clipComponent);
            }
        }
    }

    this->setSize(this->getWidth(),
        HYBRID_ROLL_HEADER_HEIGHT + this->getNumRows() * PATTERNROLL_ROW_HEIGHT);
    this->repaint(this->viewport.getViewArea());
}

int PatternRoll::getNumRows() const noexcept
{
    return this->tracks.size();
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

    const float viewStartOffsetBeat = float(this->firstBar * NUM_BEATS_IN_BAR);
    const int trackIndex = this->tracks.indexOfSorted(*track, track);
    const float sequenceLength = sequence->getLengthInBeats();
    const float sequenceStartBeat = 0.f; // TODO? sequence->getFirstBeat();

    const float w = this->barWidth * sequenceLength / NUM_BEATS_IN_BAR;
    const float x = this->barWidth *
        (sequenceStartBeat + clipBeat - viewStartOffsetBeat) / NUM_BEATS_IN_BAR;

    const float y = float(trackIndex * PATTERNROLL_ROW_HEIGHT);
    return Rectangle<float> (x, HYBRID_ROLL_HEADER_HEIGHT + y + 1,
        w, float(PATTERNROLL_ROW_HEIGHT - 1));
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
    const int patternIndex =
        jlimit(0, this->getNumRows() - 1,
            int(y / PATTERNROLL_ROW_HEIGHT));
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
        this->tracks.addSorted(*track, track);
        this->reloadRollContent();
    }
}

void PatternRoll::onChangeTrackProperties(MidiTrack *const track)
{
    if (Pattern *pattern = track->getPattern())
    {
        this->tracks.removeAllInstancesOf(track);
        this->tracks.addSorted(*track, track);
        this->repaint();
    }
}

void PatternRoll::onResetTrackContent(MidiTrack *const track)
{
    if (Pattern *pattern = track->getPattern())
    {
        this->tracks.removeAllInstancesOf(track);
        this->tracks.addSorted(*track, track);
        this->reloadRollContent();
    }
}

void PatternRoll::onRemoveTrack(MidiTrack *const track)
{
    this->tracks.removeAllInstancesOf(track);

    if (Pattern *pattern = track->getPattern())
    {
        for (int i = 0; i < pattern->size(); ++i)
        {
            const Clip &clip = pattern->getUnchecked(i);
            if (ClipComponent *component = this->componentsHashTable[clip])
            {
                this->selection.deselect(component);
                this->removeChildComponent(component);
                this->componentsHashTable.remove(clip);
                this->eventComponents.removeObject(component, true);
            }
        }

        this->setSize(this->getWidth(),
            HYBRID_ROLL_HEADER_HEIGHT + this->getNumRows() * PATTERNROLL_ROW_HEIGHT);
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
        this->addAndMakeVisible(clipComponent);

        this->batchRepaintList.add(clipComponent);
        this->triggerAsyncUpdate();

        clipComponent->toFront(false);

        this->fader.fadeIn(clipComponent, 150);

        this->eventComponents.add(clipComponent);
        this->selectEvent(clipComponent, false);

        this->componentsHashTable.set(clip, clipComponent);
    }
}

void PatternRoll::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (ClipComponent *component = this->componentsHashTable[clip])
    {
        this->batchRepaintList.add(component);
        this->triggerAsyncUpdate();

        this->componentsHashTable.remove(clip);
        this->componentsHashTable.set(newClip, component);
    }
}

void PatternRoll::onRemoveClip(const Clip &clip)
{
    if (ClipComponent *component = this->componentsHashTable[clip])
    {
        this->fader.fadeOut(component, 150);
        this->selection.deselect(component);

        this->removeChildComponent(component);
        this->componentsHashTable.remove(clip);
        this->eventComponents.removeObject(component, true);
    }
}

void PatternRoll::onPostRemoveClip(Pattern *const pattern)
{
    //
}


//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

void PatternRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    bool shouldInvalidateSelectionCache = false;

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        ClipComponent *clip = static_cast<ClipComponent *>(this->eventComponents.getUnchecked(i));
        clip->setSelected(this->selection.isSelected(clip));
    }

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        ClipComponent *clip = static_cast<ClipComponent *>(this->eventComponents.getUnchecked(i));

        if (rectangle.intersects(clip->getBounds()) && clip->isActive())
        {
            shouldInvalidateSelectionCache = true;
            itemsFound.addIfNotAlreadyThere(clip);
        }
    }

    if (shouldInvalidateSelectionCache)
    {
        this->selection.invalidateCache();
    }
}


//===----------------------------------------------------------------------===//
// ClipboardOwner
//===----------------------------------------------------------------------===//

XmlElement *PatternRoll::clipboardCopy() const
{
    auto xml = new XmlElement(Serialization::Clipboard::clipboard);

    const Lasso::GroupedSelections &selections = this->selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);

    float firstBeat = FLT_MAX;
    float lastBeat = -FLT_MAX;

    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr patternSelection(selectionsMapIterator.getValue());
        const String patternId = selectionsMapIterator.getKey();

        // create xml parent with layer id
        auto patternIdParent = new XmlElement(Serialization::Clipboard::pattern);
        patternIdParent->setAttribute(Serialization::Clipboard::patternId, patternId);
        xml->addChildElement(patternIdParent);

        for (int i = 0; i < patternSelection->size(); ++i)
        {
            if (const ClipComponent *clipComponent =
                dynamic_cast<ClipComponent *>(patternSelection->getUnchecked(i)))
            {
                patternIdParent->addChildElement(clipComponent->getClip().serialize());
                firstBeat = jmin(firstBeat, clipComponent->getBeat());
                lastBeat = jmax(lastBeat, clipComponent->getBeat());
            }
        }
    }

    xml->setAttribute(Serialization::Clipboard::firstBeat, firstBeat);
    xml->setAttribute(Serialization::Clipboard::lastBeat, lastBeat);

    return xml;
}

void PatternRoll::clipboardPaste(const XmlElement &xml)
{
    const XmlElement *root =
        (xml.getTagName() == Serialization::Clipboard::clipboard) ?
        &xml : xml.getChildByName(Serialization::Clipboard::clipboard);

    if (root == nullptr) { return; }

    float trackLength = 0;
    bool didCheckpoint = false;

    const float indicatorRoughBeat = this->getBeatByTransportPosition(this->project.getTransport().getSeekPosition());
    const float indicatorBeat = roundf(indicatorRoughBeat * 1000.f) / 1000.f;

    const float firstBeat = root->getDoubleAttribute(Serialization::Clipboard::firstBeat);
    const float lastBeat = root->getDoubleAttribute(Serialization::Clipboard::lastBeat);
    const bool indicatorIsWithinSelection = (indicatorBeat >= firstBeat) && (indicatorBeat < lastBeat);
    const float startBeatAligned = roundf(firstBeat);
    const float deltaBeat = (indicatorBeat - startBeatAligned);

    this->deselectAll();

    forEachXmlChildElementWithTagName(*root, patternElement, Serialization::Core::pattern)
    {
        Array<Clip> pastedClips;
        const String patternId = patternElement->getStringAttribute(Serialization::Clipboard::patternId);
        
        if (nullptr != this->project.findPatternByTrackId(patternId))
        {
            Pattern *targetPattern = this->project.findPatternByTrackId(patternId);
            PianoTrackTreeItem *targetLayerItem = this->project.findTrackById<PianoTrackTreeItem>(patternId);
            
            forEachXmlChildElementWithTagName(*patternElement, clipElement, Serialization::Core::clip)
            {
                Clip &&c = Clip(targetPattern).withParameters(*clipElement).copyWithNewId();
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
                        const float changeDelta = lastBeat - firstBeat;
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

bool PatternRoll::keyPressed(const KeyPress &key)
{
    if ((key == KeyPress::createFromDescription("command + x")) ||
             (key == KeyPress::createFromDescription("ctrl + x")) ||
             (key == KeyPress::createFromDescription("shift + delete")))
    {
        InternalClipboard::copy(*this, false);
        this->deleteSelection();
        return true;
    }
    else if ((key == KeyPress::createFromDescription("x")) ||
             (key == KeyPress::createFromDescription("delete")) ||
             (key == KeyPress::createFromDescription("backspace")))
    {
        this->deleteSelection();
        return true;
    }
    else if ((key == KeyPress::createFromDescription("command + v")) ||
             (key == KeyPress::createFromDescription("shift + insert")) ||
             (key == KeyPress::createFromDescription("ctrl + v")) ||
             (key == KeyPress::createFromDescription("command + shift + v")) ||
             (key == KeyPress::createFromDescription("ctrl + shift + v")))
    {
        InternalClipboard::paste(*this);
        return true;
    }

    return HybridRoll::keyPressed(key);
}

void PatternRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    HYBRID_ROLL_BULK_REPAINT_START

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        ClipComponent *clip = static_cast<ClipComponent *>(this->eventComponents.getUnchecked(i));
        clip->setFloatBounds(this->getEventBounds(clip));
    }

    HybridRoll::resized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PatternRoll::paint(Graphics &g)
{
#if PATTERNROLL_HAS_PRERENDERED_BACKGROUND

    g.setTiledImageFill(this->rowPattern, 0, PATTERN_ROLL_HEADER_OFFSET, 1.f);
    g.fillRect(this->viewport.getViewArea());

#else

    const Colour blackKey = this->findColour(HybridRoll::blackKeyColourId);
    const Colour blackKeyBright = this->findColour(HybridRoll::blackKeyBrightColourId);
    const Colour whiteKey = this->findColour(HybridRoll::whiteKeyColourId);
    const Colour whiteKeyBright = this->findColour(HybridRoll::whiteKeyBrightColourId);
    const Colour whiteKeyBrighter = whiteKeyBright.brighter(0.025f);
    const Colour rowLine = this->findColour(HybridRoll::rowLineColourId);

    const float visibleWidth = float(this->viewport.getViewWidth());
    const float visibleHeight = float(this->viewport.getViewHeight());
    const Point<int> &viewPosition = this->viewport.getViewPosition();

    const int keyStart = int(viewPosition.getY() / PATTERNROLL_ROW_HEIGHT);
    const int keyEnd = int((viewPosition.getY() + visibleHeight) / PATTERNROLL_ROW_HEIGHT);

    // Fill everything with white keys color
    g.setColour(whiteKeyBright);
    g.fillRect(float(viewPosition.getX()), float(viewPosition.getY()), visibleWidth, visibleHeight);

    for (int i = keyStart; i <= keyEnd; i++)
    {
        const int lastOctaveReminder = 4;
        const int yPos = HYBRID_ROLL_HEADER_HEIGHT + i * PATTERNROLL_ROW_HEIGHT;
        const int noteNumber = (i + lastOctaveReminder) % 12;
        const int octaveNumber = (i + lastOctaveReminder) / 12;
        const bool octaveIsOdd = ((octaveNumber % 2) > 0);

        switch (noteNumber)
        {
            case 1:
            case 3:
            case 5:
            case 8:
            case 10: // black keys
                g.setColour(octaveIsOdd ? blackKeyBright : blackKey);
                g.fillRect(float(viewPosition.getX()), float(yPos), visibleWidth, float(PATTERNROLL_ROW_HEIGHT));
                break;

            default: // white keys bevel
                g.setColour(whiteKeyBrighter);
                g.drawHorizontalLine(yPos + 1, float(viewPosition.getX()), float(viewPosition.getX() + visibleWidth));
                break;
        }

        g.setColour(rowLine);
        g.drawHorizontalLine(yPos, float(viewPosition.getX()), float(viewPosition.getX() + visibleWidth));
    }

    HelioTheme::drawNoiseWithin(this->viewport.getViewArea().toFloat(), this, g, 2.0);

#endif

    HybridRoll::paint(g);
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

XmlElement *PatternRoll::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::midiRoll);

    xml->setAttribute("barWidth", this->getBarWidth());

    xml->setAttribute("startBar", this->getBarByXPosition(this->getViewport().getViewPositionX()));
    xml->setAttribute("endBar", this->getBarByXPosition(this->getViewport().getViewPositionX() + this->getViewport().getViewWidth()));

    xml->setAttribute("y", this->getViewport().getViewPositionY());

    // m?
    //xml->setAttribute("selection", this->getLassoSelection().serialize());

    return xml;
}

void PatternRoll::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *root = (xml.getTagName() == Serialization::Core::midiRoll) ?
                             &xml : xml.getChildByName(Serialization::Core::midiRoll);

    if (root == nullptr)
    { return; }

    this->setBarWidth(float(root->getDoubleAttribute("barWidth", this->getBarWidth())));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBar = float(root->getDoubleAttribute("startBar", 0.0));
    const int x = this->getXPositionByBar(startBar);
    const int y = root->getIntAttribute("y");
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PatternRoll::reset()
{
}


//===----------------------------------------------------------------------===//
// Bg images cache
//===----------------------------------------------------------------------===//

CachedImage::Ptr PatternRoll::renderRowsPattern(HelioTheme &theme, int height)
{
    CachedImage::Ptr patternImage(new CachedImage(Image::RGB, 128, height * ROWS_OF_TWO_OCTAVES, false));

    Graphics g(*patternImage);

    const Colour blackKey = theme.findColour(HybridRoll::blackKeyColourId);
    const Colour blackKeyBright = theme.findColour(HybridRoll::blackKeyBrightColourId);
    const Colour whiteKey = theme.findColour(HybridRoll::whiteKeyColourId);
    const Colour whiteKeyBright = theme.findColour(HybridRoll::whiteKeyBrightColourId);
    const Colour whiteKeyBrighter = whiteKeyBright.brighter(0.025f);
    const Colour rowLine = theme.findColour(HybridRoll::rowLineColourId);

    float currentHeight = float(height);
    float previousHeight = 0;
    float pos_y = patternImage->getHeight() - currentHeight;
    const int lastOctaveReminder = 8;

    g.setColour(whiteKeyBright);
    g.fillRect(patternImage->getBounds());

    // draw rows
    for (int i = lastOctaveReminder;
         (i < ROWS_OF_TWO_OCTAVES + lastOctaveReminder) && ((pos_y + previousHeight) >= 0.0f);
         i++)
    {
        const int noteNumber = i % 12;
        const int octaveNumber = i / 12;
        const bool octaveIsOdd = ((octaveNumber % 2) > 0);

        previousHeight = currentHeight;

        switch (noteNumber)
        {
        case 1:
        case 3:
        case 5:
        case 8:
        case 10: // black keys
            g.setColour(octaveIsOdd ? blackKeyBright : blackKey);
            g.fillRect(0, int(pos_y + 1), patternImage->getWidth(), int(previousHeight - 1));
            break;

        default: // white keys
            g.setColour(whiteKeyBrighter);
            g.drawHorizontalLine(int(pos_y + 1), 0.f, float(patternImage->getWidth()));
            break;
        }

        // fill divider line
        g.setColour(rowLine);
        g.drawHorizontalLine(int(pos_y), 0.f, float(patternImage->getWidth()));

        currentHeight = float(height);
        pos_y -= currentHeight;
    }

    HelioTheme::drawNoise(theme, g, 2.f);

    return patternImage;
}
