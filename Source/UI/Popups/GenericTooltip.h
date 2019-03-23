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

class GenericTooltip final : public Component
{
public:

    explicit GenericTooltip(const String &tooltip) : message(tooltip)
    {
        this->setFocusContainer(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const Rectangle<int> messageBounds(this->getLocalBounds().reduced(10, 10));
        g.setColour(Colours::white.withAlpha(0.85f));
#if HELIO_DESKTOP
        g.setFont(21.f);
#elif HELIO_MOBILE
        g.setFont(28.f);
#endif
        g.drawFittedText(this->message,
                         messageBounds.getX(),
                         messageBounds.getY(),
                         messageBounds.getWidth(),
                         messageBounds.getHeight(),
                         Justification::centred,
                         3,
                         1.f);
    }
    
private:

    String message;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericTooltip)
};
