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

InstrumentEditorNode *InstrumentEditor::getComponentForNode(AudioProcessorGraph::NodeID id) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorNode *const fc = dynamic_cast <InstrumentEditorNode *>(getChildComponent(i))) {
            if (fc->filterID == id)
            { return fc; }
        }
    }
    
    return nullptr;
}

InstrumentEditorConnector *InstrumentEditor::getComponentForConnection(AudioProcessorGraph::Connection conn) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (InstrumentEditorConnector *const c =
            dynamic_cast <InstrumentEditorConnector *>(getChildComponent(i)))
        {
            if (c->connection == conn)
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
            if (! instrument.isConnected(cc->connection))
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
        if (this->getComponentForNode(f->nodeID) == nullptr)
        {
            auto const comp = new InstrumentEditorNode(instrument, f->nodeID);
            addAndMakeVisible(comp);
            comp->update();
        }
    }
    
    const auto &connections = instrument.getConnections();
    for (int i = connections.size(); --i >= 0;)
    {
        AudioProcessorGraph::Connection c = connections.at(i);
        if (getComponentForConnection(c) == nullptr)
        {
            auto const comp = new InstrumentEditorConnector(instrument);
            addAndMakeVisible(comp);
            comp->setInput(c.source);
            comp->setOutput(c.destination);
        }
    }
}

void InstrumentEditor::beginConnectorDrag(
    AudioProcessorGraph::NodeID sourceID, int sourceChannel,
    AudioProcessorGraph::NodeID destinationID, int destinationChannel,
    const MouseEvent &e)
{
    draggingConnector = dynamic_cast <InstrumentEditorConnector *>(e.originalComponent);
    
    if (draggingConnector == nullptr)
    { draggingConnector = new InstrumentEditorConnector(instrument); }
    
    AudioProcessorGraph::NodeAndChannel source;
    source.nodeID = sourceID;
    source.channelIndex = sourceChannel;

    AudioProcessorGraph::NodeAndChannel destination;
    destination.nodeID = destinationID;
    destination.channelIndex = destinationChannel;

    draggingConnector->setInput(source);
    draggingConnector->setOutput(destination);
    
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
            auto c = draggingConnector->connection;
            
            if (c.source.nodeID == 0 && ! pin->isInput)
            {
                c.source.nodeID = pin->nodeID;
                c.source.channelIndex = pin->index;
            }
            else if (c.destination.nodeID == 0 && pin->isInput)
            {
                c.destination.nodeID = pin->nodeID;
                c.destination.channelIndex = pin->index;
            }
            
            if (instrument.canConnect(c))
            {
                x = pin->getParentComponent()->getX() + pin->getX() + pin->getWidth() / 2;
                y = pin->getParentComponent()->getY() + pin->getY() + pin->getHeight() / 2;
                
                draggingConnector->setTooltip(pin->getTooltip());
            }
        }
        
        if (draggingConnector->connection.source.nodeID == 0)
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
    
    const auto c = draggingConnector->connection;
    auto srcNode = c.source.nodeID;
    auto srcChannel = c.source.channelIndex;
    auto dstNode = c.destination.nodeID;
    auto dstChannel = c.destination.channelIndex;
    
    draggingConnector = nullptr;
    
    if (InstrumentEditorPin *const pin = findPinAt(e2.x, e2.y))
    {
        if (srcNode == 0)
        {
            if (pin->isInput)
            { return; }
            
            srcNode = pin->nodeID;
            srcChannel = pin->index;
        }
        else
        {
            if (! pin->isInput)
            { return; }
            
            dstNode = pin->nodeID;
            dstChannel = pin->index;
        }
        
        instrument.addConnection(srcNode, srcChannel, dstNode, dstChannel);
    }
}
