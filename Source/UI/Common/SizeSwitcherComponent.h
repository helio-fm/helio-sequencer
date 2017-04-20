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

#include "Icons.h"

class SizeSwitcherButton : public Component
{
public:

    SizeSwitcherButton()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void setImage(Image &targetImage)
    {
        this->image = targetImage;
        this->setSize(targetImage.getWidth(), targetImage.getHeight());
    }

    void paint(Graphics &g) override
    {
        Icons::drawImageRetinaAware(this->image, g, (this->getWidth() / 2) + 1, (this->getHeight() / 2));
    }

private:

    Image image;

};


class SizeSwitcherComponent :
    public Component,
    public ActionBroadcaster
{
public:

    enum ColourIds
    {
        borderColourId       = 0x99008000,
    };

    SizeSwitcherComponent(Component *target, int min, int mid, int max);


    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;

    void resized() override;

    void parentHierarchyChanged() override;

    void parentSizeChanged() override;

    void mouseEnter(const MouseEvent &e) override;

    void mouseExit(const MouseEvent &e) override;

    void mouseUp(const MouseEvent &e) override;

private:
    
    void updateButtonsImages();

    void trigger();

    bool shouldTriggerMinimization() const;

    SafePointer<Component> targetComponent;

    int minWidth;

    int midWidth;

    int maxWidth;

private:

    float alpha;

    SizeSwitcherButton minButton;

    SizeSwitcherButton maxButton;

    ComponentAnimator animator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SizeSwitcherComponent)
};
