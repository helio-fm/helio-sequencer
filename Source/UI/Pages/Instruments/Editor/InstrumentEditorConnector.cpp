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
#include "InstrumentEditorConnector.h"
#include "Instrument.h"
#include "InstrumentEditor.h"
#include "InstrumentEditorNode.h"
#include "ColourIDs.h"

InstrumentEditorConnector::InstrumentEditorConnector(WeakReference<Instrument> instrument) :
    connection({}, {}),
    instrument(instrument),
    lastInputX(0),
    lastInputY(0),
    lastOutputX(0),
    lastOutputY(0)
{
    this->setPaintingIsUnclipped(true);
    this->setAlwaysOnTop(true);
}

void InstrumentEditorConnector::setInput(const AudioProcessorGraph::NodeAndChannel node)
{
    if (this->connection.source != node)
    {
        this->connection.source = node;
        this->update();
    }
}

void InstrumentEditorConnector::setOutput(const AudioProcessorGraph::NodeAndChannel node)
{
    if (this->connection.destination != node)
    {
        this->connection.destination = node;
        this->update();
    }
}

void InstrumentEditorConnector::dragStart(int x, int y)
{
    this->lastInputX = static_cast<float>( x);
    this->lastInputY = static_cast<float>( y);
    this->resizeToFit();
}

void InstrumentEditorConnector::dragEnd(int x, int y)
{
    this->lastOutputX = static_cast<float>( x);
    this->lastOutputY = static_cast<float>( y);
    this->resizeToFit();
}

void InstrumentEditorConnector::update()
{
    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    if (lastInputX != x1
        || lastInputY != y1
        || lastOutputX != x2
        || lastOutputY != y2)
    {
        this->resizeToFit();
    }
}

void InstrumentEditorConnector::resizeToFit()
{
    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    const Rectangle<int> newBounds(static_cast<int>(jmin(x1, x2)) - 4,
                                   static_cast<int>(jmin(y1, y2)) - 4,
                                   static_cast<int>(fabsf(x1 - x2)) + 8,
                                   static_cast<int>(fabsf(y1 - y2)) + 8);

    if (newBounds != this->getBounds())
    { this->setBounds(newBounds); }
    else
    { this->resized(); }
}

void InstrumentEditorConnector::getPoints(float &x1, float &y1, float &x2, float &y2) const
{
    x1 = lastInputX;
    y1 = lastInputY;
    x2 = lastOutputX;
    y2 = lastOutputY;

    if (InstrumentEditor *const hostPanel = this->getGraphPanel())
    {
        if (InstrumentEditorNode *srcNodeComp =
            hostPanel->getComponentForNode(this->connection.source.nodeID))
        {
            srcNodeComp->getPinPos(this->connection.source.channelIndex, false, x1, y1);
        }

        if (InstrumentEditorNode *dstNodeComp =
            hostPanel->getComponentForNode(this->connection.destination.nodeID))
        {
            dstNodeComp->getPinPos(this->connection.destination.channelIndex, true, x2, y2);
        }
    }
}

void InstrumentEditorConnector::paint(Graphics &g)
{
    g.setColour(this->findColour(ColourIDs::Instrument::shadowConnector));
    g.fillPath(linePath, AffineTransform::translation(0, 1));
    
    const bool isMidiConnector =
        (this->connection.source.channelIndex == Instrument::midiChannelNumber ||
        this->connection.destination.channelIndex == Instrument::midiChannelNumber);

    const Colour lineColour = isMidiConnector ?
        this->findColour(ColourIDs::Instrument::midiConnector) :
        this->findColour(ColourIDs::Instrument::audioConnector);

    g.setColour(lineColour);
    g.fillPath(linePath);
}

bool InstrumentEditorConnector::hitTest(int x, int y)
{
    if (hitPath.contains(static_cast<float>( x), static_cast<float>( y)))
    {
        double distanceFromStart, distanceFromEnd;
        this->getDistancesFromEnds(x, y, distanceFromStart, distanceFromEnd);

        // avoid clicking the connector when over a pin
        return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
    }

    return false;
}

void InstrumentEditorConnector::mouseDown(const MouseEvent &)
{
    this->dragging = false;
}

void InstrumentEditorConnector::mouseDrag(const MouseEvent &e)
{
    if ((!this->dragging) && ! e.mouseWasClicked())
    {
        this->dragging = true;
        this->instrument->removeConnection(this->connection);

        double distanceFromStart = 0.0, distanceFromEnd = 0.0;
        this->getDistancesFromEnds(e.x, e.y, distanceFromStart, distanceFromEnd);
        const bool isNearerSource = (distanceFromStart < distanceFromEnd);
        const AudioProcessorGraph::NodeID nodeIdZero;

        this->getGraphPanel()->beginConnectorDrag(
            isNearerSource ? nodeIdZero : this->connection.source.nodeID,
            this->connection.source.channelIndex,
            isNearerSource ? this->connection.destination.nodeID : nodeIdZero,
            this->connection.destination.channelIndex,
            e);
    }
    else if (this->dragging)
    {
        this->getGraphPanel()->dragConnector(e);
    }
}

void InstrumentEditorConnector::mouseUp(const MouseEvent &e)
{
    if (this->dragging)
    { this->getGraphPanel()->endDraggingConnector(e); }
}

void InstrumentEditorConnector::resized()
{
    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    lastInputX = x1;
    lastInputY = y1;
    lastOutputX = x2;
    lastOutputY = y2;

    x1 -= this->getX();
    y1 -= this->getY();
    x2 -= this->getX();
    y2 -= this->getY();

    const float dy = (y2 - y1);
    const float dx = (x2 - x1);

    linePath.clear();
    linePath.startNewSubPath(x1, y1);

    const float curvinessX = (this->getParentWidth() == 0) ? 0.f : (1.f - (fabs(dx) / float(this->getParentWidth()))) * 1.5f;
    const float curvinessY = (this->getParentHeight() == 0) ? 0.f : (fabs(dy) / float(this->getParentHeight())) * 1.5f;
    const float curviness = (curvinessX + curvinessY) / 2.f;

    float gravity = dy / (float(this->getParentHeight()) / 3.f);
    gravity = jmax(-1.f, gravity);
    gravity = jmin(1.f, gravity);
    gravity = (gravity / 3.f) + 0.45f;

    linePath.cubicTo(x1 + dx * (curviness * (1.f - gravity)), y1,
                     x1 + dx * (1.f - (curviness * gravity)), y2,
                     x2, y2);

    PathStrokeType wideStroke(8.0f);
    wideStroke.createStrokedPath(hitPath, linePath);

    PathStrokeType stroke(6.5f, PathStrokeType::beveled, PathStrokeType::rounded);
    stroke.createStrokedPath(linePath, linePath);

    linePath.setUsingNonZeroWinding(false);
}

InstrumentEditor *InstrumentEditorConnector::getGraphPanel() const noexcept
{
    return this->findParentComponentOfClass<InstrumentEditor>();
}

void InstrumentEditorConnector::getDistancesFromEnds(int x, int y, double &distanceFromStart, double &distanceFromEnd) const
{
    float x1, y1, x2, y2;
    this->getPoints(x1, y1, x2, y2);

    distanceFromStart = juce_hypot(x - (x1 - getX()), y - (y1 - getY()));
    distanceFromEnd = juce_hypot(x - (x2 - getX()), y - (y2 - getY()));
}
