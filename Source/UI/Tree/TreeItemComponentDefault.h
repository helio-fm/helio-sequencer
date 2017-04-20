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

#include "TreeItemComponent.h"

class TreeItemComponentDefault : public TreeItemComponent
{
public:

    explicit TreeItemComponentDefault(TreeItem &i);

    static void paintBackground(Graphics &g,
                                int width, int height,
                                bool isSelected, bool isActive);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void resized() override;

    //===------------------------------------------------------------------===//
    // LongTapListener
    //===------------------------------------------------------------------===//

    void longTapEvent(const MouseEvent &e) override;

private:

    Component *createHighlighterComponent() override;

    void paintIcon(Graphics &g);
    void paintText(Graphics &g, const Rectangle<float> &area);
    
private:

    ComponentAnimator animator;

    ScopedPointer<Component> pageMarker;
    ScopedPointer<Component> menuButton;

    bool itemIsSelected;
    bool markerIsVisible;
    float textX;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeItemComponentDefault);

};
