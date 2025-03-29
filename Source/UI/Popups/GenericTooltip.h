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

#include "HelioTheme.h"
#include "ColourIDs.h"

class GenericTooltip final : public Component
{
public:

    explicit GenericTooltip(const String &tooltip) :
        message(tooltip)
    {
        this->setFocusContainerType(Component::FocusContainerType::none);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }
    
    explicit GenericTooltip(UniquePointer<Component> &&content) :
        content(move(content))
    {
        this->setFocusContainerType(Component::FocusContainerType::none);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);

        this->addAndMakeVisible(this->content.get());
    }

    void paint(Graphics &g) override
    {
        if (this->message.isEmpty())
        {
            return;
        }

        g.setColour(findDefaultColour(ColourIDs::Tooltip::messageText));

        if (App::isRunningOnPhone())
        {
            g.setFont(Globals::UI::Fonts::S);
        }
        else
        {
            g.setFont(Globals::UI::Fonts::M);
        }

        const auto messageBounds = this->getLocalBounds().reduced(5, 5);
        HelioTheme::drawFittedText(g,
            this->message,
            messageBounds.getX(), messageBounds.getY(),
            messageBounds.getWidth(), messageBounds.getHeight(),
            Justification::centred, 3, 1.f);
    }

    void resized() override
    {
        if (this->content != nullptr)
        {
            this->content->setBounds(this->getLocalBounds().
                withSizeKeepingCentre(this->content->getWidth(), this->content->getHeight()));
        }
    }
    
private:

    String message;

    UniquePointer<Component> content;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericTooltip)
};
