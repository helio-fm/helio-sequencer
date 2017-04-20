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

class GenericTooltip : public Component
{
public:

    explicit GenericTooltip(String tooltip) : message(std::move(tooltip))
    {
        this->setFocusContainer(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const Rectangle<int> messageBounds(this->getLocalBounds().reduced(10, 10));
        g.setColour(Colours::white.withAlpha(0.8f));
        g.setFont(Font(Font::getDefaultSansSerifFontName(), 30.f, Font::plain));
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

};
