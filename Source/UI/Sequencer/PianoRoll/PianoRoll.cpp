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
#include "PianoRoll.h"
#include "HybridRollHeader.h"
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
#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "NoteComponent.h"
#include "HelperRectangle.h"
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
#include "ArpeggiatorEditorPanel.h"
#include "PianoRollToolbox.h"
#include "NoteResizerLeft.h"
#include "NoteResizerRight.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"

#define ROWS_OF_TWO_OCTAVES 24
#define DEFAULT_NOTE_LENGTH 0.25f
#define DEFAULT_NOTE_VELOCITY 0.25f

PianoRoll::PianoRoll(ProjectTreeItem &parentProject,
                     Viewport &viewportRef,
                     WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(parentProject, viewportRef, clippingDetector),
    primaryActiveLayer(nullptr),
    numRows(128),
    rowHeight(PIANOROLL_MIN_ROW_HEIGHT),
    draggingNote(nullptr),
    addNewNoteMode(false),
    mouseDownWasTriggered(false),
    defaultHighlighting() // default pattern (black and white keys)
{
    this->defaultHighlighting = new HighlightingScheme(0, Scale::getNaturalMajorScale());
    this->defaultHighlighting->setRows(this->renderBackgroundCacheFor(this->defaultHighlighting));

    this->setComponentID(ComponentIDs::pianoRollId);
    this->setRowHeight(PIANOROLL_MIN_ROW_HEIGHT + 5);

    this->helperHorizontal = new HelperRectangleHorizontal();
    this->addChildComponent(this->helperHorizontal);

    this->reloadRollContent();
}

PianoRoll::~PianoRoll()
{
    this->clearRollContent();
}

void PianoRoll::deleteSelection()
{
    if (this->selection.getNumSelected() == 0)
    {
        return;
    }
    
    // Avoids crash
    this->hideAllGhostNotes();

    OwnedArray< Array<Note> > selections;
    for (int i = 0; i < this->selection.getNumSelected(); ++i)
    {
        const Note &note = this->selection.getItemAs<NoteComponent>(i)->getNote();
        const MidiSequence *ownerLayer = note.getSequence();
        Array<Note> *arrayToAddTo = nullptr;

        for (int j = 0; j < selections.size(); ++j)
        {
            if (selections.getUnchecked(j)->size() > 0)
            {
                if (selections.getUnchecked(j)->getUnchecked(0).getSequence() == ownerLayer)
                {
                    arrayToAddTo = selections.getUnchecked(j);
                }
            }
        }

        if (arrayToAddTo == nullptr)
        {
            arrayToAddTo = new Array<Note>();
            selections.add(arrayToAddTo);
        }

        arrayToAddTo->add(note);
    }

    bool didCheckpoint = false;

    for (int i = 0; i < selections.size(); ++i)
    {
        PianoSequence *pianoLayer = static_cast<PianoSequence *>(selections.getUnchecked(i)->getUnchecked(0).getSequence());

        if (! didCheckpoint)
        {
            didCheckpoint = true;
            pianoLayer->checkpoint();
        }

        pianoLayer->removeGroup(*selections.getUnchecked(i), true);
    }
}


void PianoRoll::clearRollContent()
{
    OwnedArray<NoteComponent> deleters;
    for (const auto &e : this->componentsMap)
    {
        deleters.add(e.second);
    }

    this->componentsMap.clear();
}

void PianoRoll::reloadRollContent()
{
    this->selection.deselectAll();
    this->backgroundsCache.clear();

    this->clearRollContent();

    const auto &tracks = this->project.getTracks();
    for (auto track : tracks)
    {
        for (int j = 0; j < track->getSequence()->size(); ++j)
        {
            MidiEvent *event = track->getSequence()->getUnchecked(j);
            if (event->isTypeOf(MidiEvent::Note))
            {
                Note *note = static_cast<Note *>(event);
                auto noteComponent = new NoteComponent(*this, *note);

                this->componentsMap[*note] = noteComponent;

                const bool belongsToActiveTrack = noteComponent->belongsToAnySequence(this->activeLayers);
                noteComponent->setActive(belongsToActiveTrack, true);

                this->addAndMakeVisible(noteComponent);
            }
            else if (event->isTypeOf(MidiEvent::KeySignature))
            {
                const auto &key = static_cast<const KeySignatureEvent &>(*event);
                this->updateBackgroundCacheFor(key);
            }
        }
    }

    this->resized();
    this->repaint(this->viewport.getViewArea());
}

void PianoRoll::setActiveMidiLayers(Array<MidiSequence *> newLayers, MidiSequence *primaryLayer)
{
    // todo! check arrays for equality
    //if (this->activeLayer == layer) { return; }

    // todo! check if primary layer is within newLayers

    //Logger::writeToLog("PianoRoll::setActiveMidiLayers");

    this->selection.deselectAll();

    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const noteComponent = e.second;
        const bool belongsToNewLayers = noteComponent->belongsToAnySequence(newLayers);
        const bool belongsToOldLayers = noteComponent->belongsToAnySequence(this->activeLayers);
        noteComponent->setActive(belongsToNewLayers);
    }

    this->activeLayers = newLayers;
    this->primaryActiveLayer = primaryLayer;
    
    this->repaint(this->viewport.getViewArea());
}

MidiSequence *PianoRoll::getPrimaryActiveMidiLayer() const noexcept
{
    return this->primaryActiveLayer;
}

int PianoRoll::getNumActiveLayers() const noexcept
{
    return this->activeLayers.size();
}

MidiSequence *PianoRoll::getActiveMidiLayer(int index) const noexcept
{
    return this->activeLayers[index];
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
    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const child = e.second;
        if (child->belongsToAnySequence(this->activeLayers))
        {
            this->selection.addToSelection(child);
        }
    }
}

void PianoRoll::setChildrenInteraction(bool interceptsMouse, MouseCursor cursor)
{
    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const child = e.second;
        child->setInterceptsMouseClicks(interceptsMouse, interceptsMouse);
        child->setMouseCursor(cursor);
    }
}

//===----------------------------------------------------------------------===//
// Ghost notes
//===----------------------------------------------------------------------===//

void PianoRoll::showGhostNoteFor(NoteComponent *targetNoteComponent)
{
    auto component = new NoteComponent(*this, targetNoteComponent->getNote());
    component->setEnabled(false);

    //component->setAlpha(0.2f); // почему-то адские тормоза из-за альфы
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
    //if (PianoSequence *activePianoLayer = dynamic_cast<PianoSequence *>(this->activeLayer))
    PianoSequence *activePianoLayer = static_cast<PianoSequence *>(this->primaryActiveLayer);
    {
        activePianoLayer->checkpoint();
        Note note(activePianoLayer, key, beat, length, velocity);
        activePianoLayer->insert(note, true);
    }
}

Rectangle<float> PianoRoll::getEventBounds(FloatBoundsComponent *mc) const
{
    //jassert(dynamic_cast<NoteComponent *>(mc));
    NoteComponent *nc = static_cast<NoteComponent *>(mc);
    return this->getEventBounds(nc->getKey(), nc->getBeat(), nc->getLength());
}

Rectangle<float> PianoRoll::getEventBounds(int key, float beat, float length) const
{
    const double startOffsetBeat = this->firstBar * double(NUM_BEATS_IN_BAR);
    const double x = this->barWidth * double(beat - startOffsetBeat) / double(NUM_BEATS_IN_BAR);

    const float w = this->barWidth * length / float(NUM_BEATS_IN_BAR);
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
    if (this->helperHorizontal->isVisible())
    { return; }

    this->selection.needsToCalculateSelectionBounds();
    this->moveHelpers(0.f, 0);
    this->helperHorizontal->setAlpha(1.f);
    this->helperHorizontal->setVisible(true);
}

void PianoRoll::hideHelpers()
{
    const int animTime = SHORT_FADE_TIME(this);
    this->fader.fadeOut(this->helperHorizontal, animTime);
}

void PianoRoll::moveHelpers(const float deltaBeat, const int deltaKey)
{
    const float firstBeat = this->firstBar * float(NUM_BEATS_IN_BAR);
    const Rectangle<int> selectionBounds = this->selection.getSelectionBounds();
    const Rectangle<float> delta = this->getEventBounds(deltaKey - 1, deltaBeat + firstBeat, 1.f);

    const int deltaX = roundFloatToInt(delta.getTopLeft().getX());
    const int deltaY = roundFloatToInt(delta.getTopLeft().getY() - this->getHeight() - 1);
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
        if (NoteComponent *component = this->componentsMap[note])
        {
            this->batchRepaintList.add(component); // instead of component->repaint();
            this->triggerAsyncUpdate();
            this->componentsMap.erase(note);
            this->componentsMap[newNote] = component;
        }
    }
    else if (oldEvent.isTypeOf(MidiEvent::KeySignature))
    {
        const KeySignatureEvent &oldKey = static_cast<const KeySignatureEvent &>(oldEvent);
        const KeySignatureEvent &newKey = static_cast<const KeySignatureEvent &>(newEvent);
        if (oldKey.getRootKey() != newKey.getRootKey() ||
            !oldKey.getScale().isEquivalentTo(newKey.getScale()))
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
        this->addAndMakeVisible(component);

        this->batchRepaintList.add(component);
        this->triggerAsyncUpdate(); // instead of updateBounds

        //component->toFront(false); // already at the front
        this->fader.fadeIn(component, 150);

        this->selectEvent(component, false); // selectEvent(component, true)

        const bool isActive = component->belongsToAnySequence(this->activeLayers);
        component->setActive(isActive);

        this->componentsMap[note] = component;

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
        const Note &note = static_cast<const Note &>(event);
        if (ScopedPointer<NoteComponent> deletedComponent = this->componentsMap[note])
        {
            this->fader.fadeOut(deletedComponent, 150);
            this->selection.deselect(deletedComponent);
            this->componentsMap.erase(note);
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
        for (const auto &e : this->componentsMap)
        {
            const auto component = e.second;
            component->updateColours();
            this->batchRepaintList.add(component);
        }

        this->triggerAsyncUpdate();
    }
}

void PianoRoll::onAddTrack(MidiTrack *const track)
{
    for (int j = 0; j < track->getSequence()->size(); ++j)
    {
        const MidiEvent *const event = track->getSequence()->getUnchecked(j);
        if (event->isTypeOf(MidiEvent::Note))
        {
            const Note *const note = static_cast<const Note *const>(event);
            auto noteComponent = new NoteComponent(*this, *note);

            const bool belongsToActiveTrack = noteComponent->belongsToAnySequence(this->activeLayers);
            noteComponent->setActive(belongsToActiveTrack, true);

            this->componentsMap[*note] = noteComponent;

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

    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const auto event = track->getSequence()->getUnchecked(i);
        if (event->isTypeOf(MidiEvent::Note))
        {
            const Note &note = static_cast<const Note &>(*event);
            if (ScopedPointer<NoteComponent> componentDeleter = this->componentsMap[note])
            {
                this->fader.fadeOut(componentDeleter, 150);
                this->selection.deselect(componentDeleter);
                this->componentsMap.erase(note);
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

    for (const auto &e : this->componentsMap)
    {
        HybridRollEventComponent *const ec = e.second;
        if (ec->isActive() &&
            ec->getBeat() >= startBeat &&
            ec->getBeat() < endBeat)
        {
            this->selection.addToSelection(ec);
        }
    }
}

void PianoRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    bool shouldInvalidateSelectionCache = false;

    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const component = e.second;
        component->setSelected(this->selection.isSelected(component));
    }

    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const component = e.second;
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
// ClipboardOwner
//===----------------------------------------------------------------------===//

XmlElement *PianoRoll::clipboardCopy() const
{
    auto xml = new XmlElement(Serialization::Clipboard::clipboard);
    
    const Lasso::GroupedSelections &selections = this->selection.getGroupedSelections();
    Lasso::GroupedSelections::Iterator selectionsMapIterator(selections);

    float firstBeat = FLT_MAX;
    float lastBeat = -FLT_MAX;

    while (selectionsMapIterator.next())
    {
        SelectionProxyArray::Ptr layerSelection(selectionsMapIterator.getValue());
        const String layerId = selectionsMapIterator.getKey();

        // create xml parent with layer id
        auto layerIdParent = new XmlElement(Serialization::Clipboard::layer);
        layerIdParent->setAttribute(Serialization::Clipboard::layerId, layerId);
        xml->addChildElement(layerIdParent);

        for (int i = 0; i < layerSelection->size(); ++i)
        {
            if (const NoteComponent *noteComponent =
                dynamic_cast<NoteComponent *>(layerSelection->getUnchecked(i)))
            {
                layerIdParent->addChildElement(noteComponent->getNote().serialize());

                if (firstBeat > noteComponent->getBeat())
                {
                    firstBeat = noteComponent->getBeat();
                }

                if (lastBeat < (noteComponent->getBeat() + noteComponent->getLength()))
                {
                    lastBeat = (noteComponent->getBeat() + noteComponent->getLength());
                }
            }
        }
    }

    // a hack: alt-mode, copies also autos/annotations within given range, if any
    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
    const bool selectionIsNotEmpty = (firstBeat < lastBeat);

    if (selectionIsNotEmpty && isShiftPressed)
    {
        // todo copy from
        const auto timeline = this->project.getTimeline();
        const auto annotations = timeline->getAnnotations()->getSequence();
        auto annotationLayerIdParent = new XmlElement(Serialization::Clipboard::layer);
        annotationLayerIdParent->setAttribute(Serialization::Clipboard::layerId, annotations->getTrackId());
        xml->addChildElement(annotationLayerIdParent);

        for (int i = 0; i < annotations->size(); ++i)
        {
            if (const AnnotationEvent *event =
                dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i)))
            {
                if (const bool eventFitsInRange =
                    (event->getBeat() >= firstBeat) && (event->getBeat() < lastBeat))
                {
                    annotationLayerIdParent->addChildElement(event->serialize());
                }
            }
        }

        // and from all autos
        const auto automations = this->project.findChildrenOfType<AutomationTrackTreeItem>(false);
        for (auto automation : automations)
        {
            MidiSequence *autoLayer = automation->getSequence();
            auto autoLayerIdParent = new XmlElement(Serialization::Clipboard::layer);
            autoLayerIdParent->setAttribute(Serialization::Clipboard::layerId, autoLayer->getTrackId());
            xml->addChildElement(autoLayerIdParent);
            
            for (int j = 0; j < autoLayer->size(); ++j)
            {
                if (const AutomationEvent *event =
                    dynamic_cast<AutomationEvent *>(autoLayer->getUnchecked(j)))
                {
                    if (const bool eventFitsInRange =
                        (event->getBeat() >= firstBeat) && (event->getBeat() < lastBeat))
                    {
                        autoLayerIdParent->addChildElement(event->serialize());
                    }
                }
            }
        }
    }

    xml->setAttribute(Serialization::Clipboard::firstBeat, firstBeat);
    xml->setAttribute(Serialization::Clipboard::lastBeat, lastBeat);

    return xml;
}

void PianoRoll::clipboardPaste(const XmlElement &xml)
{
    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Clipboard::clipboard) ?
                                 &xml : xml.getChildByName(Serialization::Clipboard::clipboard);

    if (mainSlot == nullptr) { return; }

    bool didCheckpoint = false;

    const float indicatorRoughBeat = this->getBeatByTransportPosition(this->project.getTransport().getSeekPosition());
    const float indicatorBeat = roundf(indicatorRoughBeat * 1000.f) / 1000.f;

    const double firstBeat = mainSlot->getDoubleAttribute(Serialization::Clipboard::firstBeat);
    const double lastBeat = mainSlot->getDoubleAttribute(Serialization::Clipboard::lastBeat);
    const bool indicatorIsWithinSelection = (indicatorBeat >= firstBeat) && (indicatorBeat < lastBeat);
    const float startBeatAligned = roundf(float(firstBeat));
    const float deltaBeat = (indicatorBeat - startBeatAligned);

    this->deselectAll();

    forEachXmlChildElementWithTagName(*mainSlot, layerElement, Serialization::Core::layer)
    {
        Array<Note> pastedNotes;

        const String layerId = layerElement->getStringAttribute(Serialization::Clipboard::layerId);
        
        // TODO: store layer type in copy-paste info
        // when pasting, use these priorities:
        // 1. layer with the same id
        // 2. layer with the same type and controller
        // 3. active layer
        
        if (nullptr != this->project.findSequenceByTrackId<AutomationSequence>(layerId))
        {
            AutomationSequence *targetLayer = this->project.findSequenceByTrackId<AutomationSequence>(layerId);
            const bool correspondingTreeItemExists =
            (this->project.findTrackById<AutomationTrackTreeItem>(layerId) != nullptr);
            
            if (correspondingTreeItemExists)
            {
                Array<AutomationEvent> pastedEvents;
                
                forEachXmlChildElementWithTagName(*layerElement, autoElement, Serialization::Core::event)
                {
                    AutomationEvent &&ae = AutomationEvent(targetLayer).withParameters(*autoElement).copyWithNewId();
                    pastedEvents.add(ae.withDeltaBeat(deltaBeat));
                }
                
                targetLayer->insertGroup(pastedEvents, true);
            }
        }
        else if (nullptr != this->project.findSequenceByTrackId<AnnotationsSequence>(layerId))
        {
            AnnotationsSequence *targetLayer = this->project.findSequenceByTrackId<AnnotationsSequence>(layerId);
            
            // no check for a tree item as there isn't any for ProjectTimeline
            Array<AnnotationEvent> pastedAnnotations;
            
            forEachXmlChildElementWithTagName(*layerElement, annotationElement, Serialization::Core::annotation)
            {
                AnnotationEvent &&ae = AnnotationEvent(targetLayer).withParameters(*annotationElement).copyWithNewId();
                pastedAnnotations.add(ae.withDeltaBeat(deltaBeat));
            }
            
            targetLayer->insertGroup(pastedAnnotations, true);
        }
        else
        {
            PianoSequence *targetLayer = this->project.findSequenceByTrackId<PianoSequence>(layerId);
            PianoTrackTreeItem *targetLayerItem = this->project.findTrackById<PianoTrackTreeItem>(layerId);
            // use primary layer, if target is not found or not selected
            const bool shouldUsePrimaryLayer = (targetLayerItem == nullptr) ? true : (!targetLayerItem->isSelected());
            
            if (shouldUsePrimaryLayer)
            {
                targetLayer = static_cast<PianoSequence *>(this->primaryActiveLayer);
            }
            
            forEachXmlChildElementWithTagName(*layerElement, noteElement, Serialization::Core::note)
            {
                Note &&n = Note(targetLayer).withParameters(*noteElement).copyWithNewId();
                pastedNotes.add(n.withDeltaBeat(deltaBeat));
            }
            
            if (pastedNotes.size() > 0)
            {
                if (! didCheckpoint)
                {
                    targetLayer->checkpoint();
                    didCheckpoint = true;
                    
                    // also insert space if needed
                    const bool isShiftPressed = Desktop::getInstance().getMainMouseSource().getCurrentModifiers().isShiftDown();
                    if (isShiftPressed)
                    {
                        const float changeDelta = float(lastBeat - firstBeat);
                        PianoRollToolbox::shiftEventsToTheRight(this->project.getTracks(), indicatorBeat, changeDelta, false);
                    }
                }
                
                targetLayer->insertGroup(pastedNotes, true);
            }
        }

    }

    return;
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
        auto popup = new ChordBuilder(this, this->primaryActiveLayer);
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
            //if (PianoSequence *activePianoLayer = dynamic_cast<PianoSequence *>(this->activeLayer))
            //{
            //    activePianoLayer->checkpoint();
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
        InternalClipboard::copy(*this, false);
        break;
    case CommandIDs::CutEvents:
        InternalClipboard::copy(*this, false);
        this->deleteSelection();
        break;
    case CommandIDs::PasteEvents:
        InternalClipboard::paste(*this);
        break;
    case CommandIDs::DeleteEvents:
        InternalClipboard::copy(*this, false);
        this->deleteSelection();
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
            const Time lastMouseDownTime = Desktop::getInstance().getMainMouseSource().getLastMouseDownTime();
            const bool noClicksWasDone = (lastMouseDownTime < this->timeEnteredDragMode);
            const bool noDraggingWasDone = (this->draggedDistance < 5);
            const bool notTooMuchTimeSpent = (Time::getCurrentTime() - this->timeEnteredDragMode).inMilliseconds() < 500;
            if (noDraggingWasDone && noClicksWasDone && notTooMuchTimeSpent)
            { this->project.getTransport().toggleStatStopPlayback(); }
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
    default:
        break;
    }

    HybridRoll::handleCommandMessage(commandId);

// TODO more:
//    else if (key == KeyPress::createFromDescription("cursor left"))
//        PianoRollToolbox::shiftBeatRelative(this->getLassoSelection(), -0.25f);
//    else if (key == KeyPress::createFromDescription("cursor right"))
//        PianoRollToolbox::shiftBeatRelative(this->getLassoSelection(), 0.25f);
//    else if (key == KeyPress::createFromDescription("shift + cursor left"))
//        PianoRollToolbox::shiftBeatRelative(this->getLassoSelection(), -0.25f * NUM_BEATS_IN_BAR);
//    else if (key == KeyPress::createFromDescription("shift + cursor right"))
//        PianoRollToolbox::shiftBeatRelative(this->getLassoSelection(), 0.25f * NUM_BEATS_IN_BAR);
//    else if (key == KeyPress::createFromDescription("cursor up"))
//        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(), 1, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("cursor down"))
//        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(), -1, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("shift + cursor up"))
//        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(), 12, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("shift + cursor down"))
//        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(), -12, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("option + cursor up") ||
//             key == KeyPress::createFromDescription("command + cursor up") ||
//             key == KeyPress::createFromDescription("ctrl + cursor up") ||
//             key == KeyPress::createFromDescription("alt + cursor up"))
//        PianoRollToolbox::inverseChord(this->getLassoSelection(), 12, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("option + cursor down") ||
//             key == KeyPress::createFromDescription("command + cursor down") ||
//             key == KeyPress::createFromDescription("ctrl + cursor down") ||
//             key == KeyPress::createFromDescription("alt + cursor down"))
//        PianoRollToolbox::inverseChord(this->getLassoSelection(), -12, true, &this->getTransport());
//    else if (key == KeyPress::createFromDescription("1"))
//        this->project.getEditMode().setMode(HybridRollEditMode::defaultMode);
//    else if (key == KeyPress::createFromDescription("2"))
//        this->project.getEditMode().setMode(HybridRollEditMode::drawMode);
//    else if (key == KeyPress::createFromDescription("3"))
//        this->project.getEditMode().setMode(HybridRollEditMode::selectionMode);
//    else if (key == KeyPress::createFromDescription("4"))
//        this->project.getEditMode().setMode(HybridRollEditMode::dragMode);
//    else if (key == KeyPress::createFromDescription("5"))
//        this->project.getEditMode().setMode(HybridRollEditMode::wipeSpaceMode);
//    else if (key == KeyPress::createFromDescription("6"))
//        this->project.getEditMode().setMode(HybridRollEditMode::insertSpaceMode);
//    else if (key == KeyPress::createFromDescription("command + s") ||
//        key == KeyPress::createFromDescription("ctrl + s"))
//        this->project.getDocument()->forceSave();
//    else if (key == KeyPress::createFromDescription("shift + Tab"))
//        if (VersionControlTreeItem *vcsTreeItem = this->project.findChildOfType<VersionControlTreeItem>())
//            vcsTreeItem->toggleQuickStash();
//    else if (key == KeyPress::createFromDescription("f"))
//        if (this->selection.getNumSelected() > 0)
//            HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
//    else if (key == KeyPress::createFromDescription("v"))
//        if (this->selection.getNumSelected() > 0)
//            HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
//    else if (key == KeyPress::createFromDescription("o"))
//    {
//        HYBRID_ROLL_BULK_REPAINT_START
//        PianoRollToolbox::removeOverlaps(this->getLassoSelection());
//        HYBRID_ROLL_BULK_REPAINT_END
//        return true;
//    }
//    else if (key == KeyPress::createFromDescription("s"))
//    {
//        HYBRID_ROLL_BULK_REPAINT_START
//        PianoRollToolbox::snapSelection(this->getLassoSelection(), 1);
//        HYBRID_ROLL_BULK_REPAINT_END
//        return true;
//    }
//    else if (key == KeyPress::createFromDescription("command + 1") ||
//             key == KeyPress::createFromDescription("ctrl + 1"))
//    {
//        HYBRID_ROLL_BULK_REPAINT_START
//        PianoRollToolbox::randomizeVolume(this->getLassoSelection(), 0.1f);
//        HYBRID_ROLL_BULK_REPAINT_END
//        return true;
//    }
//    else if (key == KeyPress::createFromDescription("command + 2") ||
//             key == KeyPress::createFromDescription("ctrl + 2"))
//    {
//        HYBRID_ROLL_BULK_REPAINT_START
//        PianoRollToolbox::fadeOutVolume(this->getLassoSelection(), 0.35f);
//        HYBRID_ROLL_BULK_REPAINT_END
//        return true;
//    }
////    else if (key == KeyPress::createFromDescription("option + shift + a"))
////    {
////        MIDI_ROLL_BULK_REPAINT_START
////        PianoRollToolbox::arpeggiateUsingClipboardAsPattern(this->getLassoSelection());
////        MIDI_ROLL_BULK_REPAINT_END
////        return true;
////    }
////    else if (key == KeyPress::createFromDescription("a"))
////        if (this->selection.getNumSelected() > 0)
////            // TODO show arps menu
//    else if (key == KeyPress::createFromDescription("shift + a"))
//        if (this->selection.getNumSelected() > 0)
//            HelioCallout::emit(new ArpeggiatorEditorPanel(this->project, *this), this, true);
}

void PianoRoll::resized()
{
    if (!this->isShowing())
    {
        return;
    }

    HYBRID_ROLL_BULK_REPAINT_START

    for (const auto &e : this->componentsMap)
    {
        NoteComponent *const component = e.second;
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
    const float paintStartBar = roundf(float(paintStartX) / this->barWidth) + this->firstBar - 1.f;
    const float paintEndBar = roundf(float(paintEndX) / this->barWidth) + this->firstBar + 1.f;

    int prevBarX = paintStartX;
    const HighlightingScheme *prevScheme = nullptr;
    const int y = this->viewport.getViewPositionY();
    const int h = this->viewport.getViewHeight();

    for (int nextKeyIdx = 0; nextKeyIdx < sequences->size(); ++nextKeyIdx)
    {
        const auto key = static_cast<KeySignatureEvent *>(sequences->getUnchecked(nextKeyIdx));
        const int barX = int(((key->getBeat() / float(NUM_BEATS_IN_BAR)) - this->firstBar)  * this->barWidth);
        const int index = this->binarySearchForHighlightingScheme(key);

#if DEBUG
        if (index < 0)
        {
            Logger::writeToLog("Missing " + key->toString());
            jassert(index >= 0);
        }
#endif

        if (barX >= paintEndX)
        {
            const auto s = (prevScheme == nullptr) ? this->backgroundsCache.getUnchecked(index) : prevScheme;
            g.setTiledImageFill(s->getUnchecked(this->rowHeight), 0, HYBRID_ROLL_HEADER_HEIGHT, 1.f);
            g.fillRect(prevBarX, y, barX - prevBarX, h);
            HybridRoll::paint(g);
            return;
        }
        else if (barX >= paintStartX)
        {
            const auto s = (prevScheme == nullptr) ? this->backgroundsCache.getUnchecked(index) : prevScheme;
            g.setTiledImageFill(s->getUnchecked(this->rowHeight), 0, HYBRID_ROLL_HEADER_HEIGHT, 1.f);
            g.fillRect(prevBarX, y, barX - prevBarX, h);
        }

        prevBarX = barX;
        prevScheme = this->backgroundsCache.getUnchecked(index);
    }

    if (prevBarX < paintEndX)
    {
        const auto s = (prevScheme == nullptr) ? this->defaultHighlighting : prevScheme;
        g.setTiledImageFill(s->getUnchecked(this->rowHeight), 0, HYBRID_ROLL_HEADER_HEIGHT, 1.f);
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

XmlElement *PianoRoll::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::midiRoll);
    
    xml->setAttribute("barWidth", this->getBarWidth());
    xml->setAttribute("rowHeight", this->getRowHeight());

    xml->setAttribute("startBar", this->getBarByXPosition(this->getViewport().getViewPositionX()));
    xml->setAttribute("endBar", this->getBarByXPosition(this->getViewport().getViewPositionX() + this->getViewport().getViewWidth()));

    xml->setAttribute("y", this->getViewport().getViewPositionY());

    // m?
    //xml->setAttribute("selection", this->getLassoSelection().serialize());

    return xml;
}

void PianoRoll::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *root = (xml.getTagName() == Serialization::Core::midiRoll) ?
                             &xml : xml.getChildByName(Serialization::Core::midiRoll);

    if (root == nullptr)
    { return; }
    
    this->setBarWidth(float(root->getDoubleAttribute("barWidth", this->getBarWidth())));
    this->setRowHeight(root->getIntAttribute("rowHeight", this->getRowHeight()));

    // FIXME doesn't work right for now, as view range is sent after this
    const float startBar = float(root->getDoubleAttribute("startBar", 0.0));
    const int x = this->getXPositionByBar(startBar);
    const int y = root->getIntAttribute("y");
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PianoRoll::reset()
{
}


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
    const Scale &scale, int root, int height)
{
    if (height < PIANOROLL_MIN_ROW_HEIGHT)
    {
        return Image(Image::RGB, 1, 1, true);
    }

    // Image patterns of width 128px take up to 5mb of ram (rows from 6 to 30)
    // Width 256px == ~10Mb. Prerendered patterns are drawing fast asf.
    Image patternImage(Image::RGB, 128, height * ROWS_OF_TWO_OCTAVES, false);
    Graphics g(patternImage);

    const Colour blackKey = theme.findColour(HybridRoll::blackKeyColourId);
    const Colour blackKeyBright = theme.findColour(HybridRoll::blackKeyBrightColourId);
    const Colour whiteKey = theme.findColour(HybridRoll::whiteKeyColourId);
    const Colour whiteKeyBright = theme.findColour(HybridRoll::whiteKeyBrightColourId);
    const Colour rootKey = whiteKeyBright.brighter(0.075f);
    const Colour rootKeyBright = whiteKeyBright.brighter(0.080f);
    const Colour rowLine = theme.findColour(HybridRoll::rowLineColourId);

    float currentHeight = float(height);
    float previousHeight = 0;
    float pos_y = patternImage.getHeight() - currentHeight;
    const int lastOctaveReminder = 8 + CHROMATIC_SCALE_SIZE - root;

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
        else if (scale.hasKey(noteNumber))
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

PianoRoll::HighlightingScheme::HighlightingScheme(int rootKey, const Scale &scale) :
    rootKey(rootKey),
    scale(scale)
{
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
