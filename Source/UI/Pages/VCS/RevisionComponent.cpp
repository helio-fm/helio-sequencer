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
#include "RevisionComponent.h"

#include "VersionControl.h"
#include "RevisionTreeComponent.h"
#include "HeadlineContextMenuController.h"
#include "ColourIDs.h"

RevisionComponent::RevisionComponent(VersionControl &owner,
    const VCS::Revision::Ptr revision, VCS::Revision::SyncState viewState, bool isHead) :
    vcs(owner),
    revision(revision),
    isHeadRevision(isHead),
    viewState(viewState)
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

    this->remoteIndicatorImage= make<IconComponent>(Icons::remote);
    this->addAndMakeVisible(this->remoteIndicatorImage.get());

    this->localIndicatorImage= make<IconComponent>(Icons::local);
    this->addAndMakeVisible(this->localIndicatorImage.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    const auto &message = this->revision->getMessage();
    const auto timestamp = this->revision->getTimeStamp();

    this->revisionDescription->setText(message, dontSendNotification);
    this->revisionDate->setText(App::getHumanReadableDate(Time(timestamp)), dontSendNotification);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->revisionDate->setInterceptsMouseClicks(false, false);
    this->revisionDescription->setInterceptsMouseClicks(false, false);

#if NO_NETWORK

    this->localIndicatorImage->setVisible(false);
    this->remoteIndicatorImage->setVisible(false);

#else

    switch (this->viewState)
    {
    case VCS::Revision::NoSync:
        this->remoteIndicatorImage->setIconAlphaMultiplier(0.15f);
        break;
    case VCS::Revision::ShallowCopy:
        this->localIndicatorImage->setIconAlphaMultiplier(0.15f);
        break;
    case VCS::Revision::FullSync:
    default:
        break;
    }

#endif

    this->setSize(150, 50);
}

RevisionComponent::~RevisionComponent() = default;

void RevisionComponent::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::VersionControl::outline));

    if (this->isHeadRevision)
    {
        g.drawRoundedRectangle(0.0f, 0.0f, float(this->getWidth()), float(this->getHeight()), 9.000f, 1.0f);
    }

    if (this->isSelected)
    {
        g.fillRoundedRectangle(1.0f, 1.0f, float(this->getWidth() - 2), float(this->getHeight() - 2), 7.000f);
    }
}

void RevisionComponent::resized()
{
    this->revisionDescription->setBounds(4, 1, this->getWidth() - 8, 20);
    this->revisionDate->setBounds(16, 20, this->getWidth() - 32, 14);
    this->line2->setBounds(8, 0, this->getWidth() - 16, 2);
    this->line3->setBounds(8, this->getHeight() - 2, this->getWidth() - 16, 2);
    this->remoteIndicatorImage->setBounds(this->getWidth() / 2 - 12, 37, 8, 8);
    this->localIndicatorImage->setBounds(this->getWidth() / 2 + 4, 37, 8, 8);
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
            if (i == this) { return n; }
            n = i;
        }
    }

    return n;
}

RevisionComponent *RevisionComponent::right() const
{
    if (this->children.size() > 0) { return this->children.getLast(); }
    return this->wired;
}

RevisionComponent *RevisionComponent::left() const
{
    if (this->children.size() > 0) { return this->children.getFirst(); }
    return this->wired;
}
