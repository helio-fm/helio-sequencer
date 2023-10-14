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
#include "RevisionTreeComponent.h"
#include "CommandIDs.h"
#include "VersionControl.h"
#include "HistoryComponent.h"
#include "RevisionComponent.h"
#include "RevisionConnectorComponent.h"

RevisionTreeComponent::RevisionTreeComponent(VersionControl &owner) :
    vcs(owner)
{
    this->setInterceptsMouseClicks(false, true);
    this->setSize(1, 1);

    auto *root = this->initComponents(1, this->vcs.getRoot(), nullptr);
    auto *dt = this->firstWalk(root);
    
    float min = -1;
    min = this->secondWalk(dt, min);

    if (min < 0)
    {
        this->thirdWalk(dt, -min);
    }

    this->postWalk(root);
}

RevisionTreeComponent::~RevisionTreeComponent()
{
    this->deleteAllChildren();
}

void RevisionTreeComponent::deselectAll(bool sendNotification)
{
    for (auto *child : this->getChildren())
    {
        if (auto *revChild = dynamic_cast<RevisionComponent *>(child))
        {
            revChild->setSelected(false);
        }
    }

    this->selectedRevision = nullptr;

    auto *editor = this->findParentEditor();
    if (editor != nullptr && sendNotification)
    {
        editor->onRevisionSelectionChanged();
    }
}

void RevisionTreeComponent::selectComponent(RevisionComponent *revComponent,
    bool deselectOthers, bool sendNotification)
{
    // If already selected, deselect (may remove that later):
    if (this->selectedRevision == revComponent->revision)
    {
        this->deselectAll(true);
        return;
    }

    if (deselectOthers)
    {
        this->deselectAll(false);
    }

    revComponent->setSelected(true);
    this->selectedRevision = revComponent->revision;

    auto *editor = this->findParentEditor();
    if (editor != nullptr && sendNotification)
    {
        editor->onRevisionSelectionChanged();
    }
}

void RevisionTreeComponent::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::StartDragViewport ||
        commandId == CommandIDs::EndDragViewport)
    {
        this->deselectAll(true);
    }
}

VCS::Revision::Ptr RevisionTreeComponent::getSelectedRevision() const noexcept
{
    return this->selectedRevision;
}

//===----------------------------------------------------------------------===//
// Buchheim tree layout functions
//===----------------------------------------------------------------------===//

RevisionComponent *RevisionTreeComponent::initComponents(int depth,
    const VCS::Revision::Ptr revision, RevisionComponent *parentRevisionComponent)
{
#if NO_NETWORK
    const auto state = VCS::Revision::SyncState::NoSync;
#else
    const auto state = this->vcs.getRevisionSyncState(revision);
#endif
    const bool isHead = (this->vcs.getHead().getHeadingRevision() == revision);

    auto *revisionComponent = new RevisionComponent(this->vcs, revision, state, isHead);
    revisionComponent->parent = parentRevisionComponent;
    revisionComponent->y = float(depth);
    revisionComponent->number = depth;
    revisionComponent->ancestor = revisionComponent;

    this->addAndMakeVisible(revisionComponent);

    for (const auto childRevision : revision->getChildren())
    {
        auto *childComponent = this->initComponents(depth + 1, childRevision, revisionComponent);
        revisionComponent->children.add(childComponent);
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
        auto *default_ancestor = v->children[0];

        for (auto *child : v->children)
        {
            firstWalk(child);
            default_ancestor = apportion(child, default_ancestor, distance);
        }

        executeShifts(v);

        float midpoint = (v->children.getFirst()->x + v->children.getLast()->x) / 2.f;

        auto *w = v->getLeftBrother();

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

RevisionComponent *RevisionTreeComponent::apportion(RevisionComponent *v,
    RevisionComponent *default_ancestor, float distance)
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
            vor->wired = vil->right();
            vor->mod += sil - sor;
        }
        else
        {
            if (vir->left() && !vol->left())
            {
                vol->wired = vir->left();
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
    if (subtrees != 0)
    {
        wr->change -= shift / subtrees;
    }

    wr->shift += shift;
    if (subtrees != 0)
    {
        wl->change += shift / subtrees;
    }

    wr->x += shift;
    wr->mod += shift;
}

void RevisionTreeComponent::executeShifts(RevisionComponent *v)
{
    float shift = 0;
    float change = 0;

    for (auto *w : v->children)
    {
        w->x += shift;
        w->mod += shift;
        change += w->change;
        shift += w->shift + change;
    }
}

RevisionComponent *RevisionTreeComponent::ancestor(RevisionComponent *vil,
    RevisionComponent *v, RevisionComponent *default_ancestor)
{
    for (auto *child : v->children)
    {
        if (child == vil->ancestor)
        {
            return vil->ancestor;
        }
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

    for (auto *child : v->children)
    {
        min = this->secondWalk(child, min, m + v->mod, depth + 1);
    }

    return min;
}

void RevisionTreeComponent::thirdWalk(RevisionComponent *v, float n)
{
    v->x += n;

    for (auto *w : v->children)
    {
        this->thirdWalk(w, n);
    }
}

void RevisionTreeComponent::postWalk(RevisionComponent *v)
{
    const int vx = int(v->x * (v->getWidth() + 10));
    const int vy = int((this->treeDepth - v->y) * (v->getHeight() + RevisionTreeComponent::connectorHeight));

    const int newWidth = jmax(this->getWidth(), vx + v->getWidth());
    const int newHeight = jmax(this->getHeight(), vy + v->getHeight());

    this->setSize(newWidth, newHeight);
    v->setTopLeftPosition(vx, vy);

    for (auto *child : v->children)
    {
        this->postWalk(child);
        auto revisionConnector = make<RevisionConnectorComponent>(v, child);
        revisionConnector->resizeToFit();
        this->addAndMakeVisible(revisionConnector.release());
    }
}

HistoryComponent *RevisionTreeComponent::findParentEditor() const
{
    Component *parent = this->getParentComponent();

    while (parent != nullptr)
    {
        if (auto *editor = dynamic_cast<HistoryComponent *>(parent))
        {
            return editor;
        }

        parent = parent->getParentComponent();
    }

    return nullptr;
}
