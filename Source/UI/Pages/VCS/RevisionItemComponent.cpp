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

class RevisionItemHighlighter final : public Component
{
public:

    RevisionItemHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::VersionControl::highlight));
        g.fillRoundedRectangle (5.0f, 2.0f, float(this->getWidth() - 10), float(this->getHeight() - 8), 2.f);
    }
};

class RevisionItemSelectionComponent final : public Component
{
public:

    RevisionItemSelectionComponent()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const float height = float(this->getHeight());
        const auto imgOk(Icons::findByName(Icons::apply, int(height * 0.7f)));

        const auto colour1(Colours::black.withAlpha(0.07f));
        const auto colour2(Colours::black.withAlpha(0.1f));
        g.setGradientFill(ColourGradient(colour1, 0.f, 0.f, colour2, 0.f, float(this->getHeight()), false));
        g.fillRoundedRectangle(5.0f, 2.0f, float(this->getWidth() - 10), float(this->getHeight() - 8), 2.f);

        g.setOpacity(0.35f);
        const int rightTextBorder = this->getWidth() - this->getHeight() / 2 - 5;
        Icons::drawImageRetinaAware(imgOk, g, rightTextBorder, this->getHeight() / 2 - 2);
    }
};

RevisionItemComponent::RevisionItemComponent(ListBox &parentListBox) :
    DraggingListBoxComponent(parentListBox.getViewport()),
    list(parentListBox)
{
    this->itemLabel= make<Label>();
    this->addAndMakeVisible(this->itemLabel.get());
    this->itemLabel->setFont(Globals::UI::Fonts::M);
    this->itemLabel->setJustificationType(Justification::centredLeft);
    this->itemLabel->setInterceptsMouseClicks(false, false);

    this->deltasLabel= make<Label>();
    this->addAndMakeVisible(this->deltasLabel.get());
    this->deltasLabel->setFont(Globals::UI::Fonts::S);
    this->deltasLabel->setJustificationType(Justification::topLeft);
    this->deltasLabel->setInterceptsMouseClicks(false, false);
    this->deltasLabel->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.75f));

    this->separator= make<SeparatorHorizontalFading>();
    this->addAndMakeVisible(this->separator.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->selectionComponent = make<RevisionItemSelectionComponent>();
    this->addChildComponent(this->selectionComponent.get());

    this->setSize(500, 70);
}

RevisionItemComponent::~RevisionItemComponent() = default;

void RevisionItemComponent::resized()
{
    this->itemLabel->setBounds(5, 3, this->getWidth() - 10, 24);
    this->deltasLabel->setBounds(5, 24, this->getWidth() - 90, 38);
    this->separator->setBounds(10, this->getHeight() - 3, this->getWidth() - 20, 2);

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

    this->separator->setVisible(! isLastRow);

    const auto itemType = this->revisionItem->getType();
    const auto itemTypeStr = this->revisionItem->getTypeAsString();
    const auto itemDescription = TRANS(this->revisionItem->getVCSName());

    String itemDeltas;
    bool needsComma = false;
    for (int i = 0; i < this->revisionItem->getNumDeltas(); ++i)
    {
        const VCS::Delta *delta = this->revisionItem->getDelta(i);
        const String &description = delta->getHumanReadableText();

        if (description.isNotEmpty())
        {
            itemDeltas += needsComma ? ", " : "";
            itemDeltas = itemDeltas + description;
            needsComma = true;
        }
    }

    DBG(itemDeltas);

    if (itemType == VCS::RevisionItem::Type::Added ||
        itemType == VCS::RevisionItem::Type::Removed)
    {
        this->itemLabel->setText(itemTypeStr + " " + itemDescription, dontSendNotification);
    }
    else
    {
        this->itemLabel->setText(itemDescription, dontSendNotification);

    }

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
