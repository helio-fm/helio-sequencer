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

#include "CommandIDs.h"
#include "OverlayButton.h"
#include "SeparatorVertical.h"
#include "IconComponent.h"
#include "Workspace.h"

class OpenProjectButton final : public Component
{
public:

    OpenProjectButton()
    {
        this->openProjectIcon = make<IconComponent>(Icons::browse);
        this->addAndMakeVisible(this->openProjectIcon.get());

        this->openProjectLabel = make<Label>(String(), TRANS(I18n::Menu::workspaceProjectOpen));
        this->addAndMakeVisible(this->openProjectLabel.get());
        this->openProjectLabel->setJustificationType(Justification::centredLeft);
        this->openProjectLabel->setInterceptsMouseClicks(false, false);
        this->openProjectLabel->setFont(Globals::UI::Fonts::M);

        this->clickHandler = make<OverlayButton>();
        this->addAndMakeVisible(this->clickHandler.get());
        this->clickHandler->onClick = []() {
            App::Workspace().importProject("*.helio;*.mid;*.midi");
        };

        this->setSize(256, 32);
    }

    void resized() override
    {
        constexpr auto iconSize = 20;
        this->openProjectIcon->setBounds(8, (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
        constexpr auto labelMargin = iconSize + 12;
        this->openProjectLabel->setBounds(labelMargin, 0, this->getWidth() - labelMargin, this->getHeight());
        this->clickHandler->setBounds(this->getLocalBounds());
    }

private:

    UniquePointer<IconComponent> openProjectIcon;
    UniquePointer<Label> openProjectLabel;
    UniquePointer<OverlayButton> clickHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenProjectButton)
};
