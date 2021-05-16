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

#if !NO_NETWORK

#include "UpdatesInfoComponent.h"
#include "Config.h"
#include "MenuPanel.h"
#include "AppInfoDto.h"
#include "PanelBackgroundA.h"
#include "SerializationKeys.h"

UpdatesInfoComponent::UpdatesInfoComponent()
{
    this->updatesCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->updatesCombo.get());

    this->label = make<Label>(String(), TRANS(I18n::Common::updateProceed));
    this->addAndMakeVisible(this->label.get());
    this->label->setJustificationType(Justification::centredBottom);
    this->label->setFont(Globals::UI::Fonts::S);

    // here we assume that backend response will only contain
    // the updates info for the current platform, so that
    // we don't need to filter them,

    // but
    // the returned latest version can still be the same
    // as current one, so filtering them anyway:

    AppInfoDto appInfo;
    App::Config().load(&appInfo, Serialization::Config::lastUpdatesInfo);

    bool hasNewerStable = false;
    for (const auto &v : appInfo.getVersions())
    {
        if (v.isLaterThanCurrentVersion())
        {
            this->versions.add(v);
            hasNewerStable = hasNewerStable || !v.isDevelopmentBuild();
        }
    }

    if (hasNewerStable)
    {
        //this->label->setText("v" + App::getAppReadableVersion(), dontSendNotification);
        this->label->setAlpha(0.5f);

        MenuPanel::Menu menu;
        for (int i = 0; i < this->versions.size(); ++i)
        {
            const auto &version = this->versions.getUnchecked(i);
            menu.add(MenuItem::item(Icons::helio, CommandIDs::SelectVersion + i,
                version.getHumanReadableDescription()));
        }

        this->updatesCombo->initWith(this->label.get(), menu, new PanelBackgroundA());
    }
    else
    {
        this->label->setVisible(false);
    }

    this->setSize(256, 128);
}

UpdatesInfoComponent::~UpdatesInfoComponent() = default;

void UpdatesInfoComponent::resized()
{
    this->updatesCombo->setBounds(this->getLocalBounds());
    this->label->setBounds((this->getWidth() / 2) - ((this->getWidth() - 56) / 2), 2, this->getWidth() - 56, 24);
}

void UpdatesInfoComponent::handleCommandMessage(int commandId)
{
    const int idx = commandId - CommandIDs::SelectVersion;
    if (idx >= 0 && idx < this->versions.size())
    {
        // what TODO here instead of just opening a browser?
        const auto version = this->versions.getReference(idx);
        jassert(version.getLink().isNotEmpty());
        URL(version.getLink()).launchInDefaultBrowser();
    }
}

#endif
