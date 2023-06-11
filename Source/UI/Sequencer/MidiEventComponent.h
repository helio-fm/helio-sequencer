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
class RollBase;

#include "MidiEvent.h"
#include "FloatBoundsComponent.h"
#include "SelectableComponent.h"

class MidiEventComponent : public FloatBoundsComponent, public SelectableComponent
{
public:

    MidiEventComponent(RollBase &editor, bool ghostMode = false) noexcept;

    bool isActive() const noexcept;
    void setActive(bool val, bool force = false);
    void setGhostMode();

    virtual float getBeat() const noexcept = 0;
    virtual const MidiEvent::Id getId() const noexcept = 0;
    virtual void updateColours() = 0;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    static int compareElements(MidiEventComponent *c1, MidiEventComponent *c2) noexcept;

    //===------------------------------------------------------------------===//
    // SelectableComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    bool isSelected() const noexcept override;

protected:

    RollBase &roll;
    ComponentDragger dragger;

    struct MidiEventComponentFlags final
    {
        bool isSelected : 1;            // both clips and notes can be displayed as selected
        bool isInstanceOfSelected : 1;  // used to highlight all "instances" of selected clips
        bool isActive : 1;              // whether a note belongs to the selected track or not
        bool isGhost : 1;               // indicates helper notes which are used for visual cue
        bool isRecordingTarget : 1;     // indicates that a clip is used as a target to MIDI recording
        bool isMergeTarget : 1;         // indicates clips which are to be merged into one
    };

    union
    {
        uint8 componentFlags = 0;
        MidiEventComponentFlags flags;
    };

    float anchorBeat = 0.f;

    Point<int> clickOffset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiEventComponent)
};
