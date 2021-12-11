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

class ProjectMapScroller;

class ProjectMapScrollerScreen final : public Component
{
public:

    explicit ProjectMapScrollerScreen(ProjectMapScroller &scrollerRef);

    Rectangle<float> getRealBounds() const noexcept
    {
        return this->realBounds;
    }

    void setRealBounds(const Rectangle<float> &bounds)
    {
        this->realBounds = bounds;
        this->setBounds(this->realBounds.toType<int>());
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    void paint(Graphics &g) override;

private:
    
    Rectangle<float> realBounds;

    Colour colour;
    ProjectMapScroller &scroller;
    ComponentDragger dragger;

    UniquePointer<ComponentBoundsConstrainer> moveConstrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectMapScrollerScreen)
};
