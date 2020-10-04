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

#include "Workspace.h"
#include "UserProfile.h"
#include "OverlayButton.h"

class UserProfileComponent final : public Component
{
public:

    UserProfileComponent()
    {
        this->nameLabel = make<Label>();
        this->addAndMakeVisible(this->nameLabel.get());
        this->nameLabel->setFont({ 16.f });
        this->nameLabel->setJustificationType(Justification::centred);
        this->nameLabel->setColour(Label::textColourId,
            findDefaultColour(Label::textColourId).withMultipliedAlpha(0.35f));

        this->avatar = make<IconComponent>(Icons::github);
        this->addAndMakeVisible(this->avatar.get());

        this->clickHandler = make<OverlayButton>();
        this->addAndMakeVisible(this->clickHandler.get());
        // hidden at the moment, the user page is not implemented
        this->clickHandler->setVisible(false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::black.withAlpha(0.35f));
        g.fillRect(this->avatar->getBounds().expanded(2, -1));
        g.fillRect(this->avatar->getBounds().expanded(-1, 2));
        g.setColour(Colours::white.withAlpha(0.25f));
        g.fillRect(this->avatar->getBounds().expanded(1, 0));
        g.fillRect(this->avatar->getBounds().expanded(0, 1));
    }

    void resized() override
    {
        static constexpr auto imageSize = 16;
        static Rectangle<int> imageBounds(0, 0, imageSize, imageSize);
        this->clickHandler->setBounds(this->getLocalBounds());
        this->nameLabel->setBounds(this->getLocalBounds()
            .withTrimmedTop(imageSize));
        this->avatar->setBounds(imageBounds
            .withCentre(this->getLocalBounds().getCentre())
            .withY(4));
    }

    void updateProfileInfo()
    {
        const auto &userProfile = App::Workspace().getUserProfile();
        if (userProfile.isLoggedIn())
        {
            this->avatar->setIconImage(userProfile.getAvatar());
            this->nameLabel->setText("/" + userProfile.getLogin(), dontSendNotification);
            //this->clickHandler->onClick = []() {
            //    URL(App::Workspace().getUserProfile().getProfileUrl()).launchInDefaultBrowser();
            //};
        }
    }

private:

    UniquePointer<Label> nameLabel;
    UniquePointer<IconComponent> avatar;
    UniquePointer<OverlayButton> clickHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfileComponent)
};
