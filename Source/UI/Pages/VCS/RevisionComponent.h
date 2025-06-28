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

#pragma once

class VersionControl;
class HeadlineContextMenuController;

#include "Revision.h"
#include "IconComponent.h"
#include "SeparatorHorizontalFadingReversed.h"
#include "SeparatorHorizontalFading.h"
#include "ColourIDs.h"

class RevisionComponent final : public Component
{
public:

    RevisionComponent(VersionControl &owner,
        const VCS::Revision::Ptr revision, bool isHeadRevision);

    ~RevisionComponent();

    // Helpers for tree traverse:
    float x = 0.f;
    float y = 0.f;
    float mod = 0.f;
    float shift = 0.f;
    float change = 0.f;
    int number = 0;

    const VCS::Revision::Ptr revision;

    RevisionComponent *parent = nullptr;
    RevisionComponent *ancestor = nullptr;
    RevisionComponent *wired = nullptr;

    Array<RevisionComponent *> children;

    RevisionComponent *getLeftmostSibling() const;
    RevisionComponent *getLeftBrother() const;
    RevisionComponent *right() const;
    RevisionComponent *left() const;

    mutable RevisionComponent *leftmostSibling = nullptr;

    void setSelected(bool selected);

    void paint(Graphics &g) override;
    void resized() override;
    void mouseMove(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    VersionControl &vcs;

    bool isSelected = false;
    const bool isHeadRevision = false;

    UniquePointer<HeadlineContextMenuController> contextMenuController;

private:

    UniquePointer<Label> revisionDescription;
    UniquePointer<Label> revisionDate;
    UniquePointer<SeparatorHorizontalFadingReversed> line2;
    UniquePointer<SeparatorHorizontalFading> line3;

    const Colour fillColour = findDefaultColour(ColourIDs::VersionControl::revisionFill);
    const Colour outlineColour = findDefaultColour(ColourIDs::VersionControl::revisionOutline);
    const Colour highlightColour = findDefaultColour(ColourIDs::VersionControl::revisionHighlight);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionComponent)
};
