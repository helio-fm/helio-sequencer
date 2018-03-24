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
#include "RevisionTreeComponent.h"

#include "HelioCallout.h"
#include "VersionControl.h"
#include "VersionControlEditor.h"
#include "RevisionComponent.h"
#include "RevisionConnectorComponent.h"
#include "RevisionTooltipComponent.h"

#define CONNECTOR_HEIGHT 30


RevisionTreeComponent::RevisionTreeComponent(VersionControl &owner) :
    vcs(owner),
    treeDepth(0.f)
{
    this->setInterceptsMouseClicks(false, true);
    this->setSize(1, 1);

    RevisionComponent *root = this->initComponents(1, this->vcs.getRoot(), nullptr);
    RevisionComponent *dt = this->firstWalk(root);
    
    float min = -1;
    min = this->secondWalk(dt, min);

    if (min < 0) { this->thirdWalk(dt, -min); }

    this->postWalk(root);
}

RevisionTreeComponent::~RevisionTreeComponent()
{
    this->deleteAllChildren();
}


void RevisionTreeComponent::deselectAll()
{
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *child = this->getChildComponent(i);

        if (RevisionComponent *revChild = dynamic_cast<RevisionComponent *>(child))
        {
            revChild->setSelected(false);
        }
    }
}

void RevisionTreeComponent::selectComponent(RevisionComponent *revComponent, bool deselectOthers)
{
    if (deselectOthers)
    {
        this->deselectAll();
    }

    revComponent->setSelected(true);
}

void RevisionTreeComponent::showTooltipFor(RevisionComponent *revComponent, Point<int> clickPoint, const ValueTree revision)
{
    if (Component *editor = this->findParentEditor())
    {
        // todo create different tooltips for the head
        if (revComponent->isHead()) {}

        RevisionTooltipComponent *tooltip = new RevisionTooltipComponent(this->vcs, revision);
        HelioCallout::emit(tooltip, revComponent);
    }
}


//===----------------------------------------------------------------------===//
// Buchheim tree layout funcs
//===----------------------------------------------------------------------===//

RevisionComponent *RevisionTreeComponent::initComponents(int depth,
    const ValueTree revision, RevisionComponent *parentRevisionComponent)
{
    // create component for revision
    const bool isHead = (this->vcs.getHead().getHeadingRevision() == revision);

    RevisionComponent *revisionComponent = new RevisionComponent(this->vcs, revision, isHead);
    revisionComponent->parent = parentRevisionComponent;
    revisionComponent->y = float(depth);
    revisionComponent->number = depth;
    revisionComponent->ancestor = revisionComponent;

    this->addAndMakeVisible(revisionComponent);

    for (int i = 0; i < revision.getNumChildren(); ++i)
    {
        RevisionComponent *child =
            this->initComponents(depth + 1,
                revision.getChild(i), revisionComponent);
        revisionComponent->children.add(child);
    }

    return revisionComponent;
}

RevisionComponent *RevisionTreeComponent::firstWalk(RevisionComponent *v, float distance)
{
    if (v->children.size() == 0)
    {
        if (v->getLeftmostSibling() != nullptr)
        {
            v->x = v->getLeftBrother()->x + distance;
        }
        else
        {
            v->x = 0;
        }
    }
    else
    {
        RevisionComponent *default_ancestor = v->children[0];

        for (int i = 0; i < v->children.size(); ++i)
        {
            RevisionComponent *child = v->children[i];
            firstWalk(child);
            default_ancestor = apportion(child, default_ancestor, distance);
        }

        executeShifts(v);

        float midpoint = (v->children.getFirst()->x + v->children.getLast()->x) / 2.f;

        RevisionComponent *w = v->getLeftBrother();

        if (w)
        {
            v->x = w->x + distance;
            v->mod = v->x - midpoint;
        }
        else
        {
            v->x = midpoint;
        }
    }

    return v;
}

RevisionComponent *RevisionTreeComponent::apportion(RevisionComponent *v, RevisionComponent *default_ancestor, float distance)
{
    RevisionComponent *w = v->getLeftBrother();

    if (w != nullptr)
    {
        //in buchheim notation:
        //i == inner; o == outer; r == right; l == left;
        RevisionComponent *vir = v;
        RevisionComponent *vor = v;
        RevisionComponent *vil = w;

        RevisionComponent *vol = v->getLeftmostSibling();

        float sir = v->mod;
        float sor = v->mod;

        float sil = vil->mod;
        float sol = vol->mod;

        while (vil->right() && vir->left())
        {
            vil = vil->right();
            vir = vir->left();
            vol = vol->left();
            vor = vor->right();
            vor->ancestor = v;

            float shift = (vil->x + sil) - (vir->x + sir) + distance;

            if (shift > 0)
            {
                RevisionComponent *a = ancestor(vil, v, default_ancestor);
                moveSubtree(a, v, shift);
                sir = sir + shift;
                sor = sor + shift;
            }

            sil += vil->mod;
            sir += vir->mod;
            sol += vol->mod;
            sor += vor->mod;
        }

        if (vil->right() && !vor->right())
        {
            vor->thread = vil->right();
            vor->mod += sil - sor;
        }
        else
        {
            if (vir->left() && !vol->left())
            {
                vol->thread = vir->left();
                vol->mod += sir - sol;
            }

            default_ancestor = v;
        }
    }

    return default_ancestor;
}

void RevisionTreeComponent::moveSubtree(RevisionComponent *wl, RevisionComponent *wr, float shift)
{
    int subtrees = wr->number - wl->number;
    if (subtrees != 0) { wr->change -= shift / subtrees; }
    wr->shift += shift;
    if (subtrees != 0) { wl->change += shift / subtrees; }
    wr->x += shift;
    wr->mod += shift;
}

void RevisionTreeComponent::executeShifts(RevisionComponent *v)
{
    float shift = 0;
    float change = 0;

    for (int i = 0; i < v->children.size(); ++i) // v.children[::-1] ???
    {
        RevisionComponent *w = v->children[i];
        w->x += shift;
        w->mod += shift;
        change += w->change;
        shift += w->shift + change;
    }
}

RevisionComponent *RevisionTreeComponent::ancestor(RevisionComponent *vil, RevisionComponent *v, RevisionComponent *default_ancestor)
{
    for (int i = 0; i < v->children.size(); ++i)
    {
        RevisionComponent *child = v->children[i];

        if (child == vil->ancestor) { return vil->ancestor; }
    }

    return default_ancestor;
}

float RevisionTreeComponent::secondWalk(RevisionComponent *v, float &min, float m, float depth)
{
    v->x += m;
    v->y = depth;
    this->treeDepth = jmax(this->treeDepth, v->y);

    if ((min < 0) || (v->x < min))
    {
        min = v->x;
    }

    for (int i = 0; i < v->children.size(); ++i)
    {
        RevisionComponent *child = v->children[i];
        min = this->secondWalk(child, min, m + v->mod, depth + 1);
    }

    return min;
}

void RevisionTreeComponent::thirdWalk(RevisionComponent *v, float n)
{
    v->x += n;

    for (int i = 0; i < v->children.size(); ++i)
    {
        RevisionComponent *w = v->children[i];
        this->thirdWalk(w, n);
    }
}

void RevisionTreeComponent::postWalk(RevisionComponent *v)
{
    const int vx = int(v->x * (v->getWidth() + 10));
    const int vy = int((this->treeDepth - v->y) * (v->getHeight() + CONNECTOR_HEIGHT));

    const int newWidth = jmax(this->getWidth(), vx + v->getWidth());
    const int newHeight = jmax(this->getHeight(), vy + v->getHeight());

    this->setSize(newWidth, newHeight);
    v->setTopLeftPosition(vx, vy);

    for (int i = 0; i < v->children.size(); ++i)
    {
        RevisionComponent *w = v->children[i];
        this->postWalk(w);

        RevisionConnectorComponent *revisionConnector =
            new RevisionConnectorComponent(v, w);

        revisionConnector->resizeToFit();
        this->addAndMakeVisible(revisionConnector);
    }
}



VersionControlEditor *RevisionTreeComponent::findParentEditor() const
{
    Component *parent = this->getParentComponent();

    while (parent != nullptr)
    {
        if (VersionControlEditor *editor = dynamic_cast<VersionControlEditor *>(parent))
        {
            return editor;
        }

        parent = parent->getParentComponent();
    }

    return nullptr;
}
