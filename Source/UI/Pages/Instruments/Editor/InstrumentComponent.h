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
class InstrumentEditorPin;

#include "Instrument.h"
#include "ColourIDs.h"

class InstrumentComponent final : public Component
{
public:

    InstrumentComponent(WeakReference<Instrument> instrument,
        AudioProcessorGraph::NodeID nodeId);
    
    ~InstrumentComponent() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    bool hitTest(int x, int y) override;
    void paint(Graphics &g) override;
    void resized() override;

    void getPinPos(const int index, const bool isInput, float &x, float &y);
    void setSelected(bool selected);
    void update();

    const AudioProcessorGraph::NodeID nodeId;

private:

    WeakReference<Instrument> instrument;

    OwnedArray<InstrumentEditorPin> pinComponents;

#if PLATFORM_DESKTOP
    const int pinSize = 18;
    const Font font = Globals::UI::Fonts::M;
#elif PLATFORM_MOBILE
    const int pinSize = 22;
    const Font font = Globals::UI::Fonts::XS;
#endif

    const Colour fillColour = findDefaultColour(ColourIDs::Instrument::fill);
    const Colour outlineColour = findDefaultColour(ColourIDs::Instrument::outline);
    const Colour textColour = findDefaultColour(ColourIDs::Instrument::text);

    bool isSelected = false;
    Point<int> originalPos;

    int numInputs = 0;
    int numOutputs = 0;

    InstrumentEditor *getParentEditor() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentComponent)
};
