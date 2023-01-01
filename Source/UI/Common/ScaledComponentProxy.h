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

#include "ColourIDs.h"

class ScaledComponentProxy final : public Component
{
public:

    explicit ScaledComponentProxy(Component *target) :
        content(target)
    {
        jassert(this->content != nullptr);
        this->content->setTransform(App::Config().getUiFlags()->getScaledTransformFor(this->content));

        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);

        this->setSize(this->content->getWidth(), this->content->getHeight());
        this->addAndMakeVisible(this->content);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->backgroundColour);
        g.fillRect(this->getLocalBounds());
    }

    void resized() override
    {
        jassert(this->content != nullptr);
        this->content->setBounds(this->getLocalBounds() / this->scale);
    }

private:

    SafePointer<Component> content;

    const float scale = App::Config().getUiFlags()->getUiScaleFactor();
    const Colour backgroundColour = findDefaultColour(ColourIDs::Backgrounds::pageFillA);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaledComponentProxy)
};
