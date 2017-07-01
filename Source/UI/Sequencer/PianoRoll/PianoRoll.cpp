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
#include "MidiLayer.h"
#include "PianoLayer.h"
#include "AutomationLayer.h"
#include "AnnotationsLayer.h"
#include "PianoLayerTreeItem.h"
#include "AutomationLayerTreeItem.h"
#include "ProjectTreeItem.h"
#include "ProjectTimeline.h"
#include "Note.h"
#include "NoteComponent.h"
#include "HelperRectangle.h"
#include "SmoothZoomController.h"
#include "MultiTouchController.h"
#include "HelioTheme.h"
#include "NoNotesPopup.h"
#include "HybridLassoComponent.h"
#include "HybridRollEditMode.h"
#include "SerializationKeys.h"
#include "Icons.h"
#include "InternalClipboard.h"
#include "HelioCallout.h"
#include "NotesTuningPanel.h"
#include "ArpeggiatorPanel.h"
#include "ArpeggiatorEditorPanel.h"
#include "PianoRollToolbox.h"
#include "NoteResizerLeft.h"
#include "NoteResizerRight.h"
#include "Config.h"
#include "SerializationKeys.h"

#define ROWS_OF_TWO_OCTAVES 24
#define DEFAULT_NOTE_LENGTH 0.25f
#define DEFAULT_NOTE_VELOCITY 0.25f

PianoRoll::PianoRoll(ProjectTreeItem &parentProject,
                     Viewport &viewportRef,
                     WeakReference<AudioMonitor> clippingDetector) :
    HybridRoll(parentProject, viewportRef, clippingDetector),
	primaryActiveLayer(nullptr),
	numRows(128),
    rowHeight(MIN_ROW_HEIGHT),
    draggingNote(nullptr),
    addNewNoteMode(false),
    mouseDownWasTriggered(false)
{
    this->setRowHeight(MIN_ROW_HEIGHT + 5);

    //this->helperVertical = new HelperRectangleVertical();
    //this->addChildComponent(this->helperVertical);

    this->helperHorizontal = new HelperRectangleHorizontal();
    this->addChildComponent(this->helperHorizontal);

    this->header->toFront(false);
    this->indicator->toFront(false);
    
    this->reloadMidiTrack();
}

PianoRoll::~PianoRoll()
{
}


void PianoRoll::deleteSelection()
{
    if (this->selection.getNumSelected() == 0)
    {
        return;
    }
    
    // Avoids crash
    this->hideAllGhostNotes();

    // раскидать this->selection по массивам
    OwnedArray< Array<Note> > selections;

    for (int i = 0; i < this->selection.getNumSelected(); ++i)
    {
        const MidiEvent &event = this->selection.getItemAs<MidiEventComponent>(i)->getEvent();
        const Note &note = static_cast<const Note &>(event);
        MidiLayer *ownerLayer = event.getLayer();
        Array<Note> *arrayToAddTo = nullptr;

        for (int j = 0; j < selections.size(); ++j)
        {
            if (selections.getUnchecked(j)->size() > 0)
            {
                if (selections.getUnchecked(j)->getUnchecked(0).getLayer() == ownerLayer)
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
        PianoLayer *pianoLayer = static_cast<PianoLayer *>(selections.getUnchecked(i)->getUnchecked(0).getLayer());

        if (! didCheckpoint)
        {
            didCheckpoint = true;
            pianoLayer->checkpoint();
        }

        pianoLayer->removeGroup(*selections.getUnchecked(i), true);
    }

    this->grabKeyboardFocus(); // not working?
}

void PianoRoll::reloadMidiTrack()
{
    //Logger::writeToLog("PianoRoll::reloadMidiTrack");

    this->selection.deselectAll();

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        this->removeChildComponent(this->eventComponents.getUnchecked(i));
    }

    this->eventComponents.clear();
    this->componentsHashTable.clear();


    const Array<MidiLayer *> &layers = this->project.getLayersList();

    for (auto layer : layers)
    {
        for (int j = 0; j < layer->size(); ++j)
        {
            MidiEvent *event = layer->getUnchecked(j);

            if (Note *note = dynamic_cast<Note *>(event))
            {
                auto noteComponent = new NoteComponent(*this, *note);

                //noteComponent->setSelected(this->selection.isSelected(noteComponent)); // ��� �� �:
                //noteComponent->setSelected(this->activeLayer->getLassoSelection().isSelected(noteComponent));

                this->eventComponents.add(noteComponent);
                this->componentsHashTable.set(*note, noteComponent);

                const bool belongsToActiveLayer = noteComponent->belongsToLayerSet(this->activeLayers);
                noteComponent->setActive(belongsToActiveLayer, true);

                this->addAndMakeVisible(noteComponent);
            }
        }
    }

    this->resized();
    this->repaint(this->viewport.getViewArea());
}

void PianoRoll::setActiveMidiLayers(Array<MidiLayer *> newLayers, MidiLayer *primaryLayer)
{
    // todo! check arrays for equality
    //if (this->activeLayer == layer) { return; }

    // todo! check if primary layer is within newLayers

    //Logger::writeToLog("PianoRoll::setActiveMidiLayers");

    this->selection.deselectAll();

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        NoteComponent *noteComponent =
            static_cast<NoteComponent *>(this->eventComponents.getUnchecked(i));

        const bool belongsToNewLayers = noteComponent->belongsToLayerSet(newLayers);
        const bool belongsToOldLayers = noteComponent->belongsToLayerSet(this->activeLayers);

        //if (belongsToNewLayer || belongsToOldLayer)
        //{
        noteComponent->setActive(belongsToNewLayers);
        //}
    }

    this->activeLayers = newLayers;
    this->primaryActiveLayer = primaryLayer;
	
    this->repaint(this->viewport.getViewArea());
}

MidiLayer *PianoRoll::getPrimaryActiveMidiLayer() const noexcept
{
	return this->primaryActiveLayer;
}

int PianoRoll::getNumActiveLayers() const noexcept
{
	return this->activeLayers.size();
}

MidiLayer *PianoRoll::getActiveMidiLayer(int index) const noexcept
{
	return this->activeLayers[index];
}

void PianoRoll::setRowHeight(const int newRowHeight)
{
    if (newRowHeight == this->rowHeight || newRowHeight <= 1) { return; }

    this->rowHeight = newRowHeight;
    this->setSize(this->getWidth(), this->numRows * this->rowHeight);
}


//===----------------------------------------------------------------------===//
// HybridRoll
//===----------------------------------------------------------------------===//

void PianoRoll::selectAll()
{
	for (int i = 0; i < this->eventComponents.size(); ++i)
	{
		MidiEventComponent *child = this->eventComponents.getUnchecked(i);

		if (child->belongsToLayerSet(this->activeLayers))
		{
			this->selection.addToSelection(child);
		}
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
            newRowHeight > MAX_ROW_HEIGHT ||
            newRowHeight < MIN_ROW_HEIGHT)
        {
            newRowHeight = this->getRowHeight();
        }

        this->setRowHeight(newRowHeight);

        const float newHeight = float(this->getHeight());
        const float mouseOffsetY = float(absoluteOrigin.getY() - oldViewPosition.getY());
        const float newViewPositionY = float((absoluteOrigin.getY() * newHeight) / oldHeight) - mouseOffsetY;
        this->viewport.setViewPosition(Point<int>(oldViewPosition.getX(), int(newViewPositionY + 0.5f)));
    }

    HybridRoll::zoomRelative(origin, factor);
}

void PianoRoll::zoomAbsolute(const Point<float> &zoom)
{
    const float &newHeight = (this->getNumRows() * MAX_ROW_HEIGHT) * zoom.getY();
    const float &rowsOnNewScreen = float(newHeight / MAX_ROW_HEIGHT);
    const float &viewHeight = float(this->viewport.getViewHeight());
    const float &newRowHeight = floorf(viewHeight / rowsOnNewScreen + .5f);

    this->setRowHeight(int(newRowHeight));

    HybridRoll::zoomAbsolute(zoom);
}

float PianoRoll::getZoomFactorY() const
{
    // headerheight fix hack:
    const float &numRows = float(this->getNumRows());
    const float &viewHeight = float(this->viewport.getViewHeight() - HYBRID_ROLL_HEADER_HEIGHT);
    return (viewHeight / float(this->getHeight()));

//    const float &rowHeight = float(this->getRowHeight());
//    const float &rowsOnScreen = (viewHeight / rowHeight);
//    return (rowsOnScreen / numRows);

//    const float &viewHeight = float(this->viewport.getViewHeight() - MIDIROLL_HEADER_HEIGHT);
//    const float &thisHeight = float(this->getHeight() - MIDIROLL_HEADER_HEIGHT);

//    const float &viewHeight = float(this->viewport.getViewHeight());
//    const float &thisHeight = float(this->getHeight());
//    return viewHeight / thisHeight;
}


//===----------------------------------------------------------------------===//
// Note management
//===----------------------------------------------------------------------===//

void PianoRoll::addNote(int key, float beat, float length, float velocity)
{
    //if (PianoLayer *activePianoLayer = dynamic_cast<PianoLayer *>(this->activeLayer))
    PianoLayer *activePianoLayer = static_cast<PianoLayer *>(this->primaryActiveLayer);
    {
        activePianoLayer->checkpoint();
        Note note(activePianoLayer, key, beat, length, velocity);
        activePianoLayer->insert(note, true);
    }
}

Rectangle<float> PianoRoll::getEventBounds(FloatBoundsComponent *mc) const
{
	jassert(dynamic_cast<NoteComponent *>(mc));
    NoteComponent *nc = static_cast<NoteComponent *>(mc);
    return this->getEventBounds(nc->getKey(), nc->getBeat(), nc->getLength());
}

Rectangle<float> PianoRoll::getEventBounds(const int key, const float beat, const float length) const
{
    const float startOffsetBeat = float(this->firstBar * NUM_BEATS_IN_BAR);
	const float x = this->barWidth * (beat - startOffsetBeat) / NUM_BEATS_IN_BAR;
	const float w = this->barWidth * length / NUM_BEATS_IN_BAR;

    const float yPosition = float(this->getYPositionByKey(key));
    return Rectangle<float> (x, yPosition + 1, w, float(this->rowHeight - 1));
}



void PianoRoll::getRowsColsByComponentPosition(const float x, const float y, int &noteNumber, float &beatNumber) const
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
    return (this->getHeight() - this->rowHeight) - (targetKey * this->rowHeight);
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
    //this->helperVertical->setAlpha(1.f);
    this->helperHorizontal->setAlpha(1.f);
    //this->helperVertical->setVisible(true);
    this->helperHorizontal->setVisible(true);
}

void PianoRoll::hideHelpers()
{
    const int animTime = SHORT_FADE_TIME(this);
    //this->fader.fadeOut(this->helperVertical, animTime);
    this->fader.fadeOut(this->helperHorizontal, animTime);
}

void PianoRoll::moveHelpers(const float deltaBeat, const int deltaKey)
{
    const float firstBeat = float(this->firstBar * NUM_BEATS_IN_BAR);
    const Rectangle<int> selectionBounds = this->selection.getSelectionBounds();
    //const Rectangle<int> delta = this->getEventBounds(deltaKey - 1, deltaBeat, 1.f);
    const Rectangle<float> delta = this->getEventBounds(deltaKey - 1, deltaBeat + firstBeat, 1.f);

    const int deltaX = roundFloatToInt(delta.getTopLeft().getX());
    const int deltaY = roundFloatToInt(delta.getTopLeft().getY() - this->getHeight() - 1);
    //const int deltaX = delta.getTopLeft().getX();
    //const int deltaY = delta.getTopLeft().getY() - this->getHeight() - 1;
    const Rectangle<int> selectionTranslated = selectionBounds.translated(deltaX, deltaY);

    const int vX = this->viewport.getViewPositionX();
    const int vW = this->viewport.getViewWidth();
    this->helperHorizontal->setBounds(selectionTranslated.withLeft(vX).withWidth(vW));

    //const int vY = this->viewport.getViewPositionY();
    //const int vH = this->viewport.getViewHeight();
    //this->helperVertical->setBounds(selectionTranslated.withTop(vY).withHeight(vH));
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void PianoRoll::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    HybridRoll::onChangeMidiEvent(oldEvent, newEvent);
    
    if (! dynamic_cast<const Note *>(&oldEvent)) { return; }

    const Note &note = static_cast<const Note &>(oldEvent);
    const Note &newNote = static_cast<const Note &>(newEvent);

    if (NoteComponent *component = this->componentsHashTable[note])
    {
        //component->repaint(); // если делать так - будут дикие тормоза, поэтому:
        this->batchRepaintList.add(component);
        this->triggerAsyncUpdate();

        this->componentsHashTable.remove(note);
        this->componentsHashTable.set(newNote, component);
    }
}

void PianoRoll::onAddMidiEvent(const MidiEvent &event)
{
    HybridRoll::onAddMidiEvent(event);
    
    if (! dynamic_cast<const Note *>(&event)) { return; }

    const Note &note = static_cast<const Note &>(event);

    auto component = new NoteComponent(*this, note);
    this->addAndMakeVisible(component);

    this->batchRepaintList.add(component);
    this->triggerAsyncUpdate();
    // ^^ вместо:
    //component->updateBounds(this->getEventBounds(component));

    component->toFront(false);

    this->fader.fadeIn(component, 150);

    this->eventComponents.add(component);
    this->selectEvent(component, false); // selectEvent(component, true)

    const bool isActive = component->belongsToLayerSet(this->activeLayers);
    component->setActive(isActive);

    this->componentsHashTable.set(note, component);

    if (this->addNewNoteMode)
    {
        this->draggingNote = component;
        this->addNewNoteMode = false;
        this->selectEvent(this->draggingNote, true); // clear prev selection
        this->grabKeyboardFocus();
    }
}

void PianoRoll::onRemoveMidiEvent(const MidiEvent &event)
{
    HybridRoll::onRemoveMidiEvent(event);

    if (! dynamic_cast<const Note *>(&event)) { return; }
    
    const Note &note = static_cast<const Note &>(event);

    if (NoteComponent *component = this->componentsHashTable[note])
    {
        this->fader.fadeOut(component, 150);
        
        this->selection.deselect(component);

        this->removeChildComponent(component); // O(N)
        this->componentsHashTable.remove(note);
        this->eventComponents.removeObject(component, true); // O(N)
    }
}

void PianoRoll::onChangeMidiLayer(const MidiLayer *layer)
{
    if (! dynamic_cast<const PianoLayer *>(layer)) { return; }

    this->reloadMidiTrack();
}

void PianoRoll::onAddMidiLayer(const MidiLayer *layer)
{
    if (! dynamic_cast<const PianoLayer *>(layer)) { return; }

    if (layer->size() > 0)
    {
        this->reloadMidiTrack();
    }
}

void PianoRoll::onRemoveMidiLayer(const MidiLayer *layer)
{
    if (! dynamic_cast<const PianoLayer *>(layer)) { return; }

    for (int i = 0; i < layer->size(); ++i)
    {
        const Note &note = static_cast<const Note &>(*layer->getUnchecked(i));

        if (NoteComponent *component = this->componentsHashTable[note])
        {
            this->selection.deselect(component);
            this->removeChildComponent(component);
            this->componentsHashTable.remove(note);
            this->eventComponents.removeObject(component, true);
        }
    }
}


//===----------------------------------------------------------------------===//
// LassoSource
//===----------------------------------------------------------------------===//

void PianoRoll::findLassoItemsInArea(Array<SelectableComponent *> &itemsFound, const Rectangle<int> &rectangle)
{
    bool shouldInvalidateSelectionCache = false;

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        NoteComponent *note = static_cast<NoteComponent *>(this->eventComponents.getUnchecked(i));
        note->setSelected(this->selection.isSelected(note));
    }

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        NoteComponent *note = static_cast<NoteComponent *>(this->eventComponents.getUnchecked(i));

        if (rectangle.intersects(note->getBounds()) && note->isActive())
        {
            shouldInvalidateSelectionCache = true;
            itemsFound.addIfNotAlreadyThere(note);
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
        auto annotationLayerIdParent = new XmlElement(Serialization::Clipboard::layer);
        annotationLayerIdParent->setAttribute(Serialization::Clipboard::layerId, timeline->getAnnotations()->getLayerIdAsString());
        xml->addChildElement(annotationLayerIdParent);

        for (int i = 0; i < timeline->getAnnotations()->size(); ++i)
        {
            if (const AnnotationEvent *event =
                dynamic_cast<AnnotationEvent *>(timeline->getAnnotations()->getUnchecked(i)))
            {
                if (const bool eventFitsInRange =
                    (event->getBeat() >= firstBeat) && (event->getBeat() < lastBeat))
                {
                    annotationLayerIdParent->addChildElement(event->serialize());
                }
            }
        }

        // and from all autos
        const auto automations = this->project.findChildrenOfType<AutomationLayerTreeItem>(false);
        for (auto automation : automations)
        {
            MidiLayer *autoLayer = automation->getLayer();
            auto autoLayerIdParent = new XmlElement(Serialization::Clipboard::layer);
            autoLayerIdParent->setAttribute(Serialization::Clipboard::layerId, autoLayer->getLayerIdAsString());
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

    float trackLength = 0;
    bool didCheckpoint = false;

    const float indicatorRoughBeat = this->getBeatByTransportPosition(this->project.getTransport().getSeekPosition());
    const float indicatorBeat = roundf(indicatorRoughBeat * 1000.f) / 1000.f;

    const float firstBeat = mainSlot->getDoubleAttribute(Serialization::Clipboard::firstBeat);
    const float lastBeat = mainSlot->getDoubleAttribute(Serialization::Clipboard::lastBeat);
    const bool indicatorIsWithinSelection = (indicatorBeat >= firstBeat) && (indicatorBeat < lastBeat);
    const float startBeatAligned = roundf(firstBeat);
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
        
        if (nullptr != this->project.getLayerWithId<AutomationLayer>(layerId))
        {
            AutomationLayer *targetLayer = this->project.getLayerWithId<AutomationLayer>(layerId);
            const bool correspondingTreeItemExists =
            (this->project.findChildByLayerId<AutomationLayerTreeItem>(layerId) != nullptr);
            
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
        else if (nullptr != this->project.getLayerWithId<AnnotationsLayer>(layerId))
        {
            AnnotationsLayer *targetLayer = this->project.getLayerWithId<AnnotationsLayer>(layerId);
            
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
            PianoLayer *targetLayer = this->project.getLayerWithId<PianoLayer>(layerId);
            PianoLayerTreeItem *targetLayerItem = this->project.findChildByLayerId<PianoLayerTreeItem>(layerId);
            // use primary layer, if target is not found or not selected
            const bool shouldUsePrimaryLayer = (targetLayerItem == nullptr) ? true : (!targetLayerItem->isSelected());
            
            if (shouldUsePrimaryLayer)
            {
                targetLayer = static_cast<PianoLayer *>(this->primaryActiveLayer);
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
                        const float changeDelta = lastBeat - firstBeat;
                        PianoRollToolbox::shiftEventsToTheRight(this->project.getLayersList(), indicatorBeat, changeDelta, false);
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

//void PianoRoll::longTapEvent(const MouseEvent &e)
//{
//    if (this->multiTouchController->hasMultitouch())
//    {
//        return;
//    }
//
//    if (e.eventComponent == this)
//    {
//        this->insertNewNoteAt(e);
//    }
//
//    HybridRoll::longTapEvent(e);
//}

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
        auto popup = new NoNotesPopup(this, this->primaryActiveLayer);
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
            //if (PianoLayer *activePianoLayer = dynamic_cast<PianoLayer *>(this->activeLayer))
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
    // todo dismissDraggingNoteIfNeeded on editmode change?
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

// TODO: hardcoded shortcuts are evil, need to move them to config file

bool PianoRoll::keyPressed(const KeyPress &key)
{
//    Logger::writeToLog("PianoRoll::keyPressed " + key.getTextDescription());

    if (key == KeyPress::createFromDescription("f"))
    {
        if (this->selection.getNumSelected() > 0)
        {
            // zoom selected events
            HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
            return true;
        }
    }
    else if (key == KeyPress::createFromDescription("v"))
    {
        if (this->selection.getNumSelected() > 0)
        {
            HelioCallout::emit(new NotesTuningPanel(this->project, *this), this, true);
            return true;
        }
    }
    else if (key == KeyPress::createFromDescription("a"))
    {
        if (this->selection.getNumSelected() > 0)
        {
            HelioCallout::emit(new ArpeggiatorPanel(this->project.getTransport(), *this), this, true);
            return true;
        }
    }
    else if (key == KeyPress::createFromDescription("o"))
    {
        HYBRID_ROLL_BULK_REPAINT_START
        PianoRollToolbox::removeOverlaps(this->getLassoSelection());
        HYBRID_ROLL_BULK_REPAINT_END
        return true;
    }
    else if (key == KeyPress::createFromDescription("s"))
    {
        HYBRID_ROLL_BULK_REPAINT_START
        PianoRollToolbox::snapSelection(this->getLassoSelection(), 1);
        HYBRID_ROLL_BULK_REPAINT_END
        return true;
    }
    else if (key == KeyPress::createFromDescription("command + 1") ||
             key == KeyPress::createFromDescription("ctrl + 1"))
    {
        HYBRID_ROLL_BULK_REPAINT_START
        PianoRollToolbox::randomizeVolume(this->getLassoSelection(), 0.1f);
        HYBRID_ROLL_BULK_REPAINT_END
        return true;
    }
    else if (key == KeyPress::createFromDescription("command + 2") ||
             key == KeyPress::createFromDescription("ctrl + 2"))
    {
        HYBRID_ROLL_BULK_REPAINT_START
        PianoRollToolbox::fadeOutVolume(this->getLassoSelection(), 0.35f);
        HYBRID_ROLL_BULK_REPAINT_END
        return true;
    }
    
//    else if (key == KeyPress::createFromDescription("option + shift + a"))
//    {
//        MIDI_ROLL_BULK_REPAINT_START
//        PianoRollToolbox::arpeggiateUsingClipboardAsPattern(this->getLassoSelection());
//        MIDI_ROLL_BULK_REPAINT_END
//        return true;
//    }
    
    else if (key == KeyPress::createFromDescription("shift + a"))
    {
        if (this->selection.getNumSelected() > 0)
        {
            HelioCallout::emit(new ArpeggiatorEditorPanel(this->project, *this), this, true);
            return true;
        }
    }
    else if (key == KeyPress::createFromDescription("cursor up"))
    {
		PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(),
			1, true, &this->getTransport());
        return true;
    }
    else if (key == KeyPress::createFromDescription("cursor down"))
    {
        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(), 
			-1, true, &this->getTransport());
        return true;
    }
    else if (key == KeyPress::createFromDescription("shift + cursor up"))
    {
        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(),
			12, true, &this->getTransport());
        return true;
    }
    else if (key == KeyPress::createFromDescription("shift + cursor down"))
    {
        PianoRollToolbox::shiftKeyRelative(this->getLassoSelection(),
			-12, true, &this->getTransport());
        return true;
    }
    else if (key == KeyPress::createFromDescription("option + cursor up") ||
             key == KeyPress::createFromDescription("command + cursor up") ||
             key == KeyPress::createFromDescription("ctrl + cursor up") ||
             key == KeyPress::createFromDescription("alt + cursor up"))
    {
        PianoRollToolbox::inverseChord(this->getLassoSelection(), 
			12, true, &this->getTransport());
        return true;
    }
    else if (key == KeyPress::createFromDescription("option + cursor down") ||
             key == KeyPress::createFromDescription("command + cursor down") ||
             key == KeyPress::createFromDescription("ctrl + cursor down") ||
             key == KeyPress::createFromDescription("alt + cursor down"))
    {
        PianoRollToolbox::inverseChord(this->getLassoSelection(),
			-12, true, &this->getTransport());
        return true;
    }
    else if ((key == KeyPress::createFromDescription("command + x")) ||
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

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR

    if (key.isKeyCode(KeyPress::numberPad2))
    {
        this->smoothZoomController->setInitialZoomSpeed(this->smoothZoomController->getInitialZoomSpeed() - 0.01f);
    }
    else if (key.isKeyCode(KeyPress::numberPad8))
    {
        this->smoothZoomController->setInitialZoomSpeed(this->smoothZoomController->getInitialZoomSpeed() + 0.01f);
    }
    else if (key.isKeyCode(KeyPress::numberPad4))
    {
        this->smoothZoomController->setZoomReduxFactor(this->smoothZoomController->getZoomReduxFactor() - 0.01f);
    }
    else if (key.isKeyCode(KeyPress::numberPad6))
    {
        this->smoothZoomController->setZoomReduxFactor(this->smoothZoomController->getZoomReduxFactor() + 0.01f);
    }
    else if (key.isKeyCode(KeyPress::numberPad7))
    {
        this->smoothZoomController->setZoomStopFactor(this->smoothZoomController->getZoomStopFactor() - 0.0005f);
    }
    else if (key.isKeyCode(KeyPress::numberPad9))
    {
        this->smoothZoomController->setZoomStopFactor(this->smoothZoomController->getZoomStopFactor() + 0.0005f);
    }
    else if (key.isKeyCode(KeyPress::numberPad1))
    {
        this->smoothZoomController->setTimerDelay(this->smoothZoomController->getTimerDelay() - 1);
    }
    else if (key.isKeyCode(KeyPress::numberPad3))
    {
        this->smoothZoomController->setTimerDelay(this->smoothZoomController->getTimerDelay() + 1);
    }
    else if (key.isKeyCode(KeyPress::numberPad0))
    {
        const String msg = "InitialZoomSpeed: " + String(this->smoothZoomController->getInitialZoomSpeed()) + ", " +
                           "ZoomReduxFactor: " + String(this->smoothZoomController->getZoomReduxFactor()) + ", " +
                           "ZoomStopFactor: " + String(this->smoothZoomController->getZoomStopFactor()) + ", " +
                           "TimerDelay: " + String(this->smoothZoomController->getTimerDelay());

        Logger::writeToLog(msg);
    }

#endif

    return HybridRoll::keyPressed(key);
}


void PianoRoll::resized()
{
	if (!this->isShowing())
	{
		return;
	}

    HYBRID_ROLL_BULK_REPAINT_START

    for (int i = 0; i < this->eventComponents.size(); ++i)
    {
        NoteComponent *note = static_cast<NoteComponent *>(this->eventComponents.getUnchecked(i));
        note->setFloatBounds(this->getEventBounds(note));
    }

    HybridRoll::resized();

    HYBRID_ROLL_BULK_REPAINT_END
}

void PianoRoll::paint(Graphics &g)
{
#if PIANOROLL_HAS_PRERENDERED_BACKGROUND

    g.setTiledImageFill(*(static_cast<HelioTheme &>(this->getLookAndFeel()).getRollBgCache()[this->rowHeight]), 0, 0, 1.f);
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

    const int keyStart = int(viewPosition.getY() / this->rowHeight);
    const int keyEnd = int((viewPosition.getY() + visibleHeight) / this->rowHeight);

    // Fill everything with white keys color
    g.setColour(whiteKeyBright);
    g.fillRect(float(viewPosition.getX()), float(viewPosition.getY()), visibleWidth, visibleHeight);

    for (int i = keyStart; i <= keyEnd; i++)
    {
        const int lastOctaveReminder = 4;
        const int yPos = i * this->rowHeight;
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
                g.fillRect(float(viewPosition.getX()), float(yPos), visibleWidth, float(this->rowHeight));
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

    const float startBar = float(root->getDoubleAttribute("startBar", 0.0));
    const float endBar = float(root->getDoubleAttribute("endBar", 0.0));

    this->setFirstBar(jmin(this->getFirstBar(), int(floorf(startBar))));
    this->setLastBar(jmax(this->getLastBar(), int(ceilf(endBar))));

    const int x = this->getXPositionByBar(startBar);

    const int y = root->getIntAttribute("y");
    this->getViewport().setViewPosition(x, y);

    // restore selection?
}

void PianoRoll::reset()
{
}


//===----------------------------------------------------------------------===//
// Bg images cache
//===----------------------------------------------------------------------===//

void PianoRoll::repaintBackgroundsCache(HelioTheme &theme)
{
    theme.getRollBgCache().clear();

    for (int i = MIN_ROW_HEIGHT; i <= MAX_ROW_HEIGHT; ++i)
    {
        CachedImage::Ptr pattern(PianoRoll::renderRowsPattern(theme, i));
        theme.getRollBgCache().set(i, pattern);
    }
}

CachedImage::Ptr PianoRoll::renderRowsPattern(HelioTheme &theme, int height)
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
