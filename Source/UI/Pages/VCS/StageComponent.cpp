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
#include "StageComponent.h"

#include "VersionControl.h"
#include "VersionControlEditor.h"
#include "ModalDialogConfirmation.h"
#include "ModalDialogInput.h"
#include "ProgressIndicator.h"
#include "MainLayout.h"
#include "VersionControlStageSelectionMenu.h"
#include "RevisionItemComponent.h"
#include "ComponentIDs.h"

StageComponent::StageComponent(VersionControl &versionControl) : vcs(versionControl)
{
    this->setPaintingIsUnclipped(true);
    this->setComponentID(ComponentIDs::versionControlStage);

    this->titleLabel = make<Label>(String(), TRANS(I18n::VCS::stageCaption));
    this->addAndMakeVisible(this->titleLabel.get());
    this->titleLabel->setJustificationType(Justification::centred);
    this->titleLabel->setFont(Globals::UI::Fonts::L);

    this->changesList = make<ListBox>(String(), this);
    this->addAndMakeVisible(this->changesList.get());
    this->changesList->setWantsKeyboardFocus(false);
    this->changesList->getViewport()->setScrollBarThickness(2);
    this->changesList->getViewport()->setScrollBarsShown(true, false);
    this->changesList->setMultipleSelectionEnabled(true);
    this->changesList->setRowHeight(48);

    this->separator = make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->separator.get());

    this->updateList();
}

StageComponent::~StageComponent() = default;

void StageComponent::resized()
{
    constexpr auto headerSize = 40;
    this->titleLabel->setBounds(0, 0, this->getWidth(), headerSize - 4);
    this->changesList->setBounds(this->getLocalBounds().withTrimmedTop(headerSize).reduced(2));
    this->separator->setBounds(0, headerSize, this->getWidth(), 2);
}

void StageComponent::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::VersionControlSelectAll:
        this->selectAll(sendNotification);
        break;
    case CommandIDs::VersionControlSelectNone:
        this->changesList->setSelectedRows({}, sendNotification);
        break;
    case CommandIDs::VersionControlToggleQuickStash:
        this->toggleQuickStash();
        break;
    case CommandIDs::VersionControlCommitSelected:
        this->commitSelected();
        break;
    case CommandIDs::VersionControlResetSelected:
        this->resetSelected();
        break;
    case CommandIDs::VersionControlCommitAll:
        this->selectAll(sendNotification);
        this->commitSelected();
        break;
    case CommandIDs::VersionControlResetAll:
        this->selectAll(sendNotification);
        this->resetSelected();
        break;
    default:
        break;
    }
}

void StageComponent::selectAll(NotificationType notificationType)
{
    auto selection = this->changesList->getSelectedRows();
    selection.addRange({ 0, this->getNumRows() });
    this->changesList->setSelectedRows(selection, notificationType);
}

void StageComponent::clearSelection()
{
    this->changesList->setSelectedRows({}, dontSendNotification);
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *StageComponent::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate)
{
    const ScopedReadLock lock(this->diffLock);

    // juce out-of-range fix
    const int numRecords = this->stageDeltas.size();
    const bool isLastRow = (rowNumber == (numRecords - 1));

    if (rowNumber >= numRecords) { return existingComponentToUpdate; }

    if (auto *revRecord = this->stageDeltas[rowNumber].get())
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (auto *row = dynamic_cast<RevisionItemComponent *>(existingComponentToUpdate))
            {
                row->updateItemInfo(revRecord, rowNumber, isLastRow, true);
                return existingComponentToUpdate;
            }
        }
        else
        {
            auto *row = new RevisionItemComponent(*this->changesList);
            row->updateItemInfo(revRecord, rowNumber, isLastRow, true);
            return row;
        }
    }

    return nullptr;
}

void StageComponent::selectedRowsChanged(int lastRowSelected)
{
    if (this->changesList->getNumSelectedRows() > 0)
    {
        App::Layout().showSelectionMenu(this);
    }
    else
    {
        App::Layout().hideSelectionMenu();
    }

    if (auto *parent = dynamic_cast<VersionControlEditor *>(this->getParentComponent()))
    {
        parent->onStageSelectionChanged();
    }
}

int StageComponent::getNumRows()
{
    const ScopedReadLock lock(this->diffLock);
    const int numProps = this->stageDeltas.size();
    return numProps;
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void StageComponent::updateList()
{
    const ScopedWriteLock lock(this->diffLock);
    this->stageDeltas.clearQuick();
    this->stageDeltas.addArray(this->vcs.getHead().getDiff()->getItems());
    this->changesList->deselectAllRows();
    this->changesList->updateContent();
    this->repaint();
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool StageComponent::hasMenu() const noexcept { return true; }
bool StageComponent::canBeSelectedAsMenuItem() const { return false; }

UniquePointer<Component> StageComponent::createMenu()
{
    return make<VersionControlStageSelectionMenu>();
}

Image StageComponent::getIcon() const
{
    return Icons::findByName(Icons::selection, Globals::UI::headlineIconSize);
}

String StageComponent::getName() const
{
    return TRANS(I18n::Menu::Selection::vcsStage);
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

void StageComponent::toggleQuickStash()
{
    // 4 use cases:
    // 1 - has no quick stash, and has changes - clearly can toggle off, display toggle on button
    // 2 - has quick stash, and no changes - clearly can toggle on, display toggle off button
    // 3 - has no quick stash, and has no changes - don't display toggle button
    // 4 - has quick stash, but also has changes - ? display toggle off button, show alert on press ?

    const bool case1 = !this->vcs.hasQuickStash() && this->vcs.getHead().diffHasChanges();
    const bool case2 = this->vcs.hasQuickStash() && !this->vcs.getHead().diffHasChanges();
    //const bool case3 = !this->vcs.hasQuickStash() && !this->vcs.getHead().hasAnythingOnTheStage();
    const bool case4 = this->vcs.hasQuickStash() && this->vcs.getHead().diffHasChanges();

    if (case1)
    {
        this->vcs.quickStashAll();
    }
    else if (case2)
    {
        this->vcs.restoreQuickStash();
        //this->vcs.getHead().rebuildDiffSynchronously();
    }
    else if (case4)
    {
        App::Layout().showTooltip(TRANS(I18n::VCS::warningCannotRevert),
            MainLayout::TooltipIcon::Failure);
    }
}

void StageComponent::commitSelected()
{
    if (this->changesList->getSelectedRows().isEmpty())
    {
        App::Layout().showTooltip(TRANS(I18n::VCS::warningCannotCommit));
        return;
    }

    auto dialog = ModalDialogInput::Presets::commit(this->lastCommitMessage);

    dialog->onOk = [this](const String &input)
    {
        this->lastCommitMessage = {};
        this->vcs.commit(this->changesList->getSelectedRows(), input);
    };

    dialog->onCancel = [this](const String &input)
    {
        this->lastCommitMessage = input;
    };

    App::showModalComponent(move(dialog));
}

void StageComponent::resetSelected()
{
    if (this->changesList->getSelectedRows().isEmpty())
    {
        App::Layout().showTooltip(TRANS(I18n::VCS::warningCannotReset));
        return;
    }

    auto dialog = ModalDialogConfirmation::Presets::resetChanges();

    dialog->onOk = [this]()
    {
        this->vcs.resetChanges(this->changesList->getSelectedRows());
    };

    App::showModalComponent(move(dialog));
}
