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
#include "VersionControlEditor.h"

#include "VersionControl.h"
#include "MainLayout.h"

VersionControlEditor::VersionControlEditor(VersionControl &versionControl) : vcs(versionControl)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->backgroundA = make<PanelBackgroundA>();
    this->addAndMakeVisible(this->backgroundA.get());

    this->skew = make<SeparatorVerticalSkew>();
    this->addAndMakeVisible(this->skew.get());

    this->backgroundB = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->backgroundB.get());

    this->stageComponent = make<StageComponent>(versionControl);
    this->addAndMakeVisible(this->stageComponent.get());

    this->historyComponent = make<HistoryComponent>(versionControl);
    this->addAndMakeVisible(this->historyComponent.get());
}

VersionControlEditor::~VersionControlEditor() = default;

void VersionControlEditor::resized()
{
    constexpr auto skewWidth = 64;
    const auto halfWidth = this->proportionOfWidth(0.5f);

    this->backgroundA->setBounds(0, 0, halfWidth - skewWidth / 2, this->getHeight());
    this->skew->setBounds(halfWidth - skewWidth / 2, 0, skewWidth, this->getHeight());
    this->backgroundB->setBounds(halfWidth + skewWidth / 2, 0, halfWidth - skewWidth / 2, this->getHeight());

    constexpr auto contentMargin = 10;
    this->stageComponent->setBounds(15, contentMargin,
        halfWidth - skewWidth / 2 - 15,
        this->getHeight() - contentMargin * 2);
    this->historyComponent->setBounds(halfWidth + 32, contentMargin,
        halfWidth - skewWidth / 2 - 15,
        this->getHeight() - contentMargin * 2);
}

void VersionControlEditor::broughtToFront()
{
    this->updateState();
}

void VersionControlEditor::updateState()
{
    // VCS or project has changed
    App::Layout().hideSelectionMenu();
    this->stageComponent->clearSelection();
    this->historyComponent->clearSelection();
    this->vcs.getHead().rebuildDiffNow();
    this->historyComponent->rebuildRevisionTree();
}

void VersionControlEditor::onStageSelectionChanged()
{
    this->historyComponent->clearSelection();
}

void VersionControlEditor::onHistorySelectionChanged()
{
    this->stageComponent->clearSelection();
}

void VersionControlEditor::changeListenerCallback(ChangeBroadcaster *source)
{
    // VCS or project has changed
    if (this->isShowing())
    {
        this->updateState();
    }
}
