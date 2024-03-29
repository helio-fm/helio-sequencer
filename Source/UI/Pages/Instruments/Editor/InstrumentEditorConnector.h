/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class InstrumentEditor;

#include "Instrument.h"

class InstrumentEditorConnector final :
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
    void getPinPoints(float &x1, float &y1, float &x2, float &y2) const;

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void resized() override;

    AudioProcessorGraph::Connection connection;

private:

    WeakReference<Instrument> instrument;

    float lastInputX = 0.f;
    float lastInputY = 0.f;
    float lastOutputX = 0.f;
    float lastOutputY = 0.f;

    Path linePath, hitPath;
    bool dragging = false;

    InstrumentEditor *getGraphPanel() const noexcept;

    void getDistancesFromEnds(int x, int y, double &distanceFromStart, double &distanceFromEnd) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEditorConnector)
};
