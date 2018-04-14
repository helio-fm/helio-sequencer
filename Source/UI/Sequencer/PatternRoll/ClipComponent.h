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

class MidiSequence;
class HybridRoll;
class PatternRoll;

#include "Clip.h"
#include "MidiEventComponent.h"

class ClipComponent : public MidiEventComponent
{
public:

    ClipComponent(HybridRoll &editor, const Clip &clip);

    enum State
    {
        None,
        Dragging
    };

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    const Clip &getClip() const noexcept;
    PatternRoll &getRoll() const noexcept;

    void updateColours() override;

    //===------------------------------------------------------------------===//
    // MidiEventComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    String getSelectionGroupId() const override;
    float getBeat() const override;
    String getId() const override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
    void paint(Graphics& g) override;

    static int compareElements(ClipComponent *first, ClipComponent *second);

protected:

    const Clip &clip;

    Colour headColour;
    Colour headColourLighter;
    Colour headColourDarker;
    Colour fillColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};
