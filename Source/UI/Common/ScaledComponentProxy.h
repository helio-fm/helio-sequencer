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

class ScaledComponentProxy final : public Component
{
public:

    explicit ScaledComponentProxy(Component *target, int targetScale = 2) :
        content(target),
        scale(targetScale)
    {
        const auto screenArea = Desktop::getInstance().getDisplays().getMainDisplay().userArea;

        const float realWidth = float(screenArea.getWidth()) / float(this->scale);
        const float realHeight = float(screenArea.getHeight()) / float(this->scale);

        const float realScaleWidth = float(screenArea.getWidth()) / realWidth;
        const float realScaleheight = float(screenArea.getHeight()) / realHeight;

        const auto scaled = this->content->getTransform().scaled(realScaleWidth, realScaleheight);
        this->content->setTransform(scaled);

        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
        this->addAndMakeVisible(this->content);
    }

    void resized() override
    {
        if (this->content != nullptr)
        {
            this->content->setBounds(this->getLocalBounds() / this->scale);
        }
    }

private:

    SafePointer<Component> content;
    int scale;
};
