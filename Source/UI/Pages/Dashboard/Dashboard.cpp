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

#include "Common.h"
#include "Dashboard.h"
#include "SpectralLogo.h"
#include "OverlayButton.h"
#include "DashboardMenu.h"
#include "PanelBackgroundA.h"
#include "PanelBackgroundB.h"
#include "OpenProjectButton.h"
#include "CreateProjectButton.h"
#include "UpdatesInfoComponent.h"
#include "SeparatorVerticalSkew.h"
#include "SeparatorHorizontalFadingReversed.h"

Dashboard::Dashboard(Workspace &workspace) : workspace(workspace)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->backgroundA = make<PanelBackgroundA>();
    this->addAndMakeVisible(this->backgroundA.get());

    this->patreonLabel = make<Label>();
    this->addAndMakeVisible(this->patreonLabel.get());

    this->patreonLabel->setFont(Globals::UI::Fonts::S);
    this->patreonLabel->setJustificationType(Justification::centred);

    this->backgroundB = make<PanelBackgroundB>();
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

    this->updatesInfo = make<UpdatesInfoComponent>();
    this->addAndMakeVisible(this->updatesInfo.get());

    this->patreonButton = make<OverlayButton>();
    this->addAndMakeVisible(this->patreonButton.get());

    this->patreonLabel->setText(TRANS(I18n::Common::supportProject), dontSendNotification);
    this->patreonLabel->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.25f));
    this->patreonButton->onClick = []()
    {
        URL url("https://www.patreon.com/peterrudenko");
        url.launchInDefaultBrowser();
    };

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
    const auto skewWidth = smallScreenMode ? 32 : 64;
    const auto leftSectionWidth = smallScreenMode ? 220 : 320;
    const auto rightSectionX = leftSectionWidth + skewWidth;
    const auto rightSectionWidth = this->getWidth() - rightSectionX;

    this->backgroundA->setBounds(0, 0, leftSectionWidth, this->getHeight());
    this->skew->setBounds(leftSectionWidth, 0, skewWidth, this->getHeight());
    this->backgroundB->setBounds(rightSectionX, 0, rightSectionWidth, this->getHeight());

    // left section content:
    const auto logoSize = leftSectionWidth - 40;
    const auto logoX = leftSectionWidth / 2 - logoSize / 2;
    this->logo->setBounds(logoX, 32, logoSize, logoSize);

    constexpr auto buttonHeight = 32;
    const auto buttonWidth = leftSectionWidth - 50;

    const auto helperX = leftSectionWidth / 2 - buttonWidth / 2;
    this->updatesInfo->setBounds(helperX, 352, buttonWidth, 256);

    this->patreonLabel->setBounds(helperX, getHeight() - 44, buttonWidth, buttonHeight);
    this->patreonButton->setBounds(helperX, getHeight() - 44, buttonWidth, buttonHeight);

    // right section content:
    constexpr auto projectsListWidth = 400;
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
