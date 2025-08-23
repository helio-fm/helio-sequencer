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

#include "ComponentFader.h"
#include "ColourIDs.h"

class ModalCallout final : public Component
{
public:

    ModalCallout(Component *newComponent,
                 SafePointer<Component> pointAtComponent,
                 bool shouldAlignToMouse);
    
    ~ModalCallout() override;
    
    //===------------------------------------------------------------------===//
    // Static
    //===------------------------------------------------------------------===//
    
    static void emit(Component *newComponent,
                     Component *pointAtComponent,
                     bool alignsToMousePosition = false);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void paint(Graphics &) override;
    void resized() override;
    void moved() override;
    void parentSizeChanged() override;
    void parentHierarchyChanged() override;
    void childBoundsChanged(Component *) override;
    bool hitTest(int x, int y) override;
    void inputAttemptWhenModal() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &) override;
    
private:

    int getBorderSize() const noexcept;
    void findTargetPointAndUpdateBounds();
    void pointToAndFit(const Rectangle<int>& newAreaToPointTo,
        const Rectangle<int>& newAreaToFitIn);

    static constexpr float arrowSize = 5.f;

    UniquePointer<Component> contentComponent;
    SafePointer<Component> targetComponent;

    Path outline;
    Point<float> targetPoint;
    Rectangle<int> areaToFitIn;
    Rectangle<int> areaToPointTo;

    Point<float> clickPointAbs;
    const bool alignsToMouse;
    
    const Colour fillColour = findDefaultColour(ColourIDs::Callout::fill);
    const Colour frameColour = findDefaultColour(ColourIDs::Callout::frame);

    void fadeIn();
    void fadeOut();
    void dismiss();
    void updateShape();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModalCallout)
};
