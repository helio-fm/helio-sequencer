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
#include "VersionControlTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "ProjectTreeItem.h"
#include "VersionControl.h"
#include "Head.h"
#include "ProjectInfo.h"
#include "ProjectTimeline.h"
#include "MainLayout.h"
#include "App.h"
#include "FailTooltip.h"
#include "SuccessTooltip.h"
#include "Icons.h"
#include "VersionControlMenu.h"
#include "VersionControlEditor.h"
#include "HybridRoll.h"
#include "App.h"
#include "Workspace.h"

VersionControlTreeItem::VersionControlTreeItem() :
    TreeItem("Versions", Serialization::Core::versionControl),
    vcs(nullptr)
{
    this->initVCS();
    this->initEditor();
}

VersionControlTreeItem::~VersionControlTreeItem()
{
    this->shutdownEditor();
    this->shutdownVCS();
}

Colour VersionControlTreeItem::getColour() const noexcept
{
    return Colour(0xff818dff);
}

Image VersionControlTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::versionControl, HEADLINE_ICON_SIZE);
}

void VersionControlTreeItem::showPage()
{
    if (this->vcs == nullptr)
    {
        this->initVCS();
    }
    
    if (this->editor == nullptr)
    {
        this->initEditor();
    }
    
    if (this->editor != nullptr)
    {
        this->editor->updateState();
        App::Layout().showPage(this->editor, this);
    }
}

void VersionControlTreeItem::recreatePage()
{
    this->shutdownEditor();
    this->initEditor();
}

String VersionControlTreeItem::getName() const noexcept
{
    return TRANS("tree::vcs");
}

static void countStatsFor(const VCS::Revision::Ptr rootRevision, int &numRevisions, int &numDeltas)
{
    for (const auto *revItem : rootRevision->getItems())
    {
        numDeltas += revItem->getNumDeltas();
    }

    numRevisions += rootRevision->getChildren().size();
    for (auto *childRevision : rootRevision->getChildren())
    {
        countStatsFor(childRevision, numRevisions, numDeltas);
    }
}

String VersionControlTreeItem::getStatsString() const
{
    if (this->vcs)
    {
        const auto root(this->vcs->getRoot());
        
        int numRevisions = 1;
        int numDeltas = 0;
        countStatsFor(root, numRevisions, numDeltas);

        return String(TRANS_PLURAL("{x} revisions", numRevisions) + " " + TRANS("common::and") + " " + TRANS_PLURAL("{x} deltas", numDeltas));
    }

    return {};
}

void VersionControlTreeItem::commitProjectInfo()
{
    const auto *parentProject = this->findParentOfType<ProjectTreeItem>();
    if (parentProject && this->vcs)
    {
        this->vcs->quickAmendItem(parentProject->getProjectInfo());
        this->vcs->quickAmendItem(parentProject->getTimeline());
    }
}

void VersionControlTreeItem::toggleQuickStash()
{
    if (! this->vcs)
    { return; }

    this->vcs->getHead().rebuildDiffSynchronously();
    
    if (this->vcs->hasQuickStash())
    {
        if (this->vcs->getHead().hasAnythingOnTheStage())
        {
            App::Layout().showTooltip("Cannot revert, stage is not empty!");
            App::Layout().showModalComponentUnowned(new FailTooltip());
            return;
        }
        
        this->vcs->applyQuickStash();
        App::Layout().showTooltip("Toggle changes: latest state");
    }
    else
    {
        if (! this->vcs->getHead().hasAnythingOnTheStage())
        {
            App::Layout().showTooltip("Cannot reset, stage is empty!");
            App::Layout().showModalComponentUnowned(new FailTooltip());
            return;
        }
        
        this->vcs->quickStashAll();
        App::Layout().showTooltip("Toggle changes: original state");
    }
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void VersionControlTreeItem::onItemParentChanged()
{
    // Could be still uninitialized at this moment
    if (this->vcs == nullptr)
    {
        this->initVCS();
    }
}

//===----------------------------------------------------------------------===//
// Popup
//===----------------------------------------------------------------------===//

bool VersionControlTreeItem::hasMenu() const noexcept
{
    return this->vcs != nullptr;
}

ScopedPointer<Component> VersionControlTreeItem::createMenu()
{
    if (this->vcs != nullptr)
    {
        return new VersionControlMenu(*this->vcs);
    }
    
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree VersionControlTreeItem::serialize() const
{
    ValueTree tree(Serialization::Core::treeItem);
    tree.setProperty(Serialization::Core::treeItemType, this->type, nullptr);

    if (this->vcs)
    {
        tree.appendChild(this->vcs->serialize(), nullptr);
    }

    TreeItemChildrenSerializer::serializeChildren(*this, tree);
    return tree;
}

void VersionControlTreeItem::deserialize(const ValueTree &tree)
{
    this->reset();

    if (this->vcs)
    {
        forEachValueTreeChildWithType(tree, e, Serialization::Core::versionControl)
        {
            this->vcs->deserialize(e);
        }
    }

    // Proceed with basic properties and children
    TreeItem::deserialize(tree);
}

void VersionControlTreeItem::reset()
{
    if (this->vcs)
    {
        this->vcs->reset();
    }

    TreeItem::reset();
}


void VersionControlTreeItem::initVCS()
{
    ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>();

    if (parentProject &&
        (this->vcs == nullptr))
    {
        this->vcs = new VersionControl(parentProject);
        this->vcs->addChangeListener(parentProject);
        parentProject->addChangeListener(this->vcs);
    }
}

void VersionControlTreeItem::shutdownVCS()
{
    ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>();

    if (parentProject &&
        (this->vcs != nullptr))
    {
        parentProject->removeChangeListener(this->vcs);
        this->vcs->removeChangeListener(parentProject);
    }
}

void VersionControlTreeItem::initEditor()
{
    this->shutdownEditor();
    
    ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>();
    
    if (parentProject &&
        (this->vcs != nullptr))
    {
        this->editor = this->vcs->createEditor();
        this->vcs->addChangeListener(this->editor);
        parentProject->addChangeListener(this->editor);
    }
}

void VersionControlTreeItem::shutdownEditor()
{
    ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>();
    
    if (parentProject &&
        (this->vcs != nullptr) &&
        (this->editor != nullptr))
    {
        parentProject->removeChangeListener(this->editor);
        this->vcs->removeChangeListener(this->editor);
        this->editor = nullptr;
    }
}
