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

class MidiSequence;
class RollBase;
class PatternRoll;

#include "Clip.h"
#include "MidiEventComponent.h"

class ClipComponent : public MidiEventComponent
{
public:

    ClipComponent(RollBase &editor, const Clip &clip);

    enum class State : uint8
    {
        None,
        Dragging,
        Tuning
    };

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    const Clip &getClip() const noexcept;
    PatternRoll &getRoll() const noexcept;

    void updateColours() override;
    inline const Colour &getEventColour() const noexcept
    { return this->eventColour; }

    Rectangle<int> getTextArea() const noexcept;

    //===------------------------------------------------------------------===//
    // MidiEventComponent
    //===------------------------------------------------------------------===//

    const String &getSelectionGroupId() const noexcept override;
    float getBeat() const noexcept override;
    const MidiEvent::Id getId() const noexcept override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void paint(Graphics &g) override;

    //===------------------------------------------------------------------===//
    // Painting flags
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    void setHighlightedAsInstance(bool isHighlighted);
    void setHighlightedAsMergeTarget(bool isHighlighted);

    static int compareElements(ClipComponent *first, ClipComponent *second);

protected:

    const Clip &clip;

    Clip anchor;

    void startDragging();
    bool isDragging() const noexcept;
    bool getDraggingDelta(const MouseEvent &e, float &deltaBeat);
    Clip continueDragging(float deltaBeat);
    void endDragging();

    void startTuning();
    Clip continueTuning(const MouseEvent &e) const noexcept;
    Clip continueTuningLinear(float delta) const noexcept;
    void endTuning();

    friend class PatternRoll;

    bool firstChangeDone = false;
    void checkpointIfNeeded();
    void setNoCheckpointNeededForNextAction();

    State state = State::None;

    Colour fillColour;
    Colour frameColour;
    Colour frameBorderColour;
    Colour frameCornerColour;

    Colour eventColour;
    Colour eventMutedColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};
