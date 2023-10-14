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

#include "OverlayButton.h"
#include "SeparatorVertical.h"
#include "IconComponent.h"
#include "Workspace.h"

class CreateProjectButton final : public Component
{
public:

    CreateProjectButton()
    {
        this->newProjectIcon = make<IconComponent>(Icons::create);
        this->addAndMakeVisible(this->newProjectIcon.get());

        this->newProjectLabel = make<Label>(String(), TRANS(I18n::Menu::workspaceProjectCreate));
        this->addAndMakeVisible(this->newProjectLabel.get());
        this->newProjectLabel->setJustificationType(Justification::centredLeft);
        this->newProjectLabel->setInterceptsMouseClicks(false, false);
        this->newProjectLabel->setFont(Globals::UI::Fonts::M);

        this->clickHandler = make<OverlayButton>();
        this->addAndMakeVisible(this->clickHandler.get());
        this->clickHandler->onClick = []() {
            App::Workspace().createEmptyProject();
        };

        this->setSize(256, 32);
    }

    void resized() override
    {
        constexpr auto iconSize = 20;
        this->newProjectIcon->setBounds(8, (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
        constexpr auto labelMargin = iconSize + 12;
        this->newProjectLabel->setBounds(labelMargin, 0, this->getWidth() - labelMargin, this->getHeight());
        this->clickHandler->setBounds(this->getLocalBounds());
    }

private:

    UniquePointer<IconComponent> newProjectIcon;
    UniquePointer<Label> newProjectLabel;
    UniquePointer<OverlayButton> clickHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CreateProjectButton)
};
