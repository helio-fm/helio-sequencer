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

#include "PianoSequenceMap.h"
#include "ClipComponent.h"

class PianoClipComponent final : public ClipComponent, public PianoSequenceMap
{
public:

    PianoClipComponent(ProjectTreeItem &project, MidiSequence *sequence,
        HybridRoll &roll, const Clip &clip) :
        ClipComponent(roll, clip),
        PianoSequenceMap(project, sequence, roll) {}

    // Avoid `inherits via dominance` warnings
    // and be more explicit in general, when it comes to multiple inheritance:
    void resized() override { PianoSequenceMap::resized(); }
    void mouseDoubleClick(const MouseEvent &e) override { ClipComponent::mouseDoubleClick(e); }
    void mouseDown(const MouseEvent &e) override { ClipComponent::mouseDown(e); }
    void mouseDrag(const MouseEvent &e) override { ClipComponent::mouseDrag(e); }
    void mouseUp(const MouseEvent &e) override { ClipComponent::mouseUp(e); }
    void paint(Graphics &g) override { ClipComponent::paint(g); }

protected:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoClipComponent)
};
