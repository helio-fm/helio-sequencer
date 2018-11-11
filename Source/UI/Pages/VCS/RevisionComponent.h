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

#pragma once

//[Headers]
class VersionControl;
#include "Revision.h"
#include "IconComponent.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalFadingReversed.h"
#include "../../Themes/SeparatorHorizontalFading.h"

class RevisionComponent final : public Component
{
public:

    RevisionComponent(VersionControl &owner, const VCS::Revision::Ptr revision, VCS::Revision::SyncState viewState, bool isHead);
    ~RevisionComponent();

    //[UserMethods]

    // Helpers for tree traverse:

    float x;
    float y;
    float mod;
    float shift;
    float change;
    int number;

    const VCS::Revision::Ptr revision;

    RevisionComponent *parent;
    RevisionComponent *ancestor;
    RevisionComponent *wired;

    Array<RevisionComponent *> children;

    RevisionComponent *getLeftmostSibling() const;
    RevisionComponent *getLeftBrother() const;
    RevisionComponent *right() const;
    RevisionComponent *left() const;

    mutable RevisionComponent *leftmostSibling;

    // View details

    void setSelected(bool selected);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;


private:

    //[UserVariables]

    VersionControl &vcs;

    bool isSelected;
    bool isHeadRevision;
    VCS::Revision::SyncState viewState;

private:

    //[/UserVariables]

    UniquePointer<Label> revisionDescription;
    UniquePointer<Label> revisionDate;
    UniquePointer<SeparatorHorizontalFadingReversed> line2;
    UniquePointer<SeparatorHorizontalFading> line3;
    UniquePointer<IconComponent> remoteIndicatorImage;
    UniquePointer<IconComponent> localIndicatorImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RevisionComponent)
};
