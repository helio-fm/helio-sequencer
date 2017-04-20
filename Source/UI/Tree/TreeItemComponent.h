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

#include "DraggingListBoxComponent.h"
#include "LongTapController.h"

class TreeItem;
class PianoLayerTreeItem;

class TreeItemComponent : public DraggingListBoxComponent, protected LongTapListener
{
public:

    explicit TreeItemComponent(TreeItem &i);
    ~TreeItemComponent() override;
    
    void setSelected(bool shouldBeSelected) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void handleCommandMessage(int commandId) override;
    void mouseDoubleClick(const MouseEvent &event) override;
    void mouseDown(const MouseEvent &event) override;

    TreeItem &item;

protected:

    void emitCallout();
    void emitRollover();

    bool isCompactMode() const;
    Colour getItemColour() const;
    
    ScopedPointer<LongTapController> longTapController;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponent);
    
};
