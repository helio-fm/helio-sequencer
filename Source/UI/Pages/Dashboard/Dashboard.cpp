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

#include "Common.h"
#include "Dashboard.h"
#include "SpectralLogo.h"
#include "OverlayButton.h"
#include "DashboardMenu.h"
#include "PageBackgroundA.h"
#include "PageBackgroundB.h"
#include "OpenProjectButton.h"
#include "CreateProjectButton.h"
#include "SeparatorVerticalSkew.h"
#include "SeparatorHorizontalFadingReversed.h"

Dashboard::Dashboard(Workspace &workspace) : workspace(workspace)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->backgroundA = make<PageBackgroundA>();
    this->addAndMakeVisible(this->backgroundA.get());

    this->backgroundB = make<PageBackgroundB>();
    this->addAndMakeVisible(this->backgroundB.get());

    this->openProjectButton = make<OpenProjectButton>();
    this->addAndMakeVisible(this->openProjectButton.get());

    this->createProjectButton = make<CreateProjectButton>();
    this->addAndMakeVisible(this->createProjectButton.get());

    this->createProjectCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->createProjectCombo.get());

    this->skew = make<SeparatorVerticalSkew>();
    this->addAndMakeVisible(this->skew.get());
    this->logo = make<SpectralLogo>();
    this->addAndMakeVisible(this->logo.get());

    this->projectsList = make<DashboardMenu>(this->workspace);
    if (!App::isRunningOnPhone()) // no screen space for that on the phones
    {
        this->addAndMakeVisible(this->projectsList.get());
    }

    this->projectsList->updateListContent();

    // to sync projects when logged in:
    this->workspace.getUserProfile().addChangeListener(this);
}

Dashboard::~Dashboard()
{
    this->workspace.getUserProfile().removeChangeListener(this);
}

void Dashboard::resized()
{
    // background stuff:
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto skewWidth = smallScreenMode ? 32 : 48;
    constexpr auto leftSectionWidth = 220;
    const auto rightSectionX = leftSectionWidth + skewWidth;
    const auto rightSectionWidth = this->getWidth() - rightSectionX;

    this->backgroundA->setBounds(0, 0, leftSectionWidth, this->getHeight());
    this->skew->setBounds(leftSectionWidth, 0, skewWidth, this->getHeight());
    this->backgroundB->setBounds(rightSectionX, 0, rightSectionWidth, this->getHeight());

    // left section content:
    constexpr auto logoSize = 200;
    const auto logoX = leftSectionWidth / 2 - logoSize / 2 + int(skewWidth / 2.5);
    this->logo->setBounds(logoX, 24, logoSize, logoSize);

    constexpr auto buttonHeight = 32;
    constexpr auto buttonWidth = 270;

    // right section content:
    constexpr auto projectsListWidth = 350;
    const auto buttonsX = rightSectionX + 16;
    this->openProjectButton->setBounds(buttonsX, 16, buttonWidth, buttonHeight);
    this->createProjectButton->setBounds(buttonsX, 52, buttonWidth, buttonHeight);
    this->createProjectCombo->setBounds(buttonsX, 52, buttonWidth, 140);

    this->projectsList->setBounds(this->getWidth() - projectsListWidth,
        10, projectsListWidth, this->getHeight() - 20);
}

void Dashboard::changeListenerCallback(ChangeBroadcaster *source)
{
    this->projectsList->updateListContent();
}
