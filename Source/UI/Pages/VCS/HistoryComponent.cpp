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
#include "HistoryComponent.h"
#include "VersionControl.h"
#include "VersionControlEditor.h"
#include "RevisionTreeComponent.h"
#include "ViewportFitProxyComponent.h"
#include "CommandIDs.h"
#include "ComponentIDs.h"
#include "Icons.h"
#include "MainLayout.h"
#include "ModalDialogConfirmation.h"
#include "VersionControlHistorySelectionMenu.h"

HistoryComponent::HistoryComponent(VersionControl &owner) : vcs(owner)
{
    this->setPaintingIsUnclipped(true);
    this->setComponentID(ComponentIDs::versionControlHistory);

    this->revisionViewport = make<Viewport>();
    this->addAndMakeVisible(this->revisionViewport.get());

    this->revisionTreeLabel = make<Label>(String(), TRANS(I18n::VCS::historyCaption));
    this->addAndMakeVisible(this->revisionTreeLabel.get());
    this->revisionTreeLabel->setJustificationType(Justification::centred);
    this->revisionTreeLabel->setFont(Globals::UI::Fonts::L);

    this->separator = make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->separator.get());
}

HistoryComponent::~HistoryComponent() = default;

void HistoryComponent::resized()
{
    constexpr auto headerSize = 40;
    this->revisionTreeLabel->setBounds(0, 0, this->getWidth(), headerSize - 4);
    this->revisionViewport->setBounds(this->getLocalBounds().withTrimmedTop(headerSize).reduced(2));
    this->separator->setBounds(0, headerSize, this->getWidth() - 0, 2);
}

void HistoryComponent::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::VersionControlCheckout:
        if (this->vcs.getHead().diffHasChanges())
        {
            auto confirmationDialog = ModalDialogConfirmation::Presets::forceCheckout();
            confirmationDialog->onOk = [this]()
            {
                this->vcs.checkout(this->revisionTree->getSelectedRevision());
            };
            App::showModalComponent(move(confirmationDialog));
        }
        else
        {
            this->vcs.checkout(this->revisionTree->getSelectedRevision());
        }
        break;
    default:
        break;
    }
}

void HistoryComponent::clearSelection()
{
    if (this->revisionTree != nullptr)
    {
        this->revisionTree->deselectAll(false);
    }
}

void HistoryComponent::rebuildRevisionTree()
{
    this->revisionTree = new RevisionTreeComponent(this->vcs);
    auto *alignerProxy = new ViewportFitProxyComponent(*this->revisionViewport, this->revisionTree, true); // owns revisionTree
    this->revisionViewport->setViewedComponent(alignerProxy, true); // owns alignerProxy
    alignerProxy->centerTargetToViewport();
}

void HistoryComponent::onRevisionSelectionChanged()
{
    if (this->revisionTree != nullptr &&
        this->revisionTree->getSelectedRevision() != nullptr)
    {
        // Hide existing because selection caption will be always different:
        App::Layout().hideSelectionMenu();
        App::Layout().showSelectionMenu(this);
    }
    else
    {
        App::Layout().hideSelectionMenu();
    }

    if (auto *parent = dynamic_cast<VersionControlEditor *>(this->getParentComponent()))
    {
        parent->onHistorySelectionChanged();
    }
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool HistoryComponent::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> HistoryComponent::createMenu()
{
    if (this->revisionTree != nullptr)
    {
        return make<VersionControlHistorySelectionMenu>(
            this->revisionTree->getSelectedRevision(), this->vcs);
    }

    jassertfalse;
    return nullptr;
}

Image HistoryComponent::getIcon() const
{
    return Icons::findByName(Icons::revision, Globals::UI::headlineIconSize);
}

String HistoryComponent::getName() const
{
    if (this->revisionTree != nullptr &&
        this->revisionTree->getSelectedRevision() != nullptr)
    {
        return this->revisionTree->getSelectedRevision()->getMessage();
    }

    return TRANS(I18n::Menu::Selection::vcsHistory);
}

bool HistoryComponent::canBeSelectedAsMenuItem() const
{
    return false;
}
