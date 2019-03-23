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
class PluginWindow;

#include "Instrument.h"

class InstrumentComponent : public Component
{
public:

    InstrumentComponent(WeakReference<Instrument> instrument, AudioProcessorGraph::NodeID nodeId);
    ~InstrumentComponent() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void paint(Graphics &g) override;
    void resized() override;

    void getPinPos(const int index, const bool isInput, float &x, float &y);
    void setSelected(bool selected);
    void update();

    const AudioProcessorGraph::NodeID nodeId;

private:

    WeakReference<Instrument> instrument;

    Font font;
    int pinSize;
    int isSelected;
    Point<int> originalPos;

    int numInputs;
    int numOutputs;

    bool hitTest(int x, int y) override;

    InstrumentEditor *getParentEditor() const noexcept;
    InstrumentComponent(const InstrumentComponent &);
};
