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
#include "InstrumentEditorPin.h"
#include "InstrumentEditor.h"
#include "ColourIDs.h"

InstrumentEditorPin::InstrumentEditorPin(AudioProcessorGraph::NodeID nodeID, int index, bool isInput) :
    nodeID(nodeID),
    index(index),
    isInput(isInput)
{
    this->setSize(18, 18);
    this->setWantsKeyboardFocus(false);
}

void InstrumentEditorPin::paint(Graphics &g)
{
    using namespace ColourIDs::Instrument;

    const float w = float(this->getWidth());
    const float h = float(this->getHeight());
    const bool isMidiChannel = (this->index == Instrument::midiChannelNumber);

    g.setColour(this->findColour(shadowPin));
    g.drawEllipse(3.f, 4.f, w - 6.f, h - 6.f, 4.f);

    const int colourId = isMidiChannel ?
        (this->isInput ? midiIn : midiOut) :
        (this->isInput ? audioIn : audioOut);

    g.setColour(this->findColour(colourId));
    g.drawEllipse(3.f, 3.f, w - 6.f, h - 6.f, 4.f);
}

void InstrumentEditorPin::mouseDown(const MouseEvent &e)
{
    const AudioProcessorGraph::NodeID sourceId(isInput ? 0 : nodeID.uid);
    const AudioProcessorGraph::NodeID destinationId(isInput ? nodeID.uid : 0);
    this->getParentEditor()->beginConnectorDrag(sourceId, index, destinationId, index, e);
}

void InstrumentEditorPin::mouseDrag(const MouseEvent &e)
{
    this->getParentEditor()->dragConnector(e);
}

void InstrumentEditorPin::mouseUp(const MouseEvent &e)
{
    this->getParentEditor()->endDraggingConnector(e);
}

InstrumentEditor *InstrumentEditorPin::getParentEditor() const noexcept
{
    return findParentComponentOfClass<InstrumentEditor>();
}
