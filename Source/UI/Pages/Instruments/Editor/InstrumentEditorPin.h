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

class InstrumentEditorPin :
    public Component,
    public SettableTooltipClient
{
public:

    InstrumentEditorPin(AudioProcessorGraph::NodeID nodeId, int index, bool isInput);

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    const AudioProcessorGraph::NodeID nodeId;
    const int index;
    const bool isInput;

private:

    InstrumentEditor *getParentEditor() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentEditorPin)
};
