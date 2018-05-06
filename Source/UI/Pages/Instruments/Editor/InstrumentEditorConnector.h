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

#pragma once

class InstrumentEditor;

#include "Instrument.h"

class InstrumentEditorConnector :
    public Component,
    public SettableTooltipClient
{
public:

    explicit InstrumentEditorConnector(WeakReference<Instrument> instrument);

    void setInput(const AudioProcessorGraph::NodeAndChannel node);
    void setOutput(const AudioProcessorGraph::NodeAndChannel node);

    void dragStart(int x, int y);
    void dragEnd(int x, int y);

    void update();
    void resizeToFit();
    void getPoints(float &x1, float &y1, float &x2, float &y2) const;

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const MouseEvent &) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void resized() override;

    AudioProcessorGraph::Connection connection;

private:

    WeakReference<Instrument> instrument;

    float lastInputX, lastInputY, lastOutputX, lastOutputY;
    Path linePath, hitPath;
    bool dragging;

    InstrumentEditor *getGraphPanel() const noexcept;

    void getDistancesFromEnds(int x, int y, double &distanceFromStart, double &distanceFromEnd) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEditorConnector)
};
