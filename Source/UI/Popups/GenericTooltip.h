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

class GenericTooltip final : public Component
{
public:

    explicit GenericTooltip(const String &tooltip) : message(tooltip)
    {
        this->setFocusContainerType(Component::FocusContainerType::none);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const auto messageBounds = this->getLocalBounds().reduced(5, 5);
        g.setColour(Colours::white.withAlpha(0.9f));

        if (App::isRunningOnPhone())
        {
            g.setFont(Globals::UI::Fonts::S);
        }
        else
        {
            g.setFont(Globals::UI::Fonts::M);
        }

        g.drawFittedText(this->message,
            messageBounds.getX(), messageBounds.getY(),
            messageBounds.getWidth(), messageBounds.getHeight(),
            Justification::centred, 3, 1.f);
    }
    
private:

    String message;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericTooltip)
};
