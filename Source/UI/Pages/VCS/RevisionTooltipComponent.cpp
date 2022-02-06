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
#include "RevisionTooltipComponent.h"
#include "RevisionItemComponent.h"

RevisionTooltipComponent::RevisionTooltipComponent(const VCS::Revision::Ptr revision) :
    revision(revision),
    revisionItemsOnly(revision->getItems())
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);

    this->separator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->separator.get());

    this->changesList = make<ListBox>(String(), this);
    this->addAndMakeVisible(this->changesList.get());

    this->changesList->setRowSelectedOnMouseDown(false);
    this->changesList->setMultipleSelectionEnabled(false);
    this->changesList->setClickingTogglesRowSelection(false);

    this->changesList->setRowHeight(RevisionTooltipComponent::rowHeight);
    this->changesList->getViewport()->setScrollBarsShown(true, false);

    const int maxHeight = int(RevisionTooltipComponent::rowHeight * RevisionTooltipComponent::numRowsOnScreen);
    const int newHeight = jmin(maxHeight, this->getNumRows() * RevisionTooltipComponent::rowHeight);
    this->changesList->updateContent();

    this->setSize(this->getWidth(), newHeight);
}

void RevisionTooltipComponent::resized()
{
    this->changesList->setBounds(this->getLocalBounds());
    this->separator->setBounds(0, this->getHeight() - 1, this->getWidth(), 4);
}

void RevisionTooltipComponent::inputAttemptWhenModal()
{
    this->hide();
}

void RevisionTooltipComponent::hide()
{
    this->getParentComponent()->exitModalState(0);
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *RevisionTooltipComponent::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate)
{
    // juce out-of-range fix
    const int numProps = this->getNumRows();
    const bool isLastRow = (rowNumber == (numProps - 1));

    if (rowNumber >= numProps) { return existingComponentToUpdate; }

    const auto revRecord = this->revisionItemsOnly.getUnchecked(rowNumber);
    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<RevisionItemComponent *>(existingComponentToUpdate))
        {
            row->updateItemInfo(revRecord, rowNumber, isLastRow, false);
            return existingComponentToUpdate;
        }
    }
    else
    {
        auto *row = new RevisionItemComponent(*this->changesList);
        row->updateItemInfo(revRecord, rowNumber, isLastRow, false);
        return row;
    }

    return nullptr;
}

int RevisionTooltipComponent::getNumRows()
{
    return this->revisionItemsOnly.size();
}
