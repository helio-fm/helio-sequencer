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

#include "IconButton.h"
#include "HeadlineItemArrow.h"
#include "Workspace.h"
#include "ColourIDs.h"

class HeadlineNavigationPanel final : public Component
{
public:

    HeadlineNavigationPanel()
    {
        this->setOpaque(false);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, true);

        this->navigatePrevious = make<IconButton>(Icons::findByName(Icons::back, 20), CommandIDs::ShowPreviousPage);
        this->addAndMakeVisible(this->navigatePrevious.get());

        this->navigateNext = make<IconButton>(Icons::findByName(Icons::forward, 20), CommandIDs::ShowNextPage);
        this->addAndMakeVisible(this->navigateNext.get());

        this->arrow = make<HeadlineItemArrow>();
        this->addAndMakeVisible(this->arrow.get());

        this->updateState(false, false);

        this->setSize(64, Globals::UI::headlineHeight);
    }

    void updateState(bool canGoPrevious, bool canGoNext)
    {
        this->navigatePrevious->setInterceptsMouseClicks(canGoPrevious, false);
        this->navigatePrevious->setIconAlphaMultiplier(canGoPrevious ? 0.45f : 0.2f);
        this->navigateNext->setInterceptsMouseClicks(canGoNext, false);
        this->navigateNext->setIconAlphaMultiplier(canGoNext ? 0.45f : 0.2f);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(0, 0, this->getWidth(), this->getHeight() - 2);
    }

    void resized() override
    {
        const auto buttonWidth = this->getWidth() / 2;
        this->navigatePrevious->setBounds(0, 0, buttonWidth, this->getHeight() - 1);
        this->navigateNext->setBounds((this->getWidth() - this->arrow->getWidth() - 2) / 2, 0, buttonWidth, this->getHeight() - 1);
        this->arrow->setBounds(this->getWidth() - this->arrow->getWidth() - 1,
            0, this->arrow->getWidth(), this->getHeight() - 1);
    }

    void handleCommandMessage(int commandId) override
    {
        switch (commandId)
        {
        case CommandIDs::ShowPreviousPage:
            App::Workspace().navigateBackwardIfPossible();
            break;
        case CommandIDs::ShowNextPage:
            App::Workspace().navigateForwardIfPossible();
            break;
        default:
            break;
        }
    }

private:

    UniquePointer<IconButton> navigatePrevious;
    UniquePointer<IconButton> navigateNext;
    UniquePointer<HeadlineItemArrow> arrow;

    const Colour fillColour = findDefaultColour(ColourIDs::Backgrounds::headlineFill);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineNavigationPanel)
};
