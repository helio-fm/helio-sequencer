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
#include "RevisionItemComponent.h"

#include "Icons.h"
#include "ColourIDs.h"
#include "HeadlineContextMenuController.h"

class RevisionItemSelectionComponent final : public Component
{
public:

    RevisionItemSelectionComponent()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const float height = float(this->getHeight());
        const auto imgOk = Icons::findByName(Icons::apply, int(height * 0.5f));

        g.setColour(Colours::black.withAlpha(0.15f));
        g.fillRoundedRectangle(4.0f, 3.0f,
            float(this->getWidth() - 10), float(this->getHeight() - 10), 2.f);

        if (this->getWidth() > 100)
        {
            g.setOpacity(0.45f);
            const int rightTextBorder = this->getWidth() - this->getHeight() / 2 - 5;
            Icons::drawImageRetinaAware(imgOk, g, rightTextBorder, this->getHeight() / 2 - 2);
        }
    }
};

RevisionItemComponent::RevisionItemComponent(ListBox &parentListBox) :
    DraggingListBoxComponent(parentListBox.getViewport()),
    list(parentListBox)
{
    this->setPaintingIsUnclipped(true);

    this->itemLabel = make<Label>();
    this->addAndMakeVisible(this->itemLabel.get());
    this->itemLabel->setFont(Globals::UI::Fonts::M);
    this->itemLabel->setJustificationType(Justification::centredLeft);
    this->itemLabel->setInterceptsMouseClicks(false, false);

    this->deltasLabel = make<Label>();
    this->addAndMakeVisible(this->deltasLabel.get());
    this->deltasLabel->setFont(Globals::UI::Fonts::S);
    this->deltasLabel->setJustificationType(Justification::topLeft);
    this->deltasLabel->setInterceptsMouseClicks(false, false);

    this->separator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->separator.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);
    this->contextMenuController->onWillShowMenu = [this]()
    {
        if (!this->list.isRowSelected(this->row))
        {
            this->list.selectRow(this->row, true, true);
        }
    };

    this->selectionComponent = make<RevisionItemSelectionComponent>();
    this->addChildComponent(this->selectionComponent.get());
}

RevisionItemComponent::~RevisionItemComponent() = default;

void RevisionItemComponent::resized()
{
    this->itemLabel->setBounds(8, 3, this->getWidth() - 16, 24);
    this->deltasLabel->setBounds(8, 24, this->getWidth() - 24, 24);
    this->separator->setBounds(10, this->getHeight() - 3, this->getWidth() - 24, 2);

    if (this->isEnabled())
    {
        this->selectionComponent->setBounds(this->getLocalBounds());
    }
    else
    {
        this->deltasLabel->setBounds(5, 25, this->getWidth() - 30, this->getHeight() - 32);
    }
}

void RevisionItemComponent::updateItemInfo(VCS::RevisionItem::Ptr revisionItemInfo,
    int rowNumber, bool isLastRow, bool isSelectable)
{
    this->row = rowNumber;
    this->revisionItem = revisionItemInfo;
    this->setEnabled(isSelectable);

    this->separator->setVisible(!isLastRow);

    const auto itemType = this->revisionItem->getType();
    const auto itemTypeStr = this->revisionItem->getTypeAsString();
    const auto itemDescription = TRANS(this->revisionItem->getVCSName());

    String itemDeltas;
    bool needsComma = false;
    for (int i = 0; i < this->revisionItem->getNumDeltas(); ++i)
    {
        const auto *delta = this->revisionItem->getDelta(i);
        const String &description = delta->getHumanReadableText();

        if (description.isNotEmpty())
        {
            itemDeltas += needsComma ? ", " : "";
            itemDeltas = itemDeltas + description;
            needsComma = true;
        }
    }

    if (itemType == VCS::RevisionItem::Type::Added ||
        itemType == VCS::RevisionItem::Type::Removed)
    {
        this->itemLabel->setText(itemTypeStr + " " + itemDescription, dontSendNotification);
    }
    else
    {
        this->itemLabel->setText(itemDescription, dontSendNotification);
    }

    const auto textColor = findDefaultColour(Label::textColourId);
    const auto revisionColour = this->revisionItem->getDisplayColour();
    this->itemLabel->setColour(Label::textColourId, revisionColour.interpolatedWith(textColor, 0.5f));
    this->deltasLabel->setColour(Label::textColourId, revisionColour.
        interpolatedWith(textColor, 0.5f).withMultipliedAlpha(0.75f));

    this->deltasLabel->setText(itemDeltas, dontSendNotification);

    if (!this->selectionAnimator.isAnimating())
    {
        this->selectionComponent->setVisible(this->isSelected());
        this->selectionComponent->setAlpha(this->isSelected() ? 1.f : 0.f);
    }

    this->resized();
}

void RevisionItemComponent::select() const
{
    if (!this->isSelected()) { this->invertSelection(); }
}

void RevisionItemComponent::deselect() const
{
    if (this->isSelected()) { this->invertSelection(); }
}

VCS::RevisionItem::Ptr RevisionItemComponent::getRevisionItem() const noexcept
{
    return this->revisionItem;
}

void RevisionItemComponent::setSelected(bool shouldBeSelected)
{
    this->invertSelection();
}

void RevisionItemComponent::invertSelection() const
{
    const bool rowWillBeSelected = !this->list.isRowSelected(this->row);

    if (this->selectionComponent->isVisible() && !rowWillBeSelected)
    {
        this->selectionAnimator.fadeOut(this->selectionComponent.get(), Globals::UI::fadeOutShort);
    }
    else if (!this->selectionComponent->isVisible() && rowWillBeSelected)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent.get(), Globals::UI::fadeInShort);
    }

    this->list.flipRowSelection(this->row);
}

bool RevisionItemComponent::isSelected() const
{
    return this->list.isRowSelected(this->row);
}

void RevisionItemComponent::mouseUp(const MouseEvent &event)
{
    DraggingListBoxComponent::mouseUp(event);

    if (event.mods.isRightButtonDown() &&
        event.getOffsetFromDragStart().isOrigin())
    {
        this->contextMenuController->showMenu(event);
    }
}
