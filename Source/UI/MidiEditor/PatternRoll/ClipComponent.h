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

class MidiLayer;
class HybridRoll;

#include "Clip.h"
#include "FloatBoundsComponent.h"
#include "SelectableComponent.h"

class ClipComponent : public FloatBoundsComponent, public SelectableComponent
{
public:

    ClipComponent(HybridRoll &editor, Clip clip);
    const Clip getClip() const;

    float getBeat() const;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;

    static int compareElements(ClipComponent *first, ClipComponent *second);

	//===------------------------------------------------------------------===//
	// SelectableComponent
	//===------------------------------------------------------------------===//

	void setSelected(const bool selected) override;
	
	bool isSelected() const override;

	String getSelectionGroupId() const override;

protected:

    HybridRoll &roll;
    const Clip clip;

    ComponentDragger dragger;

    bool selectedState;
    float anchorBeat;
    Colour colour;
    Point<int> clickOffset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};
