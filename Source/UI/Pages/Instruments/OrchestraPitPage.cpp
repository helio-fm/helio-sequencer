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
#include "OrchestraPitPage.h"

#include "PluginScanner.h"
#include "OrchestraPitNode.h"
#include "MainLayout.h"
#include "ProgressTooltip.h"
#include "DocumentHelpers.h"
#include "Workspace.h"
#include "ComponentIDs.h"

OrchestraPitPage::OrchestraPitPage(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot) :
    pluginScanner(pluginScanner),
    instrumentsRoot(instrumentsRoot)
{
    this->setComponentID(ComponentIDs::orchestraPit);

    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->skew = make<SeparatorVerticalSkew>();
    this->addAndMakeVisible(this->skew.get());

    this->backgroundA = make<PanelBackgroundA>();
    this->addAndMakeVisible(this->backgroundA.get());

    this->backgroundB = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->backgroundB.get());

    this->pluginsList = make<AudioPluginsListComponent>(pluginScanner, instrumentsRoot);
    this->addAndMakeVisible(this->pluginsList.get());

    this->instrumentsList = make<InstrumentsListComponent>(pluginScanner, instrumentsRoot);
    this->addAndMakeVisible(this->instrumentsList.get());

    this->pluginScanner.addChangeListener(this);
    this->instrumentsRoot.addChangeListener(this);
}

OrchestraPitPage::~OrchestraPitPage()
{
    this->instrumentsRoot.removeChangeListener(this);
    this->pluginScanner.removeChangeListener(this);
}

void OrchestraPitPage::resized()
{
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto skewWidth = smallScreenMode ? 32 : 64;

    const auto rightSideWidth = int(this->getWidth() / 2.5f); // a bit smaller: a simple list of instruments
    const auto leftSideWidth = this->getWidth() - rightSideWidth - skewWidth;

    this->skew->setBounds(leftSideWidth, 0, skewWidth, this->getHeight());

    this->backgroundA->setBounds(0, 0, leftSideWidth, this->getHeight());
    this->backgroundB->setBounds(this->getWidth() - rightSideWidth, 0, rightSideWidth, this->getHeight());

    constexpr auto paddingH = 14;
    constexpr auto paddingV = 10;

    this->pluginsList->setBounds(paddingH, paddingV,
        leftSideWidth - paddingH - 2,
        this->getHeight() - paddingV * 2);

    this->instrumentsList->setBounds((this->getWidth() - rightSideWidth) + paddingH,
        paddingV,
        rightSideWidth - paddingH * 2,
        this->getHeight() - paddingV * 2);
}

void OrchestraPitPage::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::ScanAllPlugins)
    {
        App::showModalComponent(ProgressTooltip::cancellable([this]()
        {
            this->pluginScanner.cancelRunningScan();
        }));

        this->pluginScanner.runInitialScan();
    }

#if PLATFORM_DESKTOP

    else if (commandId == CommandIDs::ScanPluginsFolder)
    {
        this->scanFolderFileChooser = make<FileChooser>(TRANS(I18n::Dialog::scanFolderCaption),
            File::getCurrentWorkingDirectory(), ("*.*"), true);

        DocumentHelpers::showFileChooser(this->scanFolderFileChooser,
            Globals::UI::FileChooser::forDirectory,
            [this](URL &url)
        {
            if (!url.isLocalFile())
            {
                return;
            }

            App::showModalComponent(ProgressTooltip::cancellable([this]()
            {
                this->pluginScanner.cancelRunningScan();
            }));

            this->pluginScanner.scanFolderAndAddResults(url.getLocalFile());
        });
    }

#endif
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void OrchestraPitPage::changeListenerCallback(ChangeBroadcaster *source)
{
    this->pluginsList->showScanButtonIf(this->pluginScanner.getNumPlugins() == 0);
    this->pluginsList->updateListContent();
    this->instrumentsList->updateListContent();

    App::Layout().hideSelectionMenu();

    if (dynamic_cast<PluginScanner *>(source) &&
        !this->pluginScanner.isWorking())
    {
        // hides the spinner, if any
        App::dismissAllModalComponents();
    }
}

void OrchestraPitPage::onStageSelectionChanged()
{
    this->pluginsList->clearSelection();
}

void OrchestraPitPage::onPluginsSelectionChanged()
{
    this->instrumentsList->clearSelection();
}
