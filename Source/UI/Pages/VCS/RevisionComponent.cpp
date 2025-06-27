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
#include "RevisionComponent.h"
#include "VersionControl.h"
#include "RevisionTreeComponent.h"
#include "HeadlineContextMenuController.h"

RevisionComponent::RevisionComponent(VersionControl &owner,
    const VCS::Revision::Ptr revision, bool isHead) :
    vcs(owner),
    revision(revision),
    isHeadRevision(isHead)
{
    this->revisionDescription= make<Label>();
    this->addAndMakeVisible(this->revisionDescription.get());
    this->revisionDescription->setFont(Globals::UI::Fonts::M);
    this->revisionDescription->setJustificationType(Justification::centred);

    this->revisionDate= make<Label>();
    this->addAndMakeVisible(this->revisionDate.get());
    this->revisionDate->setFont(Globals::UI::Fonts::XS);
    this->revisionDate->setJustificationType(Justification::centred);
    this->revisionDate->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.5f));

    this->line2= make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->line2.get());

    this->line3= make<SeparatorHorizontalFading>();
    this->addAndMakeVisible(this->line3.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    const auto &message = this->revision->getMessage();
    const auto timestamp = this->revision->getTimeStamp();

    this->revisionDescription->setText(message, dontSendNotification);
    this->revisionDate->setText(App::getHumanReadableDate(Time(timestamp)), dontSendNotification);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->revisionDate->setInterceptsMouseClicks(false, false);
    this->revisionDescription->setInterceptsMouseClicks(false, false);

    this->setSize(175, 40);
}

RevisionComponent::~RevisionComponent() = default;

void RevisionComponent::paint(Graphics &g)
{
    g.setColour(this->outlineColour);

    if (this->isHeadRevision)
    {
        g.drawRoundedRectangle(0.f, 0.f,
            float(this->getWidth()), float(this->getHeight()), 9.f, 1.f);
    }

    if (this->isSelected)
    {
        g.fillRoundedRectangle(1.f, 1.f,
            float(this->getWidth() - 2), float(this->getHeight() - 2), 7.f);
    }
}

void RevisionComponent::resized()
{
    this->revisionDescription->setBounds(4, 2, this->getWidth() - 8, 20);
    this->revisionDate->setBounds(16, 20, this->getWidth() - 32, 18);
    this->line2->setBounds(8, 0, this->getWidth() - 16, 2);
    this->line3->setBounds(8, this->getHeight() - 2, this->getWidth() - 16, 2);
}

void RevisionComponent::mouseMove(const MouseEvent &e)
{
    this->setMouseCursor(MouseCursor::NormalCursor);
}

void RevisionComponent::mouseDown(const MouseEvent &e)
{
    if (e.mods.isRightButtonDown() && this->isSelected)
    {
        return;
    }

    if (auto *revTree = dynamic_cast<RevisionTreeComponent *>(this->getParentComponent()))
    {
        revTree->selectComponent(this, true);
    }
}

void RevisionComponent::mouseUp(const MouseEvent &e)
{
    if (e.mods.isRightButtonDown() &&
        e.getOffsetFromDragStart().isOrigin() &&
        this->isSelected)
    {
        this->contextMenuController->showMenu(e);
    }
}

void RevisionComponent::setSelected(bool selected)
{
    this->isSelected = selected;
    this->repaint();
}

RevisionComponent *RevisionComponent::getLeftmostSibling() const
{
    if (!this->leftmostSibling && this->parent)
    {
        if (this != this->parent->children.getFirst())
        {
            this->leftmostSibling = this->parent->children.getFirst();
        }
    }

    return this->leftmostSibling;
}

RevisionComponent *RevisionComponent::getLeftBrother() const
{
    RevisionComponent *n = nullptr;

    if (this->parent)
    {
        for (auto i : this->parent->children)
        {
            if (i == this)
            {
                return n;
            }

            n = i;
        }
    }

    return n;
}

RevisionComponent *RevisionComponent::right() const
{
    if (this->children.size() > 0)
    {
        return this->children.getLast();
    }

    return this->wired;
}

RevisionComponent *RevisionComponent::left() const
{
    if (this->children.size() > 0)
    {
        return this->children.getFirst();
    }

    return this->wired;
}
