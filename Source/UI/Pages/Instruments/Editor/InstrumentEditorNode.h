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

class Instrument;
class InstrumentEditor;
class PluginWindow;

class InstrumentEditorNode : public Component
{
public:

    InstrumentEditorNode(Instrument &graph, AudioProcessorGraph::NodeID nodeId);
    ~InstrumentEditorNode() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void paint(Graphics &g) override;
    void resized() override;

    void getPinPos(const int index, const bool isInput, float &x, float &y);
    void update();

    const AudioProcessorGraph::NodeID nodeId;

    int numInputs;
    int numOutputs;

private:

    Instrument &instrument;

    int pinSize;
    Point<int> originalPos;

    Font font;

    int numIns;
    int numOuts;

    bool hitTest(int x, int y) override;

    InstrumentEditor *getGraphPanel() const noexcept;
    InstrumentEditorNode(const InstrumentEditorNode &);
    InstrumentEditorNode &operator= (const InstrumentEditorNode &);

};
