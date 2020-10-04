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

#include "IconComponent.h"
#include "OverlayButton.h"
#include "Network.h"
#include "SessionService.h"

class LoginButton final : public Component
{
public:

    LoginButton()
    {
        this->avatar = make<IconComponent>(Icons::github);
        this->addAndMakeVisible(this->avatar.get());

        this->ctaLabel = make<Label>(String(), TRANS(I18n::Dialog::authGithub));
        this->addAndMakeVisible(this->ctaLabel.get());
        this->ctaLabel->setFont({ 16.f });
        this->ctaLabel->setJustificationType(Justification::centred);
        this->ctaLabel->setColour(Label::textColourId,
            findDefaultColour(Label::textColourId).withMultipliedAlpha(0.35f));

        this->clickHandler = make<OverlayButton>();
        this->addAndMakeVisible(this->clickHandler.get());

        this->clickHandler->onClick = []() {
            App::Network().getSessionService()->signIn("Github");
        };
    }

    void resized() override
    {
        static constexpr auto imageSize = 20;
        static Rectangle<int> imageBounds(0, 0, imageSize, imageSize);
        this->clickHandler->setBounds(this->getLocalBounds());
        this->ctaLabel->setBounds(this->getLocalBounds()
            .withTrimmedTop(imageSize));
        this->avatar->setBounds(imageBounds
            .withCentre(this->getLocalBounds().getCentre())
            .withY(4));
    }

private:

    UniquePointer<IconComponent> avatar;
    UniquePointer<Label> ctaLabel;
    UniquePointer<OverlayButton> clickHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginButton)
};
