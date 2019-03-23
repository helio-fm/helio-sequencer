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

#include "ComponentFader.h"

class MainLayout;

class HelioCallout : public Component
{
public:

    HelioCallout(Component &contentComponent,
                 Component *pointAtComponent,
                 MainLayout *parent,
                 bool shouldAlignToMouse);
    
    ~HelioCallout() override;
    
    //===------------------------------------------------------------------===//
    // Static
    //===------------------------------------------------------------------===//
    
    static void emit(Component *newComponent,
                     Component *pointAtComponent,
                     bool alignsToMousePosition = false);
    
    static int numClicksSinceLastStartedPopup();
    
    static int numClicksSinceLastClosedPopup();
    
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void paint(Graphics &) override;

    void resized() override;

    void moved() override;

    void parentSizeChanged() override;
    
    void childBoundsChanged(Component *) override;

    bool hitTest(int x, int y) override;

    void inputAttemptWhenModal() override;

    bool keyPressed(const KeyPress &) override;

    void handleCommandMessage(int) override;

    int getBorderSize() const noexcept;
    
private:

    void setArrowSize(float newSize);
    void findTargetPointAndUpdateBounds();
    void pointToAndFit(const Rectangle<int>& newAreaToPointTo,
                       const Rectangle<int>& newAreaToFitIn);

    
    float arrowSize;
    Component &contentComponent;
    Path outline;
    Point<float> targetPoint;
    Rectangle<int> lastGoodAvailableArea, lastGoodTargetArea;
    Image background;

    SafePointer<Component> targetComponent;
    Point<float> clickPointAbs;
    bool alignsToMouse;
    
    SafePointer<Component> backgroundWhite;
    
    void fadeIn();
    void fadeOut();
    void dismissAsync();
    void updateShape();
    void drawBackground(Graphics &g, const Path &path, Image &cachedImage);
    
    friend class HelioCallOutCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelioCallout)
};
