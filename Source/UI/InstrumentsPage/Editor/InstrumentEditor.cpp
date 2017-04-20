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
#include "InstrumentEditor.h"
#include "InternalPluginFormat.h"
#include "PluginWindow.h"

#include "PanelBackgroundC.h"

#include "InstrumentEditorPin.h"
#include "InstrumentEditorNode.h"
#include "InstrumentEditorConnector.h"

#include "AudioCore.h"

InstrumentEditor::InstrumentEditor(Instrument &instrumentRef,
                                   WeakReference<AudioCore> audioCoreRef) :
instrument(instrumentRef),
audioCore(std::move(audioCoreRef))
{
    this->background = new PanelBackgroundC();
    this->addAndMakeVisible(this->background);
    
    this->instrument.addChangeListener(this);
    this->audioCore->getDevice().addChangeListener(this);
    
    this->setBufferedToImage(true);
    this->setOpaque(true);
    
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);
}

InstrumentEditor::~InstrumentEditor()
{
    this->audioCore->getDevice().removeChangeListener(this);
    this->instrument.removeChangeListener(this);
    
    this->draggingConnector = nullptr;
    this->background = nullptr;
    this->deleteAllChildren();
}

void InstrumentEditor::mouseDown(const MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
    }
}

InstrumentEditorNode *InstrumentEditor::getComponentForFilter(const uint32 filterID) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorNode *const fc = dynamic_cast <InstrumentEditorNode *>(getChildComponent(i))) {
            if (fc->filterID == filterID)
            { return fc; }
        }
    }
    
    return nullptr;
}

InstrumentEditorConnector *InstrumentEditor::getComponentForConnection(const AudioProcessorGraph::Connection &conn) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorConnector *const c = dynamic_cast <InstrumentEditorConnector *>(getChildComponent(i)))
        {
            if (c->sourceFilterID == conn.sourceNodeId
                && c->destFilterID == conn.destNodeId
                && c->sourceFilterChannel == conn.sourceChannelIndex
                && c->destFilterChannel == conn.destChannelIndex)
            {
                return c;
            }
        }
    }
    
    return nullptr;
}

InstrumentEditorPin *InstrumentEditor::findPinAt(const int x, const int y) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorNode *fc = dynamic_cast<InstrumentEditorNode *>(getChildComponent(i)))
        {
            if (InstrumentEditorPin *pin = dynamic_cast<InstrumentEditorPin *>(fc->getComponentAt(x - fc->getX(), y - fc->getY())))
            {
                return pin;
            }
        }
    }
    
    return nullptr;
}

void InstrumentEditor::resized()
{
    this->background->setBounds(0, 0, this->getWidth(), this->getHeight());
    this->updateComponents();
}

void InstrumentEditor::changeListenerCallback(ChangeBroadcaster *)
{
    this->updateComponents();
}

void InstrumentEditor::updateComponents()
{
    for (int i = this->getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorNode *const fc = dynamic_cast<InstrumentEditorNode *>(getChildComponent(i)))
        {
            fc->update();
        }
    }
    
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        InstrumentEditorConnector *const cc = dynamic_cast<InstrumentEditorConnector *>(getChildComponent(i));
        
        if (cc != nullptr && cc != draggingConnector)
        {
            if (instrument.getConnectionBetween(cc->sourceFilterID, cc->sourceFilterChannel,
                                                cc->destFilterID, cc->destFilterChannel) == nullptr)
            {
                delete cc;
            }
            else
            {
                cc->update();
            }
        }
    }
    
    for (int i = instrument.getNumNodes(); --i >= 0;)
    {
        const AudioProcessorGraph::Node::Ptr f(instrument.getNode(i));
        
        if (this->getComponentForFilter(f->nodeId) == nullptr)
        {
            auto const comp = new InstrumentEditorNode(instrument, f->nodeId);
            addAndMakeVisible(comp);
            comp->update();
        }
    }
    
    for (int i = instrument.getNumConnections(); --i >= 0;)
    {
        const AudioProcessorGraph::Connection *const c = instrument.getConnection(i);
        
        if (getComponentForConnection(*c) == nullptr)
        {
            auto const comp = new InstrumentEditorConnector(instrument);
            addAndMakeVisible(comp);
            
            comp->setInput(c->sourceNodeId, c->sourceChannelIndex);
            comp->setOutput(c->destNodeId, c->destChannelIndex);
        }
    }
}

void InstrumentEditor::beginConnectorDrag(const uint32 sourceFilterID, const int sourceFilterChannel,
                                          const uint32 destFilterID, const int destFilterChannel,
                                          const MouseEvent &e)
{
    draggingConnector = dynamic_cast <InstrumentEditorConnector *>(e.originalComponent);
    
    if (draggingConnector == nullptr)
    { draggingConnector = new InstrumentEditorConnector(instrument); }
    
    draggingConnector->setInput(sourceFilterID, sourceFilterChannel);
    draggingConnector->setOutput(destFilterID, destFilterChannel);
    
    addAndMakeVisible(draggingConnector);
    draggingConnector->toFront(false);
    
    dragConnector(e);
}

void InstrumentEditor::dragConnector(const MouseEvent &e)
{
    const MouseEvent e2(e.getEventRelativeTo(this));
    
    if (draggingConnector != nullptr)
    {
        draggingConnector->setTooltip(String::empty);
        
        int x = e2.x;
        int y = e2.y;
        
        if (InstrumentEditorPin *const pin = findPinAt(x, y))
        {
            uint32 srcFilter = draggingConnector->sourceFilterID;
            int srcChannel   = draggingConnector->sourceFilterChannel;
            uint32 dstFilter = draggingConnector->destFilterID;
            int dstChannel   = draggingConnector->destFilterChannel;
            
            if (srcFilter == 0 && ! pin->isInput)
            {
                srcFilter = pin->filterID;
                srcChannel = pin->index;
            }
            else if (dstFilter == 0 && pin->isInput)
            {
                dstFilter = pin->filterID;
                dstChannel = pin->index;
            }
            
            if (instrument.canConnect(srcFilter, srcChannel, dstFilter, dstChannel))
            {
                x = pin->getParentComponent()->getX() + pin->getX() + pin->getWidth() / 2;
                y = pin->getParentComponent()->getY() + pin->getY() + pin->getHeight() / 2;
                
                draggingConnector->setTooltip(pin->getTooltip());
            }
        }
        
        if (draggingConnector->sourceFilterID == 0)
        { draggingConnector->dragStart(x, y); }
        else
        { draggingConnector->dragEnd(x, y); }
    }
}

void InstrumentEditor::endDraggingConnector(const MouseEvent &e)
{
    if (draggingConnector == nullptr)
    { return; }
    
    draggingConnector->setTooltip(String::empty);
    
    const MouseEvent e2(e.getEventRelativeTo(this));
    
    uint32 srcFilter = draggingConnector->sourceFilterID;
    int srcChannel   = draggingConnector->sourceFilterChannel;
    uint32 dstFilter = draggingConnector->destFilterID;
    int dstChannel   = draggingConnector->destFilterChannel;
    
    draggingConnector = nullptr;
    
    if (InstrumentEditorPin *const pin = findPinAt(e2.x, e2.y))
    {
        if (srcFilter == 0)
        {
            if (pin->isInput)
            { return; }
            
            srcFilter = pin->filterID;
            srcChannel = pin->index;
        }
        else
        {
            if (! pin->isInput)
            { return; }
            
            dstFilter = pin->filterID;
            dstChannel = pin->index;
        }
        
        instrument.addConnection(srcFilter, srcChannel, dstFilter, dstChannel);
    }
}
