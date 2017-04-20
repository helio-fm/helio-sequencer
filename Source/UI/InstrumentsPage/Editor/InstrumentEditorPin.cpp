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
#include "Instrument.h"
#include "InstrumentEditor.h"

InstrumentEditorPin::InstrumentEditorPin(Instrument &graph_, const uint32 filterID_,
        const int index_, const bool isInput_) :
    filterID(filterID_),
    index(index_),
    isInput(isInput_),
    graph(graph_)
{
    setSize(16, 16);
}

void InstrumentEditorPin::paint(Graphics &g)
{
    const float w = static_cast<float>( getWidth());
    const float h = static_cast<float>( getHeight());
    const bool isMidiChannel = (this->index == Instrument::midiChannelNumber);

    if (this->isInput)
    {
        if (isMidiChannel)
        { g.setColour(this->findColour(InstrumentEditor::midiInColourId)); }
        else
        { g.setColour(this->findColour(InstrumentEditor::audioInColourId)); }

        g.drawEllipse(w * 0.1f, h * 0.1f, w * 0.8f, h * 0.8f, 4.000f);
    }
    else
    {
        Colour pinFill;

        if (isMidiChannel)
        { pinFill = (this->findColour(InstrumentEditor::midiOutColourId)); }
        else
        { pinFill = (this->findColour(InstrumentEditor::audioOutColourId)); }

        g.setColour(pinFill);
        g.fillEllipse(0, 0, w, h);

        g.setColour(pinFill.withAlpha(0.15f));
        g.fillEllipse(4, 4, w - 8, h - 8);
    }
}

void InstrumentEditorPin::mouseDown(const MouseEvent &e)
{
    getGraphPanel()->beginConnectorDrag(isInput ? 0 : filterID,
                                        index,
                                        isInput ? filterID : 0,
                                        index,
                                        e);
}

void InstrumentEditorPin::mouseDrag(const MouseEvent &e)
{
    getGraphPanel()->dragConnector(e);
}

void InstrumentEditorPin::mouseUp(const MouseEvent &e)
{
    getGraphPanel()->endDraggingConnector(e);
}

InstrumentEditor *InstrumentEditorPin::getGraphPanel() const noexcept
{
    return findParentComponentOfClass<InstrumentEditor>();
}
