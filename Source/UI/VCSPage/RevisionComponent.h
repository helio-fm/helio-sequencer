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
//[/Headers]


class RevisionComponent  : public Component
{
public:

    RevisionComponent (VersionControl &owner, const VCS::Revision target, bool isHead);

    ~RevisionComponent();

    //[UserMethods]

    float x;
    float y;
    float mod;
    float shift;
    float change;
    int number;

    RevisionComponent *parent;
    RevisionComponent *ancestor;
    RevisionComponent *thread;

    Array<RevisionComponent *> children;

    RevisionComponent *getLeftmostSibling() const
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

    RevisionComponent *getLeftBrother() const
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

    RevisionComponent *right() const
    {
        if (this->thread) { return this->thread; }
        if (this->children.size() > 0) { return this->children.getLast(); }
        return nullptr;
    }

    RevisionComponent *left() const
    {
        if (this->thread) { return this->thread; }
        if (this->children.size() > 0) { return this->children.getFirst(); }
        return nullptr;
    }

    mutable RevisionComponent *leftmostSibling; //  todo private




    void setSelected(bool selected);

    bool isSelected() const;

    bool isHead() const { return this->isHeadRevision; }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void mouseMove (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;


private:

    //[UserVariables]

    VersionControl &vcs;

    const VCS::Revision revision;

    bool selected;

    bool isHeadRevision;


    DropShadowEffect shadow;


    WeakReference<RevisionComponent>::Master masterReference;

    friend class WeakReference<RevisionComponent>;

private:

    //[/UserVariables]

    ScopedPointer<Label> revisionDescription;
    ScopedPointer<Label> revisionDate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RevisionComponent)
};
