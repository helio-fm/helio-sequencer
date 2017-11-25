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

class MidiEvent;
class MidiSequence;
class HybridRoll;

#include "FloatBoundsComponent.h"
#include "SelectableComponent.h"

class HybridRollEventComponent :
    public FloatBoundsComponent, 
    public SelectableComponent
{
public:

    HybridRollEventComponent(HybridRoll &editor, bool ghostMode = false);

    bool isActive() const;
    void setActive(bool val, bool force = false);
    void setGhostMode();

    virtual float getBeat() const = 0;
    virtual String getId() const = 0;
    virtual void updateColours() = 0;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    static int compareElements(HybridRollEventComponent *first, HybridRollEventComponent *second);

    //===------------------------------------------------------------------===//
    // SelectableComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    bool isSelected() const override;

protected:

    HybridRoll &roll;
    ComponentDragger dragger;

    bool selectedState;
    bool activeState;
    float anchorBeat;

    Colour colour;
    bool ghostMode;

    // сдвиг мыши от нуля компонента во время клика.
    // если его не учитывать, то ноты двигаются неестественно
    Point<int> clickOffset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HybridRollEventComponent)
};
